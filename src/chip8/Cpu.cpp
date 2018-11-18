////////////////////////////////////////////////////////////////////////////////
/// chip8-emu
///
/// Copyright (c) 2018 Thibault Schueller
/// This file is distributed under the MIT License
////////////////////////////////////////////////////////////////////////////////

#include "Cpu.h"

#include "core/Assert.h"

#include <cstring>
#include <iostream>

namespace chip8
{
    namespace
    {
        static const u16 FontTableOffsetInBytes = 0x0000;
        static const u8 FontTable[FontTableGlyphCount][GlyphSizeInBytes] =
        {
            { 0xF0, 0x90, 0x90, 0x90, 0xF0 }, // 0
            { 0x20, 0x60, 0x20, 0x20, 0x70 }, // 1
            { 0xF0, 0x10, 0xF0, 0x80, 0xF0 }, // etc...
            { 0xF0, 0x10, 0xF0, 0x10, 0xF0 },
            { 0x90, 0x90, 0xF0, 0x10, 0x10 },
            { 0xF0, 0x80, 0xF0, 0x10, 0xF0 },
            { 0xF0, 0x80, 0xF0, 0x90, 0xF0 },
            { 0xF0, 0x10, 0x20, 0x40, 0x40 },
            { 0xF0, 0x90, 0xF0, 0x90, 0xF0 },
            { 0xF0, 0x90, 0xF0, 0x10, 0xF0 },
            { 0xF0, 0x90, 0xF0, 0x90, 0x90 },
            { 0xE0, 0x90, 0xE0, 0x90, 0xE0 },
            { 0xF0, 0x80, 0x80, 0x80, 0xF0 },
            { 0xE0, 0x90, 0x90, 0x90, 0xE0 },
            { 0xF0, 0x80, 0xF0, 0x80, 0xF0 },
            { 0xF0, 0x80, 0xF0, 0x80, 0x80 }
        };

        void load_font_table(CPUState& state)
        {
            const u16 tableOffset = FontTableOffsetInBytes;
            const u16 tableSize = FontTableGlyphCount * GlyphSizeInBytes;

            // Make sure we don't spill in program addressable space.
            Assert((tableOffset + tableSize - 1) < MinProgramAddress);

            std::memcpy(&state.memory[FontTableOffsetInBytes], &FontTable[0][0], tableSize);

            // Assing font table addresses in memory
            for (u32 tableIndex = 0; tableIndex < FontTableGlyphCount; tableIndex++)
                state.fontTableOffsets[tableIndex] = tableOffset + GlyphSizeInBytes * tableIndex;
        }
    }

    CPUState createCPUState()
    {
        CPUState state = {};

        // Leave the memory uninitialized. This is very useful when debugging with valgrind.
        state.memory = new u8[MemorySizeInBytes];

        // Set PC to first address
        state.pc = MinProgramAddress;

        load_font_table(state);

        return state;
    }

    void destroyCPUState(CPUState& state)
    {
         delete[] state.memory;
         state.memory = nullptr;
    }

    void dump_cpu_state(const CPUState& state)
    {
        // TODO
        std::cout << "I: " << std::hex << state.i << std::endl;

        for (u32 index = 0; index < VRegisterCount; index++)
            std::cout << "V" << index << ": " << std::hex << static_cast<u32>(state.vRegisters[index]) << std::endl;
    }
}
