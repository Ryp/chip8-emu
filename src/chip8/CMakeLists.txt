#///////////////////////////////////////////////////////////////////////////////
#// chip8-emu
#//
#// Copyright (c) 2018 Thibault Schueller
#// This file is distributed under the MIT License
#///////////////////////////////////////////////////////////////////////////////

set(target chip8emu_emu)
set(CHIP8EMU_EMU_BIN ${target} PARENT_SCOPE)

add_library(${target} ${CHIP8EMU_BUILD_TYPE})

target_sources(${target} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Config.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Cpu.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Cpu.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Display.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Display.h
    ${CMAKE_CURRENT_SOURCE_DIR}/EmuExport.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Execution.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Execution.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Instruction.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Instruction.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Keyboard.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Keyboard.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Memory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Memory.h
)

target_link_libraries(${target} PRIVATE
    ${CHIP8EMU_CORE_BIN}
)

reaper_configure_library(${target} "Emu")

reaper_add_tests(${target}
    ${CMAKE_CURRENT_SOURCE_DIR}/test/instructions.cpp
)
