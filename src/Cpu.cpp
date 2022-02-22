// SPDX-License-Identifier: GPL-3.0
/*
 * Cpu - Cpu representation
 *
 * Copyright (C) 2021-2022 Felipe Balbi <felipe@balbi.sh>
 */

#include <Cpu.hpp>

#include <fstream>
#include <functional>
#include <iostream>
#include <numeric>
#include <regex>
#include <sstream>
#include <string>

auto Gossip::Cpu::extract() -> void
{
    get_stat();
    get_num_cpus();
}

auto Gossip::Cpu::get_stat() -> void
{
    std::ifstream stat { directory.path() / "stat" };
    std::string first_line;
    std::string line;

    std::getline(stat, first_line);

    std::istringstream ss { first_line };

    /* First column contains the string 'cpu', skip it */
    std::getline(ss, line, ' ');
    std::getline(ss, line, ' ');

    while (std::getline(ss, line, ' ')) {
        values.push_back(std::stoi(line));
    }

    total_time = std::accumulate(values.begin(), values.end(), 0);
}

auto Gossip::Cpu::get_num_cpus() -> void
{
    std::ifstream cpuinfo { directory.path() / "cpuinfo" };
    std::string line;

    while (std::getline(cpuinfo, line)) {
        if (line.starts_with("processor"))
            num_cpus += 1;
    }
}
