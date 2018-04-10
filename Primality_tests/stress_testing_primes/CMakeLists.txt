cmake_minimum_required(VERSION 3.5)
project(stress_testing_primes)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(stress_testing_primes ${SOURCE_FILES})