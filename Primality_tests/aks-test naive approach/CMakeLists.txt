cmake_minimum_required(VERSION 3.3)
project(aks_test_naive_approach)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(aks_test_naive_approach ${SOURCE_FILES})