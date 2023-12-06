cmake_minimum_required(VERSION 3.24)
project(Convex)

set(CMAKE_CXX_STANDARD 17)

include_directories(
        include
)

link_libraries(
        glfw
        GL
        X11
        pthread
        Xrandr
        Xi
        dl
)

add_executable(
        Convex
        glad.c
        Convex/draw.hpp Convex/geometry.hpp Convex/compute.hpp
        Convex/main.cpp)
