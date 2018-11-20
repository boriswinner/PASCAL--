cmake_minimum_required(VERSION 3.12)
project(pas__)

set(CMAKE_CXX_STANDARD 17)

add_executable(pas__ main.cpp lexer.cpp lexer_executor.h)
add_executable(tests tests.cpp lexer.cpp lexer_executor.h)