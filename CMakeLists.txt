set(INCLUDE_DIRS  src/)
cmake_minimum_required(VERSION 3.23.1)
project(alloc CXX)
add_executable( 
	alloc
	src/Alloc.h 
	src/main.cpp 
	src/Token.cpp 
	src/Parse.cpp 
	src/util.cpp
	src/gen.cpp
	src/binGen.cpp
)
include_directories(${INCLUDE_DIRS})
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -gO")
set_property(TARGET alloc PROPERTY CXX_STANDARD 20)                       
