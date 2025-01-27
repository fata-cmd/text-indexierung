cmake_minimum_required(VERSION 3.16.3)

project(ti)

include(CheckCXXCompilerFlag)

# Check if the compiler supports AVX
check_cxx_compiler_flag("-mavx" COMPILER_SUPPORTS_AVX)
if(COMPILER_SUPPORTS_AVX)
    message(STATUS "AVX is supported.")
else()
    message(WARNING "AVX is not supported on this compiler.")
endif()

# Check if the compiler supports AVX2
check_cxx_compiler_flag("-mavx2" COMPILER_SUPPORTS_AVX2)
if(COMPILER_SUPPORTS_AVX2)
    message(STATUS "AVX2 is supported.")
else()
    message(WARNING "AVX2 is not supported on this compiler.")
endif()

if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_BUILD_TYPE "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -O3 -mavx2")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
enable_testing()


set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -mavx2")

add_library(MallocCount src/malloc_count.c)
target_include_directories(MallocCount SYSTEM PUBLIC .)
target_link_libraries(MallocCount PUBLIC dl)

add_executable(main main.cpp)

add_executable(fixed fixed.cpp)
add_executable(variable variable.cpp)
add_executable(hashmap hashmap.cpp)

add_executable(tests src/tests.cpp)
add_executable(gen generate_query_file.cpp)

target_link_libraries(main PUBLIC MallocCount)
target_link_libraries(fixed PUBLIC MallocCount)
target_link_libraries(variable PUBLIC MallocCount)
target_link_libraries(hashmap PUBLIC MallocCount)

add_test(NAME Tests COMMAND "tests")