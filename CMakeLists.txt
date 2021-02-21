cmake_minimum_required(VERSION 3.16)
project(kvxhttp)

set(CMAKE_CXX_STANDARD 11)

find_package(Threads REQUIRED)


add_library(http_parser kvxhttp/parser.c kvxhttp/parser.h)
add_library(http_server kvxhttp/event_loop.c kvxhttp/event_loop.h
        kvxhttp/server.c kvxhttp/server.h
        kvxhttp/map.c kvxhttp/map.h)

target_link_libraries(http_server Threads::Threads
        http_parser)

add_executable(kvxhttp main.c)

target_link_libraries(kvxhttp http_server)
