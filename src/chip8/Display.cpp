////////////////////////////////////////////////////////////////////////////////
/// chip8-emu
///
/// Copyright (c) 2018 Thibault Schueller
/// This file is distributed under the MIT License
////////////////////////////////////////////////////////////////////////////////

#include "Display.h"

#include "Cpu.h"

#include "core/Assert.h"

#include <iostream>
#include <cstring>

namespace chip8
{
    u8 read_screen_pixel(const CPUState& state, u32 x, u32 y)
    {
        const u32 screenOffsetByte = x / 8;
        const u32 screenOffsetBit = x % 8;

        const u8 value = (state.screen[y][screenOffsetByte] >> screenOffsetBit) & 0x1;

        Assert(value == 0 || value == 1);

        return value;
    }

    void write_screen_pixel(CPUState& state, u32 x, u32 y, u8 value)
    {
        Assert(value == 0 || value == 1);

        const u32 screenOffsetByte = x / 8;
        const u32 screenOffsetBit = x % 8;

        const u8 mask = static_cast<u8>(1 << screenOffsetBit);
        const u8 screenByteValue = state.screen[y][screenOffsetByte];

        state.screen[y][screenOffsetByte] = static_cast<u8>(screenByteValue & ~mask) | static_cast<u8>(value << screenOffsetBit);
    }
}
