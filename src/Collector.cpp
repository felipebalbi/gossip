// SPDX-License-Identifier: GPL-3.0
/*
 * Collector - Data Collector Class
 *
 * Copyright (C) 2021-2022 Felipe Balbi <felipe@balbi.sh>
 */

#include <Collector.hpp>
#include <Cpu.hpp>
#include <Process.hpp>
#include <filesystem>
#include <iostream>
#include <iterator>
#include <set>
#include <sstream>
#include <string>
#include <thread>

Gossip::Collector::Collector(std::string& pids_str,
    std::chrono::seconds& seconds, int num_samples, std::ostream& output)
    : seconds(seconds)
    , num_samples(num_samples)
    , output(output)
{
    if (pids_str.empty())
        return;

    std::istringstream ss { pids_str };
    std::string pid;

    while (std::getline(ss, pid, ',')) {
        try {
            pids.insert(std::stoi(pid));
        } catch (const std::exception& err) {
            std::cerr << "Skipping invalid PID: " << pid << std::endl;
            continue;
        }
    }
}

auto Gossip::Collector::collect_data() -> void
{
    for (int i = 0; i < num_samples; ++i) {
        process_directories();

        if (i == num_samples - 1) {
            break;
        }

        std::this_thread::sleep_for(seconds);
    }
}

auto Gossip::Collector::process_directories() -> void
{
    const std::filesystem::path procfs { "/proc" };
    const std::filesystem::directory_entry procdir { procfs };

    std::time_t timestamp = std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::now());
    std::tm tm = *std::localtime(&timestamp);

    for (auto const& entry : std::filesystem::directory_iterator { procfs }) {
        int pid;

        /*
         * If this is not the directory for a process, `std::stoi()'
         * will throw `std::invalid_argument'. Let's catch it here and
         * skip the directory right away.
         */
        try {
            pid = std::stoi(entry.path().filename());
        } catch (const std::invalid_argument& err) {
            /* Skipping non-process directories */
            continue;
        }

        /*
         * If user requested a specific list of PIDs to be tracked and
         * that list does not contain the current PID, just silently
         * skip it.
         */
        if (!pids.empty() && !pids.contains(pid)) {
            continue;
        }

        try {
            Gossip::Process process { entry, tm };
            Gossip::Cpu cpu { procdir };

            cpu.extract();
            process.extract();

            output << cpu;
            output << process;
        } catch (const std::invalid_argument& err) {
            /* Skipping non-directories */
        } catch (const std::runtime_error& err) {
            /* Skipping empty smaps_rollup */
        }
    }
}
