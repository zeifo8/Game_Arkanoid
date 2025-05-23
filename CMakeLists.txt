cmake_minimum_required(VERSION 3.20)
project(Arkanoid LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/utf-8)
endif()

SFMLConfig.cmake)
find_package(SFML 3 CONFIG REQUIRED COMPONENTS Window Graphics System)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY   ${CMAKE_BINARY_DIR}/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY   ${CMAKE_BINARY_DIR}/$<CONFIG>)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY   ${CMAKE_BINARY_DIR}/$<CONFIG>)

include_directories(${PROJECT_SOURCE_DIR}/include)
file(GLOB SRC_FILES
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

add_executable(Arkanoid ${SRC_FILES})

target_link_libraries(Arkanoid PRIVATE
    SFML::Window
    SFML::Graphics
    SFML::System
)

