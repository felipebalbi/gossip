// SPDX-License-Identifier: GPL-3.0
/*
 * Process - Process representation
 *
 * Copyright (C) 2021-2022 Felipe Balbi <felipe@balbi.sh>
 */

#include <Process.hpp>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

auto Gossip::Process::extract() -> void
{
    std::time_t timestamp = std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::now());

    tm = *std::localtime(&timestamp);

    get_pid();
    get_cmdline();
    get_values();
}

auto Gossip::Process::get_pid() -> void
{
    if (!directory.is_directory()) {
        throw std::invalid_argument { "Expecting a directory" };
    }

    std::string process_id = directory.path().filename().string();

    /*
     * If this is not the directory for a process, `std::stoi()' will
     * throw `std::invalid_argument'. Let it happen, upper layers will
     * just ignore the directory.
     */
    pid = std::stoi(process_id);
}

auto Gossip::Process::get_cmdline() -> void
{
    std::ifstream process_name { directory.path() / "cmdline" };
    std::string cmdline;

    getline(process_name, cmdline, '\0');

    std::istringstream iss(cmdline);
    getline(iss, comm, ' ');

    if (comm.empty()) {
        comm = "unknown";
    }
}

auto Gossip::Process::get_values() -> void
{
    std::ifstream process_smaps { directory.path() / "smaps_rollup" };
    std::string contents = std::string();

    constexpr auto read_size = std::size_t(1024);
    auto buf = std::string(read_size, '\0');

    /*
     * First line contains address space details which we're not
     * interested in. It's safe to just drop it
     */
    process_smaps.getline(buf.data(), read_size, '\n');

    while (process_smaps.read(buf.data(), read_size)) {
        contents.append(buf, 0, process_smaps.gcount());
    }

    contents.append(buf, 0, process_smaps.gcount());

    /*
     * If the contents of smaps_rollup are empty, ignore this
     * process. It must be a kernel thread, such as a kworker.
     */
    if (contents.empty()) {
        throw std::runtime_error { "No data for `" + comm + "'" + ":"
            + std::to_string(pid) };
    }

    std::regex values_regex("[0-9]+");
    auto values_begin
        = std::sregex_iterator(contents.begin(), contents.end(), values_regex);
    auto values_end = std::sregex_iterator();

    for (std::sregex_iterator i = values_begin; i != values_end; ++i) {
        std::smatch match = *i;
        std::string str = match.str();
        int value = std::stoi(str);

        values.push_back(value);
    }
}