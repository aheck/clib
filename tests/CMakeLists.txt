cmake_minimum_required(VERSION 3.10)

include_directories("../src")

link_libraries(Check::check)

add_executable(test_gstring test_gstring.c)
add_executable(test_ghashtable test_ghashtable.c)
add_executable(test_glist test_glist.c)

target_link_libraries(test_ghashtable -lm)
target_link_libraries(test_glist -lm)