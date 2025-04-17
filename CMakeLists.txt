cmake_minimum_required(VERSION 3.10)
project(ImGuiGLFW_OpenGL3)

set(CMAKE_CXX_STANDARD 11)

# ImGui source files
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/imgui)
set(IMGUI_BACKENDS ${IMGUI_DIR}/backends)

file(GLOB IMGUI_SRC
    ${IMGUI_DIR}/*.cpp
    ${IMGUI_BACKENDS}/imgui_impl_glfw.cpp
    ${IMGUI_BACKENDS}/imgui_impl_opengl3.cpp
)

# Add your main file
set(SOURCES
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${IMGUI_SRC}
)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${IMGUI_DIR}
    ${IMGUI_BACKENDS}
)

# WebAssembly (Emscripten) build
if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
    message(STATUS "Building for WebAssembly (Emscripten)")

    set(CMAKE_EXECUTABLE_SUFFIX ".html")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_GLFW=3 -s FULL_ES3=1 -s ALLOW_MEMORY_GROWTH=1 -s WASM=1 -s EXIT_RUNTIME=1")

    add_executable(ImGuiWeb ${SOURCES})

# Native build
else()
    message(STATUS "Building for native (desktop)")

    find_package(OpenGL REQUIRED)
    find_package(PkgConfig REQUIRED)
    pkg_search_module(GLFW REQUIRED glfw3)

    include_directories(${GLFW_INCLUDE_DIRS})
    link_directories(${GLFW_LIBRARY_DIRS})

    add_executable(ImGuiNative ${SOURCES})
    target_link_libraries(ImGuiNative ${GLFW_LIBRARIES} ${OPENGL_gl_LIBRARY} dl)
endif()
