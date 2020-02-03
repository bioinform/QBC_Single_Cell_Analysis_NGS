#ifndef CPPUTIL_UTIL_OPTIONS_PARSER
#define CPPUTIL_UTIL_OPTIONS_PARSER

#include <iostream>
#include <ostream>
#include <boost/program_options.hpp>

namespace di_cpp { namespace util {
//http://www.boost.org/doc/libs/1_61_0/doc/html/program_options/tutorial.html
//http://www.radmangames.com/programming/how-to-use-boost-program_options
class OptionsParser {
public:
    virtual ~OptionsParser() {}
    virtual void PrintStatus(std::ostream &os) const {}  

    bool help() const { return help_; }

    bool error() const { return error_; }

    boost::program_options::options_description const& desc() const { return desc_; }

    boost::program_options::variables_map const& vm() const { return vm_; }

protected:
    decltype(auto) AddOptions() { return desc_.add_options(); }
    decltype(auto) AddExpOptions() { return exp_desc_.add_options(); }
    decltype(auto) AddPosOptions(char const* const c) { return pos_desc_.add(c, 1); }

    OptionsParser(): desc_("Allowed options"), exp_desc_(), pos_desc_(), vm_(), help_(false), error_(false) {
        AddOptions()("help,h", boost::program_options::bool_switch(&help_)->default_value(false), "produce help message");
    }

    bool Parse(int argc, char const*const argv[]) {
        std::cerr << "#command:";
        for (int ii = 0; ii < argc; ++ii) { std::cerr << " " << argv[ii]; }
        std::cerr << std::endl;
        if (ParseHelp(argc, argv)) {
            help_ = true;
            error_ = false;
            return error_;
        }
        return ParseAll(argc, argv);
    }

    bool ParseAll(int argc, char const*const argv[], bool allow_unregistered = false) {
        vm_.clear();
        error_ = false;
        boost::program_options::options_description all_desc;
        all_desc.add(desc_).add(exp_desc_);
        try {
            if (allow_unregistered) {
                boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(all_desc).allow_unregistered().run(), vm_);
            }
            else {
                boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(all_desc).positional(pos_desc_).run(), vm_);
            }
            boost::program_options::notify(vm_);
        }
        catch(boost::program_options::error& e) {
            std::cerr << __FILE__ << ":" << __LINE__ << ": " << e.what() << '\n';
            vm_.clear();
            error_ = true;
        }
        return error_;
    }

    bool ParseHelp(int argc, char const*const argv[]) {
        OptionsParser help_parser;
        auto err = help_parser.ParseAll(argc, argv, true);
        return help_parser.help();
    }

private:
    boost::program_options::options_description desc_;
    boost::program_options::options_description exp_desc_;
    boost::program_options::positional_options_description pos_desc_;
    boost::program_options::variables_map vm_;
    bool help_;
    bool error_;
};

std::ostream& operator<<(std::ostream& os, OptionsParser const& op) {
    os << op.desc();
    return os;
}

} }

#endif
