cmake_minimum_required(VERSION 3.15)
project(PolySim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set output directory to project root for easier Python imports
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# Find Python
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# Fetch pybind11
include(FetchContent)
FetchContent_Declare(
  pybind11
  GIT_REPOSITORY https://github.com/pybind/pybind11.git
  GIT_TAG        v2.11.1
)
FetchContent_MakeAvailable(pybind11)

# Define the module
pybind11_add_module(polysim src/bindings/main.cpp)

# Link against Python
target_link_libraries(polysim PRIVATE pybind11::module)

if(MSVC)
    target_compile_options(polysim PRIVATE /W4)
else()
    target_compile_options(polysim PRIVATE -Wall -Wextra -Wpedantic)
endif()
