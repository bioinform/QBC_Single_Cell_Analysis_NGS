#include <tuple>
#include   <stdio.h>
#include   <string>
#include <limits>
#include <iostream>
#include   <fstream>
#include   <vector>
#include   <time.h>
#include   <map>
#include   <sstream>
#include <omp.h>
#include   "kernel.h"
#include "kernel_ops.h"
//#include "cuda.h"
//#include "cuda_runtime.h"
//#include "device_launch_parameters.h"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
using namespace std;

void cudaFreeHost(void* ptr) {
  free(ptr);
}

void cudaFree(void* ptr) {
  free(ptr);
}

#define __constant__
#define __global__

using cudaEvent_t = int;

void atomicAdd(int& d, int delta) {
#pragma omp atomic
  d += delta;
}

void atomicAdd(int* d, int delta) {
  atomicAdd(*d, delta);
}

int cudaMemcpyHostToDevice = 0;
int cudaMemcpyDeviceToHost = 0;

void cudaEventCreate(cudaEvent_t*) {};

void cudaEventRecord(cudaEvent_t, int) {};

void cudaDeviceSynchronize(){};

void cudaEventSynchronize(cudaEvent_t){};

void cudaEventElapsedTime(float*, cudaEvent_t, cudaEvent_t) {};

void* cudaMemcpy(void* dst, const void * src, size_t count, int kind) {
  return memcpy(dst, src, count);
}

void* cudaMemset(void* dst, int ch, size_t count) {
  return memset(dst, ch, count);
}

void* cudaMemcpyToSymbol(void* dst, const void * src, size_t count) {
  return memcpy(dst, src, count);
}

void cudaMalloc(void** devPtr, size_t size) {
  *devPtr = malloc(size);
}

void cudaMallocHost(void** devPtr, size_t size) {
  *devPtr = malloc(size);
}

void cudaSetDevice(int) {}

struct HardCodedLegacy {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(WIN64)
    enum {FOR_WINDOWS = 1};
#else
    enum {FOR_WINDOWS = 0};
#endif

    // because boost wasn't gauranteed to be built properly in legacy code, will change later
    static char SEP() { return FOR_WINDOWS ? '\\' : '/'; }

    static std::string SCCodeFile(std::string const& directory) {
        return directory + SEP() + "SC_Codes.txt";
    }
    static std::string AHCACodeFile(std::string const& directory) {
	    return directory + SEP() + "AHCA_Codes.txt";

    }
    static std::string OligosFile(std::string const& directory) {
	    return directory + SEP() + SEP() + "oligos.txt";
    }
};

//For reading windows formatted file with '\r\n' delimitor on linux
std::istream& UniversalGetLine(std::istream& is, std::string&t) {
    std::getline(is, t);
    if(! HardCodedLegacy::FOR_WINDOWS && (t.size() > 0 && t.back() == '\r') ){
        t.resize(t.size() - 1);
    }
    return is;
}

//+ Define MACRO
#define   MAX_SEQUENCE_LEN 512  // Max length of a sequence   :::Changed 256 --> 512
#define   MAX_SEQUENCE_NUM 10240 // Max number of sequences be processed at same time.
#define   MAX_TEMPLATE_LEN 20

#define   VALID_SC_LEN 7
#define   VALID_AHCA_LEN 9
#define   VALID_AHCACODE_LEN 30   // ::: Valid AHCA code lenght 9 - 30, max 30
#define   VALID_RANDOM_LEN 10
#define   ERROR_CODE_LEN 21

/* RESULT_SEGMENT_SIZE
the size of a segment where stored all extracted codes for each sequence
It stores : random code + 4 barcode + ahca code + errorcode.
*/
#define   RESULT_SEGMENT_SIZE ((VALID_RANDOM_LEN+1) + 4 *(VALID_SC_LEN+1) + (VALID_AHCACODE_LEN + 1) + (ERROR_CODE_LEN+1))  // COMMENT

/* BLOCK_SIZE
Number of threads in a CUDA block. Each thread process one sequence. You can increase or decrease this number to see how it effect on the performance
Note : I use shared memory on each block : __shared__ int DIST[BLOCK_SIZE][VALID_AHCA_LEN +1][VALID_AHCA_LEN +1];
Max support shared memory is 48KB (2.x or above) and 16KB (<2.x device). So you can't set BLOCK_SIZE larger.
For example : BLOCK_SIZE = 64, VALID_AHCA_LEN = 9, it consumed about 64 x 10 x 10 x sizeof(int) = 25600 bytes ~25KB.
if you have BLOCK_SIZE = 32, device < 2.0 can work.
*/
#define   BLOCK_SIZE 64  // COMMENT

#define   BARCODE_VALID 0
#define   BARCODE_NOTVALID 1

#define   AHCA_VALID 0
#define   AHCA_NOTVALID 1
#define   MAX_AHCA_DIST 30

#define   TYPE_FORWARD 0
#define   TYPE_BACKWARD 1
#define   TYPE_JUNK 2
//- End define MACRO

//bool saveBadLines; //remove legacy code gloabl variables
//int validSCnums; //Legacy code does not use this variable, it uses validSCNumber 
//+ Statistic Variable


//string whereatCU; //Legacy code does not use this variable. 
//string whereatCount; //Legacy code does not use this variable.


//+ End Statistic Variable

//+ Define all buffers
// Note : we can't use string functions as strlen in CUDA.
//AHCAinfo ahcaInfo;
//vector<AHCAinfo> vAHCA_Info;
// Buffer point to device memory
//- End define all buffers

//+ Define constant memory use in GPU
namespace cuda_constants {
__constant__ char SC3[MAX_TEMPLATE_LEN]; // COMMENT store SC3 template code
__constant__ char SC[6 * MAX_TEMPLATE_LEN]; // COMMENT store SC2L, SC2R, SCNL, SCNR, SC1L,SCR template codes
__constant__ char Anneal_Pre[MAX_TEMPLATE_LEN]; // COMMENT  store Anneal_Pre template code.
__constant__ char Anneal_Post[MAX_TEMPLATE_LEN]; // COMMENT store Anneal_Post template code.
// Note : your template codes should have size < MAX_TEMPLATE_LEN,
// if not, increase MAX_TEMPLATE_LEN larger.

__constant__ char ERROR_MARK[4]; // COMMENT store "2N1" , use for error mark: error in SC2, SCN or SC1.
__constant__ char NUMBER_MARK[10]; // COMMENT store "0123456789", use for marking in GPU only.

__constant__ char SC_MISS_BOTH[VALID_SC_LEN + 1]; // COMMENT  store "xx---xx"; //
__constant__ char SC_MISS_LEFT[VALID_SC_LEN + 1]; // COMMENT  store "xx0L0xx";
__constant__ char SC_MISS_RIGHT[VALID_SC_LEN + 1]; // COMMENT store "xx0R0xx";
}


//- End define constant memory

void cleanUpMem(LegacyBuffers& buffers)
{
	// COMMENT : free memory on host, these buffer allocated as "pinned" memory
	printf("\n   Start clean up memory:\n");
	time_t start_time, end_time;

	time(&start_time);

	if (buffers.h_sequenceHeader != NULL)
	{
		cudaFreeHost(buffers.h_sequenceHeader);
		buffers.h_sequenceHeader = NULL;
	}
	if (buffers.h_sequenceLength != NULL)
	{
		cudaFreeHost(buffers.h_sequenceLength);
		buffers.h_sequenceLength = NULL;
	}
	if (buffers.h_sequenceBuffer != NULL)
	{
		cudaFreeHost(buffers.h_sequenceBuffer);
		buffers.h_sequenceBuffer = NULL;
	}
	if (buffers.h_detectionInfo != NULL)
	{
		cudaFreeHost(buffers.h_detectionInfo);
		buffers.h_detectionInfo = NULL;
	}

	if (buffers.h_resultBuffer != NULL)
	{
		cudaFreeHost(buffers.h_resultBuffer);
		buffers.h_resultBuffer = NULL;
	}
	if (buffers.h_RandomCodeMatchingTable != NULL)
	{
		cudaFreeHost(buffers.h_RandomCodeMatchingTable);
		buffers.h_RandomCodeMatchingTable = NULL;
	}
	if (buffers.h_settedRandomCodeValueTable != NULL)
	{
		cudaFreeHost(buffers.h_settedRandomCodeValueTable);
		buffers.h_settedRandomCodeValueTable = NULL;
	}
	if (buffers.h_RandomCodeValueTable != NULL)
	{
		cudaFreeHost(buffers.h_RandomCodeValueTable);
		buffers.h_RandomCodeValueTable = NULL;
	}

	if (buffers.d_validAHCALength != NULL)
	{
		cudaFree(buffers.d_validAHCALength);
		buffers.d_validAHCALength = NULL;
	}
	if (buffers.d_validAHCAmutStart != NULL)
	{
		cudaFree(buffers.d_validAHCAmutStart);
		buffers.d_validAHCAmutStart = NULL;
	}
	if (buffers.d_validAHCAmutEnd != NULL)
	{
		cudaFree(buffers.d_validAHCAmutEnd);
		buffers.d_validAHCAmutEnd = NULL;
	}
	if (buffers.d_validAHCAMutation != NULL)
	{
		cudaFree(buffers.d_validAHCAMutation);
		buffers.d_validAHCAMutation = NULL;
	}

	if (buffers.d_validAHCATagNameBuffer != NULL)
	{
		cudaFree(buffers.d_validAHCATagNameBuffer);
		buffers.d_validAHCATagNameBuffer = NULL;
	}
	if (buffers.d_AHCAAssignList != NULL)
	{
		cudaFree(buffers.d_AHCAAssignList);
		buffers.d_AHCAAssignList = NULL;
	}
	if (buffers.d_validAHCAType != NULL)
	{
		cudaFree(buffers.d_validAHCAType);
		buffers.d_validAHCAType = NULL;
	}

	// Free device variables
	if (buffers.d_sequenceHeader != NULL)
	{
		cudaFree(buffers.d_sequenceHeader);
		buffers.d_sequenceHeader = NULL;
	}
	if (buffers.d_sequenceBuffer != NULL)
	{
		cudaFree(buffers.d_sequenceBuffer);
		buffers.d_sequenceBuffer = NULL;
	}
	if (buffers.d_sequenceLength != NULL){
		cudaFree(buffers.d_sequenceLength);
		buffers.d_sequenceLength = NULL;
	}
	if (buffers.d_validSCBuffer != NULL) {
		cudaFree(buffers.d_validSCBuffer);
		buffers.d_validSCBuffer = NULL;
	}
	if (buffers.d_detectionInfo != NULL) {
		cudaFree(buffers.d_detectionInfo);
		buffers.d_detectionInfo = NULL;
	}
	if (buffers.d_oligosBuffer != NULL){
		cudaFree(buffers.d_oligosBuffer);
		buffers.d_oligosBuffer = NULL;
	}
	if (buffers.d_oligosLength != NULL) {
		cudaFree(buffers.d_oligosLength);
		buffers.d_oligosLength = NULL;
	}


	if (buffers.d_validAHCABuffer != NULL) {
		cudaFree(buffers.d_validAHCABuffer);
		buffers.d_validAHCABuffer = NULL;
	}
	if (buffers.d_resultBuffer != NULL){
		cudaFree(buffers.d_resultBuffer);
		buffers.d_resultBuffer = NULL;
	}
	if (buffers.d_RandomCodeBuffer != NULL) {
		cudaFree(buffers.d_RandomCodeBuffer);
		buffers.d_RandomCodeBuffer = NULL;
	}
        if (buffers.d_RandomNumBuffer != NULL) {
                cudaFree(buffers.d_RandomNumBuffer);
                buffers.d_RandomNumBuffer = NULL;
        }
	if (buffers.d_RandomCodeMatchingTable != NULL){
		cudaFree(buffers.d_RandomCodeMatchingTable);
		buffers.d_RandomCodeMatchingTable = NULL;
	}

	time(&end_time);
	float totalTimed = (float)difftime(end_time, start_time);
	printf("	   Time to clean up memory: %.2f seconds\n\n", totalTimed);

}

void readTemplate(string directoryName, LegacyBuffers& buffers) // COMMENT Initialize all template code
{
	char * h_oligosBuff; // COMMENT : host oligos buffer
	int * h_oligosLength; // COMMENT : host oligos length buffer
	readOligos(directoryName, h_oligosBuff, h_oligosLength, buffers.oligosNumber); // COMMENT read Oligos.txt file

	cudaMalloc((void**)& buffers.d_oligosBuffer, buffers.oligosNumber * (MAX_TEMPLATE_LEN + 1) * sizeof(char)); // COMMENT allocate memory for device oligos buffer
	cudaMemcpy(buffers.d_oligosBuffer, h_oligosBuff, buffers.oligosNumber * (MAX_TEMPLATE_LEN + 1) * sizeof(char), cudaMemcpyHostToDevice); // COMMENT copy oligos  buffer to device

	cudaMalloc((void**)& buffers.d_oligosLength, buffers.oligosNumber * sizeof(int)); // COMMENT  allocate memory for oligos length buffer
	cudaMemcpy(buffers.d_oligosLength, h_oligosLength, buffers.oligosNumber * sizeof(int), cudaMemcpyHostToDevice); // COMMENT copy data from host to device
	delete h_oligosBuff;
	delete h_oligosLength;

	const char * xSC3 = "GCTCACCAAGTCCTAGT";
	const char * xSC1l = "GCGACTTCC";
	const char * xSCNl = "GTCTTTGCC";
	const char * xSC2l = "GCGTTACCC";

	const char * xSC1r = "CCAACCACC";
	const char * xSCNr = "CCGTTGTGG";
	const char * xSC2r = "CCCATTCTC";
	const char * xAnneal_Pre = "GCTCCCTGTCTGACG";
	const char * xAnneal_Post = "AGATCGGAAGAG";

	char * tempSC = new char[6 * MAX_TEMPLATE_LEN];
	memcpy(&tempSC[0 * MAX_TEMPLATE_LEN], xSC2l, strlen(xSC2l) * sizeof(char)); // COMMENT : copy all SC code to tempSC buffer.
	memcpy(&tempSC[1 * MAX_TEMPLATE_LEN], xSC2r, strlen(xSC2r) * sizeof(char));
	memcpy(&tempSC[2 * MAX_TEMPLATE_LEN], xSCNl, strlen(xSCNl) * sizeof(char));
	memcpy(&tempSC[3 * MAX_TEMPLATE_LEN], xSCNr, strlen(xSCNr) * sizeof(char));
	memcpy(&tempSC[4 * MAX_TEMPLATE_LEN], xSC1l, strlen(xSC1l) * sizeof(char));
	memcpy(&tempSC[5 * MAX_TEMPLATE_LEN], xSC1r, strlen(xSC1r) * sizeof(char));

	buffers.templateLenInfo.SC3 = strlen(xSC3); // COMMENT : store SC3 length for GPU use
	cudaMemcpyToSymbol(cuda_constants::SC3, xSC3, strlen(xSC3) * sizeof(char)); // COMMENT copy SC3 template to constant memory in GPU

	buffers.templateLenInfo.SC[0] = strlen(xSC2l); // COMMENT copy all SC tempalte length to buffer.
	buffers.templateLenInfo.SC[1] = strlen(xSC2r);
	buffers.templateLenInfo.SC[2] = strlen(xSCNl);
	buffers.templateLenInfo.SC[3] = strlen(xSCNr);
	buffers.templateLenInfo.SC[4] = strlen(xSC1l);
	buffers.templateLenInfo.SC[5] = strlen(xSC1r);

	cudaMemcpyToSymbol(cuda_constants::SC, tempSC, 6 * MAX_TEMPLATE_LEN * sizeof(char)); // COMMENT COMMENT copy other SC template to constant memory in GPU

	buffers.templateLenInfo.Anneal_Pre = strlen(xAnneal_Pre); // COMMENT copy this length.
	cudaMemcpyToSymbol(cuda_constants::Anneal_Pre, xAnneal_Pre, strlen(xAnneal_Pre) * sizeof(char)); // COMMENT copy to constant memory

	buffers.templateLenInfo.Anneal_Post = strlen(xAnneal_Post);
	cudaMemcpyToSymbol(cuda_constants::Anneal_Post, xAnneal_Post, strlen(xAnneal_Post) * sizeof(char));

	const char * MARK = "2N1";
	cudaMemcpyToSymbol(cuda_constants::ERROR_MARK, MARK, 4 * sizeof(char)); // COMMENT // copy MARK to constant memory

	const char * NUMBER = "0123456789";
	cudaMemcpyToSymbol(cuda_constants::NUMBER_MARK, NUMBER, 10 * sizeof(char)); // COMMENT

	const char * missBoth = "xx---xx";
	const char * missLeft = "xx0L0xx";
	const char * missRight = "xx0R0xx";

	cudaMemcpyToSymbol(cuda_constants::SC_MISS_BOTH, missBoth, (VALID_SC_LEN + 1) * sizeof(char));
	cudaMemcpyToSymbol(cuda_constants::SC_MISS_LEFT, missLeft, (VALID_SC_LEN + 1) * sizeof(char));
	cudaMemcpyToSymbol(cuda_constants::SC_MISS_RIGHT, missRight, (VALID_SC_LEN + 1) * sizeof(char));
}

int LegacyBuffers::Initialize(const string& directoryName, const bool savebad, const int mutation, const int deviceId) // COMMENT : Read input data, allocate memory....
{
	//saveBadLines = savebad;
	allowedEdit = mutation;
  AHCA_map.clear();
	totalSeqs = 0;
	cleanUpMem(*this);
	//reset(); // directory calling reset function
	SInfo.reset();
	SCList.clear();
	int deviceCount = -1;
#if 0
	cudaGetDeviceCount(&deviceCount);
	if (deviceCount == 0)
	{
		printf("There is no CUDA enabled GPU on your system. Quit\n");
		return 0;
	}
#endif
	cudaSetDevice(deviceId); // COMMENT : choose device card to run based on its Id.
	SInfo.totalSequenceInFile = 0; // COMMENT : counter - number of read sequence from input file
	sequenceCounter = 0;

	cudaMallocHost((void**)& h_sequenceHeader, MAX_SEQUENCE_NUM * (MAX_SEQUENCE_LEN + 1)* sizeof(char)); // COMMENT : host buffer to store chunk of input sequence header, allocate on page-locked memory
	cudaMallocHost((void**)& h_sequenceBuffer, MAX_SEQUENCE_NUM * (MAX_SEQUENCE_LEN + 1)* sizeof(char)); // COMMENT : host buffer to store chunk of input sequence, allocate on page-locked memory
	cudaMallocHost((void**)& h_sequenceLength, MAX_SEQUENCE_NUM * sizeof(int)); // COMMENT : store array of sequence length, allocated on page-lock memory

	// Initialize for device variables
	cudaMalloc((void **)& d_sequenceHeader, MAX_SEQUENCE_NUM * (MAX_SEQUENCE_LEN + 1) * sizeof(char)); // COMMENT device buffer to store chunk of input sequence header
	cudaMalloc((void **)& d_sequenceBuffer, MAX_SEQUENCE_NUM * (MAX_SEQUENCE_LEN + 1) * sizeof(char)); // COMMENT device buffer to store chunk of input sequence
	cudaMalloc((void **)& d_sequenceLength, MAX_SEQUENCE_NUM * sizeof(int)); // COMMENT store array of sequence length, transfered from h_sequenceLength

	char * h_validSCCodeBuffer; // COMMENT : store all SC codes from SC.txt file
	validSCNumber = 0;
	int code = ReadValidSCCode(directoryName, h_validSCCodeBuffer, *this); // COMMENT : read SC.txt file and store in h_validSCCodeBuffer as 1 dimension array
	//validSCnums = validSCNumber;
	if (code == 0)
	{
		return 0;
	}
	cudaMalloc((void**)& d_validSCBuffer, validSCNumber * (VALID_SC_LEN + 1) * sizeof(char)); // COMMENT : Allocate device memory to store all valid SC code
	cudaMemcpy(d_validSCBuffer, h_validSCCodeBuffer, validSCNumber * (VALID_SC_LEN + 1) * sizeof(char), cudaMemcpyHostToDevice); // COMMENT : copy data from host to device for further usage.
	delete h_validSCCodeBuffer;

	char * h_validAHCACodeBuffer; // COMMENT : host buffer to store all valid AHCA code which read from AHCA.txt file
	int * h_validAHCALength;
	int * h_validAHCAmutStart;
	int * h_validAHCAmutEnd;
	int * h_validAHCAMutation;

	char * h_AHCAAssignList;      // The AHCAs to be assigned if found
	int * h_validAHCAType;

	validAHCANumber = 0;
	// ::: COMMENT Read AHCA codes, Mutation, tagNames, Assigend AHCA (Type A and Type B)
	readValidAHCA(directoryName, h_validAHCACodeBuffer, h_AHCAAssignList, h_validAHCALength, h_validAHCAmutStart, h_validAHCAmutEnd, h_validAHCAMutation, validAHCANumber, h_validAHCAType, AHCA_map); // ::: COMMENT : Read AHCA.txt file and store in h_validAHCaCodeBuffer

	cudaMalloc((void**)& d_validAHCABuffer, validAHCANumber * (VALID_AHCACODE_LEN + 1) * sizeof(char)); // ::: COMMENT : Allocate device memory to store validAHCA codes
	cudaMemcpy(d_validAHCABuffer, h_validAHCACodeBuffer, validAHCANumber * (VALID_AHCACODE_LEN + 1) * sizeof(char), cudaMemcpyHostToDevice); // ::: COMMENT copy data from host to device

	cudaMalloc((void**)& d_AHCAAssignList, validAHCANumber * (VALID_AHCA_LEN + 1) * sizeof(char)); // ::: COMMENT : Allocate device memory to store Assigned codes
	cudaMemcpy(d_AHCAAssignList, h_AHCAAssignList, validAHCANumber * (VALID_AHCA_LEN + 1) * sizeof(char), cudaMemcpyHostToDevice); // ::: COMMENT copy data from host to device

	cudaMalloc((void**)& d_validAHCALength, validAHCANumber * sizeof(int)); // ::: COMMENT : Allocate device memory to store validAHCA codes Length
	cudaMemcpy(d_validAHCALength, h_validAHCALength, validAHCANumber * sizeof(int), cudaMemcpyHostToDevice); // ::: COMMENT copy data from host to device

	cudaMalloc((void**)& d_validAHCAmutStart, validAHCANumber * sizeof(int)); // ::: COMMENT : Allocate device memory to store validAHCA mutant start position
	cudaMemcpy(d_validAHCAmutStart, h_validAHCAmutStart, validAHCANumber * sizeof(int), cudaMemcpyHostToDevice); // ::: COMMENT copy data from host to device

	cudaMalloc((void**)& d_validAHCAmutEnd, validAHCANumber * sizeof(int)); // ::: COMMENT : Allocate device memory to store validAHCA mutant start position
	cudaMemcpy(d_validAHCAmutEnd, h_validAHCAmutEnd, validAHCANumber * sizeof(int), cudaMemcpyHostToDevice); // ::: COMMENT copy data from host to device
	
	cudaMalloc((void**)& d_validAHCAMutation, validAHCANumber * sizeof(int)); //  ::: COMMENT : Allocate device memory to store validAHCA codes Mutation
	cudaMemcpy(d_validAHCAMutation, h_validAHCAMutation, validAHCANumber * sizeof(int), cudaMemcpyHostToDevice); // ::: COMMENT copy data from host to device

	cudaMalloc((void**)& d_validAHCAType, validAHCANumber * sizeof(int)); //  ::: COMMENT : Allocate device memory to store validAHCA codes Mutation
	cudaMemcpy(d_validAHCAType, h_validAHCAType, validAHCANumber * sizeof(int), cudaMemcpyHostToDevice); // ::: COMMENT copy data from host to device

	delete h_validAHCACodeBuffer;
	delete h_AHCAAssignList;
	delete h_validAHCALength;
	delete h_validAHCAmutStart;
	delete h_validAHCAmutEnd;
	delete h_validAHCAMutation;
	delete h_validAHCAType;

	cudaMallocHost((void **)& h_detectionInfo, MAX_SEQUENCE_NUM * sizeof(DetectionInfo)); // COMMENT : host array store additional info about sequences
	cudaMalloc((void **)& d_detectionInfo, MAX_SEQUENCE_NUM * sizeof(DetectionInfo)); // COMMENT : device array store additional infos about sequences, device -copy---> host.

	// Allocate memory for result buffer
	cudaMallocHost((void **)& h_resultBuffer, MAX_SEQUENCE_NUM * RESULT_SEGMENT_SIZE * sizeof(char)); // Buffer stores result on host.
	cudaMalloc((void **)& d_resultBuffer, MAX_SEQUENCE_NUM * RESULT_SEGMENT_SIZE * sizeof(char)); // Buffer stores result on device
  
  //The legacy code does not initialize the memory used for result buffer.
  //This creates the discrepancy of the result between windows and linux platform.
  cudaMemset(h_resultBuffer, 0, MAX_SEQUENCE_NUM * RESULT_SEGMENT_SIZE * sizeof(char));
  cudaMemset(d_resultBuffer, 0, MAX_SEQUENCE_NUM * RESULT_SEGMENT_SIZE * sizeof(char));

	cudaMalloc((void **)& d_RandomCodeBuffer, MAX_RANDOM_NUMBER_EACH_AHCA * (VALID_RANDOM_LEN + 1) * sizeof(char));
        cudaMalloc((void **)& d_RandomNumBuffer, MAX_RANDOM_NUMBER_EACH_AHCA * sizeof(int));

	//d_RandomCodeMatchingTable
	cudaMallocHost((void **)& h_RandomCodeMatchingTable, MAX_RANDOM_NUMBER_EACH_AHCA * sizeof(int));
	cudaMalloc((void **)& d_RandomCodeMatchingTable, MAX_RANDOM_NUMBER_EACH_AHCA * sizeof(int));

	h_settedRandomCodeValueTable = new int[MAX_RANDOM_NUMBER_EACH_AHCA + 1];
	for (int i = 0; i <= MAX_RANDOM_NUMBER_EACH_AHCA; i++)
	{
		h_settedRandomCodeValueTable[i] = 1; // Set 1 to all detected Random Code
	}
	h_RandomCodeValueTable = new int[MAX_RANDOM_NUMBER_EACH_AHCA + 1];
  return 1;
}


void ProcessSequences(const string& fileName, const bool saveBadLines, LegacyBuffers& buffers, vector<Cell> & cellHash, vector<string> & junkLines, vector<string> & badLines, StatisticsInfo & statisticsInfo, vector<string> & SC_stats, string ENTRY, string & whereatCU, string & whereatCount)
{
	buffers.h_SCx_stat = new int[buffers.validSCNumber * 4];
	for (int i = 0; i < buffers.validSCNumber * 4; i++)
	{
		buffers.h_SCx_stat[i] = 0;
	}

	cudaMalloc((void**)& buffers.d_SCx_stat, buffers.validSCNumber * 4 * sizeof(int));
	cudaMemcpy(buffers.d_SCx_stat, buffers.h_SCx_stat, buffers.validSCNumber * 4 * sizeof(int), cudaMemcpyHostToDevice);
	ifstream infile;
	infile.open(fileName.c_str());

	if (!infile.is_open())
	{
		printf("Cannot open %s file. Exit\n", fileName.c_str());
		exit(0);
	}

	string line;
	string sequence = "";
	int greaterthan = 1000000;
	int doubleme = 0;
	/*	Reading input sequence from file line by line until reaching a limit MAX_SEQUENCE_NUM, then process this part of
	sequence on GPU. Then continue the same way for further part.	*/
	int i = 0;
	while (!infile.eof())
	{
		UniversalGetLine(infile, line);
		int pos = line.find(">"); // DEBUG set to 0 if ">" found; otherwise -1

		if (pos == string::npos && line.size() > 0){ // Not header line // DEBUG found sequence
			sequence += line;
			continue;
		}

		/*		Check a line is not header, then copy this line to h_sequenceBuffer for GPU processing		*/
		if (pos != string::npos) // DEBUG found header
		{
			if (sequence == "")
			{
                                // save header
                                strcpy(&buffers.h_sequenceHeader[buffers.sequenceCounter * (MAX_SEQUENCE_LEN + 1)], line.c_str()); // copy sequence header to buffer
				continue;
			}else if (buffers.sequenceCounter != MAX_SEQUENCE_NUM){
                            // as long as buffers.sequenceCounter != MAX_SEQUENCE_NUM, save header at next idx
                            strcpy(&buffers.h_sequenceHeader[(buffers.sequenceCounter+1) * (MAX_SEQUENCE_LEN + 1)], line.c_str()); // copy sequence header to buffer
                        }
			int len = sequence.size();

			if (len > MAX_SEQUENCE_LEN)
			{
				printf("Detected a sequence has length over limit of %d. Exit\n", MAX_SEQUENCE_LEN);
				exit(0);
			}

			buffers.h_sequenceLength[buffers.sequenceCounter] = len;													// We need to know length of a sequence, store this info in h_sequenceLength
			strcpy(&buffers.h_sequenceBuffer[buffers.sequenceCounter * (MAX_SEQUENCE_LEN + 1)], sequence.c_str());		// copy sequence to buffer
			buffers.sequenceCounter++;																			//increase the counter by 1
			buffers.SInfo.totalSequenceInFile++;
			sequence = "";
			buffers.totalSeqs++;	i++;

			string GTWithCommas = to_string(greaterthan);
			int insertPosition = GTWithCommas.length() - 3;
			while (insertPosition > 0) {
				GTWithCommas.insert(insertPosition, ",");
				insertPosition -= 3;
			}

			string numWithCommas = to_string(buffers.SInfo.totalSequenceInFile);
			int i_insertPosition = numWithCommas.length() - 3;
			while (i_insertPosition > 0) {
				numWithCommas.insert(i_insertPosition, ",");
				i_insertPosition -= 3;
			}	

			if (buffers.totalSeqs == greaterthan)
			{
				printf("	Sequences read from  %s    %s\n", ENTRY.c_str(), numWithCommas.c_str(), GTWithCommas.c_str());
				buffers.totalSeqs = 0;	doubleme++;
				if (doubleme == 2)
				{
					doubleme = 0;
					greaterthan = greaterthan * 2;
				}
			}
			
			if (i == 99999)
			{
				char a[50]; 	sprintf(a, "Seq read: %s\n", ENTRY.c_str());
				char b[50]; 	sprintf(b, "Count: %s\n", numWithCommas.c_str());
				whereatCU = string(a);
				whereatCount = string(b);
				i = 0;
			}
		}

		// Process a part of sequence which reaching the limit.
		if (buffers.sequenceCounter == MAX_SEQUENCE_NUM) { // Read Max sequences number -> Process it
			// Processing
			workOutGPU(saveBadLines, buffers, cellHash, junkLines, badLines);
			buffers.sequenceCounter = 0; // Reset the counter for a part (chunk)
                        strcpy(&buffers.h_sequenceHeader[(buffers.sequenceCounter) * (MAX_SEQUENCE_LEN + 1)], line.c_str());
		}
	}

	infile.close();

	// This section use to get the last line in the input file, so we not miss a line as your script
	if (sequence != "") {
		int len = sequence.size();

		if (len > MAX_SEQUENCE_LEN)
		{
			printf("Detected a sequence has length over limit of %d. Exit\n", MAX_SEQUENCE_LEN);
//			_getch();
			std::cin.ignore(std::numeric_limits<streamsize>::max(), '\n');
			throw std::runtime_error("Detected a sequence has length over MAX_SEQUENCE_LEN");
			exit(0);
		}

		buffers.h_sequenceLength[buffers.sequenceCounter] = len;
		strcpy(&buffers.h_sequenceBuffer[buffers.sequenceCounter * (MAX_SEQUENCE_LEN + 1)], sequence.c_str());
		buffers.sequenceCounter++;
		buffers.SInfo.totalSequenceInFile++;
	}
	// end copy last line

	if (buffers.sequenceCounter > 0) {																	// Process remaining sequences
		workOutGPU(saveBadLines, buffers, cellHash, junkLines, badLines);
		buffers.sequenceCounter = 0;
	}

	// This part is retrieve info about valid SC1,SC2, SCN, SC3 --> use for printing in statistis file.
	cudaMemcpy(buffers.h_SCx_stat, buffers.d_SCx_stat, buffers.validSCNumber * 4 * sizeof(int), cudaMemcpyDeviceToHost);

	int sc3 = 0;
	int sc2 = 0;
	int scn = 0;
	int sc1 = 0;
	stringstream ssXSC3;
	stringstream ssXSC2;
	stringstream ssXSCN;
	stringstream ssXSC1;

	for (int i = 0; i < buffers.validSCNumber; i++)														// Go through all valid code.
	{
		if (buffers.h_SCx_stat[i * 4] > 0) { // ith SC have number of SC3
			sc3++;
			ssXSC3 << "SC3_" << sc3 << "\t" << buffers.SCList[i] << "\t" << buffers.h_SCx_stat[i * 4] << "\n";	// print this to output :
		}
		if (buffers.h_SCx_stat[i * 4 + 1] > 0) {
			sc2++;
			ssXSC2 << "SC2_" << sc2 << "\t" << buffers.SCList[i] << "\t" << buffers.h_SCx_stat[i * 4 + 1] << "\n";
		}
		if (buffers.h_SCx_stat[i * 4 + 2] > 0) {
			scn++;
			ssXSCN << "SCN_" << scn << "\t" << buffers.SCList[i] << "\t" << buffers.h_SCx_stat[i * 4 + 2] << "\n";
		}
		if (buffers.h_SCx_stat[i * 4 + 3] > 0) {
			sc1++;
			ssXSC1 << "SC1_" << sc1 << "\t" << buffers.SCList[i] << "\t" << buffers.h_SCx_stat[i * 4 + 3] << "\n";
		}
	}

	SC_stats.push_back(ssXSC1.str());
	SC_stats.push_back(ssXSC2.str());
	SC_stats.push_back(ssXSC3.str());
	SC_stats.push_back(ssXSCN.str());

	if (buffers.d_SCx_stat != NULL) cudaFree(buffers.d_SCx_stat);												// Free memory
	if (buffers.h_SCx_stat != NULL) free(buffers.h_SCx_stat);

	printf("\n---> Ended GPU processing of sequences from %s.\n\n", ENTRY.c_str(), buffers.SInfo.gpuTime);
	printf("---->   Total kernel gpu time %f\n", buffers.SInfo.gpuTime);
	printf("---->   Total substr time %f\n\n", buffers.SInfo.subtractTime);

	buffers.SInfo.goodCodes = cellHash.size();
	statisticsInfo = buffers.SInfo; // Copy all statistic Info
	buffers.SInfo.reset();
}

//function not used
#if 0
int hashSc(string scCheck)
{
	for (int i = 0; i < validSCNumber; i++)
	{
		if (scCheck == SCList[i])
		{
			return i;
		}
	}
	return -1;
} 
#endif

//change the interface to allow passing LegacyBuffers
int ReadValidSCCode(string directoryName, char * & h_validSCBuffer, LegacyBuffers& buffers)
{
	const string& filename = HardCodedLegacy::SCCodeFile(directoryName);
	buffers.validSCNumber = 0;
	ifstream infile;
	infile.open(filename.c_str());
	if (!infile.is_open())
	{
		printf("Cannot open %s file. Exit\n", filename.c_str());
		return 0;
	}

	string line;

	while (!infile.eof())
	{
		UniversalGetLine(infile, line);
		if (line.length() > 0 && line != ""){
			if (line.length() == VALID_SC_LEN) {
				buffers.validSCNumber++;
				buffers.SCList.push_back(line);
			}
			else {
				printf("Detected an SC <%s> with illegal length %d. Skip this SC\n", line.c_str(), line.length());
			}
		}
	}
	infile.close();

	h_validSCBuffer = new char[buffers.validSCNumber * (VALID_SC_LEN + 1)];// COMMENT
	// COMMENT What does this general section do?
	//Store all SC codes in 1 dimension  buffer
	for (int i = 0; i < buffers.validSCNumber; i++)
	{
		strcpy(&h_validSCBuffer[i * (VALID_SC_LEN + 1)], buffers.SCList[i].c_str());
	}

	return 1;
}


// ::: Read AHCA_Codes.txt file and store to variables
void readValidAHCA(string directoryName, char * & h_validAHCABuffer, char * & h_AHCAAssignList, int * & h_validAHCALength, 
	int * & h_validAHCAMutStart, int * & h_validAHCAMutEnd,
	int * & h_validAHCAMutation, int & validAHCANumber, int * & h_validAHCAType, map <string, string> & AHCA_map)  // XX: added the typelist
{
	const string& filename = HardCodedLegacy::AHCACodeFile(directoryName);
	validAHCANumber = 0;
	ifstream infile;
	infile.open(filename.c_str());
	if (!infile.is_open())
	{
		printf("Cannot open %s file. Exit\n", filename.c_str());
		exit(0);
	}

	string line;
	vector<string> AHCACodeList;

	vector<int> AHCACodeLengthList;		// length of the code
	vector<int> AHCACodeMutationList;	// mutation # allowed for this sequence
	vector<int> AHCACodeMutStartList;	// Absolute homology start
	vector<int> AHCACodeMutEndList;		// Absolute homology end
	vector<string> AHCAAssignList;			// What should be assigned.
	vector<int> AHCATypeList;		// XX:  Need to store the type of algorithm here for each AHCA ( Is it type A ("anti") or type B ("nucl")? )

	// COMMENT What does this general section do? // read AHCA line by line
	while (!infile.eof())
	{
		UniversalGetLine(infile, line);
		if (line.length() > 0 && line != ""){// COMMENT What does this general section do? //
			//Check line not null and split line to get AHCA key and value.
			//old => AACAACACC:TCRb
			//new => anti:TTGTGGACT(1):CD8a:TTGTGGACT
			// ::: Treat Strings
			int pos;
			pos = line.find(":");				// find the type

			string sType = line.substr(0, pos);   // XX:   WHY IS THIS NOT USED??  This should be used to determine whether A or B algorithm should be used later.
			int aType = -1;
			int typeAHCA = 0;

			typeAHCA = sType.find("anti");
			if (typeAHCA >= 0)
			{
				aType = 0;						// XX:  Stores whether the sequence is a anti-body or a nucl sequence
			}

			typeAHCA = sType.find("nucl");
			if (typeAHCA >= 0)
			{
				aType = 1;						// XX:  Stores whether the sequence is a anti-body or a nucl sequence
			}

			line = line.substr(pos + 1);
			pos = line.find(":");							// find the Sequence of the code for which to search

			string AHCA_Codes = line.substr(0, pos);	// xx:  Sequence of the code for which to search

			line = line.substr(pos + 1);
			pos = line.find(":");

			string tagName = line.substr(0, pos);			// What we call the marker if found
			string assignAHCA = line.substr(pos + 1);			//  The AHCA to be assigned if found

			pos = 0;
			line = AHCA_Codes;
			string AHCA_Code;
			int num_AHCA_Codes = 0;

			while (!(pos < 0))
			{
				pos = line.find("/");

				if (pos > 0){
					AHCA_Code = line.substr(0, pos);
					line = line.substr(pos + 1);
					num_AHCA_Codes++;
				}
				else{
					AHCA_Code = line;
				}

				int pos1 = AHCA_Code.find("(");
				int pos2 = AHCA_Code.find(")");
				int mutation = std::stoi(AHCA_Code.substr(pos1 + 1, pos2 - pos1 - 1));

				int pos3 = AHCA_Code.find("{");
				int pos4 = AHCA_Code.find("-");
				int pos5 = AHCA_Code.find("}");
				int noMutStart = -1;
				int noMutEnd = -1;
				if (pos3 > 0 && pos4 > 0 && pos5 > 0)
				{
					noMutStart = std::stoi(AHCA_Code.substr(pos3 + 1, pos4 - pos3 - 1));
					noMutEnd   = std::stoi(AHCA_Code.substr(pos4 + 1, pos5 - pos4 - 1));
					aType = 3;   // XX:  Stores that the sequence is a masked nucl sequence

				}


				AHCA_Code = AHCA_Code.substr(0, pos1);
				int len = pos1;

				// Store AHCA.
				if (assignAHCA.length() == VALID_AHCA_LEN) {
					validAHCANumber++;
					AHCAAssignList.push_back(assignAHCA);
					AHCATypeList.push_back(aType);			// Is this an anti (0) or a nucl (0)

					AHCA_map[assignAHCA] = tagName;
					AHCACodeList.push_back(AHCA_Code);    //  Store AHCA Code for which to search to vector
					AHCACodeLengthList.push_back(len);  // ::: Comment store the length of AHCA Code to vector
					AHCACodeMutationList.push_back(mutation);  // ::: Comment store the mutation AHCA Code to vector
					AHCACodeMutStartList.push_back(noMutStart);
					AHCACodeMutEndList.push_back(noMutEnd);



				}
			}
		}
	}
	infile.close();

	h_validAHCABuffer = new char[validAHCANumber * (VALID_AHCACODE_LEN + 1)];
	h_AHCAAssignList = new char[validAHCANumber * (VALID_AHCA_LEN + 1)];  //  xx:  need to store the algorithm type to use here to h_AHCATypeList
	h_validAHCALength = new int[validAHCANumber];
	h_validAHCAMutation = new int[validAHCANumber];
	h_validAHCAType = new int[validAHCANumber];  //  xx:  need to store the algorithm type to use here to h_AHCATypeList
	h_validAHCAMutStart = new int[validAHCANumber];
	h_validAHCAMutEnd = new int[validAHCANumber];

	for (int i = 0; i < validAHCANumber; i++)
	{
		strcpy(&h_validAHCABuffer[i * (VALID_AHCACODE_LEN + 1)], AHCACodeList[i].c_str());
		strcpy(&h_AHCAAssignList[i * (VALID_AHCA_LEN + 1)], AHCAAssignList[i].c_str());
		h_validAHCALength[i] = AHCACodeLengthList[i];
		h_validAHCAMutation[i] = AHCACodeMutationList[i];
		h_validAHCAType[i] = AHCATypeList[i];
		h_validAHCAMutStart[i] = AHCACodeMutStartList[i];
		h_validAHCAMutEnd[i] = AHCACodeMutEndList[i];
	}
}

void readOligos(string directoryName, char * & h_oligosBuffer, int * & h_oligosLength, int & oligosNumber)
{
	const string& filename = HardCodedLegacy::OligosFile(directoryName);
	oligosNumber = 0;
	ifstream infile;
	infile.open(filename.c_str());
	if (!infile.is_open())
	{
		printf("Cannot open %s file. Exit\n", filename.c_str());
		exit(0);
	}

	string line;
	vector<string> OligosList;
	// COMMENT What does this general section do? // Read Oligos file line by line ; for each line extract needed data (SC3:GCTCACCAAGTCCTAGT:blue:40 --extract--> GCTCACCAAGTCCTAGT )

	while (!infile.eof())
	{
		UniversalGetLine(infile, line);
		if (line.length() > 0 && line != ""){// COMMENT What does this general section do? //Split a line into 2 parts : key and value
			int pos = line.find(":");
			string trimline = line.substr(pos + 1, string::npos);// COMMENT What does this general section do? //get the key
			pos = trimline.find(":");
			string extractedLine = trimline.substr(0, pos);// COMMENT What does this general section do? // get the value
			if (extractedLine.length() <= MAX_TEMPLATE_LEN) {
				oligosNumber++;
				OligosList.push_back(extractedLine); // push the value to Oglios list.
			}
			else {
				printf("Detect a Oligos <%s> have lenght %d > %d . Skip this Oligos\n", extractedLine.c_str(), extractedLine.length(), MAX_TEMPLATE_LEN);
			}
		}
	}
	infile.close();

	h_oligosBuffer = new char[oligosNumber * (MAX_TEMPLATE_LEN + 1)];// COMMENT : 1 dimension array stores all Oligos data.
	h_oligosLength = new int[oligosNumber];
	for (int i = 0; i < oligosNumber; i++)
	{
		strcpy(&h_oligosBuffer[i * (MAX_TEMPLATE_LEN + 1)], OligosList[i].c_str());
		h_oligosLength[i] = OligosList[i].length();
	}
}

//Kernel for GPU, this code is executed in GPU.
__global__ void string_match_kernel(const int tidx, char * d_sequenceBuffer, int * d_sequenceLength, int sequenceCounter, 	int validSCNumber, 
	char * d_validSCBuffer, int validAHCANumber, char * d_validAHCABuffer, char * d_AHCAAssignList, int * d_validAHCALength, 
	int * d_validAHCAmutStart, int * d_validAHCAmutEnd,	int * d_validAHCAMutation, int * d_validAHCAType,
	TemplateLenInfo templateLenInfo, DetectionInfo * d_detectionInfo, char * d_resultBuffer, int AllowedEditNums,
	int * d_SCx_stat
	);

//	Process a part of input sequence. All detected sequence info stored in cellHash, and store junk and bad sequences in junkLines and badLines respectively
void workOutGPU(const bool saveBadLines, LegacyBuffers& buffers, vector<Cell> & cellHash, vector<string> & junkLines, vector<string> & badLines)
{
	cudaEvent_t start; // COMMENT : use for calculate elapsed time in GPU. start point and end point
	cudaEvent_t end; // COMMENT
	float timeValue; // COMMENT : elapsed time
	cudaEventCreate(&start); // COMMENT : create event in gpu.
	cudaEventCreate(&end); // COMMENT
	cudaEventRecord(start, 0); // COMMENT : record event start. (default at stream 0).
        cudaMemcpy(buffers.d_sequenceHeader, buffers.h_sequenceHeader, buffers.sequenceCounter * (MAX_SEQUENCE_LEN + 1) * sizeof(char), cudaMemcpyHostToDevice); // COMMENT : copy sequence header from host to device
	cudaMemcpy(buffers.d_sequenceBuffer, buffers.h_sequenceBuffer, buffers.sequenceCounter * (MAX_SEQUENCE_LEN + 1) * sizeof(char), cudaMemcpyHostToDevice); // COMMENT : copy sequence buffer from host to device
	cudaMemcpy(buffers.d_sequenceLength, buffers.h_sequenceLength, buffers.sequenceCounter * sizeof(int), cudaMemcpyHostToDevice); // COMMENT : copy sequence length buffer from host to device

	//dim3 grid((buffers.sequenceCounter - 1) / BLOCK_SIZE + 1); // COMMENT	: Create kernel lauching configuration
  int num_work_items = ((buffers.sequenceCounter - 1) / BLOCK_SIZE + 1) * BLOCK_SIZE;
#pragma omp parallel
{
  if (omp_get_thread_num() == 0) {
    std::cerr << "using " << omp_get_num_threads() << "threads\n";
  }
#pragma omp for
  for (int ii = 0; ii < num_work_items; ++ii) {
    string_match_kernel(ii, buffers.d_sequenceBuffer, buffers.d_sequenceLength, buffers.sequenceCounter, 
      buffers.validSCNumber, buffers.d_validSCBuffer, buffers.validAHCANumber, buffers.d_validAHCABuffer, buffers.d_AHCAAssignList, buffers.d_validAHCALength,
      buffers.d_validAHCAmutStart, buffers.d_validAHCAmutEnd,	buffers.d_validAHCAMutation, buffers.d_validAHCAType,
      buffers.templateLenInfo, buffers.d_detectionInfo, buffers.d_resultBuffer, buffers.allowedEdit, buffers.d_SCx_stat);
  }
}
#if 0
	string_match_kernel << <grid, BLOCK_SIZE >> >(buffers.d_sequenceBuffer, buffers.d_sequenceLength, buffers.sequenceCounter, // COMMENT : Lauch the kernel, which run on GPU.
		buffers.validSCNumber, buffers.d_validSCBuffer, buffers.validAHCANumber, buffers.d_validAHCABuffer, buffers.d_AHCAAssignList, buffers.d_validAHCALength,
		buffers.d_validAHCAmutStart, buffers.d_validAHCAmutEnd,	buffers.d_validAHCAMutation, buffers.d_validAHCAType,
		buffers.templateLenInfo, buffers.d_detectionInfo, buffers.d_resultBuffer, buffers.allowedEdit, buffers.d_SCx_stat);
#endif

	cudaDeviceSynchronize();// COMMENT : Synchronize the kernel with CPU. ( CPU wait until kernel completed)
	cudaMemcpy(buffers.h_detectionInfo, buffers.d_detectionInfo, buffers.sequenceCounter * sizeof(DetectionInfo), cudaMemcpyDeviceToHost);// COMMENT : copy result data from device to host
	cudaMemcpy(buffers.h_resultBuffer, buffers.d_resultBuffer, buffers.sequenceCounter * RESULT_SEGMENT_SIZE * sizeof(char), cudaMemcpyDeviceToHost);// COMMENT : copy result data from device to host
	cudaEventRecord(end, 0); // COMMENT : record event end.
	cudaEventSynchronize(end); // COMMENT : synchronize the event .
	cudaEventElapsedTime(&timeValue, start, end); // COMMENT calculate eclapsed tim base on start and end event.
	buffers.SInfo.gpuTime += timeValue;// COMMENT : add this elapsed time to final GPU eclapsed time
	time_t start1, end1;// COMMENT : use to calculate elapsed time on the CPU.
	time(&start1);// COMMENT : record time at this point into start1
	// COMMENT what does this section do?
	for (int i = 0; i < buffers.sequenceCounter; i++) // Go through all the sequences (on this part)
	{
		DetectionInfo detectInfo = buffers.h_detectionInfo[i];// COMMENT // get dectionInfo for ith sequence
                char * result = &buffers.h_resultBuffer[i * RESULT_SEGMENT_SIZE];
                string barcode = &result[(VALID_RANDOM_LEN + 1)];
                string ahca = &result[(VALID_RANDOM_LEN + 1) + 4 * (VALID_SC_LEN + 1)];
                string random = result;
                string removeMe;
                string errorcode;
                string header = &buffers.h_sequenceHeader[i * (MAX_SEQUENCE_LEN + 1)];
                string seq;
		if (detectInfo.sequenceType == TYPE_JUNK){// COMMENT : check if sequence is JUNK, then increase number of junkcodes.
			buffers.SInfo.junkCodes++;
                        int orderNum = junkLines.size() + 1;
                        char cnumber[20];
                        sprintf(cnumber, ">%8d|", orderNum);
                        string number = cnumber;
                        errorcode = &result[(VALID_RANDOM_LEN + 1) + 4 * (VALID_SC_LEN + 1) + (VALID_AHCA_LEN + 1)];
                        seq = &buffers.h_sequenceBuffer[i * (MAX_SEQUENCE_LEN + 1)];
                        string jL = number + barcode + "|" + ahca + "|" + random + "|" + errorcode + "|" + header + "\n" + seq + "\n";
                        junkLines.push_back(jL);
			continue;
		}
		else if (detectInfo.sequenceType == TYPE_FORWARD) {// COMMENT : check if sequence is forward code
			buffers.SInfo.forwardCodes++;
		}
		else // or backward code
		{
			buffers.SInfo.backwardCodes++;
		}

		buffers.SInfo.badSC1 += detectInfo.badSC1;
		buffers.SInfo.badSC2 += detectInfo.badSC2;
		buffers.SInfo.badSCN += detectInfo.badSCN;
		buffers.SInfo.badSC3 += detectInfo.badSC3;
		buffers.SInfo.badSC1xxxx += detectInfo.badSC1xxxx;
		buffers.SInfo.badSC2xxxx += detectInfo.badSC2xxxx;
		buffers.SInfo.badSCNxxxx += detectInfo.badSCNxxxx;
		buffers.SInfo.badSC3xxxx += detectInfo.badSC3xxxx;
		buffers.SInfo.RCerror += detectInfo.RCerror;
		buffers.SInfo.mutationRate += detectInfo.mutationRate;
		buffers.SInfo.totalDNARead += detectInfo.totalDNARead;
		if (detectInfo.badSC1 + detectInfo.badSC2 + detectInfo.badSCN + detectInfo.badSC3 > 0) {
			buffers.SInfo.ErrorInSC++;
		}
                buffers.SInfo.mutationRate_SC3 += detectInfo.mutationRate_SC3;
                buffers.SInfo.mutationRate_SC2 += detectInfo.mutationRate_SC2;
                buffers.SInfo.mutationRate_SCN += detectInfo.mutationRate_SCN;
                buffers.SInfo.mutationRate_SC1 += detectInfo.mutationRate_SC1;
                buffers.SInfo.mutationRate_AHCA += detectInfo.mutationRate_AHCA;
                buffers.SInfo.totalDNARead_SC3 += detectInfo.totalDNARead_SC3;
                buffers.SInfo.totalDNARead_SC2 += detectInfo.totalDNARead_SC2;
                buffers.SInfo.totalDNARead_SCN += detectInfo.totalDNARead_SCN;
                buffers.SInfo.totalDNARead_SC1 += detectInfo.totalDNARead_SC1;
                buffers.SInfo.totalDNARead_AHCA += detectInfo.totalDNARead_AHCA;

		//printf("%s, %d\n",result,detectInfo.errorCount);
		if (detectInfo.errorCount == 0) // If sequence is valid
		{
			try
			{
				Cell tempCell(header, barcode, ahca, random, removeMe);
				cellHash.push_back(tempCell);
			}
			catch (...)
			{
				printf("Add cell failed\n");
			}
		}
		else // Badlines
		{
			if (saveBadLines)
			{
				int orderNum = badLines.size() + 1;
				char cnumber[20];
				sprintf(cnumber, ">%8d|", orderNum);
				string number = cnumber;

				errorcode = &result[(VALID_RANDOM_LEN + 1) + 4 * (VALID_SC_LEN + 1) + (VALID_AHCA_LEN + 1)];

				seq = &buffers.h_sequenceBuffer[i * (MAX_SEQUENCE_LEN + 1)];

                                string bL = number + barcode + "|" + ahca + "|" + random + "|" + errorcode + "|" + header + "\n" + seq + "\n";

				badLines.push_back(bL);
			}
			buffers.SInfo.totalBadCodes++;
		}
	}
	time(&end1);
	double dif = difftime(end1, start1);
	buffers.SInfo.subtractTime += (float)dif;
}

__global__ void string_match_kernel(const int tidx, char * d_sequenceBuffer, int * d_sequenceLength, int sequenceCounter, 	int validSCNumber, 
	char * d_validSCBuffer, int validAHCANumber, char * d_validAHCABuffer, char * d_AHCAAssignList, int * d_validAHCALength, 
	int * d_validAHCAmutStart, int * d_validAHCAmutEnd,	int * d_validAHCAMutation, int * d_validAHCAType,
	TemplateLenInfo templateLenInfo, DetectionInfo * d_detectionInfo, char * d_resultBuffer, int AllowedEditNums, int * d_SCx_stat)
{
        int mutationRate_SC3 = 0;
        int mutationRate_SC2 = 0;
        int mutationRate_SCN = 0;
        int mutationRate_SC1 = 0;
        int mutationRate_AHCA = 0;
        double totalDNARead_SC3 = 0.0;
        double totalDNARead_SC2 = 0.0;
        double totalDNARead_SCN = 0.0;
        double totalDNARead_SC1 = 0.0;
        double totalDNARead_AHCA = 0.0;

	//int tidx = blockIdx.x * blockDim.x + threadIdx.x;// COMMENT : get index of the thread
        //there is probably a BUG associated with the following declaration
	int dist[VALID_AHCACODE_LEN + 1][VALID_AHCACODE_LEN + 1]; // This use for Levenshtein both Barcode and AHCA
	if (tidx < sequenceCounter) // only work with thread index < number of sequence
	{
		char * sequence = &d_sequenceBuffer[tidx * (MAX_SEQUENCE_LEN + 1)];// COMMENT : get the first address of input sequence at position of index.
		int sequenceLength = d_sequenceLength[tidx];// COMMENT : get the length of sequence at this position

		DetectionInfo * detectionInfo = &d_detectionInfo[tidx];// COMMENT : get detectionInfo address, we will store our addtinal info in this.
		detectionInfo->sequenceType = TYPE_JUNK;// COMMENT : At beginning , a sequence is JUNK. we will check if it not JUNK later and change its type.
		bool isForwardSequence = false;

#pragma region Variables
		detectionInfo->badSC1 = 0;
		detectionInfo->badSC2 = 0;
		detectionInfo->badSC3 = 0;
		detectionInfo->badSCN = 0;
		detectionInfo->badSC1xxxx = 0;
		detectionInfo->badSC2xxxx = 0;
		detectionInfo->badSC3xxxx = 0;
		detectionInfo->badSCNxxxx = 0;
		detectionInfo->RCerror = 0;
		detectionInfo->mutationRate = 0;
		detectionInfo->totalDNARead = 0.0;
                detectionInfo->mutationRate_SC3 = 0;
                detectionInfo->mutationRate_SC2 = 0;
                detectionInfo->mutationRate_SCN = 0;
                detectionInfo->mutationRate_SC1 = 0;
                detectionInfo->mutationRate_AHCA = 0;
                detectionInfo->totalDNARead_SC3 = 0.0;
                detectionInfo->totalDNARead_SC2 = 0.0;
                detectionInfo->totalDNARead_SCN = 0.0;
                detectionInfo->totalDNARead_SC1 = 0.0;
                detectionInfo->totalDNARead_AHCA = 0.0;
		int mutationRate = 0;
		double totalDNARead = 0.0;
		int errorOffset = 0;
		int forwardOffset = 0; // used for tracing the last offset where a pattern is found, start at 0
		int i = forwardOffset;

		int errorCount = 0;

#pragma endregion

		// These variables point to addresses which store output results
		char * detectedRandom = &d_resultBuffer[tidx * RESULT_SEGMENT_SIZE];
		char * detectedBarcode1 = &d_resultBuffer[tidx * RESULT_SEGMENT_SIZE + (VALID_RANDOM_LEN + 1)];
		char * detectedAHCA = &d_resultBuffer[tidx * RESULT_SEGMENT_SIZE + (VALID_RANDOM_LEN + 1) + 4 * (VALID_SC_LEN + 1)];
		char * detectedError = &d_resultBuffer[tidx * RESULT_SEGMENT_SIZE + (VALID_RANDOM_LEN + 1) + 4 * (VALID_SC_LEN + 1) + (VALID_AHCACODE_LEN + 1)];

#pragma region SC3, Random Code and reverse complement as needed
		//++++++++ Find Random code and SC3 +++++++++//
		bool found = false;
		int backwardsChecked = 0;
		int correctStart = 0;
		while (backwardsChecked < 2 && !found)
		{
			backwardsChecked++;

			while (i < sequenceLength - templateLenInfo.SC3 && !found)// COMMENT : loop over the sequence
			{
				int offset = 0;
				int mutationNum = 0;
				while (mutationNum <= 2 && offset < templateLenInfo.SC3) // COMMENT : check if we can found SC3 at i position
				{
					if (sequence[i + offset] != cuda_constants::SC3[offset])
					{
						mutationNum++;
					}
					offset++;
				}

                                // Check if SC3 found at current offset i
                                if (offset == templateLenInfo.SC3 && mutationNum <= 2)
				{
					mutationRate += mutationNum;
					totalDNARead += (double)offset;
					isForwardSequence = true;
					backwardsChecked = 3;
					found = true;
					int startOffset = i - 17;  //? 17 by Un

					if (startOffset < 0)
					{
						startOffset += sequenceLength - 1;
					}
					forwardOffset = i; // save this offset
					// Check if Random sequence have enough length
					if (sequenceLength - startOffset < VALID_RANDOM_LEN)
					{
						detectedError[errorOffset++] = 'R';
						detectedError[errorOffset++] = 'C';
						for (int c = 0; c < sequenceLength - startOffset; c++)
						{
							detectedRandom[c] = sequence[startOffset + c];
						}
						for (int c = sequenceLength - startOffset; c < VALID_RANDOM_LEN; c++)
						{
							detectedRandom[c] = 'x';
						}
						detectedRandom[VALID_RANDOM_LEN] = '\0';
						errorCount++;
						detectionInfo->RCerror = 1;
					}
					else // if Random sequence have enough length, copy it to the result
					{
						for (int c = 0; c < VALID_RANDOM_LEN; c++)
						{
							detectedRandom[c] = sequence[startOffset + c];
						}
						detectedRandom[VALID_RANDOM_LEN] = '\0';
					}
				}
				i++;
			}

			if (!found && backwardsChecked == 1)		// reverse the sequence and check again
			{
				i = forwardOffset;
				char reverseseq[512];
				int j = 0;
				//printf("%d\n", sequenceLength);
				for (int scan = sequenceLength - 1; scan >= 0; scan--)
				{
					char a = sequence[scan];
					//printf("%c", a);
          reverseseq[j] = a; // this is a one-line minimum fix for the legacy code

					if (a == 'A') reverseseq[j] = 'T';
					if (a == 'T') reverseseq[j] = 'A';
					if (a == 'G') reverseseq[j] = 'C';
					if (a == 'C') reverseseq[j] = 'G';
					j++;
				}
				int k = forwardOffset;
				while (k < sequenceLength)
				{
					sequence[k] = reverseseq[k];
					k++;
				}
			}
		}

		if (!found) // In case can't find SC3 (2 possible mutation) in the sequence
		{
			detectedError[errorOffset++] = 'R';
			detectedError[errorOffset++] = 'C';
			for (int c = 0; c < VALID_RANDOM_LEN; c++)
			{
				detectedRandom[c] = 'x';
			}
			detectedRandom[VALID_RANDOM_LEN] = '\0';
			errorCount++;
			detectionInfo->RCerror = 1;
		}

		//++++++++++ Find 1st barcode ++++++++++++//
		found = false;
		i = forwardOffset; // start at saved offset
		int index;
		while (i < sequenceLength - templateLenInfo.SC3 && !found)
		{
			int offset = 0;
			int mutationNum = 0;
			while (mutationNum <= 2 && offset < templateLenInfo.SC3)
			{
				if (sequence[i + offset] != cuda_constants::SC3[offset])
				{
					mutationNum++;
				}
				offset = offset + 1;
			}
			// Check if SC3 found at current offset i
			if (offset == templateLenInfo.SC3 && mutationNum <= 2)
			{
                                // S3 anchor has been found for the second time
                                // Do not double count mutationNum and offset
				//mutationRate += mutationNum;
				//totalDNARead += offset;
				found = true;
				forwardOffset = i;
				index = i - 7;
			}
			i++;
		}

		if (found) {
			isForwardSequence = true;
			int scId = 0;
			int startOffset = index;

			if (sequenceLength - startOffset < VALID_SC_LEN) // In case the 1st barcode does not have enough length
			{
				//detectedError[errorOffset++] = '.';
				detectedError[errorOffset++] = '3';
				for (int c = 0; c < sequenceLength - startOffset; c++)
				{
					detectedBarcode1[c] = sequence[startOffset + c];;
				}
				for (int c = sequenceLength - startOffset; c < VALID_SC_LEN; c++)
				{
					detectedBarcode1[c] = 'x';
				}
				detectedBarcode1[VALID_SC_LEN] = ' ';
				errorCount++;
				detectionInfo->badSC3 = 1;
				detectionInfo->badSC3xxxx = 1;
			}
			else // Enough length, check if the 1st barcode is valid
			{
				int BarCodeValid = BARCODE_NOTVALID;
				while (BarCodeValid != BARCODE_VALID && scId < validSCNumber)
				{
					char * SC = &d_validSCBuffer[scId * (VALID_SC_LEN + 1)];
					int count = 0;

					for (count = 0; count < VALID_SC_LEN; count++)
					{
						if (sequence[startOffset + count] != SC[count])
						{
							break;
						}
					}

					if (count == VALID_SC_LEN)
					{
						BarCodeValid = BARCODE_VALID;

						atomicAdd(&d_SCx_stat[scId * 4 + 0], 1);
                                                totalDNARead_SC3 += (double)count;
					}
					scId++;
				}
				//++++++ Try to fix the barcode part++++++//

				if (BarCodeValid != BARCODE_VALID && AllowedEditNums > 0)
				{
					if (AllowedEditNums == 1) // Use my optimize fix
					{
						scId = 0;
						while (BarCodeValid != BARCODE_VALID && scId < validSCNumber)
						{
							char * SC = &d_validSCBuffer[scId * (VALID_SC_LEN + 1)];
							int count = 0;
							int mutation = 0;
							while (count < VALID_SC_LEN)
							{
								if (sequence[startOffset + count] != SC[count])
								{
									mutation++;
									if (mutation > 1)
									{
										break;
									}
								}
								count++;
							}

							if (count == VALID_SC_LEN) // Found candiate in list that could be similar
							{
								BarCodeValid = BARCODE_VALID;
								for (int kk = 0; kk < VALID_SC_LEN; kk++) // fix the sequence
								{
									sequence[startOffset + kk] = SC[kk];
								}
								atomicAdd(&d_SCx_stat[scId * 4 + 0], 1);
                                                                mutationRate_SC3 += mutation;
                                                                totalDNARead_SC3 += (double)count;
							}
							scId++;
						}
					}
					else // Use Levenshein Distance fix
					{
						scId = 0;
            // take out gpu thread idx
						//int tid = threadIdx.x;
						while (BarCodeValid != BARCODE_VALID && scId < validSCNumber)
						{
							char * SC = &d_validSCBuffer[scId * (VALID_SC_LEN + 1)];

							dist[0][0] = 0;
							for (unsigned int ii = 1; ii <= VALID_SC_LEN; ++ii)
							{
								dist[ii][0] = ii;
							}
							for (unsigned int ii = 1; ii <= VALID_SC_LEN; ++ii)
							{
								dist[0][ii] = ii;
							}

							for (unsigned int ii = 1; ii <= VALID_SC_LEN; ++ii){
								for (unsigned int jj = 1; jj <= VALID_SC_LEN; ++jj)
								{
									dist[ii][jj] = min(min(dist[ii - 1][jj] + 1, dist[ii][jj - 1] + 1),
										dist[ii - 1][jj - 1] + (sequence[startOffset + ii - 1] == SC[jj - 1] ? 0 : 1));
								}
							}
							if (dist[VALID_SC_LEN][VALID_SC_LEN] <= AllowedEditNums)
							{
								BarCodeValid = BARCODE_VALID;
								for (int kk = 0; kk < VALID_SC_LEN; kk++) // fix the sequence
								{
									sequence[startOffset + kk] = SC[kk];
								}
								atomicAdd(&d_SCx_stat[scId * 4 + 0], 1);
                                                                mutationRate_SC3 += dist[VALID_SC_LEN][VALID_SC_LEN];
                                                                totalDNARead_SC3 += (double)VALID_SC_LEN;
							}

							scId++;
						}
					}
				}

				//+++++++ End fix part+++++++++ //

				if (BarCodeValid != BARCODE_VALID) // Barcode not in list, mark it as error
				{
					//detectedError[errorOffset++] = '.';
					detectedError[errorOffset++] = '3';
					errorCount++;
					detectionInfo->badSC3 = 1;
				}

				for (int c = 0; c < VALID_SC_LEN; c++) // Copy it this barcode to output buffer
				{
					detectedBarcode1[c] = sequence[startOffset + c];
				}
				detectedBarcode1[VALID_SC_LEN] = ' ';
			}
		}
		else // In case can not find SC3 (with 1 possible mutation)
		{
			//detectedError[errorOffset++] = '.';
			detectedError[errorOffset++] = '3';
			for (int c = 0; c < VALID_SC_LEN; c++)
			{
				detectedBarcode1[c] = 'x';
			}
			detectedBarcode1[VALID_SC_LEN] = ' ';
			errorCount++;
			detectionInfo->badSC3 = 1;
			detectionInfo->badSC3xxxx = 1;
		}

#pragma endregion

#pragma region SC2, SCN, SC1
		//++++++++ Find 2nd - 4th Barcodes++++++++++++//
		for (int temp = 0; temp < 3; temp++)
		{
			int foundL = false;
			int foundR = false;
			int indexL = MAX_SEQUENCE_LEN;
			int indexR = MAX_SEQUENCE_LEN;

			forwardOffset += 8;
			i = forwardOffset;
			char * detectedBarcode2_4 = &d_resultBuffer[tidx * RESULT_SEGMENT_SIZE + (VALID_RANDOM_LEN + 1) + (temp + 1) *(VALID_SC_LEN + 1)];

			while (i < sequenceLength - templateLenInfo.SC[temp * 2] && !foundL)
			{
				int offset = 0;
				int mutationNum = 0;
				while (mutationNum <= 1 && offset < templateLenInfo.SC[temp * 2])
				{
					if (sequence[i + offset] != cuda_constants::SC[(temp * 2) * MAX_TEMPLATE_LEN + offset])
					{
						mutationNum++;
					}
					offset = offset + 1;
				}

				if (offset == templateLenInfo.SC[temp * 2] && mutationNum <= 1) // Check if the left pattern is found
				{
					mutationRate += mutationNum;
					totalDNARead += (double)offset;
					foundL = true;
					forwardOffset = i;
					indexL = i;
				}
				i++;
			}
			forwardOffset += 14;
			i = forwardOffset;
			while (i < sequenceLength - templateLenInfo.SC[temp * 2 + 1] && !foundR)
			{
				int offset = 0;
				int mutationNum = 0;
				while (mutationNum <= 1 && offset < templateLenInfo.SC[temp * 2 + 1])
				{
					if (sequence[i + offset] != cuda_constants::SC[(temp * 2 + 1) * MAX_TEMPLATE_LEN + offset])
					{
						mutationNum++;
					}
					offset = offset + 1;
				}

				if (offset == templateLenInfo.SC[temp * 2 + 1] && mutationNum <= 1) // Check if the right pattern is found
				{
					mutationRate += mutationNum;
					totalDNARead += (double)offset;
					foundR = true;
					forwardOffset = i;
					indexR = i;
				}
				i++;
			}

			if (foundL && foundR) // Found the barcode both left and right side;
			{
				int startOffset = indexL + templateLenInfo.SC[temp * 2];
				int length = indexR - startOffset;
				isForwardSequence = true;
				// Check barcode if barcode valid;
				if (length == VALID_SC_LEN)
				{
					int BarCodeValid = BARCODE_NOTVALID;
					int scId = 0;
					while (BarCodeValid != BARCODE_VALID && scId < validSCNumber)
					{
						char * SC = &d_validSCBuffer[scId * (VALID_SC_LEN + 1)];
						int count = 0;

						for (count = 0; count < VALID_SC_LEN; count++)
						{
							if (sequence[startOffset + count] != SC[count])
							{
								break;
							}
						}

						if (count == VALID_SC_LEN)
						{
							BarCodeValid = BARCODE_VALID;
							atomicAdd(&d_SCx_stat[scId * 4 + 1 + temp], 1);
                                                        if (temp == 0){ totalDNARead_SC2 += (double)count; }
                                                        else if (temp == 1){ totalDNARead_SCN += (double)count; }
                                                        else if (temp == 2){ totalDNARead_SC1 += (double)count; }
						}
						scId++;
					}

					//++++++ Try to fix the barcode part++++++//

					if (BarCodeValid != BARCODE_VALID && AllowedEditNums > 0)
					{
						if (AllowedEditNums == 1) // Use my optimize fix
						{
							scId = 0;
							while (BarCodeValid != BARCODE_VALID && scId < validSCNumber)
							{
								char * SC = &d_validSCBuffer[scId * (VALID_SC_LEN + 1)];
								int count = 0;
								int mutation = 0;
								while (count < VALID_SC_LEN)
								{
									if (sequence[startOffset + count] != SC[count])
									{
										mutation++;
										if (mutation > 1)
										{
											break;
										}
									}
									count++;
								}

								if (count == VALID_SC_LEN) // Found candiate in list that could be similar
								{
									BarCodeValid = BARCODE_VALID;
									for (int kk = 0; kk < VALID_SC_LEN; kk++) // fix the sequence
									{
										sequence[startOffset + kk] = SC[kk];
									}
									atomicAdd(&d_SCx_stat[scId * 4 + 1 + temp], 1);
                                                                        if (temp == 0){ mutationRate_SC2 += mutation; totalDNARead_SC2 += (double)count; }
                                                                        else if (temp == 1){ mutationRate_SCN += mutation; totalDNARead_SCN += (double)count; }
                                                                        else if (temp == 2){ mutationRate_SC1 += mutation; totalDNARead_SC1 += (double)count; }
                                                                        else{ printf("ERROR implementing mutationRate_SCx\n"); exit(EXIT_SUCCESS); }
								}
								scId++;
							}
						}
						else // Use Levenshein Distance fix
						{
							scId = 0;
              //take out gpu thread idx
							//int tid = threadIdx.x;
							while (BarCodeValid != BARCODE_VALID && scId < validSCNumber)
							{
								char * SC = &d_validSCBuffer[scId * (VALID_SC_LEN + 1)];

								dist[0][0] = 0;
								for (unsigned int ii = 1; ii <= VALID_SC_LEN; ++ii)
								{
									dist[ii][0] = ii;
								}
								for (unsigned int ii = 1; ii <= VALID_SC_LEN; ++ii)
								{
									dist[0][ii] = ii;
								}

								for (unsigned int ii = 1; ii <= VALID_SC_LEN; ++ii){
									for (unsigned int jj = 1; jj <= VALID_SC_LEN; ++jj)
									{
										dist[ii][jj] = min(min(dist[ii - 1][jj] + 1, dist[ii][jj - 1] + 1),
											dist[ii - 1][jj - 1] + (sequence[startOffset + ii - 1] == SC[jj - 1] ? 0 : 1));
									}
								}
								if (dist[VALID_SC_LEN][VALID_SC_LEN] <= AllowedEditNums)
								{
									BarCodeValid = BARCODE_VALID;
									for (int kk = 0; kk < VALID_SC_LEN; kk++) // fix the sequence
									{
										sequence[startOffset + kk] = SC[kk];
									}
									atomicAdd(&d_SCx_stat[scId * 4 + 1 + temp], 1);
                                                                        if (temp == 0){ mutationRate_SC2 += dist[VALID_SC_LEN][VALID_SC_LEN]; totalDNARead_SC2 += (double)VALID_SC_LEN; }
                                                                        else if (temp == 1){ mutationRate_SCN += dist[VALID_SC_LEN][VALID_SC_LEN]; totalDNARead_SCN += (double)VALID_SC_LEN; }
                                                                        else if (temp == 2){ mutationRate_SC1 += dist[VALID_SC_LEN][VALID_SC_LEN]; totalDNARead_SC1 += (double)VALID_SC_LEN; }
                                                                        else{ printf("ERROR implementing mutationRate_SCx in Levenshtein corr\n"); exit(EXIT_SUCCESS); }
								}

								scId++;
							}
						}
					}

					//+++++++ End fix part+++++++++ //

					if (BarCodeValid != BARCODE_VALID) // the barcode is not in list, mark it as error code
					{
						//detectedError[errorOffset++] = '.';
						detectedError[errorOffset++] = cuda_constants::ERROR_MARK[temp]; // different mark for each barcode
						errorCount++;
						if (temp == 0){
							detectionInfo->badSC2 = 1;
						}
						else if (temp == 1) {
							detectionInfo->badSCN = 1;
						}
						else
						{
							detectionInfo->badSC1 = 1;
						}
					}

					for (int c = 0; c < VALID_SC_LEN; c++) // copy the barcode to output buffer
					{
						detectedBarcode2_4[c] = sequence[startOffset + c];
					}
					detectedBarcode2_4[VALID_SC_LEN] = ' ';
				}
				else if (length < VALID_SC_LEN)// Barcode not enough the length
				{
					//detectedError[errorOffset++] = '.';
					detectedError[errorOffset++] = cuda_constants::ERROR_MARK[temp];

					for (int c = 0; c < length; c++)
					{
						detectedBarcode2_4[c] = sequence[startOffset + c];
					}
					for (int c = length; c < VALID_SC_LEN; c++)
					{
						detectedBarcode2_4[c] = 'x';
					}
					detectedBarcode2_4[VALID_SC_LEN] = ' ';
					errorCount++;
					if (temp == 0){
						detectionInfo->badSC2 = 1;
						detectionInfo->badSC2xxxx = 1;
					}
					else if (temp == 1) {
						detectionInfo->badSCN = 1;
						detectionInfo->badSCNxxxx = 1;
					}
					else
					{
						detectionInfo->badSC1 = 1;
						detectionInfo->badSC1xxxx = 1;
					}
				}
				else { // lenght > VALID_SC_LEN
					//detectedError[errorOffset++] = '.';
					detectedError[errorOffset++] = cuda_constants::ERROR_MARK[temp];

					for (int c = 0; c < VALID_SC_LEN; c++)
					{
						detectedBarcode2_4[c] = 'x';
					}
					detectedBarcode2_4[VALID_SC_LEN] = ' ';
					errorCount++;
					if (temp == 0){
						detectionInfo->badSC2 = 1;
						detectionInfo->badSC2xxxx = 1;
					}
					else if (temp == 1) {
						detectionInfo->badSCN = 1;
						detectionInfo->badSCNxxxx = 1;
					}
					else
					{
						detectionInfo->badSC1 = 1;
						detectionInfo->badSC1xxxx = 1;
					}
				}
			}
			else // Can not find both left and right pattern
			{
				//detectedError[errorOffset++] = '.';
				detectedError[errorOffset++] = cuda_constants::ERROR_MARK[temp];
				errorCount++;

				if (foundL) // Found only left side
				{
					for (int c = 0; c < VALID_SC_LEN; c++)
					{
						detectedBarcode2_4[c] = cuda_constants::SC_MISS_RIGHT[c];
					}
					detectedBarcode2_4[VALID_SC_LEN] = ' ';
				}
				else if (foundR) // Found only right side
				{
					for (int c = 0; c < VALID_SC_LEN; c++)
					{
						detectedBarcode2_4[c] = cuda_constants::SC_MISS_LEFT[c];
					}
					detectedBarcode2_4[VALID_SC_LEN] = ' ';
				}
				else // Not found any thing
				{
					for (int c = 0; c < VALID_SC_LEN; c++)
					{
						detectedBarcode2_4[c] = cuda_constants::SC_MISS_BOTH[c];
					}
					detectedBarcode2_4[VALID_SC_LEN] = ' ';
				}
				if (temp == 0){
					detectionInfo->badSC2 = 1;
					detectionInfo->badSC2xxxx = 1;
				}
				else if (temp == 1) {
					detectionInfo->badSCN = 1;
					detectionInfo->badSCNxxxx = 1;
				}
				else
				{
					detectionInfo->badSC1 = 1;
					detectionInfo->badSC1xxxx = 1;
				}
			}
		}// End for (int temp....)

		d_resultBuffer[tidx * RESULT_SEGMENT_SIZE + (VALID_RANDOM_LEN + 1) + 4 * (VALID_SC_LEN + 1) - 1] = '\0'; // and a mark for end all barcodes


#pragma endregion

#pragma region AHCA

		//+++++++++ Find AHCA code++++++++++++++++++//
		int foundPre_Anneal = false;
		int indexAHCA = MAX_SEQUENCE_LEN;

		// Step through the sequence and determine if sequence matches with <= 1
		// XX:   Only uses pre-anneal...  though it should be like SC 2, N, and 1 where it looks for "left" (pre-anneal) and "right" (post-aneal).
		// XX:  Find pre_anneal.
		i = forwardOffset;
		while (i <= sequenceLength - templateLenInfo.Anneal_Pre && !foundPre_Anneal)
		{
			int offset = 0;
			int mutationNum = 0;

			while (mutationNum <= 2 && offset < templateLenInfo.Anneal_Pre)
			{
				if (sequence[i + offset] != cuda_constants::Anneal_Pre[offset])
				{
					mutationNum++;
				}
				offset = offset + 1;
			}

			if (offset == templateLenInfo.Anneal_Pre && mutationNum <= 2 && (i - 1 + templateLenInfo.Anneal_Pre + 9) <= sequenceLength)
			{
				mutationRate += mutationNum;
				totalDNARead += (double)offset;
				foundPre_Anneal = true;
				indexAHCA = i + templateLenInfo.Anneal_Pre;  // set indexAHCA to the end of the found sequence.
			}
			i++;
		}

		//  XX::  If found pre_anneal, should start +9 from end of pre_anneal to see if POST_anneal exists.
		int foundPost_Anneal = false;
		int j = indexAHCA + VALID_AHCA_LEN;
		int offset = 0;
		int mutationNum = 0;

		if (j <= sequenceLength - templateLenInfo.Anneal_Post)
		{
			while (mutationNum <= 2 && offset < templateLenInfo.Anneal_Post)
			{
				if (sequence[j + offset] != cuda_constants::Anneal_Post[offset])
				{
					mutationNum++;
				}
				offset = offset + 1;
			}

			if (offset == templateLenInfo.Anneal_Post && mutationNum <= 2 && (j - 1 + templateLenInfo.Anneal_Post) <= sequenceLength)
			{
				foundPost_Anneal = true;
			}
		}

		///////////////////////////////////////////////////////////////
		// ::: Implementation of the finding algorithm for type A (AHCA code) and B (nucleic acid sequence).////
		///////////////////////////////////////////////////////////////

		if (foundPre_Anneal)
		{
			int startOffset = indexAHCA;
			int AHCAValid = AHCA_NOTVALID;
			int AHCADistance = MAX_AHCA_DIST;

			int ahcaId = 0;
			int errorCount0 = errorCount;
			int errorOffset0 = errorOffset;
			int len;

			// ::: proceed at the startoffset
			int len_to_end_of_seq = sequenceLength - startOffset;

			while (startOffset < sequenceLength && len_to_end_of_seq >= VALID_AHCA_LEN) // BUG: startOffset < MAX_SEQUENCE_LEN
			{
				if (foundPost_Anneal && startOffset > indexAHCA + 1)
					break;

				errorCount = errorCount0;
				errorOffset = errorOffset0;

				// Iterate through the AHCA allowed list to check;
				AHCAValid = AHCA_NOTVALID;
				ahcaId = 0;
				int AHCAType = 0;
				// First check for maximum identity.
				while (AHCAValid != AHCA_VALID && ahcaId < validAHCANumber)
				{
					AHCAType = d_validAHCAType[ahcaId];																// This is the AHCA algorithm type
					if (AHCAType == 0 && (startOffset > indexAHCA + 1 || !foundPost_Anneal))						// Don't search for an antibody after the first position of the AHCA, or if post_anneal not found
					{
						ahcaId++;
						continue;
					}

					char * AHCA = &d_validAHCABuffer[ahcaId * (VALID_AHCACODE_LEN + 1)];							// Sequence for which we are searching
					char * AHCAAssign = &d_AHCAAssignList[ahcaId * (VALID_AHCA_LEN + 1)];							// Sequence we will assign if found
					len = d_validAHCALength[ahcaId];																// * sizeof(int)));

					int count = 0;
					for (count = 0; count < len; count++)
					{
						if (sequence[startOffset + count] != AHCA[count])				// Copy in the assigned sequence
						{
							break;
						}
					}

					if (count == len)
					{
						AHCAValid = AHCA_VALID;
						for (int kk = 0; kk < VALID_AHCA_LEN; kk++) // correct the sequence by copying the search sequence to the "found" sequence
						{
							sequence[startOffset + kk] = AHCAAssign[kk];
						}
                                                totalDNARead_AHCA += (double)count;
						continue;
						//printf("1->%s, %d, %d\n", AHCA, len, AHCACodemutation);
					}
					ahcaId++;
				}

				//++++++ AHCA was not identical, see if we can find with errors, and then for type A try to fix the AHCA ++++++//
				AHCAType = 0;
				AHCADistance = MAX_AHCA_DIST; //
				if (AHCAValid != AHCA_VALID) {
					if (AllowedEditNums == 1) // Use optimized fix (THIS WILL ALWAYS BE 1 for SC)
					{
						int ahcaId = 0;
						while (AHCAValid != AHCA_VALID && ahcaId < validAHCANumber)
						{
							AHCAType = d_validAHCAType[ahcaId];													// This is the AHCA algorithm type
							if (AHCAType == 0 && (startOffset > indexAHCA + 1 || !foundPost_Anneal))			// Don't search for an antibody after the first position of the AHCA, or if post_anneal not found
							{
								ahcaId++;
								continue;
							}

							char * AHCA = &d_validAHCABuffer[ahcaId * (VALID_AHCACODE_LEN + 1)];
							len = d_validAHCALength[ahcaId];

							int AHCACodemutation = d_validAHCAMutation[ahcaId];
							char * AHCAAssign = &d_AHCAAssignList[ahcaId * (VALID_AHCA_LEN + 1)];	// Sequence for which we are searching


							int count = 0;
							int mutation = 0;

							if (AHCAType == 3)			// this is a special sequence with masked region requiring absolute identity.
							{
								int mutStart = d_validAHCAmutStart[ahcaId];		// get the start position
								int mutEnd = d_validAHCAmutEnd[ahcaId];			// get the end   position

								while (count < len)
								{
									if (sequence[startOffset + count] != AHCA[count])		// check for identity
									{
										if (count >= mutStart && count <= mutEnd )			// not identical-- check if within identity required region
										{	break;	}

										mutation++;
										if (mutation > AHCACodemutation)
										{	break;	}
									}
									count++;
								}
							}
							else
							{
								while (count < len)
								{
									if (sequence[startOffset + count] != AHCA[count])
									{
										mutation++;
										if (mutation > AHCACodemutation)
										{	break;	}
									}
									count++;
								}
							}

							if (count == len) // Found candidate in list that could be similar
							{
								AHCAValid = AHCA_VALID;
								//printf("2->%s, %d, %d\n", AHCA, len, AHCACodemutation);
								for (int kk = 0; kk < VALID_AHCA_LEN; kk++) // correct the sequence by copying the search sequence to the "found" sequence
								{
									sequence[startOffset + kk] = AHCAAssign[kk];
								}
								AHCADistance = 1;
                                                                mutationRate_AHCA += mutation;
                                                                totalDNARead_AHCA += (double)count;
							}
							ahcaId++;
						}
					}

					else if (AHCAType == 0)                    // Use Levenshtein Distance fix only on Anti (type A)
					{
						ahcaId = 0;
            //take out gpu thread
						//unsigned int tid = threadIdx.x;
						while (AHCAValid != AHCA_VALID && ahcaId < validAHCANumber)
						{
							len = d_validAHCALength[ahcaId];
							int AHCACodemutation = d_validAHCAMutation[ahcaId];
							char * AHCAAssign = &d_AHCAAssignList[ahcaId * (VALID_AHCA_LEN + 1)];	// Sequence for which we are searching

							dist[0][0] = 0;
							for (unsigned int ii = 1; ii <= VALID_AHCACODE_LEN; ++ii)
							{
								dist[ii][0] = ii;
							}
							for (unsigned int ii = 1; ii <= VALID_AHCACODE_LEN; ++ii)
							{
								dist[0][ii] = ii;
							}

							for (unsigned int ii = 1; ii <= VALID_AHCACODE_LEN; ++ii){
								for (unsigned int jj = 1; jj <= VALID_AHCACODE_LEN; ++jj)
								{
									dist[ii][jj] = min(min(dist[ii - 1][jj] + 1, dist[ii][jj - 1] + 1),
										dist[ii - 1][jj - 1] + (sequence[startOffset + ii - 1] == cuda_constants::SC[jj - 1] ? 0 : 1));
								}
							}

							if (dist[VALID_AHCACODE_LEN][VALID_AHCACODE_LEN] < AHCADistance) {
								AHCADistance = dist[VALID_AHCACODE_LEN][VALID_AHCACODE_LEN];
                                                                //mutationRate_AHCA += dist[VALID_AHCACODE_LEN][VALID_AHCACODE_LEN];
                                                                //totalDNARead_AHCA += (double)VALID_AHCACODE_LEN;
							}

							if (dist[VALID_AHCACODE_LEN][VALID_AHCACODE_LEN] <= AHCACodemutation)//AllowedEditNums)    //???
							{
								AHCAValid = AHCA_VALID;
								//printf("3->%s, %d, %d\n", AHCA, len, AHCACodemutation);
								for (int kk = 0; kk < VALID_AHCA_LEN; kk++) // fix the sequence
								{
									sequence[startOffset + kk] = AHCAAssign[kk];
								}
                                                                //mutationRate_AHCA += dist[VALID_AHCACODE_LEN][VALID_AHCACODE_LEN];
                                                                //totalDNARead_AHCA += (double)VALID_AHCACODE_LEN;
								if (AHCADistance != len) { // FOUND by Levenshtein
									detectedError[errorOffset++] = 'A';
									detectedError[errorOffset++] = 'L';
									detectedError[errorOffset++] = '(';
									detectedError[errorOffset++] = cuda_constants::NUMBER_MARK[AHCADistance];
									detectedError[errorOffset++] = ')';

									errorCount++;
								}
							}
							ahcaId++;
						}
					}
				}
				if (AHCAValid == AHCA_VALID) //  Valid AHCA
					break;
				startOffset++;  // ::: Increase startOffset
                                len_to_end_of_seq = sequenceLength - startOffset;
			}

			//+++++++ End fix part+++++++++ //

			if (AHCAValid != AHCA_VALID) //Not valid AHCA
			{
				detectedError[errorOffset++] = ':';
				detectedError[errorOffset++] = '5';
				detectedError[errorOffset++] = '5';
				detectedError[errorOffset++] = '(';
				detectedError[errorOffset++] = cuda_constants::NUMBER_MARK[AHCADistance];
				detectedError[errorOffset++] = ')';

				errorCount++;
			}

			for (int c = 0; c < VALID_AHCA_LEN; c++)
			{
				detectedAHCA[c] = sequence[startOffset + c];
			}
			detectedAHCA[VALID_AHCA_LEN] = '\0';
		}
		else // Cannot find AHCA
		{
			for (int c = 0; c < VALID_AHCA_LEN; c++)
			{
				detectedAHCA[c] = 'x';
			}
			detectedAHCA[VALID_AHCA_LEN] = '\0';

			detectedError[errorOffset++] = ':';
			detectedError[errorOffset++] = 'A';
			detectedError[errorOffset++] = 'L';
			detectedError[errorOffset++] = '9';
			detectedError[errorOffset++] = '9';

			errorCount++;
		}

#pragma endregion

		detectedError[errorOffset++] = '\0'; // and end mark for output error buffer

		// Summary

		detectionInfo->errorCount = errorCount;

		if (isForwardSequence){
			detectionInfo->sequenceType = TYPE_FORWARD;
		}
		detectionInfo->mutationRate = mutationRate;
		detectionInfo->totalDNARead = totalDNARead;
                detectionInfo->mutationRate_SC3 = mutationRate_SC3;
                detectionInfo->mutationRate_SC2 = mutationRate_SC2;
                detectionInfo->mutationRate_SCN = mutationRate_SCN;
                detectionInfo->mutationRate_SC1 = mutationRate_SC1;
                detectionInfo->mutationRate_AHCA = mutationRate_AHCA;
                detectionInfo->totalDNARead_SC3 = totalDNARead_SC3;
                detectionInfo->totalDNARead_SC2 = totalDNARead_SC2;
                detectionInfo->totalDNARead_SCN = totalDNARead_SCN;
                detectionInfo->totalDNARead_SC1 = totalDNARead_SC1;
                detectionInfo->totalDNARead_AHCA = totalDNARead_AHCA;
	} // End if( index < sequenceCounter)
}

// Will directly call the SInfo.reset() and SCList.clear() in the Initializ() function
/*
void reset() {
	SInfo.reset();
	SCList.clear();
}
*/

vector<string> checkDevice()
{
	vector<string> deviceList;
#if 0
	int devCount;
	cudaGetDeviceCount(&devCount);

	// Iterate through devices
	for (int i = 0; i < devCount; ++i)	{
		cudaDeviceProp devProp;
		cudaGetDeviceProperties(&devProp, i);
		deviceList.push_back(devProp.name);
	}
#endif
	return deviceList;
}

__global__ void RandomCode_Similarity_kernel(const int idx, char * d_RandomCodeBuffer, int * d_RandomCodeMatchingTable, int RandomCodeSize, int i)
{
  //take out gpu idx
	//int idx = blockIdx.x * blockDim.x + threadIdx.x;

	if (idx < RandomCodeSize)
	{
		char * randomCode1 = &d_RandomCodeBuffer[idx * (VALID_RANDOM_LEN + 1)];
		char * randomCode2 = &d_RandomCodeBuffer[i * (VALID_RANDOM_LEN + 1)];
		int count = 0;
		int mutation = 0;
		while (count < VALID_RANDOM_LEN)
		{
			if (randomCode1[count] != randomCode2[count])
			{
				mutation++;
				if (mutation > 1)
				{
					break;
				}
			}
			count++;
		}

		if (count == VALID_RANDOM_LEN)
		{
			d_RandomCodeMatchingTable[idx] = 1;
		}
		else
		{
			d_RandomCodeMatchingTable[idx] = 0;
		}
	}
}

std::tuple<int,int> GetRandomCodeNumber(LegacyBuffers& buffers, char * const h_RandomCodeBuffer, const int RandomCodeSize, int * const h_RandomNumBuffer)
{
	int number = 0;
        int this_num_cpu_reads_uid_mismatch = 0;
	cudaMemcpy(buffers.d_RandomCodeBuffer, h_RandomCodeBuffer, RandomCodeSize * (VALID_RANDOM_LEN + 1) * sizeof(char), cudaMemcpyHostToDevice);
        cudaMemcpy(buffers.d_RandomNumBuffer, h_RandomNumBuffer, RandomCodeSize * sizeof(int), cudaMemcpyHostToDevice);
	memcpy(buffers.h_RandomCodeValueTable, buffers.h_settedRandomCodeValueTable, RandomCodeSize * sizeof(int));

	for (int i = 0; i < RandomCodeSize; i++) {
#if 0
		RandomCode_Similarity_kernel << <(RandomCodeSize - 1) / BLOCK_SIZE + 1, BLOCK_SIZE >> >(buffers.d_RandomCodeBuffer, buffers.d_RandomCodeMatchingTable, RandomCodeSize, i);
#endif
    int num_work_items = ((RandomCodeSize - 1) / BLOCK_SIZE + 1) * BLOCK_SIZE;
#pragma omp parallel for
    for (int ii = 0; ii < num_work_items; ++ii) {
      RandomCode_Similarity_kernel(ii, buffers.d_RandomCodeBuffer, buffers.d_RandomCodeMatchingTable, RandomCodeSize, i);
    }
		cudaMemcpy(buffers.h_RandomCodeMatchingTable, buffers.d_RandomCodeMatchingTable, RandomCodeSize * sizeof(int), cudaMemcpyDeviceToHost);

		for (int j = i; j < RandomCodeSize; j++)
		{
			if (i == j || buffers.h_RandomCodeValueTable[j] == 0 || buffers.h_RandomCodeValueTable[i] == 0) {
				continue;
			}

			if (buffers.h_RandomCodeMatchingTable[j] == 1){
				buffers.h_RandomCodeValueTable[j] = 0;
                                this_num_cpu_reads_uid_mismatch+=buffers.d_RandomNumBuffer[j];
			}
		}
	}

	for (int i = 0; i < RandomCodeSize; i++)
	{
		number += buffers.h_RandomCodeValueTable[i];
	}
        return std::make_tuple(number, this_num_cpu_reads_uid_mismatch);
}
