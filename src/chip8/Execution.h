////////////////////////////////////////////////////////////////////////////////
/// chip8-emu
///
/// Copyright (c) 2018 Thibault Schueller
/// This file is distributed under the MIT License
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "EmuExport.h"
#include "Config.h"
#include "Cpu.h"

namespace chip8
{
    CHIP8EMU_EMU_API void load_program(CPUState& state, const u8* program, u16 size);
    CHIP8EMU_EMU_API u16 load_next_instruction(CPUState& state);
    CHIP8EMU_EMU_API void execute_instruction(const EmuConfig& config, CPUState& state, u16 instruction);
}
