// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * gossip - Per-process memory usage tracker
 *
 * Copyright (C) 2021 Microsoft Corporation
 *
 * Author: Felipe Balbi <felipe.balbi@microsoft.com>
 */
#include <catch2/catch.hpp>
#include <Smaps.hpp>

#include <random>
#include <string>

#define SMAPS_CONTENT \
"1bf24dc1000-7fff55198000 ---p 00000000 00:00 0                           [rollup]\
Rss:               24540 kB\
Pss:                5972 kB\
Pss_Anon:           4990 kB\
Pss_File:            982 kB\
Pss_Shmem:             0 kB\
Shared_Clean:      15420 kB\
Shared_Dirty:       8260 kB\
Private_Clean:         0 kB\
Private_Dirty:       860 kB\
Referenced:        23460 kB\
Anonymous:          9120 kB\
LazyFree:              0 kB\
AnonHugePages:         0 kB\
ShmemPmdMapped:        0 kB\
FilePmdMapped:         0 kB\
Shared_Hugetlb:        0 kB\
Private_Hugetlb:       0 kB\
Swap:                  0 kB\
SwapPss:               0 kB\
Locked:                0 kB"

SCENARIO("smaps can be parsed", "[Smaps]")
{

    GIVEN("An smaps_rollup content")
    {
        std::string str(SMAPS_CONTENT);

        AND_GIVEN("An instance of Gossip::Smaps")
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<int> pid(1, 1000);
            int random_pid = pid(gen);
            Gossip::Smaps smaps(random_pid, str);

            WHEN("the pid is requested")
            {

                THEN("it should match the initialization")
                {
                    REQUIRE(smaps.get_pid() == random_pid);
                }
            }
        }
    }
}
