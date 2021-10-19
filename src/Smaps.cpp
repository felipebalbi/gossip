// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * gossip - Per-process memory usage tracker
 *
 * Copyright (C) 2021 Microsoft Corporation
 *
 * Author: Felipe Balbi <felipe.balbi@microsoft.com>
 */

#include <Smaps.hpp>

#include <string>

auto Gossip::Smaps::get_pid() -> int { return pid; }
