# HW02
# \author Sabbir
# \date 2026-02-23

# This file has error that you must resolve

project(part2
        DESCRIPTION "Part 2 of HW02"
        VERSION 0.2026.2
        LANGUAGES CXX)

# HINT: See ITK/Examples/Install reference implementation

find_package(ITK 6.0.0 REQUIRED)
include(${ITK_USE_FILE})

add_executable(hw02part2 hw02part2.cxx)
target_link_libraries(hw02part2 PRIVATE ${ITK_LIBRARIES})