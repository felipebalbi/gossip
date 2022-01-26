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
#include <iostream>
#include <iterator>
#include <regex>
#include <sstream>
#include <string>
#include <thread>
#include <utility>

static auto parse_and_process_contents(
    auto& contents, auto& tm, auto& output_file) -> void
{
    std::regex values_regex("[0-9]+");
    auto values_begin
        = std::sregex_iterator(contents.begin(), contents.end(), values_regex);
    auto values_end = std::sregex_iterator();

    for (std::sregex_iterator i = values_begin; i != values_end; ++i) {
        std::smatch match = *i;
        std::string str = match.str();

        output_file << str << ",";
    }

    output_file << std::put_time(&tm, "%F %T %z") << std::endl;
}

static auto read_file(auto& stream, auto& contents) -> void
{
    constexpr auto read_size = std::size_t(1024);
    auto buf = std::string(read_size, '\0');

    /*
     * First line contains address space details which we're not
     * interested in. It's safe to just drop it
     */
    stream.getline(buf.data(), read_size, '\n');

    while (stream.read(buf.data(), read_size)) {
        contents.append(buf, 0, stream.gcount());
    }

    contents.append(buf, 0, stream.gcount());
}

static auto parse_directory(
    auto& entry, auto& process_id, auto& tm, auto& output_file) -> void
{
    std::ifstream process_name { entry.path() / "comm" };
    std::ifstream process_smaps { entry.path() / "smaps_rollup" };

    std::string comm;
    getline(process_name, comm);

    std::string contents = std::string();
    read_file(process_smaps, contents);

    /*
     * If the contents of smaps_rollup are empty, ignore this
     * process. It must be a kernel thread, such as a kworker.
     */
    if (contents.empty()) {
        return;
    }

    output_file << process_id << "," << comm << ",";

    parse_and_process_contents(contents, tm, output_file);
}

static auto process_directory(auto& entry, auto timestamp, auto& output_file)
    -> void
{
    std::tm tm = *std::localtime(&timestamp);
    const std::regex process_regex("([0-9]+)");
    std::smatch process_match;

    std::string process_id = entry.path().filename().string();

    if (!entry.is_directory()) {
        return;
    }

    if (!std::regex_match(process_id, process_match, process_regex)) {
        return;
    }

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

static auto collect_data(std::chrono::seconds& seconds, int num_samples,
    std::ofstream& output_file) -> void
{
    for (int i = 0; i < num_samples; ++i) {
        process_directories(output_file);

        if (i == num_samples - 1) {
            break;
        }

        std::this_thread::sleep_for(seconds);
    }
}

auto main(int argc, char* argv[]) -> int
{
    constexpr auto program_name = "gossip";
    constexpr auto default_interval = 1;
    constexpr auto default_num_samples = 10;

    argparse::ArgumentParser program(program_name, GOSSIP_VERSION);

    try {
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

        program.parse_args(argc, argv);
        auto interval = program.get<int>("--interval");
        auto num_samples = program.get<int>("--num-samples");
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

        collect_data(seconds, num_samples, output_file);
    } catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program << std::endl;
        std::exit(1);
    }

    return 0;
}
