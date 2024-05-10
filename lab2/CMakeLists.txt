cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 20) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) 
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set (DEBUG_FLAGS -ggdb3 -O0 -Wall -Wextra -Weffc++ -g -fsanitize=address)

project(integral-computation)

add_executable( ${PROJECT_NAME} integral.cpp)
target_compile_options (${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:${DEBUG_FLAGS}>)
target_link_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:${DEBUG_FLAGS}>)

target_include_directories( ${PROJECT_NAME} PRIVATE arg-parser.h debug.h)