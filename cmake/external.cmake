#///////////////////////////////////////////////////////////////////////////////
#// chip8-emu
#//
#// Copyright (c) 2018 Thibault Schueller
#// This file is distributed under the MIT License
#///////////////////////////////////////////////////////////////////////////////

add_library(doctest INTERFACE)
target_include_directories(doctest SYSTEM INTERFACE ${CMAKE_SOURCE_DIR}/external/doctest)
