#///////////////////////////////////////////////////////////////////////////////
#// chip8-emu
#//
#// Copyright (c) 2018 Thibault Schueller
#// This file is distributed under the MIT License
#///////////////////////////////////////////////////////////////////////////////

set(target chip8emu_emu)
set(CHIP8EMU_EMU_BIN ${target} PARENT_SCOPE)

add_library(${target} ${CHIP8EMU_BUILD_TYPE})

find_package(XCB REQUIRED)
target_link_libraries(${target} PUBLIC ${LIBXCB_LIBRARIES})

# FIXME find this and scanner auto
set(CHIP8EMU_WAYLAND_SPEC_XML /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml)
#set(CHIP8EMU_WAYLAND_SPEC_XML /usr/share/wayland/wayland.xml)
set(CHIP8EMU_WAYLAND_PROTOCOL_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR})
set(CHIP8EMU_WAYLAND_PROTOCOL_SOURCE ${CHIP8EMU_WAYLAND_PROTOCOL_INCLUDE_DIR}/WaylandProtocol.c)
set(CHIP8EMU_WAYLAND_PROTOCOL_CLIENT_HEADER ${CHIP8EMU_WAYLAND_PROTOCOL_INCLUDE_DIR}/WaylandClientProtocol.h)

add_custom_command(
    OUTPUT ${CHIP8EMU_WAYLAND_PROTOCOL_SOURCE} ${CHIP8EMU_WAYLAND_PROTOCOL_CLIENT_HEADER}
    MAIN_DEPENDENCY ${CHIP8EMU_WAYLAND_SPEC_XML}
    COMMAND wayland-scanner private-code ${CHIP8EMU_WAYLAND_SPEC_XML} ${CHIP8EMU_WAYLAND_PROTOCOL_SOURCE}
    COMMAND wayland-scanner client-header ${CHIP8EMU_WAYLAND_SPEC_XML} ${CHIP8EMU_WAYLAND_PROTOCOL_CLIENT_HEADER}
)

target_include_directories(${target} PRIVATE ${CHIP8EMU_WAYLAND_PROTOCOL_INCLUDE_DIR})

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

    # FIXME
    ${CMAKE_CURRENT_SOURCE_DIR}/os-create-anonymous-file.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/os-create-anonymous-file.h

    ${CHIP8EMU_WAYLAND_PROTOCOL_SOURCE}
    ${CHIP8EMU_WAYLAND_PROTOCOL_CLIENT_HEADER}
)

target_link_libraries(${target} PRIVATE
    ${CHIP8EMU_CORE_BIN}
    # FIXME
    wayland-client
)

reaper_configure_library(${target} "Emu")

reaper_add_tests(${target}
    ${CMAKE_CURRENT_SOURCE_DIR}/test/instructions.cpp
)
