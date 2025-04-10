cmake_minimum_required(VERSION 3.10)

project(Engine)
IF(WIN32)
set(CMAKE_C_COMPILER cl.exe)
set(CMAKE_CXX_COMPILER cl.exe)
ELSE()
add_compile_options(-g3)
ENDIF()

add_executable(Engine   
    src/engine/engine.cpp 
    src/engine/boardstate.cpp
    src/engine/piece.cpp
    src/engine/decisiontree.cpp
)
