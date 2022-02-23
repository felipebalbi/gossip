// SPDX-License-Identifier: GPL-3.0
/*
 * Test cases
 *
 * Copyright (C) 2021-2022 Felipe Balbi <felipe@balbi.sh>
 */
#include <Cpu.hpp>
#include <catch2/catch.hpp>
#include <filesystem>
#include <fstream>
#include <sstream>

TEST_CASE("CPU can extract its data", "[Cpu]")
{
    std::filesystem::current_path(std::filesystem::temp_directory_path());
    std::filesystem::create_directory("proc");

    const std::filesystem::path proc { std::filesystem::temp_directory_path()
        / "proc" };
    const std::filesystem::directory_entry procdir { proc };

    SECTION("extracting data initializes relevant fields")
    {
        const std::filesystem::path cpuinfo { proc / "cpuinfo" };
        const std::filesystem::path stat { proc / "stat" };

        std::ofstream cpuinfo_stream { cpuinfo };
        std::ofstream stat_stream { stat };

        cpuinfo_stream.seekp(0);
        cpuinfo_stream
            << "processor\nprocessor\nprocessor\nprocessor\nprocessor\n"
               "processor\nprocessor\nprocessor\nprocessor\nprocessor\n"
               "processor\nprocessor\nprocessor\nprocessor\nprocessor\n"
               "processor\nprocessor\nprocessor\nprocessor\nprocessor\n"
            << std::endl;

        stat_stream.seekp(0);
        stat_stream << "cpu  1 2 3 4 5 6 7 8 9 10" << std::endl;

        Gossip::Cpu cpu { procdir };

        std::ostringstream expected {};
        std::ostringstream output {};

        expected << "55,20,";

        cpu.extract();
        output << cpu;

        REQUIRE(output.str() == expected.str());
    }

    SECTION("extracts data correctly regardless of number of processors")
    {
        const std::filesystem::path cpuinfo { proc / "cpuinfo" };
        const std::filesystem::path stat { proc / "stat" };

        std::ofstream cpuinfo_stream { cpuinfo };
        std::ofstream stat_stream { stat };

        cpuinfo_stream.seekp(0);
        cpuinfo_stream
            << "processor\nprocessor\nprocessor\nprocessor\nprocessor\n"
               "processor\nprocessor\nprocessor\nprocessor\nprocessor\n"
               "processor\nprocessor\nprocessor\nprocessor\nprocessor\n"
               "processor\nprocessor\nprocessor\nprocessor\nprocessor\n"
               "processor\nprocessor\nprocessor\nprocessor\nprocessor\n"
               "processor\nprocessor\nprocessor\nprocessor\nprocessor\n"
               "processor\nprocessor\nprocessor\nprocessor\nprocessor\n"
               "processor\nprocessor\nprocessor\nprocessor\nprocessor\n"
               "processor\nprocessor\nprocessor\nprocessor\nprocessor\n"
               "processor\nprocessor\nprocessor\nprocessor\nprocessor\n"
               "processor\nprocessor\nprocessor\nprocessor\nprocessor\n"
               "processor\nprocessor\nprocessor\nprocessor\nprocessor\n"
               "processor\nprocessor\nprocessor\nprocessor\nprocessor\n"
               "processor\nprocessor\nprocessor\nprocessor\nprocessor\n"
               "processor\nprocessor\nprocessor\nprocessor\nprocessor\n"
               "processor\nprocessor\nprocessor\nprocessor\nprocessor\n"
               "processor\nprocessor\nprocessor\nprocessor\nprocessor\n"
               "processor\nprocessor\nprocessor\nprocessor\nprocessor\n"
               "processor\nprocessor\nprocessor\nprocessor\nprocessor\n"
               "processor\nprocessor\nprocessor\nprocessor\nprocessor\n"
            << std::endl;

        stat_stream.seekp(0);
        stat_stream << "cpu  1 2 3 4 5 6 7 8 9 10" << std::endl;

        Gossip::Cpu cpu { procdir };

        std::ostringstream expected {};
        std::ostringstream output {};

        expected << "55,100,";

        cpu.extract();
        output << cpu;

        REQUIRE(output.str() == expected.str());
    }

    SECTION("extracts data correctly regardless of CPU schedule time")
    {
        const std::filesystem::path cpuinfo { proc / "cpuinfo" };
        const std::filesystem::path stat { proc / "stat" };

        std::ofstream cpuinfo_stream { cpuinfo };
        std::ofstream stat_stream { stat };

        cpuinfo_stream.seekp(0);
        cpuinfo_stream
            << "processor\nprocessor\nprocessor\nprocessor\nprocessor\n"
               "processor\nprocessor\nprocessor\nprocessor\nprocessor\n"
               "processor\nprocessor\nprocessor\nprocessor\nprocessor\n"
               "processor\nprocessor\nprocessor\nprocessor\nprocessor\n"
            << std::endl;

        stat_stream.seekp(0);
        stat_stream << "cpu  10 20 30 40 50 60 70 80 90 100" << std::endl;

        Gossip::Cpu cpu { procdir };

        std::ostringstream expected {};
        std::ostringstream output {};

        expected << "550,20,";

        cpu.extract();
        output << cpu;

        REQUIRE(output.str() == expected.str());
    }

    std::filesystem::remove_all("proc");
}
