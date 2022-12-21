cmake_minimum_required(VERSION 3.10)

if ("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
  message(FATAL_ERROR "In-source builds are disabled.
    Please create a subfolder and use `cmake ..` inside it.
    NOTE: cmake will now create CMakeCache.txt and CMakeFiles/*.
          You must delete them, or cmake will refuse to work.")
endif()

project(clib C)

set(C_STANDARD 99)
set(C_STANDARD_REQUIRED ON)
add_compile_options(-Wall)

message(STATUS "Binary dir: ${CMAKE_BINARY_DIR}")
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

option(ENABLE_ADDRESS_SANITIZER "Whether to build tests with address sanitizer" OFF)
option(ENABLE_MEMORY_SANITIZER "Whether to build tests with memory sanitizer" OFF)
option(ENABLE_LEAK_SANITIZER "Whether to build tests with leak sanitizer" OFF)

if(ENABLE_ADDRESS_SANITIZER)
    message(STATUS "Building with address sanitizer")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
    set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
elseif(ENABLE_MEMORY_SANITIZER)
    message(STATUS "Building with memory sanitizer")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=memory")
    set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=memory")
elseif(ENABLE_LEAK_SANITIZER)
    message(STATUS "Building with leak sanitizer")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=leak")
    set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=leak")
else()
    message(STATUS "Building with no sanitizer")
endif()

find_package(check REQUIRED)
enable_testing()

add_subdirectory(tests)
add_subdirectory(perf_tests)
