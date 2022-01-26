// SPDX-License-Identifier: GPL-3.0
/*
 * Collector - Data Collector Class
 *
 * Copyright (C) 2021-2022 Felipe Balbi <felipe@balbi.sh>
 */

#include <Collector.hpp>
#include <Process.hpp>
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

    for (auto const& entry : std::filesystem::directory_iterator { procfs }) {
        try {
            Gossip::Process process { entry };
            process.extract();

            output << process;
        } catch (const std::invalid_argument& err) {
            /* Skipping non-directories */
        } catch (const std::runtime_error& err) {
            /* Skipping empty smaps_rollup */
        }
    }
}
