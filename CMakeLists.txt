cmake_minimum_required(VERSION 3.14)
project(MiniBitmap)

set(CMAKE_CXX_STANDARD 17)

add_library(MiniBitmap INTERFACE)
target_include_directories(MiniBitmap INTERFACE .)

add_executable(MiniBitmap_Test test.cpp)
target_link_libraries(MiniBitmap_Test MiniBitmap)