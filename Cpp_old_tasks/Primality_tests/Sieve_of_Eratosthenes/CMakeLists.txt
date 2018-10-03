cmake_minimum_required(VERSION 3.3)
project(Sieve_of_Eratosthenes)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(Sieve_of_Eratosthenes ${SOURCE_FILES})