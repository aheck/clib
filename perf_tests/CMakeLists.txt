cmake_minimum_required(VERSION 3.10)

set(CLIB_SRC_DIR "../src")

add_executable(perf_test_ghashtable perf_test_ghashtable.c)
target_include_directories(perf_test_ghashtable PRIVATE ${CLIB_SRC_DIR})
