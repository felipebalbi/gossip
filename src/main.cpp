// SPDX-License-Identifier: GPL-3.0
/*
 * gossip - Per-process memory usage tracker
 *
 * Copyright (C) 2021-2022 Felipe Balbi <felipe@balbi.sh>
 */
#include <argparse/argparse.hpp>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <ranges>
#include <regex>
#include <thread>
#include <utility>

static auto process_directory(auto& entry, auto timestamp) -> void
{
    std::tm tm = *std::localtime(&timestamp);
    const std::regex process_regex("([0-9]+)");
    constexpr auto buffer_size = 256 * 1024;
    std::smatch process_match;

    std::string process_id = entry.path().filename().string();

    if (!entry.is_directory())
        return;

    if (!std::regex_match(process_id, process_match, process_regex))
        return;

    std::ifstream process_name;
    std::ifstream process_smaps;

    process_name.open(entry.path() / "cmdline");
    if (!process_name.is_open())
        return;

    process_smaps.open(entry.path() / "smaps_rollup");
    if (!process_smaps.is_open())
        return;

    std::string cmdline;
    std::string smaps_rollup(buffer_size, ' ');

    getline(process_name, cmdline, ' ');

    if (cmdline.empty())
        return;

    process_smaps.read(smaps_rollup.data(), buffer_size);

    const std::regex pss_regex("Pss:\\s+([0-9]+) kB");
    std::smatch pss_match;

    auto ret = std::regex_search(smaps_rollup, pss_match, pss_regex);
    if (!ret)
        return;

    auto pss = pss_match[1].str();

    std::cout.imbue(std::locale("en_US.utf8"));
    std::cout << process_id << ": " << cmdline << "," << pss << ","
              << std::put_time(&tm, "%F %T %z") << std::endl;
}

static auto process_directories(auto& procfs) -> void
{
    std::time_t timestamp = std::time(nullptr);

    for (auto const& entry : std::filesystem::directory_iterator { procfs }) {
        process_directory(entry, timestamp);
    }
}

static auto collect_data(int interval, int num_samples) -> void
{
    for (int i = 0; i < num_samples; ++i) {
        const std::filesystem::path procfs { "/proc" };

        process_directories(procfs);

        if (i == num_samples - 1)
            break;

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

    return 0;
}
