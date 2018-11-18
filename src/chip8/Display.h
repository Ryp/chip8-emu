////////////////////////////////////////////////////////////////////////////////
/// chip8-emu
///
/// Copyright (c) 2018 Thibault Schueller
/// This file is distributed under the MIT License
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "EmuExport.h"

#include "core/Types.h"

namespace chip8
{
    struct CPUState;
    struct EmuConfig;
    struct Palette;

    u8 read_screen_pixel(const CPUState& state, u32 x, u32 y);
    void write_screen_pixel(CPUState& state, u32 x, u32 y, u8 value);

    CHIP8EMU_EMU_API void fill_image_buffer(u8* imageOutput, const CPUState& state, const Palette& palette);
    CHIP8EMU_EMU_API void test(CPUState& state, const EmuConfig& config);
}
