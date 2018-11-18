////////////////////////////////////////////////////////////////////////////////
/// chip8-emu
///
/// Copyright (c) 2018 Thibault Schueller
/// This file is distributed under the MIT License
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "core/Types.h"

namespace chip8
{
    enum class MemoryUsage
    {
        Read,
        Write,
        Execute
    };

    u16 load_u16_big_endian(const u8* rawMem);
    bool is_valid_memory_range(u16 baseAddress, u16 sizeInBytes, MemoryUsage usage);
}
