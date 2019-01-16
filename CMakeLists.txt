cmake_minimum_required(VERSION 3.0)

project(ublox)
include_directories(${CMAKE_SOURCE_DIR})
#include_directories(${CMAKE_SOURCE_DIR}/afc/include)
aux_source_directory(${CMAKE_SOURCE_DIR} SRC)
set(CMAKE_CXX_STANDARD 14) 
if (${CMAKE_C_COMPILER_ID} STREQUAL MSVC)
add_definitions("-D_CRT_SECURE_NO_WARNINGS") 
endif()

add_executable(ublox ${SRC})


