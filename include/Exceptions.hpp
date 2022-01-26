// SPDX-License-Identifier: GPL-3.0
/*
 * Process - Process representation
 *
 * Copyright (C) 2021-2022 Felipe Balbi <felipe@balbi.sh>
 */

#ifndef __EXCEPTIONS_HPP
#define __EXCEPTIONS_HPP

#include <stdexcept>
#include <string>

namespace Gossip {
class empty_process : std::exception {
public:
    explicit empty_process(const std::string& what_arg);
    explicit empty_process(const char* what_arg);
};
};

#endif /* __EXCEPTIONS_HPP */
