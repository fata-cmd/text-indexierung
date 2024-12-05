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

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -O3 -mavx2")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
enable_testing()


set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -mavx2")

add_executable(main main.cpp)
add_executable(ut unit_tests.cpp)

add_test(NAME UnitTests COMMAND "ut")