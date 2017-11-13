cmake_minimum_required(VERSION 2.8.12)
project(virtual_studio)
find_package(OpenCV REQUIRED)
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

include_directories(include)
include_directories( ${OpenCV_INCLUDE_DIRS} )


set(CMAKE_CXX_FLAGS "-O3 -std=c++11 -march=native")

add_executable(virtual_studio src/demo.cpp src/helper.cpp src/keying.cpp src/projection.cpp)

target_link_libraries( virtual_studio ${OpenCV_LIBS} )
