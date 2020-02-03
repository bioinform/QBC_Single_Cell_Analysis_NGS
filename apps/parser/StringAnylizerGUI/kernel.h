#pragma once
#include  <stdio.h>
#include  <string>
#include  <vector>
//
using namespace std;

struct Cell
{
	string header;
        string barcode;
	string ahca;
	string random;
	string removeMe;
	int num;
	Cell(string _header, string _barcode, string _ahca, string _random, string _removeMe) :
		header(_header), barcode(_barcode), ahca(_ahca), random(_random), removeMe(_removeMe){
		num = 1;
	}

	const std::string& uidsc3() const {
		buffer_ = this->random;
		buffer_.append(barcode.begin(), barcode.begin() + 7);
		return buffer_;
	}
private:
	mutable std::string buffer_;

};

struct StatisticsInfo
{
	int goodCodes;
	int forwardCodes;
	int backwardCodes;
	int totalBadCodes;
	int junkCodes;
	int AHCAtooSmall;
	int NoAHCAfound;
	int RCerror;
	int badSC1;
	int badSC2;
	int badSCN;
	int badSC3;
	int badSC1xxxx;
	int badSC2xxxx;
	int badSCNxxxx;
	int badSC3xxxx;
	int ErrorInSC;
	int mutationRate;
	double totalDNARead;
	int totalSequenceInFile;
	float gpuTime;
	float subtractTime;
        int mutationRate_SC3;
        int mutationRate_SC2;
        int mutationRate_SCN;
        int mutationRate_SC1;
        int mutationRate_AHCA;
        double totalDNARead_SC3;
        double totalDNARead_SC2;
        double totalDNARead_SCN;
        double totalDNARead_SC1;
        double totalDNARead_AHCA;


	StatisticsInfo() {
		goodCodes = 0;
		forwardCodes = 0;
		backwardCodes = 0;
		totalBadCodes = 0;
		junkCodes = 0;
		AHCAtooSmall = 0;
		NoAHCAfound = 0;
		RCerror = 0;
		badSC1 = 0;
		badSC2 = 0;
		badSCN = 0;
		badSC3 = 0;
		badSC1xxxx = 0;
		badSC2xxxx = 0;
		badSCNxxxx = 0;
		badSC3xxxx = 0;
		ErrorInSC = 0;
		mutationRate = 0;
		totalDNARead = 0.0;
		totalSequenceInFile = 0;
		gpuTime = 0;
		subtractTime = 0;
                mutationRate_SC3 = 0;
                mutationRate_SC2 = 0;
                mutationRate_SCN = 0;
                mutationRate_SC1 = 0;
                mutationRate_AHCA = 0;
                totalDNARead_SC3 = 0.0;
                totalDNARead_SC2 = 0.0;
                totalDNARead_SCN = 0.0;
                totalDNARead_SC1 = 0.0;
                totalDNARead_AHCA = 0.0;

	}
	void reset() {
		goodCodes = 0;
		forwardCodes = 0;
		backwardCodes = 0;
		totalBadCodes = 0;
		junkCodes = 0;
		AHCAtooSmall = 0;
		NoAHCAfound = 0;
		RCerror = 0;
		badSC1 = 0;
		badSC2 = 0;
		badSCN = 0;
		badSC3 = 0;
		badSC1xxxx = 0;
		badSC2xxxx = 0;
		badSCNxxxx = 0;
		badSC3xxxx = 0;
		ErrorInSC = 0;
		mutationRate = 0;
		totalDNARead = 0.0;
		totalSequenceInFile = 0;
		gpuTime = 0;
		subtractTime = 0;
                mutationRate_SC3 = 0;
                mutationRate_SC2 = 0;
                mutationRate_SCN = 0;
                mutationRate_SC1 = 0;
                mutationRate_AHCA = 0;
                totalDNARead_SC3 = 0.0;
                totalDNARead_SC2 = 0.0;
                totalDNARead_SCN = 0.0;
                totalDNARead_SC1 = 0.0;
                totalDNARead_AHCA = 0.0;

	}
};

// this is exact line-by-line copy from the legacy code
struct DetectionInfo // This struct store all information about the sequence after processing.
{
	int errorCount;
	int sequenceType;
	int badSC1;
	int badSC2;
	int badSCN;
	int badSC3;
	int badSC1xxxx;
	int badSC2xxxx;
	int badSCNxxxx;
	int badSC3xxxx;
	int mutationRate;
	double totalDNARead;
	int RCerror;
	int ErrorInSC;
        int mutationRate_SC3;
        int mutationRate_SC2;
        int mutationRate_SCN;
        int mutationRate_SC1;
        int mutationRate_AHCA;
        double totalDNARead_SC3;
        double totalDNARead_SC2;
        double totalDNARead_SCN;
        double totalDNARead_SC1;
        double totalDNARead_AHCA;
};

// this is exact line-by-line copy from the legacy code
//+ Define struct
struct TemplateLenInfo
{
	int SC3;
	int SC[6];

	int Anneal_Pre;
	int Anneal_Post;
};
//struct AHCAinfo
//{
	//string type;
	//string AHCA_Code;
	//int mutation;
	//string tagName;
	//string assignAHCA;
//};
//


struct LegacyBuffers {
// all the declarations are exact line by line copies from legacy code
// kernel.cu , more work is needed to clean up these buffers
  char * h_resultBuffer; // COMMENT : host buffer to store returned result from GPU
  char * h_sequenceHeader; // COMMENT : host buffer to store input header sequences, use to copy to GPU
  char * h_sequenceBuffer; // COMMENT : host buffer to store input sequences, use to copy to GPU
  int * h_sequenceLength; // COMMENT : each sequence have a length, store these informations for GPU use.
  char * d_resultBuffer; // COMMENT device buffer to store results when GPU working
  char * d_sequenceHeader; // COMMENT device buffer to store input header sequences on GPU ( transfed from h_sequenceBuffer to d_sequenceBuffer)
  char * d_sequenceBuffer; // COMMENT device buffer to store input sequences on GPU ( transfed from h_sequenceBuffer to d_sequenceBuffer)
  int * d_sequenceLength; // COMMENT  device buffer to store all sequences's lengths.
  char * d_validSCBuffer; // COMMENT  store all SC code from SC valid.txt file (on device)
  char * d_validAHCABuffer; // COMMENT store all AHCA code from AHCA.txt file
  //--by un
  int * d_validAHCALength;  // ::: COMMENT length array of AHCA Codes
  int * d_validAHCAMutation;  // :::  COMMENT Mutation array of AHCA codes
  int * d_validAHCAmutStart;  // :::  COMMENT Mutation array of AHCA mutation start codes
  int * d_validAHCAmutEnd;  // :::  COMMENT Mutation array of AHCA mutation start codes
  char * d_validAHCATagNameBuffer;  // ::: device buffer to store tag names of AHCA
  //---
  char * d_AHCAAssignList; // COMMENT store all Assigned AHCA code from AHCA.txt file
  int * d_validAHCAType;  // :::  COMMENT Type array of AHCA codes for A or B algorithm

  DetectionInfo * d_detectionInfo; // COMMENT store all addtional infomation about detected sequence such as good/bad sequense, where of error...
  char * d_oligosBuffer; // COMMENT // store all code from Oligos.txt file
  int * d_oligosLength; // COMMENT store all lengths of codes in Oligos.txt file

  char * d_RandomCodeBuffer;
  int * d_RandomNumBuffer;
  int * h_RandomCodeMatchingTable;
  int * d_RandomCodeMatchingTable;

  int * h_settedRandomCodeValueTable;
  int * h_RandomCodeValueTable;

  int * h_SCx_stat;
  int * d_SCx_stat;
  DetectionInfo * h_detectionInfo;
  int allowedEdit;
  int validSCNumber;
  int validAHCANumber;
  int oligosNumber;
  int sequenceCounter;
  int totalSeqs;
  StatisticsInfo SInfo;
  vector<string> SCList;
  TemplateLenInfo templateLenInfo;
  std::map<std::string, std::string> AHCA_map;
  int Initialize(const string& directoryName, const bool savebad, const int mutation, const int deviceId); // COMMENT : Read input data, allocate memory....

};

#define  MAX_RANDOM_NUMBER_EACH_AHCA 1000000
#define  VALID_RANDOM_LEN 10    // Length of detected RANDOM (DECA) code
#define  NUM_RANDOM_LIMIT_FOR_GPU 10
