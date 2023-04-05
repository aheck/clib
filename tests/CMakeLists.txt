cmake_minimum_required(VERSION 3.10)

set(CLIB_SRC_DIR "../src")

#
# GArray
#
add_executable(test_garray test_garray.c)
target_include_directories(test_garray PRIVATE ${CLIB_SRC_DIR})
target_link_libraries(test_garray PRIVATE Check::check)
add_test(NAME test_garray COMMAND test_garray)

#
# GString
#
add_executable(test_gstring test_gstring.c)
target_include_directories(test_gstring PRIVATE ${CLIB_SRC_DIR})
target_link_libraries(test_gstring PRIVATE Check::check)
add_test(NAME test_gstring COMMAND test_gstring)

#
# GHashTable
#
add_executable(test_ghashtable test_ghashtable.c)
target_include_directories(test_ghashtable PRIVATE ${CLIB_SRC_DIR})
target_link_libraries(test_ghashtable PRIVATE Check::check)
if(LINUX)
    target_link_libraries(test_ghashtable PRIVATE -lm)
endif()
add_test(NAME test_ghashtable COMMAND test_ghashtable)

#
# GList
#
add_executable(test_glist test_glist.c)
target_include_directories(test_glist PRIVATE ${CLIB_SRC_DIR})
target_link_libraries(test_glist PRIVATE Check::check)
if(LINUX)
    target_link_libraries(test_glist PRIVATE -lm)
endif()
add_test(NAME test_glist COMMAND test_glist)

#
# Integration
#
add_executable(test_integration test_integration.c)
target_include_directories(test_integration PRIVATE ${CLIB_SRC_DIR})
target_link_libraries(test_integration PRIVATE Check::check)
add_test(NAME test_integration COMMAND test_integration)
