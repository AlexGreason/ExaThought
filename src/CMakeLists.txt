cmake_minimum_required(VERSION 3.10)
project(ExaThought)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra")

add_executable(ExaThought main.cpp board.cpp board.h bitboards.cpp bitboards.h move.cpp move.h movegen.cpp movegen.h consts.h pgn.h pgn.cpp eval.cpp eval.h predicates.cpp predicates.h)