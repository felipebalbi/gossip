// SPDX-License-Identifier: GPL-3.0
/*
 * gossip - Per-process memory usage tracker
 *
 * Copyright (C) 2021-2022 Felipe Balbi <felipe@balbi.sh>
 */
#include <argparse/argparse.hpp>
#include <chrono>
#include <iostream>
#include <thread>
#include <utility>

static auto collect_data(int interval, int num_samples)
{
    for (int i = 0; i < num_samples; ++i) {
        std::cout << "Thread 1 executing\n";
        std::this_thread::sleep_for(std::chrono::seconds(interval));
    }
}

auto main(int argc, char* argv[]) -> int
{
    constexpr auto program_name = "gossip";
    constexpr auto default_interval = 30;
    constexpr auto default_num_samples = 1000;

    argparse::ArgumentParser program(program_name);

    program.add_argument("-i", "--interval")
        .help("Sampling interval")
        .default_value(default_interval)
        .scan<'i', int>();

    program.add_argument("-n", "--num-samples")
        .help("Stop after this many samples")
        .default_value(default_num_samples)
        .scan<'i', int>();

    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }

    auto interval = program.get<int>("--interval");
    auto num_samples = program.get<int>("--num-samples");

    std::thread collector { collect_data, interval, num_samples };
    collector.join();

    std::cout << interval << " " << num_samples << std::endl;

    return 0;
}
