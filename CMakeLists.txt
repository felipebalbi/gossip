# SPDX-License-Identifier: GPL-2.0-or-later
cmake_minimum_required(VERSION 3.21)

project(gossip VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0 -pg -ggdb -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -flto")
set(CMAKE_CXX_FLAGS "-pthread")

# Temporarily disabling `clang-tidy' as it can't handle view
# composition syntax
#
# set(CMAKE_CXX_CLANG_TIDY "clang-tidy")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING
    "Choose build type: None Debug Release RelWithDebInfo MinSizeRel"
    FORCE)
endif()

include_directories(include)
add_subdirectory(src)
