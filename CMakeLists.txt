cmake_minimum_required(VERSION 3.15)

project(LexviEngine LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the library
add_library(${PROJECT_NAME} STATIC
	src/Internals/Application/ApplicationStack.cpp
	src/pch.cpp
)

# Include directories for this library
target_include_directories(${PROJECT_NAME} PUBLIC include)

# Include Precompiled header
target_precompile_headers(LexviEngine PRIVATE "include/pch.h")
