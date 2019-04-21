////////////////////////////////////////////////////////////////////////////////
/// chip8-emu
///
/// Copyright (c) 2018 Thibault Schueller
/// This file is distributed under the MIT License
////////////////////////////////////////////////////////////////////////////////

#include "Memory.h"

#include "Cpu.h"

#include "core/Assert.h"

namespace chip8
{
    u16 load_u16_big_endian(const u8* rawMem)
    {
        Assert(rawMem != nullptr);

        const u16 msb = static_cast<u16>((*rawMem) << 8);
        const u16 lsb = static_cast<u16>(*(rawMem + 1));

        return msb | lsb;
    }

    // Returns true if the address range is read/writeable/executable by the program,
    // false otherwise.
    bool is_valid_memory_range(u16 baseAddress, u16 sizeInBytes, MemoryUsage usage)
    {
        Assert(sizeInBytes > 0); // Invalid address range size

        const u16 endAddress = baseAddress + (sizeInBytes - 1);

        if (endAddress < baseAddress)
            return false; // Overflow

        switch (usage)
        {
            case MemoryUsage::Read:
                return endAddress <= MaxProgramAddress;
            case MemoryUsage::Write:
            case MemoryUsage::Execute:
                return baseAddress >= MinProgramAddress && endAddress <= MaxProgramAddress;
            default:
                Assert(false);
                return false;
        }
    }
}
