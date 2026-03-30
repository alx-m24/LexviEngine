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

include(FetchContent)
FetchContent_Declare(assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG v5.4.3
)

FetchContent_MakeAvailable(assimp)
set_target_properties(assimp PROPERTIES COMPILE_FLAGS "-Wno-error")
target_compile_options(assimp PRIVATE
    -Wno-error
    -Wno-unknown-pragmas
    -Wno-implicit-fallthrough
    -Wno-deprecated-copy
    -Wno-ignored-qualifiers
)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG docking
)

FetchContent_MakeAvailable(imgui)

FetchContent_Declare(
    implot
    GIT_REPOSITORY https://github.com/epezent/implot.git
    GIT_TAG master
)

FetchContent_MakeAvailable(implot)

add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
)

add_library(implot STATIC
    ${implot_SOURCE_DIR}/implot.cpp
    ${implot_SOURCE_DIR}/implot_items.cpp
)

target_include_directories(implot PUBLIC
    ${implot_SOURCE_DIR}
)

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

# STB
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
)
FetchContent_MakeAvailable(stb)

target_include_directories(${PROJECT_NAME}
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${glm_SOURCE_DIR}
        ${glfw_SOURCE_DIR}/include
        ${stb_SOURCE_DIR}
)

target_link_libraries(imgui PUBLIC glfw glad)
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR}/backends)

target_link_libraries(implot PUBLIC imgui)

target_link_libraries(LexviEngine PUBLIC
    assimp
    implot
    imgui
    glad
    glfw
    glm
)

target_compile_features(LexviEngine PUBLIC cxx_std_23)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(assimp PRIVATE -Wno-unknown-pragmas -Wno-implicit-fallthrough)
    target_compile_options(${PROJECT_NAME} PRIVATE -static-libgcc)
endif()
