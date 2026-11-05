cmake_minimum_required(VERSION 3.10.2)
cmake_policy(VERSION 3.10.2...3.19.2)

#-----------------------------------------------------------------------------
# Enable C++17
##  Set the default target properties for ITK
if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17) # Supported values are ``17``, ``20``, and ``23``.
endif()
if(NOT CMAKE_CXX_STANDARD_REQUIRED)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()
if(NOT CMAKE_CXX_EXTENSIONS)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()
set(VALID_CXX_STANDARDS "17" "20" "23")
if(NOT CMAKE_CXX_STANDARD IN_LIST VALID_CXX_STANDARDS )
    MESSAGE(FATAL_ERROR "CMAKE_CXX_STANDARD:STRING=${CMAKE_CXX_STANDARD} not in know standards list\n ${VALID_CXX_STANDARDS}.")
endif()
#-----------------------------------------------------------------------------

project( HW04
        VERSION 0.0.2026
        LANGUAGES CXX)

find_package(ITK 6.0 REQUIRED)
include(${ITK_USE_FILE})

find_package(SlicerExecutionModel REQUIRED GenerateCLP)
include(${GenerateCLP_USE_FILE})
include(${SlicerExecutionModel_USE_FILE})
include(${SlicerExecutionModel_CMAKE_DIR}/SEMMacroBuildCLI.cmake)

set(MYHAWKID "sahmed8") # <----- YOU MUST CHANGE THIS!

add_subdirectory(src)
