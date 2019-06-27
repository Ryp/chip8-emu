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
    static const unsigned int VRegisterCount = 16;
    static const unsigned int StackSize = 16;
    static const unsigned int MemorySizeInBytes = 0x1000;

    // Fonts
    static const unsigned int FontTableGlyphCount = 16;
    static const unsigned int GlyphSizeInBytes = 5;

    // Display
    static const unsigned int ScreenWidth = 64;
    static const unsigned int ScreenHeight = 32;
    static const unsigned int ScreenLineSizeInBytes = ScreenWidth / 8;

    // Memory
    static const u16 MinProgramAddress = 0x0200;
    static const u16 MaxProgramAddress = 0x0FFF;

    // Timings
    static const unsigned int DelayTimerFrequency = 60;
    static const unsigned int InstructionExecutionFrequency = 500;
    static const unsigned int DelayTimerPeriodMs = 1000 / DelayTimerFrequency;
    static const unsigned int InstructionExecutionPeriodMs = 1000 / InstructionExecutionFrequency;

    enum VRegisterName
    {
        V0, V1, V2, V3,
        V4, V5, V6, V7,
        V8, V9, VA, VB,
        VC, VD, VE, VF
    };

    struct CPUState
    {
        u16 pc;
        u8 sp;
        u16 stack[StackSize];
        u8 vRegisters[VRegisterCount];
        u16 i;

        u8 delayTimer;
        u8 soundTimer;

        // Implementation detail
        u32 delayTimerAccumulator;
        u32 executionTimerAccumulator;

        u8* memory;

        u16 keyState;

        u16 keyStatePrev;
        bool isWaitingForKey;

        u16 fontTableOffsets[FontTableGlyphCount];
        u8 screen[ScreenHeight][ScreenLineSizeInBytes];
    };

    CHIP8EMU_EMU_API CPUState createCPUState();
    CHIP8EMU_EMU_API void destroyCPUState(CPUState& state);
}
