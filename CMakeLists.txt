# SPDX-License-Identifier: GPL-2.0-or-later
cmake_minimum_required(VERSION 3.18)

set(GOSSIP_VERSION 1.0.0)

project(gossip VERSION ${GOSSIP_VERSION} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${CMAKE_TOOLCHAIN_EXTRA_CFLAGS} -O0 -pg -ggdb -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_TOOLCHAIN_EXTRA_CFLAGS} -O2 -flto")
set(CMAKE_CXX_CLANG_TIDY "clang-tidy")

add_definitions(-DGOSSIP_VERSION="${GOSSIP_VERSION}")

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING
    "Choose build type: None Debug Release RelWithDebInfo MinSizeRel"
    FORCE)
endif()

include_directories(include)
add_subdirectory(src)
