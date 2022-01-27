FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v2.13.8
)

FetchContent_MakeAvailable(Catch2)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)

add_executable(tests test.cpp)
target_link_libraries(tests PRIVATE libgossip Catch2::Catch2)

include(CTest)
include(Catch)

catch_discover_tests(tests)
