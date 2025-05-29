cmake_minimum_required(VERSION 3.20)
project(Arkanoid LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/utf-8)
endif()

find_package(SFML 3 CONFIG REQUIRED COMPONENTS 
    Window
    Graphics
    System
)

file(GLOB SRC_FILES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

add_executable(Arkanoid ${SRC_FILES})

target_include_directories(Arkanoid PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(Arkanoid PRIVATE
    SFML::Window
    SFML::Graphics
    SFML::System
)

set_target_properties(Arkanoid PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY   ${CMAKE_BINARY_DIR}/$<CONFIG>
)
