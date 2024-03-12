cmake_minimum_required(VERSION 3.12)

project(imgui-knobs LANGUAGES CXX)

add_library(imgui-knobs STATIC
  imgui-knobs.cpp
  imgui-knobs.h
)
