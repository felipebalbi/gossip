// SPDX-License-Identifier: GPL-3.0
/*
 * Cpu - Cpu representation
 *
 * Copyright (C) 2021-2022 Felipe Balbi <felipe@balbi.sh>
 */

#ifndef __CPU_HPP
#define __CPU_HPP

#include <filesystem>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace Gossip {
class Cpu {
public:
    Cpu(const std::filesystem::directory_entry& directory)
        : directory(directory)
    {
        total_time = 0;
        num_cpus = 0;
    }

    auto extract() -> void;

    friend std::ostream& operator<<(std::ostream& os, const Cpu& cpu)
    {
        os << cpu.total_time << "," << cpu.num_cpus << ",";

        return os;
    }

private:
    auto get_stat() -> void;
    auto get_num_cpus() -> void;

    int total_time;
    int num_cpus;

    std::vector<int> values;

    const std::filesystem::directory_entry& directory;
};
};

#endif /* __CPU_HPP */
