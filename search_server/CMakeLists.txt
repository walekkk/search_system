project(search_system)
cmake_minimum_required(VERSION 2.8)

set(LIB_NAME database)

add_subdirectory(database)

add_executable(search_server service/search_work.cpp)

target_link_libraries(search_server ${LIB_NAME})
