cmake_minimum_required(VERSION 2.8)

project(storecast_assignment)

add_subdirectory(googletest)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR} ${gmock_SOURCE_DIR}/include)

configure_file("cube.obj" "cube.obj" COPYONLY)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} "main.cpp" "objparser.cpp")

add_executable("${PROJECT_NAME}-unittests" "unit_tests.cpp" "objparser.cpp")

target_link_libraries("${PROJECT_NAME}-unittests" gtest gmock)



