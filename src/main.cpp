// SPDX-License-Identifier: GPL-3.0
/*
 * gossip - Per-process memory usage tracker
 *
 * Copyright (C) 2021-2022 Felipe Balbi <felipe@balbi.sh>
 */
#include <argparse/argparse.hpp>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ranges>
#include <thread>
#include <utility>
#include <regex>

static auto collect_data(int interval, int num_samples)
{
    const std::regex process_regex("([0-9]+)");
    std::smatch process_match;

    for (int i = 0; i < num_samples; ++i) {
        const std::filesystem::path procfs { "/proc" };

        for (auto const& entry :
            std::filesystem::directory_iterator { procfs }) {
            std::string fname = entry.path().filename().string();

            if (!entry.is_directory())
                continue;

            if (!std::regex_match(fname, process_match, process_regex))
                continue;

            std::ifstream process_name;
            std::ifstream process_pss;

            process_name.open(entry.path() / "cmdline");
            if (!process_name.is_open())
                continue;

            process_pss.open(entry.path() / "smaps_rollup");
            if (!process_pss.is_open())
                continue;

            std::string cmdline;
            std::string pss;

            getline(process_name, cmdline);

            if (cmdline.empty())
                continue;

            getline(process_pss, pss);
            getline(process_pss, pss);
            getline(process_pss, pss);

            std::cout << fname << ": " << cmdline << ", " << pss << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::seconds(interval));
    }
}

auto main(int argc, char* argv[]) -> int
{
    constexpr auto program_name = "gossip";
    constexpr auto default_interval = 30;
    constexpr auto default_num_samples = 1000;

    argparse::ArgumentParser program(program_name);

    program.add_argument("-i", "--interval")
        .help("Sampling interval")
        .default_value(default_interval)
        .scan<'i', int>();

    program.add_argument("-n", "--num-samples")
        .help("Stop after this many samples")
        .default_value(default_num_samples)
        .scan<'i', int>();

    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }

    auto interval = program.get<int>("--interval");
    auto num_samples = program.get<int>("--num-samples");

    std::thread collector { collect_data, interval, num_samples };
    collector.join();

    std::cout << interval << " " << num_samples << std::endl;

    return 0;
}
