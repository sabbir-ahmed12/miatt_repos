# Project created by Sabbir on 2026-02-23

cmake_minimum_required(VERSION 3.22.1)

##  Set the default target properties for ITK
if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17) # Supported values are 17, 20, and 23.
endif()
if(NOT CMAKE_CXX_STANDARD_REQUIRED)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()
if(NOT CMAKE_CXX_EXTENSIONS)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

project(HW02
        DESCRIPTION "MIATT 2026 Homework 02"
        VERSION 0.2026.0
        LANGUAGES CXX)

add_subdirectory(part1)
add_subdirectory(part2)


