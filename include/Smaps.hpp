// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * gossip - Per-process memory usage tracker
 *
 * Copyright (C) 2021 Microsoft Corporation
 *
 * Author: Felipe Balbi <felipe.balbi@microsoft.com>
 */

#ifndef SMAPS_HPP
#define SMAPS_HPP

#include <string>

namespace Gossip {

class Smaps {
public:
    Smaps(int pid, std::string data)
        : pid(pid) {};

    auto get_pid() -> int;

private:
    int pid;
};
};

#endif /* SMAPS_HPP */
