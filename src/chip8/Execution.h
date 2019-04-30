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
    u16 load_next_instruction(CPUState& state);

    CHIP8EMU_EMU_API void execute_step(const EmuConfig& config, CPUState& state, unsigned int deltaTimeMs);

    void update_timers(CPUState& state, unsigned int& executionCounter, unsigned int deltaTimeMs);
    CHIP8EMU_EMU_API void execute_instruction(const EmuConfig& config, CPUState& state, u16 instruction);
}
