// SPDX-License-Identifier: GPL-3.0
/*
 * gossip - Per-process memory usage tracker
 *
 * Copyright (C) 2021-2022 Felipe Balbi <felipe@balbi.sh>
 */
#include <Collector.hpp>
#include <argparse/argparse.hpp>
#include <fstream>

auto main(int argc, char* argv[]) -> int
{
    constexpr auto program_name = "gossip";
    constexpr auto default_num_samples = 10;
    constexpr auto default_interval = 1;

    argparse::ArgumentParser program(program_name, GOSSIP_VERSION);

    try {
        program.add_argument("-i", "--interval")
            .help("Sampling interval in seconds")
            .default_value(default_interval)
            .scan<'i', int>();

        program.add_argument("-n", "--num-samples")
            .help("Stop after these many samples")
            .default_value(default_num_samples)
            .scan<'i', int>();

        program.add_argument("-p", "--pids")
            .help("Comma separated list of PIDs to track")
            .default_value(std::string(""));

        program.add_argument("-o", "--output")
            .help("Output file name")
            .default_value(std::string("output.csv"));

        program.parse_args(argc, argv);

        auto interval = program.get<int>("--interval");
        auto num_samples = program.get<int>("--num-samples");
        auto pids = program.get<std::string>("--pids");
        auto output = program.get<std::string>("--output");

        auto seconds = std::chrono::seconds(interval);
        std::ofstream output_file(output, std::ios::ate);

        output_file << "# "
                       "PID,Comm,Rss,Pss,Pss_Anon,Pss_File,Pss_Shmem,Shared_"
                       "Clean,Shared_Dirty,"
                       "Private_Clean,Private_Dirty,Referenced,Anonymous,"
                       "LazyFree,AnonHugePages,"
                       "ShmemPmdMapped,FilePmdMapped,Shared_Hugetlb,Private_"
                       "Hugetlb,Swap,SwapPss,"
                       "Locked,Timestamp"
                    << std::endl;

        Gossip::Collector collector { pids, seconds, num_samples, output_file };

        collector.collect_data();
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program << std::endl;
        std::exit(1);
    }

    return 0;
}
