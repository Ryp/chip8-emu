#///////////////////////////////////////////////////////////////////////////////
#// chip8-emu
#//
#// Copyright (c) 2018 Thibault Schueller
#// This file is distributed under the MIT License
#///////////////////////////////////////////////////////////////////////////////

set(target chip8emu_core)
set(CHIP8EMU_CORE_BIN ${target} PARENT_SCOPE)

add_library(${target} ${CHIP8EMU_BUILD_TYPE})

target_sources(${target} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Assert.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Assert.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Compiler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/CoreExport.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Debugger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Debugger.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Platform.h
    ${CMAKE_CURRENT_SOURCE_DIR}/StackTrace.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/StackTrace.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Types.h
)

if(UNIX)
    # Setup libunwind
    find_package(Libunwind REQUIRED)
    target_link_libraries(${target} PRIVATE Libunwind::Libunwind)

    # Link with libdl
    target_link_libraries(${target} PRIVATE -ldl)
endif()

reaper_configure_library(${target} "Core")
