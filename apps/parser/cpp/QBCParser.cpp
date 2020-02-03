#include <tuple>
#include "stdafx.h"
#include <iostream>
#include <time.h>
#include <cstring>
#include <boost/filesystem.hpp>
#include "QBCParser.h"
#include "../StringAnylizerGUI/kernel.h"
#include "../StringAnylizerGUI/kernel_ops.h"

namespace rss {
    int QBCParser::Initialize( std::string const& directory
                             , bool const save_bad_lines
                             , int const mutation_allowed
                             , int const device_id) {
 
        int ret = buffers_.Initialize(directory, save_bad_lines, mutation_allowed, device_id);
        if (ret != 0) {
            readTemplate(directory, buffers_);
        }
        return ret;
    }

    QBCParser::Results QBCParser::Process(std::string const& file, LegacyLandMines& landmines) {
        Results ret;
        ProcessSequences(file, options_.save_bad_lines, buffers_, ret.cell_hash(), ret.junk_lines(), ret.bad_lines(), stats_, ret.sc_stat(), options_.entry, landmines.whereatthis, landmines.countingme);
        printf("Processed Sequences Finished for %s.\n", options_.entry.c_str());
        return ret;
    }

    std::vector<std::string> QBCParser::CheckDevice() const {
        return checkDevice();
    }

    void QBCParser::CleanUpMem() {
        cleanUpMem(buffers_);
    }

    std::tuple<int,int> QBCParser::GetRandomCodeNumber(char * const random_code_buffer, const int random_code_size, int * const random_num_buffer) {
      return ::GetRandomCodeNumber(buffers_, random_code_buffer, random_code_size, random_num_buffer);
    }

//straight up copy from legacy MyMainClass.cpp
void QBCParser::SendVarstoForm(LegacyLandMines& landmines, string whereatString, int & doubleme, int & jj, int &kk, int & ll, int & greaterthan, char b[50])
{
	jj++;	ll++;	kk++;	
	
	string numWithCommas = to_string(ll);
	int insertPosition = numWithCommas.length() - 3;
	while (insertPosition > 0) {
		numWithCommas.insert(insertPosition, ",");
		insertPosition -= 3;
	}

	try
	{
		if (kk > 10000)
		{
            char a[60]; 	sprintf(a, "%s > %s.\n", options_.entry.c_str(), b);
			char b[60]; 	sprintf(b, "%s count: %s\n", whereatString.c_str(), numWithCommas.c_str());
			landmines.whereatthis = string(a);
			landmines.countingme = string(b);
			kk = 0;
		}
	}
	catch (exception &e)
	{
		printf("Here 1");
	}

	string GTWithCommas = to_string(greaterthan);
	insertPosition = GTWithCommas.length() - 3;
	while (insertPosition > 0) {
		GTWithCommas.insert(insertPosition, ",");
		insertPosition -= 3;
	}	
	
	if (jj > greaterthan-1)
	{
		printf("		%s Counting for %s(%s):\t\t%s\t\t%s\n", whereatString.c_str(), options_.entry.c_str(), b, numWithCommas.c_str(), GTWithCommas.c_str());
		jj = 0;
		doubleme++;
		if (doubleme == 2)
		{
			doubleme = 0;
			greaterthan = greaterthan * 2;
		}
	}
}


// change the legacy code interface to allow output dir
void QBCParser::SaveResultData(LegacyLandMines& landmines, const string& fasta_file, const string& out_dir, vector<Cell> & cellHash, const vector<string>& junklines, const vector<string>& badlines,  int flag, char b[50])
{
	composed_view& composed_cellHash = get<0>(cell_hash_list_);
    print_view& print_cellHash = get<1>(cell_hash_list_);
    random_view&  random_cellHash = get<2>(cell_hash_list_);
    SC3_view& SC3_cellHash = get<3>(cell_hash_list_);
    auto& uidsc3_cellHash = get<UIDSC3_VIEW>(cell_hash_list_);

	time_t start_time, end_time;
	float totalTimed;

  //roger comment: the legacy code save result in the input directory.
  //Here we put the result into a output directory that is specified by command line
  boost::filesystem::path fasta_p(fasta_file);
  boost::filesystem::path out_p(out_dir);
  string FileName = (out_p / fasta_p.filename()).string();
	string fasta = ".fasta";
	string extended = ".extendedFrags";


	std::string::size_type ii = FileName.find(fasta);
	if (ii != std::string::npos)
		FileName.erase(ii, fasta.length());

	std::string::size_type pp = FileName.find(extended);
	if (pp != std::string::npos)
		FileName.erase(pp, extended.length());

	// The number of sequences.
	int number = cellHash.size();
	int filter_number = 0;
	int jj = 0;
	int kk = 0;
	int ll = 0;
	int doubleme =0;
	int greaterthan = 100000;

	string numWithCommas = to_string(number);
	int insertPosition = numWithCommas.length() - 3;
	while (insertPosition > 0) {
		numWithCommas.insert(insertPosition, ",");
		insertPosition -= 3;
	}


    printf("\n	Number of unfiltered codes for %s(%s) : %s\n\n", options_.entry.c_str(), b, numWithCommas.c_str());
	//printf("\n	Number of unfiltered codes for %s(%s) : %d\n\n", ENTRY.c_str(), b, cellHash.size());

	string filetag = "";
	if (options_.none){ filetag += "_None"; }
	if (options_.moderate){ filetag += "_LStr"; }
	if (options_.stringent){ filetag += "_Strt"; }
	if (options_.sc3_remove){ filetag += "_SC3R"; }
	if (options_.rc_remove)
	{	filetag += "_RC(-)";	}
	else
	{	filetag += "_RC(+)";	}

#pragma region Create_the_CellHash
        int num_post_parse_reads = 0;
        int num_dedup_UID_barcode_ahca_reads = 0;
        int num_post_dedup_UID_barcode_ahca_uids = 0;
	//Copy the sequences into the cellhash buffer
    printf("--> Creating the cellHash for %s(%s).\n", options_.entry.c_str(), b);
        num_post_parse_reads = cellHash.size();
	time(&start_time);
	for (int i = 0; i < number; i++)
	{
		//Get the cell.
		Cell * cell = &cellHash[i];

		SendVarstoForm(landmines, "CellHash", doubleme, jj, kk, ll, greaterthan, b);

		// Get iterater of CellHash
		composed_view::iterator lower, upper;
		boost::tie(lower, upper) = composed_cellHash.equal_range(boost::make_tuple(cell->barcode, cell->ahca, cell->random));
		string iAHCA = cell->ahca;
		string barcode_s = cell->barcode;
		string newi = barcode_s.c_str();
                string header = cell->header;

		string removeMoi = newi.substr(8, 23) + iAHCA;

		if (std::distance(lower, upper) == 1)  // if there is only one element
		{
			Cell mcell = *lower;			// Copy that element
			mcell.num++;					// increase the count
			mcell.removeMe = removeMoi;
                        mcell.header+="\t"+header; // add another header
			composed_cellHash.replace(lower, mcell);
                        num_dedup_UID_barcode_ahca_reads++;
		}
		else
		{
      // the legacy code make a new copy of cell, don't think that is necessary
			Cell newCell = *cell;
			newCell.removeMe = removeMoi;
			*cell = newCell;
      // Insert composed cell hash
			composed_cellHash.insert(*cell);  //  add the cell
		}
	}

	time(&end_time);
	totalTimed = (float)difftime(end_time, start_time);
    printf("		----> Time processing cellHash for %s(%s)     : %.2f seconds\n\n", options_.entry.c_str(), b, totalTimed);
        num_post_dedup_UID_barcode_ahca_uids = composed_cellHash.size();

#pragma endregion

#pragma region SC3_Remove
	// before removing the mutant random codes, 
	//  to determine which is REAL must "delete" all with SC2, SCN, SC1 and AHCA that are the same.  
	// Add "1" to any cell with > 5 events.
	// Need to remove singlebase mutations.

	if (options_.sc3_remove)		// removing SC issue
	{
		filetag += "SCrem_";
		time(&start_time);

		jj = 0;	ll = 0;	kk = 0;
		greaterthan = 100000;
		doubleme = 0;

		bool islast = false;
        printf("	Deleting the SC3 failures for %s(%s).\n", options_.entry.c_str(), b);
		for (SC3_view::iterator remove_iter = SC3_cellHash.begin(); remove_iter != SC3_cellHash.end();)
		{
			SendVarstoForm(landmines, "SC3 Failures", doubleme, jj, kk, ll, greaterthan, b);
			//Make the iterater of removeMe
			string remove_s = remove_iter->removeMe;		// get the barcode for the iterated cell event.		
			SC3_view::iterator lower, upper;				// get upper and lower bounds (should be only 1)
			boost::tie(lower, upper) = SC3_cellHash.equal_range(remove_s);
			int step = distance(lower, upper);

			if (step > 1){
				SC3_view::iterator most_common = lower;  // Get best elements
				for (SC3_view::iterator it = lower; it != upper; ++it){
					if (most_common->num < it->num)
						most_common = it;
				}
				// remove other elements
				for (SC3_view::iterator it = lower; it != upper;){
					if (it != most_common || options_.stringent){
						filter_number += it->num;
						it = SC3_cellHash.erase(it);
					}
					else ++it;
					remove_iter = it;
					if (remove_iter == SC3_cellHash.end()) break;
				}
			}
			else
			{
				remove_iter++;
			}
			
			//advance(remove_iter, step);
		}
		time(&end_time);
		totalTimed = (float)difftime(end_time, start_time);
        printf("		  Time processing SC3 errors for %s(%s)     : %.2f seconds\n\n", options_.entry.c_str(), b, totalTimed);

	}

#pragma endregion

#pragma region Stringency
        int num_pre_chimera_pruned_uids = 0;
        int num_chimera_pruned_reads = 0;
        int num_chimera_pruned_uids = 0;
        int num_post_chimera_pruned_reads = 0;
        int num_post_chimera_pruned_uids = 0;
	int itercounter = 0;
	// Moderate:  Cull sequences if random & SC3 is the same.  Keep the HIGHEST expressed RC-SC3.
	// Stringent: Cull sequences if random       is the same.  Keep the HIGHEST expressed RC.
	if (options_.moderate || options_.stringent) {

		time(&start_time);

		jj = 0;	ll = 0;	kk = 0;
		greaterthan = 100000;
		doubleme = 0;

		std::ofstream outf(FileName + filetag + ".crossoverFilter");
        printf(" --> Filtering for stringency for %s(%s).\n", options_.entry.c_str(), b);
		int i = 0;
		num_pre_chimera_pruned_uids = uidsc3_cellHash.size();

		//Cycle random codes and find highest expressed random code.
		

	auto filter_fcn = [&](auto& cellHash) {
		const int begin_size = distance(cellHash.begin(), cellHash.end());
    		
		for (auto iter = cellHash.begin(); iter != cellHash.end();){

            SendVarstoForm(landmines, "Stringency", doubleme, jj, kk, ll, greaterthan, b);

			//Make the iterater of random code
			const auto lower_upper = cellHash.equal_range(options_.moderate ? iter->uidsc3() : iter->random);
			int step = distance(lower_upper.first, lower_upper.second);
			if (step > 1){
        			bool has_tie = false;
				auto most_common = lower_upper.first;  // Best elements (highest expressed)
				for (auto it = lower_upper.first; it != lower_upper.second; ++it){
					if (it != most_common && it->num == most_common->num ) {
						has_tie = true;
					}
					if (most_common->num < it->num)
					{
						most_common = it;
						has_tie = false;
					}
				}
                                Cell most_common_stable = *most_common;
				for (auto it = lower_upper.first; it != lower_upper.second;){
#if 0
					string s_sc3 = most_common->barcode.substr(0, 7);			// Get random code of sequence.
					string d_sc3 = it->barcode.substr(0, 7);					// Cycle through the random codes from the hash table.

					if (it != most_common && (s_sc3 == d_sc3 || options_.stringent)) {	// Find equal code
#endif
					if ((it->num < most_common_stable.num) || (has_tie && (it->num < options_.pcr_crossover_threshold))) {
						filter_number += it->num;
                                                num_chimera_pruned_reads+=it->num;
                                                num_chimera_pruned_uids++;
						if (outf.is_open()) {
							outf << it->header + "|uid: " + it->random + "|barcodes: "+ it->barcode + "|ahca: " + it->ahca + "|num: " + std::to_string(it->num) << std::endl;
						}
						it = cellHash.erase(it);							// Remove lower expressed RC/SC3 combinations
					}
					else ++it;
					iter = it;
					if (iter == cellHash.end()) break;
				}

			}
			else
			{
				iter++;
			}
			//advance(iter, step);
		
		}
		outf << "number of uid before filter: " <<begin_size << "\nnumber of uid after filter: " << distance(cellHash.begin(), cellHash.end()) << std::endl;
		outf.close();
	};
	options_.moderate ? filter_fcn(uidsc3_cellHash) : filter_fcn(random_cellHash);
		time(&end_time);
		totalTimed = (float)difftime(end_time, start_time);
        printf("		----> Time processing Random Code stringency checks for %s(%s)    : %.2f seconds\n", options_.entry.c_str(), b, totalTimed);
                num_post_chimera_pruned_reads = num_post_parse_reads-num_chimera_pruned_reads;
                num_post_chimera_pruned_uids = num_pre_chimera_pruned_uids-num_chimera_pruned_uids;
	}

#pragma endregion

#pragma region ByCells Save
        int num_byCells_cells = 0;
        int num_byCells_uids = 0;
        int num_byCells_reads = 0;

	//if (options_.save_by_cells)
	//{
		time(&start_time);

		jj = 0;	ll = 0;	kk = 0;
		greaterthan = 100000;
		doubleme = 0;

        printf("	Storing .byCells for %s(%s).\n", options_.entry.c_str(), b);

		string outputFileName = FileName + filetag + ".byCells";
#if 0
		FILE * fout;
		fopen_s(&fout, outputFileName.c_str(), "w"); // Changed the fout to fout_s  was:   fout = fopen(outputFileName.c_str(), "w");
#endif
        FILE* fout = fopen(outputFileName.c_str(), "w"); // change fopen_s in legacy code to fopen() because fopen_s() is not easily available on linux for c++14

		for (print_view::iterator barcode_iter = print_cellHash.begin(), bar_end(print_cellHash.end()); barcode_iter != bar_end;)
		{
            SendVarstoForm(landmines, ".byCells", doubleme, jj, kk, ll, greaterthan, b);
			string barcode_s = barcode_iter->barcode; // barcodes
			print_view::iterator lower, upper;
			tie(lower, upper) = print_cellHash.equal_range(boost::make_tuple(barcode_s));
			int step = std::distance(lower, upper);
                        num_byCells_cells++;
                        num_byCells_uids+=step;
			advance(barcode_iter, step);

			if (options_.save_by_cells){ fprintf(fout, "%s\n", barcode_s.c_str()); }
			for (print_view::iterator ahca = lower; ahca != upper;)
			{
				print_view::iterator ahca_lower, ahca_upper;
				string ahca_s = ahca->ahca; // AHCA
				boost::tie(ahca_lower, ahca_upper) = print_cellHash.equal_range(boost::make_tuple(barcode_s, ahca_s));
				int ahca_step = distance(ahca_lower, ahca_upper);
				advance(ahca, ahca_step);
				if (options_.save_by_cells){ fprintf(fout, "\t\t\t\t\t\t\t\t%s\n", ahca_s.c_str()); }
				for (print_view::iterator random = ahca_lower; random != ahca_upper; random++)
				{
					if (options_.save_by_cells){ fprintf(fout, "\t\t\t\t\t\t\t\t\t\t\t\t\t%s\t%d\n", random->random.c_str(), random->num); }
                                        num_byCells_reads+=random->num;
				}
			}

			if (options_.save_by_cells){ fprintf(fout, "\n"); }
		}

		fclose(fout);

		time(&end_time);
		totalTimed = (float)difftime(end_time, start_time);
        printf("		   Time processing byCells for %s(%s)     : %.2f seconds\n\n", options_.entry.c_str(), b, totalTimed);
	//}

	numWithCommas = to_string(cellHash.size() - filter_number);
	insertPosition = numWithCommas.length() - 3;
	while (insertPosition > 0) {
		numWithCommas.insert(insertPosition, ",");
		insertPosition -= 3;
	}

    printf("\n	Number of valid codes for %s(%s) : %s\n\n", options_.entry.c_str(), b, numWithCommas.c_str());
    stats_.goodCodes = cellHash.size() - filter_number;

#pragma endregion

#pragma region FCS Save

	time(&start_time);
    printf("	Saving *.byFCS for %s(%s).\n", options_.entry.c_str(), b);

	jj = 0;	ll = 0;	kk = 0;
	greaterthan = 100;
	doubleme = 0;

	/////// Print FCS output//////////////////////
	string outputFCSFileName = FileName + filetag + ".byFCS";
	FILE * fo;
	fo = fopen(outputFCSFileName.c_str(), "w");

	// write titles to file.
	fprintf(fo, "Barcode\t");

	for (auto it = buffers_.AHCA_map.cbegin(); it != buffers_.AHCA_map.cend(); ++it)  //???
	{	fprintf(fo, "%s\t", it->second.c_str());	}

	fprintf(fo, "\n");

	// Create temporary buffer to store Random Code of each AHCA for GPU processing
	char * h_RandomCodeBuffer;
	h_RandomCodeBuffer = new char[MAX_RANDOM_NUMBER_EACH_AHCA * (VALID_RANDOM_LEN + 1)];
        // Create temporary buffer to store Random Num of each AHCA for GPU processing
        int * h_RandomNumBuffer;
        h_RandomNumBuffer = new int[MAX_RANDOM_NUMBER_EACH_AHCA];

	printf("\t\tRC Remove: %s\n", options_.rc_remove ? "True" : "False");

        int num_1_UID_per_cell_cells = 0;
        int num_non_cpu_reads_expected = 0;
        int num_non_cpu_reads_from_uid_mismatch = 0;
        int num_non_cpu_uids_from_uid_mismatch = 0;
        int num_cpu_reads_expected = 0;
        int num_cpu_reads_from_uid_mismatch = 0;
        int num_cpu_uids_from_uid_mismatch = 0;
        int num_uids_processed = 0;
        int num_reads_processed = 0;
        int num_1_UID_per_cell_pruned_cells = 0;
        int num_1_UID_per_cell_pruned_uids = 0;
        int num_1_UID_per_cell_pruned_reads = 0;
        int final_num_cells = 0;
        int final_num_uids = 0;
        int final_num_reads = 0;

	for (print_view::iterator barcode_iter = print_cellHash.begin(), bar_end(print_cellHash.end()); barcode_iter != bar_end;)
	{
        SendVarstoForm(landmines, "FCS Save", doubleme, jj, kk, ll, greaterthan, b);
		string numWithCommas = to_string(ll);

		string barcode_s = barcode_iter->barcode;
		print_view::iterator lower, upper;
		tie(lower, upper) = print_cellHash.equal_range(boost::make_tuple(barcode_s));
		int step = std::distance(lower, upper);
                num_1_UID_per_cell_cells+=step;
		advance(barcode_iter, step);
		
		map<string, int> linehash;
                map<string, int> marker2reads;

		for (auto it = buffers_.AHCA_map.cbegin(); it != buffers_.AHCA_map.cend(); ++it)  //???
		{	
                    linehash[it->first] = 0;
                    marker2reads[it->first] = 0;
                }

		try{
			for (print_view::iterator ahca = lower; ahca != upper;)
			{
				
				print_view::iterator ahca_lower, ahca_upper;
				string ahca_s = ahca->ahca;
				try{
					boost::tie(ahca_lower, ahca_upper) = print_cellHash.equal_range(boost::make_tuple(barcode_s, ahca_s));
								}
				catch (exception &e)
				{
					std::cout << "Here 4: " <<  numWithCommas << e.what() << endl;
				}
				int ahca_step = distance(ahca_lower, ahca_upper);
					advance(ahca, ahca_step);

				if (ahca_step >= NUM_RANDOM_LIMIT_FOR_GPU && options_.rc_remove) // using GPU for processing
				{
					try{
						int randomCodeNumber = 0;
                                                int this_num_non_cpu_reads_uid_mismatch = 0;
						for (print_view::iterator random = ahca_lower; random != ahca_upper; random++)
						{
							strcpy(&h_RandomCodeBuffer[randomCodeNumber * (VALID_RANDOM_LEN + 1)], random->random.c_str());
                                                        h_RandomNumBuffer[randomCodeNumber] = random->num;
							randomCodeNumber++;
                                                        num_non_cpu_reads_expected+=random->num;
                                                        marker2reads[ahca_s]+=random->num; // all UID seqs or non-unique reads
						}
                                                tie(linehash[ahca_s], this_num_non_cpu_reads_uid_mismatch) = rss::QBCParser::Instance().GetRandomCodeNumber(h_RandomCodeBuffer, randomCodeNumber, h_RandomNumBuffer);
                                                marker2reads[ahca_s]-=this_num_non_cpu_reads_uid_mismatch;
                                                num_non_cpu_reads_from_uid_mismatch+=this_num_non_cpu_reads_uid_mismatch;
                                                if(ahca_step != linehash[ahca_s]){ num_non_cpu_uids_from_uid_mismatch+=(ahca_step-linehash[ahca_s]); }
					}
					catch (exception &e){
						std::cout << "Here 5: " << numWithCommas << e.what() << endl;
					}

				}
				else // CPU only
				{
					vector<string> arrayhashkey;
					vector<int> arrayhashvalue;
                                        vector<int> array_num_seq; // DEBUG contains number reads per UID
					try
					{
						for (print_view::iterator random = ahca_lower; random != ahca_upper; random++)
						{
							arrayhashkey.push_back(random->random);
							arrayhashvalue.push_back(1);
                                                        array_num_seq.push_back(random->num);
                                                        num_cpu_reads_expected+=random->num;
                                                        marker2reads[ahca_s]+=random->num;
						}
					}
					catch (exception &e){
						std::cout << "Here 6: " << numWithCommas << e.what() << endl;
					}

					int arrayhash_size = arrayhashkey.size();

					try{
						for (int i = 0; i < arrayhash_size; i++)
						{
							string iString = to_string(i);
							if (options_.rc_remove)
							{
								for (int j = i; j < arrayhash_size; j++)
								{
									string jString = to_string(j);

									try{
										if (arrayhashkey[i].compare(arrayhashkey[j]) == 0 || arrayhashvalue[j] == 0 || arrayhashvalue[i] == 0)
										{
											continue;
										}
									}
									catch (exception &e){
										std::cout << "Here 8: " << numWithCommas << e.what() << "\n" << iString << ":" << jString << endl;
									}

									int counter = 0;
									int mutation = 0;
									string counterString = "";
									// Check 2 keys if have only 1 mutation
									for (counter = 0; counter < arrayhashkey[i].length(); counter++)
									{
										counterString = to_string(counter);
										try
										{
											char temp = arrayhashkey[i].at(counter);
										}
										catch (exception &e){
											//cout << "    Here 99 i: " << numWithCommas << e.what() << "\n" << barcode_s << "\n" << iString << ":" << jString << ":" << counterString << endl;
										}

										try
										{
											char temp = arrayhashkey[j].at(counter);
										}
										catch (exception &e){
											//cout << "    Here 99 j: " << numWithCommas << e.what() << "\n" << barcode_s << "\n" << iString << ":" << jString << ":" << counterString << endl;
										}


										try{
											if (arrayhashkey[i].at(counter) != arrayhashkey[j].at(counter))
											{
												mutation++;
											}
											if (mutation > 1)
											{
												break;
											}
										}
										catch (exception &e){
											//cout << "Here 9: " << numWithCommas << e.what() << "\n" << barcode_s << "\n" << iString << ":" << jString << ":" << counterString << endl;
										}

									}

									try{
										if (counter == arrayhashkey[i].length())
										{
											arrayhashvalue[j] = 0;
                                                                                        marker2reads[ahca_s]-=array_num_seq[j];
                                                                                        num_cpu_reads_from_uid_mismatch+=array_num_seq[j];
                                                                                        num_cpu_uids_from_uid_mismatch++;
										}
									}
									catch (exception &e){
										//cout << "Here 10: " << numWithCommas << e.what() << "\n" << iString << ":" << jString << ":" << counterString << endl;
									}

								}
							}

							int sum = 0;
							for (int i = 0; i < arrayhash_size; i++)
							{
								if (arrayhashvalue[i] > 0)
								{
									sum++;
								}
							}
							linehash[ahca_s] = sum;
						}
					}
					catch (exception &e){
						std::cout << "Here 7: " << numWithCommas << e.what() << endl;
					}
				}
			}
		}
		catch (exception &e){
			std::cout << "Here 33: " << numWithCommas << e.what() << endl;
		}

		int iTemp = 0;
		for (map<string, int>::iterator it = linehash.begin(); it != linehash.end(); ++it)
		{	iTemp = iTemp + it->second;	}
                num_uids_processed+=iTemp;


                int reads_for_all_markers = 0; // DEBUG number of reads for all AHCAs
                for (map<string, int>::iterator it = marker2reads.begin(); it != marker2reads.end(); ++it)
                {   
                   reads_for_all_markers = reads_for_all_markers + it->second;
                }
                num_reads_processed+=reads_for_all_markers;

		if (iTemp > 1)
		{
			try
			{
				fprintf(fo, "%s\t", barcode_s.c_str());
                                final_num_cells++;
			}
			catch (exception &e){
				cout << "Here 12: " << numWithCommas << e.what() << endl;
			}


			for (map<string, int>::iterator it = linehash.begin(); it != linehash.end(); ++it)
			{
				try
				{
					fprintf(fo, "%d\t", it->second);
                                        final_num_uids+=it->second;
				}
				catch (exception &e)
				{
					cout << "Here 2: " << numWithCommas << e.what() << endl;
				}

			}
			fprintf(fo, "\n");		
                        final_num_reads+=reads_for_all_markers;
		}
                else{
                    num_1_UID_per_cell_pruned_cells++;
                    num_1_UID_per_cell_pruned_uids+=iTemp;
                    num_1_UID_per_cell_pruned_reads+=reads_for_all_markers;
                }
	}
        final_num_reads+=(num_non_cpu_reads_from_uid_mismatch + num_cpu_reads_from_uid_mismatch); // DEBUG include num of non-unique reads that were not properly recorded during dedup of UID 1bp mismatch   

	fclose(fo);
	time(&end_time);
	totalTimed = (float)difftime(end_time, start_time);
    printf("		   Time processing FCS for %s(%s)     : %.2f seconds\n\n", options_.entry.c_str(), b, totalTimed);

#pragma endregion

#pragma region SaveBadlines
	if (options_.save_bad_lines)
	{
		time(&start_time);
        printf("	Saving BadLines for %s(%s)\n", options_.entry.c_str(), b);
		doubleme = 0;
		string badLineFileName = FileName + filetag + ".bad";
		FILE * foutbadLine;

		foutbadLine = fopen(badLineFileName.c_str(), "w");
		int num = badlines.size();
		for (int i = 0; i < num; i++)
		{
            SendVarstoForm(landmines, "BadLines", doubleme, jj, kk, ll, greaterthan, b);
			fprintf(foutbadLine, "%s", badlines[i].c_str());
		}



		fclose(foutbadLine);
		time(&end_time);
		totalTimed = (float)difftime(end_time, start_time);
        printf("		   Time processing BadLines for %s(%s)     : %.2f seconds\n", options_.entry.c_str(), b, totalTimed);
        printf("	Number of badlines : %d\n\n", stats_.totalBadCodes);

	}
#pragma endregion

#pragma region SaveJunklines
        time(&start_time);
        printf("        Saving JunkLines for %s(%s)\n", options_.entry.c_str(), b);
        doubleme = 0;
        string junkLineFileName = FileName + filetag + ".junk";
        FILE * foutjunkLine;

        foutjunkLine = fopen(junkLineFileName.c_str(), "w");
        int num = junklines.size();
        for (int i = 0; i < num; i++)
        {   
            SendVarstoForm(landmines, "JunkLines", doubleme, jj, kk, ll, greaterthan, b);
            fprintf(foutjunkLine, "%s", junklines[i].c_str());
        }
        fclose(foutjunkLine);
        time(&end_time);
        totalTimed = (float)difftime(end_time, start_time);
        printf("                   Time processing JunkLines for %s(%s)     : %.2f seconds\n", options_.entry.c_str(), b, totalTimed);
        printf("        Number of junklines : %d\n\n", stats_.junkCodes);
#pragma endregion

#pragma region SaveMetrics
        time(&start_time);
        printf("        Saving metrics for %s(%s)\n", options_.entry.c_str(), b);
        doubleme = 0;
        string metricsFileName = FileName + filetag + ".metrics";
        FILE * foutMetrics;

        foutMetrics = fopen(metricsFileName.c_str(), "w");
        fprintf(foutMetrics, "==================================================\n");
        fprintf(foutMetrics, "Stage: parsing\n");
        fprintf(foutMetrics, "==================================================\n");
        fprintf(foutMetrics, "%-88s %9i\n", "total input reads:", stats_.totalSequenceInFile);
        fprintf(foutMetrics, "%-88s %9i\n", "reads filtered due to missing all anchors:", stats_.junkCodes);
        fprintf(foutMetrics, "%-88s %9i\n", "reads filtered due to anchor or UID or barcode or AHCA errors:", stats_.totalBadCodes);
        int num_parse_pruned = stats_.junkCodes+stats_.totalBadCodes;
        fprintf(foutMetrics, "%-88s %9i\n", "total reads filtered:", num_parse_pruned);
        fprintf(foutMetrics, "%-88s %9i\n", "total reads after parsing:", num_post_parse_reads);
        float pct_parse_pruned = (float(num_parse_pruned)/stats_.totalSequenceInFile)*100;
        float pct_post_parse_pruned = (float(num_post_parse_reads)/stats_.totalSequenceInFile)*100;
        fprintf(foutMetrics, "==================================================\n");
        fprintf(foutMetrics, "Stage: dedup by UID-barcodes-AHCA\n");
        fprintf(foutMetrics, "==================================================\n");
        fprintf(foutMetrics, "%-88s %9i\n", "duplicate reads:", num_dedup_UID_barcode_ahca_reads);
        float pct_dedup_UID_barcode_ahca = (float(num_dedup_UID_barcode_ahca_reads)/stats_.totalSequenceInFile)*100;
        fprintf(foutMetrics, "%-88s %9.2f\n", "% duplication rate:", pct_dedup_UID_barcode_ahca);
        fprintf(foutMetrics, "%-88s %9i\n", "unique reads after dedup:", num_post_dedup_UID_barcode_ahca_uids);
        float pct_post_dedup_UID_barcode_ahca = (float(num_post_dedup_UID_barcode_ahca_uids)/stats_.totalSequenceInFile)*100;
        fprintf(foutMetrics, "==================================================\n");
        fprintf(foutMetrics, "Stage: filter PCR chimera\n");
        fprintf(foutMetrics, "==================================================\n");
        fprintf(foutMetrics, "%-88s %9i\n", "unique reads filtered due to PCR chimera filter:", num_chimera_pruned_uids);
        fprintf(foutMetrics, "%-88s %9i\n", "total unique reads after parsing and PCR chimera filter:", num_post_chimera_pruned_uids);
        float pct_chimera_pruned_uids = (float(num_chimera_pruned_uids)/stats_.totalSequenceInFile)*100;
        float pct_post_chimera_pruned_uids = (float(num_post_chimera_pruned_uids)/stats_.totalSequenceInFile)*97;
        fprintf(foutMetrics, "%-88s %9i\n", "total reads filtered due to PCR chimera filter:", num_chimera_pruned_reads);
        fprintf(foutMetrics, "%-88s %9i\n", "total reads after parsing and PCR chimera filter:", num_post_chimera_pruned_reads);
        float pct_chimera_pruned_reads = (float(num_chimera_pruned_reads)/stats_.totalSequenceInFile)*100;
        float pct_post_chimera_pruned_reads = (float(num_post_chimera_pruned_reads)/stats_.totalSequenceInFile)*100;
        fprintf(foutMetrics, "%-88s %9i\n", "total cells after parsing and PCR chimera filter:", num_byCells_cells);
        fprintf(foutMetrics, "==================================================\n");
        fprintf(foutMetrics, "Stage: dedup by UID-1bp-mismatch\n");
        fprintf(foutMetrics, "==================================================\n");
        int num_uids_from_dedup_uid_mismatch=num_non_cpu_uids_from_uid_mismatch + num_cpu_uids_from_uid_mismatch;
        int num_reads_from_dedup_uid_mismatch=num_non_cpu_reads_from_uid_mismatch + num_cpu_reads_from_uid_mismatch;
        fprintf(foutMetrics, "%-88s %9i\n", "duplicate unique reads:", num_uids_from_dedup_uid_mismatch);
        fprintf(foutMetrics, "%-88s %9i\n", "unique reads after dedup:", num_uids_processed);
        float pct_dedup_uid_mismatch_uids = (float(num_uids_from_dedup_uid_mismatch)/stats_.totalSequenceInFile)*100;
        float pct_post_dedup_uid_mismatch_uids = (float(num_uids_processed)/stats_.totalSequenceInFile)*100;
        fprintf(foutMetrics, "%-88s %9.2f\n", "% unique duplication rate:", pct_dedup_uid_mismatch_uids);
        fprintf(foutMetrics, "%-88s %9i\n", "duplicate total reads:", num_reads_from_dedup_uid_mismatch);
        float pct_dedup_uid_mismatch_reads = (float(num_reads_from_dedup_uid_mismatch)/stats_.totalSequenceInFile)*100;
        fprintf(foutMetrics, "%-88s %9.2f\n", "% duplication rate:", pct_dedup_uid_mismatch_reads);
        fprintf(foutMetrics, "==================================================\n");
        fprintf(foutMetrics, "Stage: filter cells with one UID\n");
        fprintf(foutMetrics, "==================================================\n");
        fprintf(foutMetrics, "%-88s %9i\n", "unique reads filtered due to single-UID-per-cell filter:", num_1_UID_per_cell_pruned_uids);
        fprintf(foutMetrics, "%-88s %9i\n", "total reads filtered due to single-UID-per-cell filter:", num_1_UID_per_cell_pruned_reads);
        fprintf(foutMetrics, "%-88s %9i\n", "cells filtered due to single-UID-per-cell filter:", num_1_UID_per_cell_pruned_cells);
        float pct_1_UID_per_cell_pruned_uids = (float(num_1_UID_per_cell_pruned_uids)/stats_.totalSequenceInFile)*100;
        float pct_1_UID_per_cell_pruned_reads = (float(num_1_UID_per_cell_pruned_reads)/stats_.totalSequenceInFile)*100;
        float pct_1_UID_per_cell_pruned_cells = (float(num_1_UID_per_cell_pruned_cells)/num_byCells_cells)*100;
        fprintf(foutMetrics, "==================================================\n");
        fprintf(foutMetrics, "Stage: final \n");
        fprintf(foutMetrics, "==================================================\n");
        fprintf(foutMetrics, "%-88s %9i\n", "final unique reads after after parsing, dedup, PCR chimera and single UID-cell filters:", final_num_uids);
        fprintf(foutMetrics, "%-88s %9i\n", "final total reads after parsing, dedup, PCR chimera and single UID-cell filters:", final_num_reads);
        fprintf(foutMetrics, "%-88s %9i\n", "final cells after after parsing, dedup, PCR chimera and single UID-cell filters:", final_num_cells);
        float final_pct_uids = (float(final_num_uids)/stats_.totalSequenceInFile)*100;
        float final_pct_reads = (float(final_num_reads)/stats_.totalSequenceInFile)*100;
        float final_pct_cells = (float(final_num_cells)/num_byCells_cells)*100;

        float sum_pct_unique = pct_parse_pruned + pct_dedup_UID_barcode_ahca + pct_chimera_pruned_uids + pct_dedup_uid_mismatch_uids + pct_1_UID_per_cell_pruned_uids + final_pct_uids;
        float sum_pct_total = pct_parse_pruned + pct_chimera_pruned_reads + pct_1_UID_per_cell_pruned_reads + final_pct_reads;

        fprintf(foutMetrics, "\n");
        fprintf(foutMetrics, "==================================================\n");
        fprintf(foutMetrics, "Num (%) dedup or filtered units by stage\n");
        fprintf(foutMetrics, "==================================================\n");
        fprintf(foutMetrics, "%15s\t%15s\t%22s\t%27s\t%21s\t%24s\t%28s\t%15s\n", "unit", "start", "filtered from parsing", "dedup by UID-barcodes-AHCA", "filtered PCR chimera", "dedup by 1 UID mismatch", "filtered cells with one UID", "final");
        fprintf(foutMetrics, "%15s\t%15i\t%13i (%6.2f)\t%18i (%6.2f)\t%12i (%6.2f)\t%15i (%6.2f)\t%19i (%6.2f)\t%6i (%6.2f)\n", "unique_read", stats_.totalSequenceInFile, num_parse_pruned, pct_parse_pruned, num_dedup_UID_barcode_ahca_reads, pct_dedup_UID_barcode_ahca, num_chimera_pruned_uids, pct_chimera_pruned_uids, num_uids_from_dedup_uid_mismatch, pct_dedup_uid_mismatch_uids, num_1_UID_per_cell_pruned_uids, pct_1_UID_per_cell_pruned_uids, final_num_uids, final_pct_uids, sum_pct_unique);
        fprintf(foutMetrics, "%15s\t%15i\t%13i (%6.2f)\t%27s\t%12i (%6.2f)\t%24s\t%19i (%6.2f)\t%6i (%6.2f)\t\n", "total_read", stats_.totalSequenceInFile, num_parse_pruned, pct_parse_pruned, "NA", num_chimera_pruned_reads, pct_chimera_pruned_reads, "NA", num_1_UID_per_cell_pruned_reads, pct_1_UID_per_cell_pruned_reads, final_num_reads, final_pct_reads, sum_pct_total);
        fprintf(foutMetrics, "%15s\t%15s\t%22s\t%27s\t%21s\t%24s\t%28i\t%15i\n", "cell", "NA", "NA", "NA", "NA", "NA", num_1_UID_per_cell_pruned_cells, final_num_cells);
        fprintf(foutMetrics, "\n");
        fprintf(foutMetrics, "==================================================\n");
        fprintf(foutMetrics, "Num (%) surviving units by stage\n");
        fprintf(foutMetrics, "==================================================\n");
        fprintf(foutMetrics, "%15s\t%15s\t%20s\t%26s\t%26s\t%28s\t%40s\n", "unit", "start", "after parsing", "after perfect match dedup", "after PCR chimera filter", "after 1 UID mismatch dedup","final after cell with one UID filter");
        fprintf(foutMetrics, "%15s\t%15i\t%11i (%6.2f)\t%17i (%6.2f)\t%17i (%6.2f)\t%19i (%6.2f)\t%31i (%6.2f)\n", "unique_read", stats_.totalSequenceInFile, num_post_parse_reads, pct_post_parse_pruned, num_post_dedup_UID_barcode_ahca_uids, pct_post_dedup_UID_barcode_ahca, num_post_chimera_pruned_uids, pct_post_chimera_pruned_uids, num_uids_processed, pct_post_dedup_uid_mismatch_uids, final_num_uids, final_pct_uids);
        fprintf(foutMetrics, "%15s\t%15i\t%11i (%6.2f)\t%17i (%6.2f)\t%17i (%6.2f)\t%19i (%6.2f)\t%31i (%6.2f)\n", "total_read", stats_.totalSequenceInFile, num_post_parse_reads, pct_post_parse_pruned, num_post_parse_reads, pct_post_parse_pruned, num_post_chimera_pruned_reads, pct_post_chimera_pruned_reads, num_post_chimera_pruned_reads, pct_post_chimera_pruned_reads, final_num_reads, final_pct_reads);
        fprintf(foutMetrics, "%15s\t%15s\t%20s\t%26s\t%26i\t%28s\t%31i (%6.2f)\n", "cell", "NA", "NA", "NA", num_byCells_cells, "NA", final_num_cells, final_pct_cells);
        fclose(foutMetrics);
        time(&end_time);
        totalTimed = (float)difftime(end_time, start_time);
        printf("                   Time writing metrics for %s(%s)     : %.2f seconds\n", options_.entry.c_str(), b, totalTimed);
#pragma endregion

    printf("	End of printing for %s(%s).\n", options_.entry.c_str(), b);

}

//straight up copy from legacy MyMainClass.cpp
//roger comment: change the legacy code interface to allow output dir
void QBCParser::Statistics(string const& fasta_file, const string& out_dir, vector<string> SC_stat, const float totalTime, const float hashTime) const
{
    string filetag = "";
    if (options_.none){ filetag += "_None"; }
    if (options_.moderate){ filetag += "_LStr"; }
    if (options_.stringent){ filetag += "_Strt"; }
    if (options_.sc3_remove){ filetag += "_SC3R"; }

    FILE * fo;

    //roger comment: the legacy code save result in the input directory.
    //Here we put the result into a output directory that is specified by command line
    boost::filesystem::path fasta_p(fasta_file);
    boost::filesystem::path out_p(out_dir);
    string FileName = (out_p / fasta_p.filename()).string() + filetag + ".Statistics";

    string fasta = ".fasta";
    string extended = ".extendedFrags";

    std::string::size_type ii = FileName.find(fasta);
    if (ii != std::string::npos)
        FileName.erase(ii, fasta.length());

    std::string::size_type jj = FileName.find(extended);
    if (jj != std::string::npos)
        FileName.erase(jj, extended.length());

    fo = fopen(FileName.c_str(), "w");
    fprintf(fo, "  ** Valid Codes **:\t%d\n", stats_.goodCodes);
    fprintf(fo, "Raw Forward Codes:\t%d\n", stats_.forwardCodes);
    fprintf(fo, "Raw Backward Codes:\t%d\n", stats_.backwardCodes);
    fprintf(fo, "-------------------\t------------\n");
    fprintf(fo, "Total non-Valid Codes:\t%d\n", stats_.totalBadCodes);
    fprintf(fo, "\n");

    fprintf(fo, "Junk Sequences:\t%d\n", stats_.junkCodes);
    fprintf(fo, "Sequences in File:\t%d\n", stats_.totalSequenceInFile);
    fprintf(fo, "\n");

    fprintf(fo, "AHCA Too Small:\t%d\n", stats_.AHCAtooSmall);
    fprintf(fo, "No AHCA Found:\t%d\n", stats_.NoAHCAfound);
    fprintf(fo, "RC Errors:\t%d\n", stats_.RCerror);

    fprintf(fo, "\n");
    fprintf(fo, "Filtered bad SC1:\t%d\n", stats_.badSC1);
    fprintf(fo, "Filtered bad SC2:\t%d\n", stats_.badSC2);
    fprintf(fo, "Filtered bad SCN:\t%d\n", stats_.badSCN);
    fprintf(fo, "Filtered bad SC3:\t%d\n", stats_.badSC3);
    fprintf(fo, "-------------------\t------------\n");
    fprintf(fo, "Total bad SCs:\t%d\n", stats_.badSC1 + stats_.badSC2 + stats_.badSC3 + stats_.badSCN);
    fprintf(fo, "Non-Overlapping bad SCs:\t%d\n", stats_.ErrorInSC);
    fprintf(fo, "\n");

    fprintf(fo, "Filtered xxx SC1:\t%d\n", stats_.badSC1xxxx);
    fprintf(fo, "Filtered xxx SC2:\t%d\n", stats_.badSC2xxxx);
    fprintf(fo, "Filtered xxx SCN:\t%d\n", stats_.badSCNxxxx);
    fprintf(fo, "Filtered xxx SC3:\t%d\n", stats_.badSC3xxxx);
    fprintf(fo, "-------------------\t------------\n");
    fprintf(fo, "Total xxx SCs:\t%d\n", stats_.badSC1xxxx + stats_.badSC2xxxx + stats_.badSC3xxxx + stats_.badSCNxxxx);

    // calcuate updated num mutations include counts from barcodes and AHCA
    int updated_mutationRate = stats_.mutationRate + stats_.mutationRate_SC3 + stats_.mutationRate_SC2 + stats_.mutationRate_SCN + stats_.mutationRate_SC1 + stats_.mutationRate_AHCA;
    double updated_totalDNARead = stats_.totalDNARead + stats_.totalDNARead_SC3 + stats_.totalDNARead_SC2 + stats_.totalDNARead_SCN + stats_.totalDNARead_SC1 + stats_.totalDNARead_AHCA;
    fprintf(fo, "Mutations in template:\t%d\n", updated_mutationRate);
    fprintf(fo, "Template nucleotides total:\t%.2f\n", updated_totalDNARead);
    fprintf(fo, "Estimated mutation rate:\t%.2f %c\n", 100 * (updated_mutationRate / updated_totalDNARead), '%');
    fprintf(fo, "\n\n\n");

    fprintf(fo, "Time processing      : %.2f seconds\n", totalTime);
    fprintf(fo, "                             ----------------\n");
    fprintf(fo, "     GPU processing time       :  %.2f seconds\n", stats_.gpuTime);
    fprintf(fo, "     Process result from GPU time        :  %.2f seconds\n", stats_.subtractTime);
    fprintf(fo, "     Hashing and saving output       :  %.2f seconds\n\n\n", hashTime);

    for (int i = 0; i < SC_stat.size(); i++)
    {
        fprintf(fo, "%s\n", SC_stat[i].c_str());
    }

    fclose(fo);
}
//minimal change from MyMainClass.cpp
void QBCParser::PostProcess( LegacyLandMines& landmines, Results& results, std::string const& fasta_file, std::string const& out_dir, const int filter
                           , bool & None_, bool const Moderate_, bool const Stringent_, bool const SC3_Remove_, bool const RC_remove_
                           , const time_t& start_file
                           ) {
    printf("\tproc None:\t\t%s\n", None_ ? "True" : "False");
    printf("\tproc Moderate:\t\t%s\n", Moderate_ ? "True" : "False");
    printf("\tproc Stringent:\t\t%s\n", Stringent_ ? "True" : "False");
    printf("\tproc SC3save:\t\t%s\n", SC3_Remove_ ? "True" : "False");
    printf("\tproc RC_remove:\t\t%s\n", RC_remove_ ? "True" : "False");
    printf("\trun_singlets:\t\t%s\n", options_.run_singlets ? "True" : "False");
    printf("\tsave_bad_lines:\t\t%s\n", options_.save_bad_lines ? "True" : "False");
    printf("\tsave_by_cells:\t\t%s\n\n", options_.save_by_cells ? "True" : "False");
    int i = 1;
    time_t start_save, end_save;
    while (i <= 3)
    {
#pragma region Choose Filtering Schema
        string oString = "...";
        if (i == 1 && !options_.none && !options_.moderate && !options_.stringent)
        {
            None_ = true;  oString = "None_a";
        }

        if (i == 1 && (None_ || options_.none))
        {
            options_.none = None_;
            options_.moderate = false;
            options_.stringent = false;
            oString = "None";
        }
        else
        {
            if (i == 2 && Moderate_)
            {
                options_.none = false;
                options_.moderate = Moderate_;
                options_.stringent = false;
                oString = "Mod";
            }
            else
            {
                if (i == 3 && Stringent_)
                {
                    options_.none = false;
                    options_.moderate = false;
                    options_.stringent = Stringent_;
                    oString = "High";
                }
                else {
                    i++; continue;
                }
            }
        }

        char a[50];
        if (RC_remove_)
        {
            sprintf(a, "%s%s", oString.c_str(), "_RCremoved");
        }
        else
        {
            sprintf(a, "%s%s", oString.c_str(), "_RCkept");
        }

#pragma endregion

        time(&start_save);
        printf("\n----------------------------------------------------------------------\n");
        printf("\n%d: Start to save data for %s(%s).\n", i, options_.entry.c_str(), a);

        printf("\tproc None:\t\t%s\n", options_.none ? "True" : "False");
        printf("\tproc Moderate:\t\t%s\n", options_.moderate ? "True" : "False");
        printf("\tproc Stringent:\t\t%s\n", options_.stringent ? "True" : "False");
        printf("\tproc SC3save:\t\t%s\n", SC3_Remove_ ? "True" : "False");
        printf("\tproc RC_remove:\t\t%s\n\n", RC_remove_ ? "True" : "False");

        SaveResultData(landmines, fasta_file, out_dir, results.cell_hash(), results.junk_lines(), results.bad_lines(), filter, a);

        time(&end_save);
        time_t end_file;
        time(&end_file);
        const float totalTime = (float)difftime(end_file, start_file);
        const float hashTime = (float)difftime(end_save, start_save);

        time_t start_time, end_time;
        time(&start_time);
        Statistics(fasta_file, out_dir, results.sc_stat(), totalTime, hashTime);
        time(&end_time);
        float totalTimed = (float)difftime(end_time, start_time);
        printf("\n	Time processing statistics           : %.2f seconds\n", totalTimed);

        time(&start_time);
        ResetStatistics();
        time(&end_time);
        totalTimed = (float)difftime(end_time, start_time);
        printf("	Time processing resetStatistics      : %.2f seconds\n", totalTimed);
        i++;
    }
}

}
