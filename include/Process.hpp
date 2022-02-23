// SPDX-License-Identifier: GPL-3.0
/*
 * Process - Process representation
 *
 * Copyright (C) 2021-2022 Felipe Balbi <felipe@balbi.sh>
 */

#ifndef __PROCESS_HPP
#define __PROCESS_HPP

#include <filesystem>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace Gossip {
class Process {
public:
    Process(const std::filesystem::directory_entry& directory)
        : directory(directory)
    {
        pid = -1;
        total_time = 0;
    }

    auto extract() -> void;

    friend std::ostream& operator<<(std::ostream& os, const Process& process)
    {
        os << process.pid << "," << process.comm << ",";

        for (int value : process.values) {
            os << value << ",";
        }

        os << process.total_time << ",";

        return os;
    }

private:
    auto get_pid() -> void;
    auto get_cmdline() -> void;
    auto get_smaps_rollup() -> void;
    auto get_stat() -> void;

    int total_time;
    int pid;

    std::string comm;
    std::vector<int> values;

    const std::filesystem::directory_entry& directory;
};
};

#endif /* __PROCESS_HPP */
