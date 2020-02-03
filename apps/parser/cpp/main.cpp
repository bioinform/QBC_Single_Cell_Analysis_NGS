#include "QBCParser.h"
#include "ProgramOptions.h"
#include "Timer.h"

int Driver(int argc, char** argv) {
    const rss::qbc::ProgramOptions opts(argc, argv);
    if (opts.error() or opts.help()) {
        std::cerr << opts << std::endl;
        return 1;
    }
    opts.PrintStatus(std::cout);

    if (!boost::filesystem::exists(opts.out_dir())) {
      boost::filesystem::create_directory(opts.out_dir());
    }
    if (!boost::filesystem::is_directory(opts.out_dir())) {
      throw runtime_error(opts.out_dir().string() + "does not exist or cannot be created\n");
    }

    rss::QBCParser& qbc_instance = rss::QBCParser::Instance();
    LegacyLandMines landmines;

    qbc_instance.options().none = opts.none();
    qbc_instance.options().moderate = opts.moderate();
    qbc_instance.options().stringent = opts.stringent();
    qbc_instance.options().sc3_remove = opts.sc3_remove();
    qbc_instance.options().rc_remove = opts.rc_remove();
    qbc_instance.options().run_singlets = opts.run_singlets();
    qbc_instance.options().save_bad_lines = opts.save_bad_lines();
    qbc_instance.options().save_by_cells = opts.save_by_cells();
    qbc_instance.options().entry = (*boost::filesystem::canonical(opts.input_dir()).rbegin()).string();
    qbc_instance.options().pcr_crossover_threshold = opts.pcr_crossover_threshold();

    int filter = 0;
    int code = qbc_instance.Initialize(boost::filesystem::canonical(opts.input_dir()).string(), opts.save_bad_lines(), opts.mutation_number(), opts.device_id());
    if (code == 0) {
        throw std::runtime_error("qbc initialization error code 0\n");
    }

    auto f = opts.fasta();
    bool tmp_none = opts.none();
    qbc_instance.ProcessFile(landmines, boost::filesystem::canonical(f).string(), opts.out_dir().string(), filter, tmp_none, opts.moderate(), opts.stringent(), opts.sc3_remove(), opts.rc_remove());

    qbc_instance.CleanUpMem();
    return 0;
}

int main(int argc, char** argv) {
    DITIMER()
    return Driver(argc, argv);
}
