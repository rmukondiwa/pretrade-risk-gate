cmake_minimum_required(VERSION 3.16)
project(pretrade_risk_gate CXX)

# --- global settings ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# default to a release (optimized) build if none specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O2 -Wall")

# hot-path headers live here; everything can find "gate/..."
include_directories(${CMAKE_SOURCE_DIR}/include)

# threads (for -pthread)
find_package(Threads REQUIRED)

# --- binaries ---

# replay: the threaded pipeline (needs threads)
add_executable(replay bench/replay.cpp)
target_include_directories(replay PRIVATE ${CMAKE_SOURCE_DIR}/bench)
target_link_libraries(replay PRIVATE Threads::Threads)

# harness: the benchmark
add_executable(harness bench/harness.cpp)
target_include_directories(harness PRIVATE ${CMAKE_SOURCE_DIR}/bench)
target_link_libraries(harness PRIVATE Threads::Threads)

# --- tests ---
enable_testing()

add_executable(test_token_bucket test/test_token_bucket.cpp)

add_executable(test_risk_gate test/test_risk_gate.cpp)
target_include_directories(test_risk_gate PRIVATE ${CMAKE_SOURCE_DIR}/bench)
target_link_libraries(test_risk_gate PRIVATE Threads::Threads)

add_executable(test_seqlock test/test_seqlock.cpp)
target_link_libraries(test_seqlock PRIVATE Threads::Threads)

add_test(NAME token_bucket COMMAND test_token_bucket)
add_test(NAME risk_gate    COMMAND test_risk_gate)
add_test(NAME seqlock      COMMAND test_seqlock)