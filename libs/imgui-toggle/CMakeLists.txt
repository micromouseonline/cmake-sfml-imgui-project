cmake_minimum_required(VERSION 3.12)

project(imgui-toggle LANGUAGES CXX)

add_library(imgui-toggle STATIC
        imgui_toggle.cpp
        imgui_toggle_presets.cpp
        imgui_toggle_palette.cpp
        imgui_toggle_renderer.cpp
)

target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
