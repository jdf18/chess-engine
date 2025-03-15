cmake_minimum_required(VERSION 3.10)

IF(WIN32)
set(CMAKE_C_COMPILER cl.exe)
set(CMAKE_CXX_COMPILER cl.exe)
ENDIF()

project(Engine)

add_executable(Engine src/engine/engine.cpp src/engine/boardstate.cpp)
