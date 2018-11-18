////////////////////////////////////////////////////////////////////////////////
/// chip8-emu
///
/// Copyright (c) 2018 Thibault Schueller
/// This file is distributed under the MIT License
////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace chip8
{
    struct Color
    {
        float r;
        float g;
        float b;
    };

    struct Palette
    {
        Color primary;
        Color secondary;
    };

    struct EmuConfig
    {
        bool debugMode;
        Palette palette;
        int screenScale;
    };
}
