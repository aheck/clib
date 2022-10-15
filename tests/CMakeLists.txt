cmake_minimum_required(VERSION 3.10)

include_directories("../src")

link_libraries(Check::check)

add_executable(test_gstring test_gstring.c)
add_test(NAME test_gstring COMMAND test_gstring)

add_executable(test_ghashtable test_ghashtable.c)
if(LINUX)
    target_link_libraries(test_ghashtable -lm)
endif()
add_test(NAME test_ghashtable COMMAND test_ghashtable)

add_executable(test_glist test_glist.c)
if(LINUX)
    target_link_libraries(test_glist -lm)
endif()
add_test(NAME test_glist COMMAND test_glist)
