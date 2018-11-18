#///////////////////////////////////////////////////////////////////////////////
#// chip8-emu
#//
#// Copyright (c) 2018 Thibault Schueller
#// This file is distributed under the MIT License
#///////////////////////////////////////////////////////////////////////////////

add_subdirectory(core)
add_subdirectory(chip8)

# Main executable
set(CHIP8EMU_BIN chip8emu)

add_executable(${CHIP8EMU_BIN})

target_sources(${CHIP8EMU_BIN} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_link_libraries(${CHIP8EMU_BIN} PRIVATE
    ${CHIP8EMU_CORE_BIN}
    ${CHIP8EMU_EMU_BIN}
)

reaper_configure_executable(${CHIP8EMU_BIN} "Chip8Emu")
