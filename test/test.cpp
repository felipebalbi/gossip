// SPDX-License-Identifier: GPL-3.0
/*
 * Test cases
 *
 * Copyright (C) 2021-2022 Felipe Balbi <felipe@balbi.sh>
 */
#include <Process.hpp>
#include <catch2/catch_all.hpp>
#include <filesystem>
#include <fstream>
#include <sstream>

TEST_CASE("Processes can extract their data", "[Process]")
{
    std::filesystem::current_path(std::filesystem::temp_directory_path());
    std::filesystem::create_directory("proc");

    const std::filesystem::path proc { std::filesystem::temp_directory_path()
        / "proc" };

    int pid = GENERATE(take(50, random(1, 100000)));

    const std::filesystem::path base { proc / std::to_string(pid) };

    std::filesystem::create_directory(base);

    const std::filesystem::path cmdline { base / "cmdline" };
    const std::filesystem::path smaps_rollup { base / "smaps_rollup" };

    std::ofstream cmdline_stream { cmdline };
    std::ofstream smaps_stream { smaps_rollup };

    cmdline_stream << "process" << pid;
    cmdline_stream.flush();

    smaps_stream << "56011b60e000-7ffcff7ca000 ---p 00000000 00:00 0"
                    "                   [rollup]\n"
                    "Rss:                   1 kB\n"
                    "Pss:                   2 kB\n"
                    "Pss_Anon:              3 kB\n"
                    "Pss_File:              4 kB\n"
                    "Pss_Shmem:             5 kB\n"
                    "Shared_Clean:          6 kB\n"
                    "Shared_Dirty:          7 kB\n"
                    "Private_Clean:         8 kB\n"
                    "Private_Dirty:         9 kB\n"
                    "Referenced:           10 kB\n"
                    "Anonymous:            11 kB\n"
                    "LazyFree:             12 kB\n"
                    "AnonHugePages:        13 kB\n"
                    "ShmemPmdMapped:       14 kB\n"
                    "FilePmdMapped:        15 kB\n"
                    "Shared_Hugetlb:       16 kB\n"
                    "Private_Hugetlb:      17 kB\n"
                    "Swap:                 18 kB\n"
                    "SwapPss:              19 kB\n"
                    "Locked:               20 kB"
                 << std::endl;

    const std::filesystem::directory_entry entry { base };

    REQUIRE(entry.is_directory());

    std::time_t timestamp = std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::now());
    std::tm tm = *std::localtime(&timestamp);

    Gossip::Process process { entry, tm };

    SECTION("extracting data initializes relevant fields")
    {
        std::ostringstream expected {};
        std::ostringstream output {};

        expected << pid << ",process" << pid
                 << ",1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,"
                 << std::put_time(&tm, "%F %T %z") << std::endl;

        process.extract();
        output << process;

        REQUIRE(output.str() == expected.str());
    }

    std::filesystem::remove_all("proc");
}
