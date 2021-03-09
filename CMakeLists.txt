cmake_minimum_required(VERSION 3.17)
project(SimpleAssembler)

set(CMAKE_CXX_STANDARD 20)

include_directories(src)

add_executable(SimpleAssembler src/main.cpp src/parser/lexer.cpp src/parser/parser.cpp)