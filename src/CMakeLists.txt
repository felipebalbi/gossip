# SPDX-License-Identifier: GPL-2.0-or-later
add_library(libgossip STATIC Smaps.cpp)

add_executable(gossip main.cpp)
target_link_libraries(gossip libgossip)
