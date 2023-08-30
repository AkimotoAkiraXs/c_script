cmake_minimum_required(VERSION 3.23)
project(c_script)
set(CMAKE_CXX_STANDARD 14)

add_executable(arrayString source/ArrayString.cpp)
add_executable(durationWeekTime source/durationWeekTime.cc)
add_executable(test source/test.cc)
add_executable(pmDuration source/pmDuration.c source/pmDurationY.c source/pmDurationX.c)