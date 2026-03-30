cmake_minimum_required(VERSION 3.20)

project(LexviEngine LANGUAGES CXX)

file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    src/*.cpp
    include/*.hpp
    include/*.h
)

add_library(LexviEngine STATIC ${ENGINE_SOURCES})

target_include_directories(LexviEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(LexviEngine PUBLIC cxx_std_23)
