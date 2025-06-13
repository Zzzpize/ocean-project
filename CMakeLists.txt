cmake_minimum_required(VERSION 3.10)
project(OceanSimulation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PGE_HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include/olc/olcPixelGameEngine.h)

add_executable(OceanSimulation 
    src/main.cpp
    src/ocean.cpp
    src/algae.cpp
    src/herbivore.cpp
    src/predator.cpp
    src/utils/logger.cpp
)

target_include_directories(OceanSimulation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(WIN32)
    target_link_libraries(OceanSimulation PRIVATE gdi32 user32 opengl32)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_definitions(OceanSimulation PRIVATE _CRT_SECURE_NO_WARNINGS)
        target_compile_options(OceanSimulation PRIVATE /EHsc)
    else()
        target_link_libraries(OceanSimulation PRIVATE ole32)
    endif()
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
elseif(UNIX AND NOT APPLE)
    target_link_libraries(OceanSimulation PRIVATE X11 GL pthread)
elseif(APPLE)
    target_link_libraries(OceanSimulation PRIVATE "-framework OpenGL")
endif()

set(ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets)
if(EXISTS ${ASSETS_DIR})
    add_custom_command(TARGET OceanSimulation POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        ${ASSETS_DIR}
        $<TARGET_FILE_DIR:OceanSimulation>/assets
        COMMENT "Copying assets directory"
    )
endif()