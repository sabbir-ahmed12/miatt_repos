# HW02
# \author Sabbir
# \date 2026-02-23

project(part1
        DESCRIPTION "Part 1 of HW02"
        VERSION 0.2026.1
        LANGUAGES CXX)

add_library(math_stuff math_stuff.cxx)
add_executable(hw02part1 hw02part1.cxx)
target_link_libraries(hw02part1 math_stuff)
