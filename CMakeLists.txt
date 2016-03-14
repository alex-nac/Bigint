cmake_minimum_required(VERSION 3.2)
project(C_Lion_bigint)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY /Users/Alex/ClionProjects/C_Lion_bigint/bin)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2")
set(SOURCE_FILES big_integer_testing
                 big_integer_testing.cpp
                 big_integer.h
                 big_integer.cpp
                 gtest/gtest-all.cc
                 gtest/gtest.h
                 gtest/gtest_main.cc
                 cow_vector/cow_vector.h
                 cow_vector/cow_vector.cpp)
add_executable(BIG_INT ${SOURCE_FILES})
#/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++