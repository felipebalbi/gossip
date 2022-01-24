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
#include <sstream>
#include <thread>
#include <utility>

static auto parse_directory(
    auto& entry, auto& process_id, auto& tm, auto& output_file) -> void
{
    std::ifstream process_name { entry.path() / "comm" };
    std::ifstream process_smaps { entry.path() / "smaps_rollup" };

    std::string comm;
    getline(process_name, comm);

    std::string line;
    while (getline(process_smaps, line)) {
        const std::regex pss_regex("^Pss:\\s+([0-9]+) kB$");
        std::smatch pss_match;

        auto ret = std::regex_search(line, pss_match, pss_regex);
        if (!ret)
            continue;

        auto pss = pss_match[1].str();

        output_file << process_id << "," << comm << "," << pss << ","
                    << std::put_time(&tm, "%F %T %z") << std::endl;
    }
}

static auto process_directory(auto& entry, auto timestamp, auto& output_file)
    -> void
{
    std::tm tm = *std::localtime(&timestamp);
    const std::regex process_regex("([0-9]+)");
    std::smatch process_match;

    std::string process_id = entry.path().filename().string();

    if (!entry.is_directory())
        return;

    if (!std::regex_match(process_id, process_match, process_regex))
        return;

    parse_directory(entry, process_id, tm, output_file);
}

static auto process_directories(auto& output_file) -> void
{
    const std::filesystem::path procfs { "/proc" };
    std::time_t timestamp = std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::now());

    for (auto const& entry : std::filesystem::directory_iterator { procfs }) {
        process_directory(entry, timestamp, output_file);
    }
}

static auto collect_data(
    int interval, int num_samples, std::ofstream& output_file) -> void
{
    for (int i = 0; i < num_samples; ++i) {
        process_directories(output_file);

        if (i == num_samples - 1)
            break;

        std::this_thread::sleep_for(std::chrono::seconds(interval));
    }
}

auto main(int argc, char* argv[]) -> int
{
    constexpr auto program_name = "gossip";
    constexpr auto default_interval = 1;
    constexpr auto default_num_samples = 10;

    argparse::ArgumentParser program(program_name);

    program.add_argument("-i", "--interval")
        .help("Sampling interval in seconds")
        .default_value(default_interval)
        .scan<'i', int>();

    program.add_argument("-n", "--num-samples")
        .help("Stop after this many samples")
        .default_value(default_num_samples)
        .scan<'i', int>();

    program.add_argument("-o", "--output")
        .help("Output file name")
        .default_value(std::string("output.csv"));

    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }

    auto interval = program.get<int>("--interval");
    auto num_samples = program.get<int>("--num-samples");
    auto output = program.get<std::string>("--output");

    std::ofstream output_file(output, std::ios::ate);
    collect_data(interval, num_samples, output_file);

    return 0;
}
