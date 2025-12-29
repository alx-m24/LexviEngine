cmake_minimum_required(VERSION 3.5)

project(LexviEngine LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the library
add_library(${PROJECT_NAME} STATIC
	src/Application/ApplicationStack.cpp
	src/Application/Application.cpp
	src/Threading/ThreadRegistry.cpp
	src/Utils/GetSystemInfo.cpp
	src/Window/Window.cpp
	src/Input/Input.cpp
	src/LexviEngine.cpp
	src/pch.cpp
)

# Include directories for this library
target_include_directories(${PROJECT_NAME} PUBLIC 
	include
)

# Include Precompiled header
target_precompile_headers(${PROJECT_NAME} PRIVATE "include/LexviEngine/pch.h")


if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${PROJECT_NAME} PRIVATE -static-libgcc)
endif()

# ---- FetchContent for GLFW and GLAD ----
include(FetchContent)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE) # Disabling docs
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

# GLAD
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v0.1.36
)
FetchContent_MakeAvailable(glad)

# GLM
FetchContent_Declare(
	glm
	GIT_REPOSITORY https://github.com/g-truc/glm.git
	GIT_TAG 1.0.2
)
FetchContent_MakeAvailable(glm)

# ---- Link the libraries ----
target_link_libraries(${PROJECT_NAME} PUBLIC glfw glad glm)
