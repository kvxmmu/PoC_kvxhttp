cmake_minimum_required(VERSION 3.16)
project(kvxhttp)

set(CMAKE_CXX_STANDARD 11)

add_library(http_parser kvxhttp/parser.c kvxhttp/parser.h)

add_executable(kvxhttp main.c)

target_link_libraries(kvxhttp http_parser)
