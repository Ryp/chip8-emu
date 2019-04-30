////////////////////////////////////////////////////////////////////////////////
/// chip8-emu
///
/// Copyright (c) 2018 Thibault Schueller
/// This file is distributed under the MIT License
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SDL2Export.h"

namespace chip8
{
    struct CPUState;
    struct EmuConfig;
}

namespace sdl2
{
    CHIP8EMU_SDL2_API int execute_main_loop(chip8::CPUState& state, const chip8::EmuConfig& config);
}
