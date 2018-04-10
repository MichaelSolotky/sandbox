cmake_minimum_required(VERSION 3.5)
project(BPSW_test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(BPSW_test ${SOURCE_FILES})