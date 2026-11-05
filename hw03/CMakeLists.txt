# --- Set the CMake version ---
cmake_minimum_required(VERSION 3.22)

# --- Set the project name ---
project(HW03
        DESCRIPTION "MIATT 2026 Homework 03"
        VERSION 0.2026.0
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Set the ITK version ---
set(MINIMUM_ITK_VERSION 5.4)

# --- Find ITK ---
find_package(ITK ${MINIMUM_ITK_VERSION} REQUIRED)

# --- Include the source subdirectory ---
add_subdirectory(src)