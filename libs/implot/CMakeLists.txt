cmake_minimum_required(VERSION 3.12)

project(implot LANGUAGES CXX)

add_library(implot STATIC
        implot.cpp
        implot_items.cpp

)

target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
