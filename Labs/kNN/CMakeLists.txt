cmake_minimum_required(VERSION 3.17)
project(kNN)

set(CMAKE_CXX_STANDARD 17)

add_executable(prediction prediction.cpp)
add_executable(f-score f_score.cpp)