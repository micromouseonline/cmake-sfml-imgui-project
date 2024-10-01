cmake_minimum_required(VERSION 3.12)
project(sfml-imgui-implot-demo LANGUAGES CXX)


# Set project directories
# this is where the compiled and linked binary will go
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE})
# and this is where the compiled components will be stored ready for linking
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE})

# static linking for easy installation
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
include(cmake/compiler-flags.cmake)
set(CMAKE_CXX_STANDARD 17)


include(FetchContent)

################################
FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x
)
FetchContent_MakeAvailable(SFML)

################################
FetchContent_Declare(
        ImGui
        GIT_REPOSITORY https://github.com/ocornut/imgui
        GIT_TAG v1.89.9-docking
)
FetchContent_MakeAvailable(ImGui)
FetchContent_GetProperties(ImGui SOURCE_DIR IMGUI_DIR)
set(IMGUI_SFML_FIND_SFML OFF)
include_directories(${imgui_SOURCE_DIR})

################################
FetchContent_Declare(
        ImGui-SFML
        GIT_REPOSITORY https://github.com/SFML/imgui-sfml
        GIT_TAG 2.6.x)
FetchContent_MakeAvailable(ImGui-SFML)

################################
FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 8.0.1  # You can specify the version you need
)
FetchContent_MakeAvailable(fmt)

################################


################################# we have library code with its own CMakeLists.txt
add_subdirectory(libs/imgui-knobs)
add_subdirectory(libs/imgui-toggle)
add_subdirectory(libs/implot)
add_subdirectory(libs/portable-file-dialogs)

################################ and finally the target application
add_executable(${PROJECT_NAME}
        src/main.cpp
        src/demo-code.cpp
        libs/implot/implot_demo.cpp
)


target_link_libraries(${PROJECT_NAME} PRIVATE
        sfml-graphics
        ImGui-SFML::ImGui-SFML
        imgui-knobs
        imgui-toggle
        implot
        fmt::fmt
)

message(STATUS "----------------------------------------------------")
message(STATUS "CMAKE_CXX_STANDARD is set to: ${CMAKE_CXX_STANDARD}")
message(STATUS "CMAKE_BUILD_TYPE is set to: ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_INSTALL_PREFIX is set to: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_ARCHIVE_OUTPUT_DIRECTORY is set to: ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
message(STATUS "CMAKE_RUNTIME_OUTPUT_DIRECTORY is set to: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
# Get all variables and print them
#get_cmake_property(_variableNames VARIABLES)
#foreach (_variableName ${_variableNames})
#    string(TOUPPER "${_variableName}" _lowerVariableName)
#    if (_lowerVariableName MATCHES "^SFML" OR _lowerVariableName MATCHES "^IMGUI")
#        message(STATUS "${_variableName} = ${${_variableName}}")
#    endif ()
#endforeach ()
message(STATUS "----------------------------------------------------")

if (WIN32)
    add_custom_command(
            TARGET ${PROJECT_NAME}
            COMMENT "Copy OpenAL DLL"
            PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>
            VERBATIM)
endif ()

################################ be sure to copy any assets into the binary directory
add_custom_command(
        TARGET ${PROJECT_NAME}
        COMMENT "Copy assets"
        POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
        VERBATIM)

#install(TARGETS ${PROJECT_NAME})
