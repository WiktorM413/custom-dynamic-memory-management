cmake_minimum_required(VERSION 3.20)
project(dynMemory)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE include)

include(FetchContent)
FetchContent_Declare(
	googletest,
	URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE TEST_SOURCES tests/*.cpp)
add_executable(${PROJECT_NAME}_tests ${TEST_SOURCES})

target_link_libraries(${PROJECT_NAME}_tests PRIVATE gtest_main)
target_include_directories(${PROJECT_NAME}_tests PRIVATE include)

enable_testing()
add_test(NAME all_tests COMMAND ${PROJECT_NAME}_tests)