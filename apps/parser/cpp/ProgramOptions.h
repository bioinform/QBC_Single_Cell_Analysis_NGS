#ifndef PROGRAM_OPTIONS_H_
#define PROGRAM_OPTIONS_H_

#include <boost/filesystem.hpp>
#include <cpputil/util/OptionsParser.h>
#include "QBCParser.h" 

namespace rss { namespace qbc {

struct ProgramOptions : public di_cpp::util::OptionsParser {
    ProgramOptions(const int argc, char const* const argv[]) {
        AddOptions()("fasta,f", boost::program_options::value<boost::filesystem::path>(&fasta_)->required(), "single input fasta file");
        AddOptions()("input_dir,i", boost::program_options::value<boost::filesystem::path>(&input_dir_)->required(), "input directory for qdata");
        AddOptions()("out_dir,o", boost::program_options::value<boost::filesystem::path>(&out_dir_)->required(), "output directory");
        AddOptions()("save_bad_lines", boost::program_options::value<bool>(&save_bad_lines_)->default_value(false), "save_bad_lines as in legacy codes");
        AddOptions()("save_by_cells", boost::program_options::value<bool>(&save_by_cells_)->default_value(false), "save_by_cells as in legacy codes");
        AddOptions()("stringent", boost::program_options::value<bool>(&stringent_)->default_value(false), "stringent as in legacy codes");
        AddOptions()("moderate", boost::program_options::value<bool>(&moderate_)->default_value(true), "moderate as in legacy codes");
        AddOptions()("none", boost::program_options::value<bool>(&none_)->default_value(false), "none as in legacy codes");
        AddOptions()("sc3_remove", boost::program_options::value<bool>(&sc3_remove_)->default_value(false), "sc3_remove as in legacy codes");
        AddOptions()("rc_remove", boost::program_options::value<bool>(&rc_remove_)->default_value(true), "rc_remove as in legacy codes");
        AddOptions()("run_singlets", boost::program_options::value<bool>(&run_singlets_)->default_value(true), "run_singlets as in legacy codes");
        AddOptions()("mutation_number", boost::program_options::value<int>(&mutation_number_)->default_value(1), "mutation_number allowed as in legacy codes");
        AddOptions()("device_id", boost::program_options::value<int>(&device_id_)->default_value(0), "device_id as in legacy codes");
        AddOptions()("pcr_crossover_threshold,c", boost::program_options::value<int>(&pcr_crossover_threshold_)->default_value(QBCParser::Options::DEFAULT_PCR_CROSSOVER_THRESHOLD), "when pcr crossover detected, program will remove uid family with size strictly below this threshold.");
        Parse(argc, argv);
    }


    decltype(auto) pcr_crossover_threshold() const {
      return (pcr_crossover_threshold_);
    }

    decltype(auto) input_dir() const {
        return (input_dir_);
    }

    decltype(auto) out_dir() const {
        return (out_dir_);
    }

    decltype(auto) fasta() const {
        return (fasta_);
    }

    decltype(auto) save_bad_lines() const {
        return (save_bad_lines_);
    }

    decltype(auto) save_by_cells() const {
        return (save_by_cells_);
    }

    decltype(auto) stringent() const {
        return (stringent_);
    }

    decltype(auto) moderate() const {
        return (moderate_);
    }

    decltype(auto) none() const {
        return (none_);
    }

    decltype(auto) none() {
        return (none_);
    }

    decltype(auto) sc3_remove() const {
        return (sc3_remove_);
    }

    decltype(auto) rc_remove() const {
        return (rc_remove_);
    }

    decltype(auto) run_singlets() const {
        return (run_singlets_);
    }

    decltype(auto) mutation_number() const {
        return (mutation_number_);
    }

    decltype(auto) device_id() const {
        return (device_id_);
    }

    void PrintStatus(std::ostream& os) const {
        os << "fasta file: " << fasta_.string() << std::endl;;
        os << "input directory: " << input_dir_.string() << std::endl;
        os << "output directory: " << out_dir_.string() << std::endl;
        os << std::boolalpha << "save bad lines: " << save_bad_lines_ << std::endl;
        os << std::boolalpha << "save by cells: " << save_by_cells_ << std::endl;
        os << std::boolalpha << "stringent: " << stringent_ << std::endl;
        os << std::boolalpha << "moderate: " << moderate_ << std::endl;
        os << std::boolalpha << "none: " << none_ << std::endl;
        os << std::boolalpha << "sc3_remove: " << sc3_remove_ << std::endl;
        os << std::boolalpha << "rc_remove: " << rc_remove_ << std::endl;
        os << std::boolalpha << "run_singlets: " << run_singlets_ << std::endl;
        os << "mutaiton_number: " << mutation_number_ << std::endl;
        os << "device_id: " << device_id_ << std::endl;
    }


private:
    boost::filesystem::path input_dir_;
    boost::filesystem::path out_dir_;
    boost::filesystem::path fasta_;

    bool save_bad_lines_;
    bool save_by_cells_;
    bool stringent_;
    bool moderate_;
    bool none_;
    bool sc3_remove_;
    bool rc_remove_;
    bool run_singlets_;
    int mutation_number_;
    int device_id_;
    int pcr_crossover_threshold_;
};


} }
#endif
