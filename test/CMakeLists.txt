# SPDX-License-Identifier: GPL-2.0-or-later
unset(CMAKE_CXX_CLANG_TIDY)

find_package(Catch2 REQUIRED)

add_test(NAME test_runner COMMAND test_runner)
add_executable(test_runner main.cpp test_Smaps.cpp)
target_link_libraries(test_runner libgossip Catch2::Catch2)
