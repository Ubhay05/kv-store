cmake_minimum_required(VERSION 3.15)
project(kv-store)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")

# Source files shared between server and other targets
set(COMMON_SOURCES
    src/store.cpp
    src/lru.cpp
    src/ttl.cpp
    src/parser.cpp
    src/wal.cpp
    src/thread_pool.cpp
    src/server.cpp
)

# --- Server ---
add_executable(kv-server
    src/main.cpp
    ${COMMON_SOURCES}
)
target_include_directories(kv-server PRIVATE src)
target_link_libraries(kv-server pthread)

# --- Client ---
add_executable(kv-client
    client/client.cpp
)

# --- Benchmark ---
add_executable(kv-benchmark
    benchmark/benchmark.cpp
)