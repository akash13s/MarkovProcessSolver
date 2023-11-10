cmake_minimum_required(VERSION 3.26)
project(MarkovProcessSolver)

set(CMAKE_CXX_STANDARD 11)

add_executable(MarkovProcessSolver main.cpp
        MarkovProcessSolver.cpp
        MarkovProcessSolver.h
)
