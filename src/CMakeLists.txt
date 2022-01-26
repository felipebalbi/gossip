# SPDX-License-Identifier: GPL-2.0-or-later
include(FetchContent)

FetchContent_Declare(
  argparse
  GIT_REPOSITORY https://github.com/p-ranav/argparse
  GIT_TAG        v2.2
)

FetchContent_MakeAvailable(argparse)

add_executable(gossip main.cpp)
target_link_libraries(gossip argparse::argparse stdc++fs)
