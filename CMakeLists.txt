cmake_minimum_required(VERSION 2.8)

project(storecast_assignment)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} "main.cpp" "objparser.h")
