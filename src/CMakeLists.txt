cmake_minimum_required(VERSION 3.12)
project(ExaThought)

set(CMAKE_CXX_STANDARD 14)

add_executable(ExaThought main.cpp board.cpp board.h bitboards.cpp bitboards.h move.cpp move.h movegen.cpp movegen.h consts.h)