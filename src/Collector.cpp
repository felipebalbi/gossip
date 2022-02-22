// SPDX-License-Identifier: GPL-3.0
/*
 * Collector - Data Collector Class
 *
 * Copyright (C) 2021-2022 Felipe Balbi <felipe@balbi.sh>
 */

#include <Collector.hpp>
#include <Process.hpp>
#include <filesystem>
#include <iostream>
#include <iterator>
#include <sstream>
#include <thread>

Gossip::Collector::Collector(
    std::chrono::seconds& seconds, int num_samples, std::ostream& output)
    : seconds(seconds)
    , num_samples(num_samples)
    , output(output)
{
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
    std::time_t timestamp = std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::now());
    std::tm tm = *std::localtime(&timestamp);

    for (auto const& entry : std::filesystem::directory_iterator { procfs }) {
        try {
            Gossip::Process process { entry, tm };
            process.extract();

            output << process;
        } catch (const std::invalid_argument& err) {
            /* Skipping non-directories */
        } catch (const std::runtime_error& err) {
            /* Skipping empty smaps_rollup */
        }
    }
}
