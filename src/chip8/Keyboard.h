////////////////////////////////////////////////////////////////////////////////
/// chip8-emu
///
/// Copyright (c) 2018 Thibault Schueller
/// This file is distributed under the MIT License
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Cpu.h"

#include "core/Types.h"

namespace chip8
{
    // Original keyboard layout:
    // 1  2  3  C
    // 4  5  6  D
    // 7  8  9  E
    // A  0  B  F
    using KeyID = u8;

    static const u8 KeyIDCount = 16;

    bool is_key_pressed(const CPUState& state, KeyID key);
    KeyID wait_for_key_press();

    CHIP8EMU_EMU_API void set_key_pressed(CPUState& state, KeyID key, bool pressedState);
}
