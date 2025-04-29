cmake_minimum_required(VERSION 3.10)

project(Engine)
set(CMAKE_CXX_STANDARD 20)

IF(WIN32)
set(CMAKE_C_COMPILER cl.exe)
set(CMAKE_CXX_COMPILER cl.exe)
ELSE()
add_compile_options(-g3)
ENDIF()

add_executable(Engine   
    src/engine/engine.cpp 
    src/engine/piece.cpp
    src/engine/movemaps.cpp
    src/engine/boardstate.cpp
    src/engine/boardstateModifications.cpp
    src/engine/boardstatePseudolegal.cpp
    src/engine/fenparser.cpp
    src/engine/decisiontree.cpp
    src/engine/bitboard.cpp)
