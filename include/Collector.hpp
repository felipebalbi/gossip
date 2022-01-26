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

namespace Gossip {
class Collector {
public:
    Collector(
        std::chrono::seconds& seconds, int num_samples, std::ostream& output)
        : seconds(seconds)
        , num_samples(num_samples)
        , output(output)
    {
    }

    auto collect_data() -> void;

private:
    auto process_directories() -> void;

    std::chrono::seconds& seconds;
    std::ostream& output;

    int num_samples;
};
};

#endif /* __COLLECTOR_HPP */
