cmake_minimum_required(VERSION 3.20)
project(RegisterCircles CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for tooling (clangd, etc.)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

# ── Targets ──────────────────────────────────────────────────────────────────

add_executable(generate_images src/generate_images.cxx)
target_link_libraries(generate_images PRIVATE ${ITK_LIBRARIES})

add_executable(register_circles src/register_circles.cxx)
target_link_libraries(register_circles PRIVATE ${ITK_LIBRARIES})

# ── Tests ─────────────────────────────────────────────────────────────────────

enable_testing()
add_subdirectory(tests)
