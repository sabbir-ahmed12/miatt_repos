# --- Set the CMake version ---
cmake_minimum_required(VERSION 3.23)

# --- Set the project name ---
project(ece5940_hw01)

# --- Compile project using C++17 ---
set(CMAKE_CXX_STANDARD 17)
# --- Do not silently downgrade C++ version ---
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Set the ITK version ---
set(MINIMUM_ITK_VERSION 5.4)

# --- Find ITK ---
find_package(ITK ${MINIMUM_ITK_VERSION} REQUIRED)

# --- Add source folder ---
add_subdirectory(src)
