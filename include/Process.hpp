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
#include <string>
#include <vector>

namespace Gossip {
class Process {
public:
    Process(const std::filesystem::directory_entry& directory)
        : directory(directory)
    {
        pid = -1;
    }

    auto extract() -> void;

    friend std::ostream& operator<<(std::ostream& os, const Process& process)
    {
        os << process.pid << "," << process.comm << ",";

        for (int value : process.values) {
            os << value << ",";
        }

        os << std::put_time(&process.tm, "%F %T %z") << std::endl;

        return os;
    }

private:
    auto get_pid() -> void;
    auto get_cmdline() -> void;
    auto get_values() -> void;
    
    int pid;
    std::tm tm;
    std::string comm;
    std::vector<int> values;
    const std::filesystem::directory_entry& directory;
};
};

#endif /* __PROCESS_HPP */
