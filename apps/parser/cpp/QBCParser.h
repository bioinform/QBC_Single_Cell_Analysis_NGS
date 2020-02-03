#pragma once
#include <tuple>
#include <vector>
#include <string>
#include <map>
#include "../StringAnylizerGUI/kernel.h"
#include "Resources.h"
namespace rss {
    // This is an interface class to divide the legacy code into CLI/GUI and C++/Cuda
    // This is just initial quick-and-dirty work trying to achieve linux run-time with  minimal code change
    class QBCParser
    {
    public:
        struct Options {
            enum { DEFAULT_PCR_CROSSOVER_THRESHOLD = 3 };
            Options()
                : entry("C:\\")
                , save_bad_lines(false)
                , save_by_cells(false)
                , stringent(false)
                , moderate(false)
                , none(false)
                , sc3_remove(false)
                , rc_remove(true)
                , run_singlets(true)
                , pcr_crossover_threshold(DEFAULT_PCR_CROSSOVER_THRESHOLD)
            {}
            std::string entry;
            bool save_bad_lines;
            bool save_by_cells;
            bool stringent;
            bool moderate;
            bool none;
            bool sc3_remove;
            bool rc_remove;
            bool run_singlets;
            int pcr_crossover_threshold;
	};
        struct Results {
            using Cells = std::vector<Cell>;
            using Strings = std::vector<std::string>;
            Cells& cell_hash() { return cell_hash_; }
            Strings& junk_lines() { return junk_lines_; }
            Strings& bad_lines() { return bad_lines_; }
            Strings& sc_stat() { return sc_stat_; }
            Cells const& cell_hash() const { return cell_hash_; }
            Strings const& junk_lines() const { return junk_lines_; }
            Strings const& bad_lines() const { return bad_lines_; }
            Strings const& sc_stat() const { return sc_stat_; }
        private:
            Cells cell_hash_;
            Strings junk_lines_;
            Strings bad_lines_;
            Strings sc_stat_;
        };

        // the underlying legacy code is NOT safe, at this stage without more work, be adviced that it's effectively a singleton
        static QBCParser& Instance() {
            static QBCParser instance;
            return instance;
        }

        int Initialize( std::string const& directory
                      , bool const save_bad_lines
                      , int const mutation_allowed
                      , int const device_id);

        //bad interface from legacy code, will change later
        void ProcessFile( LegacyLandMines& landmines, std::string const& fasta_file, std::string const& outdir, const int filter
                        , bool & None_, bool const Moderate_, bool const Stringent_, bool const SC3_Remove_, bool const RC_remove_
                        ){
            time_t start_file;
            time(&start_file);
            auto results = Process(fasta_file, landmines);
            PostProcess(landmines, results, fasta_file, outdir, filter, None_, Moderate_, Stringent_, SC3_Remove_, RC_remove_, start_file);
        }

        void CleanUpMem();

        Options& options() {
            return options_;
        }

        std::vector<std::string> CheckDevice() const;

    private:
        ~QBCParser() {
        }

        Results Process(std::string const& file, LegacyLandMines& landmines);

        void PostProcess(LegacyLandMines& landmines, Results& results, std::string const& fasta_file, const std::string& outdir
                , const int filter, bool & None_, bool const Moderate_, bool const Stringent_, bool const SC3_Remove_, bool const RC_remove_
                , const time_t& start_file
            );

        //bad interface from legacy code, will change later
        void SaveResultData(LegacyLandMines& landmines, const string& fileName, const string& outdir, vector<Cell> & cellHash, const vector<string>& junklines, const vector<string>& badlines, int flag, char b[50]);
        //bad interface from legacy code, will change later
        void Statistics(std::string const& fasta_file, const string& outdir, std::vector<std::string> SC_stat, const float totalTime, const float hashTime) const;
        std::tuple<int,int> GetRandomCodeNumber(char * const random_code_buffer, int const random_code_size, int * const random_num_buffer);

        Options options_;
        StatisticsInfo stats_; //from legacy code. need a second look
        CellHashList cell_hash_list_; //from legacy code. need a second look, should be able to be scoped in SaveResultData
        LegacyBuffers buffers_;

        void ResetStatistics() {
            stats_.reset();
            cell_hash_list_.clear();
        }
        void SendVarstoForm(LegacyLandMines& landmines, string whereatString, int & doubleme, int & jj, int &kk, int & ll, int & greaterthan, char b[50]);
    };
}
