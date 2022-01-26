// SPDX-License-Identifier: GPL-3.0
/*
 * Collector - Data Collector Class
 *
 * Copyright (C) 2021-2022 Felipe Balbi <felipe@balbi.sh>
 */

#include <Collector.hpp>
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

auto Gossip::Collector::collect_data() -> void
{
    for (int i = 0; i < num_samples; ++i) {
        process_directories(output);

        if (i == num_samples - 1) {
            break;
        }

        std::this_thread::sleep_for(seconds);
    }
}

auto Gossip::Collector::process_directories(auto& output) -> void
{
    const std::filesystem::path procfs { "/proc" };
    std::time_t timestamp = std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::now());

    for (auto const& entry : std::filesystem::directory_iterator { procfs }) {
        process_directory(entry, timestamp);
    }
}

auto Gossip::Collector::process_directory(auto& entry, auto timestamp) -> void
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

    parse_directory(entry, process_id, tm);
}

auto Gossip::Collector::parse_directory(auto& entry, auto& process_id, auto& tm)
    -> void
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

    output << process_id << "," << comm << ",";

    parse_and_process_contents(contents, tm);
}

auto Gossip::Collector::read_file(auto& stream, auto& contents) -> void
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

auto Gossip::Collector::parse_and_process_contents(auto& contents, auto& tm)
    -> void
{
    std::regex values_regex("[0-9]+");
    auto values_begin
        = std::sregex_iterator(contents.begin(), contents.end(), values_regex);
    auto values_end = std::sregex_iterator();

    for (std::sregex_iterator i = values_begin; i != values_end; ++i) {
        std::smatch match = *i;
        std::string str = match.str();

        output << str << ",";
    }

    output << std::put_time(&tm, "%F %T %z") << std::endl;
}
