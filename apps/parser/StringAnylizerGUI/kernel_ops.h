#pragma once
#include  "kernel.h"

//the following declarations are moved from kernel.h when decoupling from windows GUI
void workOutGPU(const bool saveBadLinkes, LegacyBuffers& buffers, vector<Cell> & cellHash, vector<string> & junkLines, vector<string> & badLines);// COMMENT: Process a chunk of sequence in GPU
//change legacy code interface, now mutationAllowed is pass through buffers
//initialize() has been moved to LegacyBuffer
//int initialize(string directoryName, bool saveBadLines, LegacyBuffers& buffers, int mutationAllowed, map <string, string> & AHCA_map, int deviceId)
void reset();
void cleanUpMem(LegacyBuffers& buffers);
void readTemplate(string directoryName, LegacyBuffers& buffers);// COMMENT : read the template file Oligos.txt
// change the interface to allow pass LegacyBuffers
int ReadValidSCCode(string directoryName, char * & h_validSCBuffer, LegacyBuffers& buffers);// COMMENT read SC.txt file
//int hashSc(string scCheck); // function not used
void readValidAHCA(string directoryName, char * & h_validAHCABuffer, char * & h_AHCAAssignList, int * & h_validAHCALenBuffer,
	int * & h_validAHCAMutStart, int * & h_validAHCAMutEnd,
	int * & h_validAHCAMutationBuffer, int & validAHCANumber,	
	int * & h_validAHCAType, map <string, string> & AHCA_map);// COMMENT read AHCA code from file

void readOligos(string directoryName, char * & h_oligosBuffer, int * & h_oligosLength, int & oligosNumber);// COMMENT read the template file Oligos.txt

void ProcessSequences(const string& fileName, const bool saveBadLinkes, LegacyBuffers& buffer, vector<Cell> & cellHash, vector<string> & junkLines, vector<string> & badLines, StatisticsInfo & statisticsInfo, 
	vector<string> &SC_stat, string ENTRY, string & whereatCU, string & whereatCount);// COMMENT : process a sequence file
vector<string> checkDevice();// COMMENT : get list of device name.
std::tuple<int,int> GetRandomCodeNumber(LegacyBuffers &buffers, char* const h_RandomCodeBuffer, int const RandomCodeSize, int * const h_RandomNumBuffer);
