cmake_minimum_required(VERSION 3.10)
project(OceanSimulation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(OceanSimulation 
    src/main.cpp
    src/algae.cpp
    src/herbivore.cpp
    src/predator.cpp
    src/ocean.cpp
)

target_include_directories(OceanSimulation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)