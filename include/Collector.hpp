// SPDX-License-Identifier: GPL-3.0
/*
 * Collector - Data Collector Class
 *
 * Copyright (C) 2021-2022 Felipe Balbi <felipe@balbi.sh>
 */

#ifndef __COLLECTOR_HPP
#define __COLLECTOR_HPP

#include <chrono>
#include <iostream>
#include <set>
#include <string>

namespace Gossip {
class Collector {
public:
    Collector(std::string& pids_str, std::chrono::seconds& seconds,
        int num_samples, std::ostream& output);

    auto collect_data() -> void;

private:
    auto process_directories() -> void;

    std::chrono::seconds& seconds;
    std::set<int> pids;
    std::ostream& output;

    int num_samples;
};
};

#endif /* __COLLECTOR_HPP */
