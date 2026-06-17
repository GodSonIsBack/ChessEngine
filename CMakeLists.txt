cmake_minimum_required(VERSION 3.10)
project(ChessEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Release build by default (enables -O3 on GCC/Clang)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(engine
    src/main.cpp
    src/Board.cpp
    src/MoveGenerator.cpp
    src/Perft.cpp
)

target_include_directories(engine PRIVATE ${CMAKE_SOURCE_DIR}/include)
