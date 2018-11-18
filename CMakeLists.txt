#///////////////////////////////////////////////////////////////////////////////
#// chip8-emu
#//
#// Copyright (c) 2018 Thibault Schueller
#// This file is distributed under the MIT License
#///////////////////////////////////////////////////////////////////////////////

cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

# Avoid source tree pollution
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "In-source builds are discouraged. Make a separate folder for building:\nmkdir build; cd build; cmake ..;cd ..\nBefore that, remove the files already created:\nrm -rf CMakeCache.txt CMakeFiles")
endif()

# Monoconfig generator
set(CHIP8EMU_DEFAULT_BUILD_TYPE "Debug")
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type provided, defaulting to: ${CHIP8EMU_DEFAULT_BUILD_TYPE}")
    set(CMAKE_BUILD_TYPE ${CHIP8EMU_DEFAULT_BUILD_TYPE} CACHE STRING "" FORCE)
endif()
# Set the valid options for cmake-gui drop-down list
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug;Release")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY HELPSTRING "Choose the type of build")

project(Chip8Emu
    VERSION 0.1.0
    LANGUAGES C CXX
    DESCRIPTION "CHIP-8 Emulator")

# Export compile commands in a json file. Useful for clang-based tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable testing. Remember to run the tests before submitting code.
option(CHIP8EMU_BUILD_TESTS               "Build tests"                   ON)

# Recommended option if you want to quickly iterate on libraries.
# The runtime performance should be comparable to a classic static build.
option(CHIP8EMU_BUILD_SHARED_LIBRARIES    "Build shared libraries"        ON)

# Enable CTest
if (CHIP8EMU_BUILD_TESTS)
    enable_testing()
endif()

# Select linkage type
if(CHIP8EMU_BUILD_SHARED_LIBRARIES)
    set(CHIP8EMU_BUILD_TYPE SHARED)
else()
    set(CHIP8EMU_BUILD_TYPE STATIC)
endif()

# The CXX_STANDARD property is a requirement
set(CXX_STANDARD_REQUIRED ON)

# Enable folder support (useful for Visual Studio)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Override binary output paths
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${${CMAKE_PROJECT_NAME}_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${${CMAKE_PROJECT_NAME}_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${${CMAKE_PROJECT_NAME}_BINARY_DIR})

# Provide custom cmake scripts
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(Chip8Emu)

add_subdirectory(src)
