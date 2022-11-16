cmake_minimum_required(VERSION 3.10)
project(compress)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -DIL_STD -m64 -Wall -g")

set(SOURCE_FILES
	src/main.cpp
)

add_executable(
	run ${SOURCE_FILES}
)
