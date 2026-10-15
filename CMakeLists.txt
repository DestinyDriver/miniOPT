cmake_minimum_required(VERSION 3.12)
project(MiniOpt VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /WX-)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# MiniOpt Core Library
file(GLOB_RECURSE MINIO_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/lexer/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/parser/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ir/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/optimizer/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/codegen/*.cpp"
)

add_library(miniopt_lib ${MINIO_SOURCES})
target_include_directories(miniopt_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# CLI Executable
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
    add_executable(miniopt src/main.cpp)
    target_link_libraries(miniopt PRIVATE miniopt_lib)
endif()

# Unit Tests
enable_testing()

file(GLOB TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/tests/*_tests.cpp")
foreach(TEST_SRC ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SRC} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SRC})
    target_link_libraries(${TEST_NAME} PRIVATE miniopt_lib)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()
