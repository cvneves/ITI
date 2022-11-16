cmake_minimum_required(VERSION 3.10)
project(compress)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -DIL_STD -m64 -Wall -g")

set(ENCODE_SRC_FILES
	src/encode.cpp
)

set(DECODE_SRC_FILES
	src/decode.cpp
)

add_executable(
	encode ${ENCODE_SRC_FILES}
)

add_executable(
	decode ${DECODE_SRC_FILES}
)
