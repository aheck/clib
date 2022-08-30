cmake_minimum_required(VERSION 3.10)

include_directories("/usr/local/include" "../src")
link_directories("/usr/local/lib")

link_libraries(check::check)

add_executable(test_gstring test_gstring.c)
add_executable(test_ghashtable test_ghashtable.c)
add_executable(test_glist test_glist.c)
