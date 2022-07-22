# SPDX-License-Identifier: GPL-3.0
include(FetchContent)

FetchContent_Declare(
  argparse
  GIT_REPOSITORY https://github.com/p-ranav/argparse
  GIT_TAG        v2.6
)

FetchContent_MakeAvailable(argparse)

add_library(libgossip OBJECT Collector.cpp Process.cpp Cpu.cpp)
add_executable(gossip main.cpp)
target_link_libraries(gossip $<TARGET_OBJECTS:libgossip> argparse::argparse)
