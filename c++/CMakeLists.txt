cmake_minimum_required(VERSION 2.8.12)
project(virtual_studio)
find_package(OpenCV REQUIRED)

include_directories(include)
include_directories( ${OpenCV_INCLUDE_DIRS} )


set(CMAKE_CXX_FLAGS "-O3")

add_executable(virtual_studio src/demo.cpp src/helper.cpp src/keying.cpp src/projection.cpp)

target_link_libraries( virtual_studio ${OpenCV_LIBS} )