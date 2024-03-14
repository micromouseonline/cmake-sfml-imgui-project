cmake_minimum_required(VERSION 3.12)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)

FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

FetchContent_Declare(ImGui
    GIT_REPOSITORY https://github.com/ocornut/imgui
    GIT_TAG v1.89.9-docking)
FetchContent_MakeAvailable(ImGui)
FetchContent_GetProperties(ImGui SOURCE_DIR IMGUI_DIR)
set(IMGUI_SFML_FIND_SFML OFF)

FetchContent_Declare(ImGui-SFML
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(ImGui-SFML)

include_directories(libs/sfml)
include_directories(libs/imgui-knobs)
include_directories(libs/imgui-toggle)
include_directories(libs/implot)
include_directories(${imgui_SOURCE_DIR})

# Find SFML components
# find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
add_subdirectory(libs/imgui-knobs)
add_subdirectory(libs/imgui-toggle)
add_subdirectory(libs/implot)

add_executable(CMakeSFMLProject
    src/main.cpp
    src/demo-code.cpp

    libs/implot/implot_demo.cpp

    # libs/imgui-knobs/imgui-knobs.cpp
)
target_link_libraries(CMakeSFMLProject PRIVATE
    sfml-graphics
    ImGui-SFML::ImGui-SFML
    imgui-knobs
    imgui-toggle
    implot
)
target_compile_features(CMakeSFMLProject PRIVATE cxx_std_17)

if(WIN32)
    add_custom_command(
        TARGET CMakeSFMLProject
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:CMakeSFMLProject>
        VERBATIM)
endif()

add_custom_command(
    TARGET CMakeSFMLProject
    COMMENT "Copy assets"
    POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:CMakeSFMLProject>/assets
    VERBATIM)

install(TARGETS CMakeSFMLProject)
