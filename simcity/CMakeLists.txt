cmake_minimum_required(VERSION 3.10)
project(SimCity)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add source files
file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
    "src/*/*.cpp"
    "src/*/*/*.cpp"
)

# Create executable
add_executable(simcity ${SOURCES})

# Add compiler warnings
if(MSVC)
    target_compile_options(simcity PRIVATE /W4)
else()
    target_compile_options(simcity PRIVATE -Wall -Wextra -Wpedantic)
endif()