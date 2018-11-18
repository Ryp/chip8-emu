////////////////////////////////////////////////////////////////////////////////
/// chip8emu
///
/// Copyright (c) 2018 Thibault Schueller
/// This file is distributed under the MIT License
////////////////////////////////////////////////////////////////////////////////

#include <doctest/doctest.h>

#include "chip8/Execution.h"

TEST_CASE("Instructions")
{
    const chip8::EmuConfig config = {};
    chip8::CPUState state = chip8::createCPUState();

    SUBCASE("CLS")
    {
        state.screen[0][0] = 0b11001100;
        state.screen[chip8::ScreenHeight - 1][chip8::ScreenLineSizeInBytes - 1] = 0b10101010;

        chip8::execute_instruction(config, state, 0x00E0);

        CHECK_EQ(state.screen[0][0], 0x00);
        CHECK_EQ(state.screen[chip8::ScreenHeight - 1][chip8::ScreenLineSizeInBytes - 1], 0x00);
    }

    SUBCASE("JP")
    {
        chip8::execute_instruction(config, state, 0x1240);

        CHECK_EQ(state.pc, 0x0240);

        chip8::execute_instruction(config, state, 0x1FFE);

        CHECK_EQ(state.pc, 0x0FFE);
    }

    SUBCASE("CALL/RET")
    {
        chip8::execute_instruction(config, state, 0x2F00);

        CHECK_EQ(state.sp, 1);
        CHECK_EQ(state.pc, 0x0F00);

        chip8::execute_instruction(config, state, 0x2A00);

        CHECK_EQ(state.sp, 2);
        CHECK_EQ(state.pc, 0x0A00);

        chip8::execute_instruction(config, state, 0x00EE);

        CHECK_EQ(state.sp, 1);
        CHECK_EQ(state.pc, 0x0F00);

        chip8::execute_instruction(config, state, 0x00EE);

        CHECK_EQ(state.sp, 0);
        CHECK_EQ(state.pc, chip8::MinProgramAddress);
    }

    SUBCASE("SE")
    {
        chip8::execute_instruction(config, state, 0x3000);

        CHECK_EQ(state.vRegisters[chip8::V0], 0);
        CHECK_EQ(state.pc, chip8::MinProgramAddress + 4);
    }

    SUBCASE("SNE")
    {
        chip8::execute_instruction(config, state, 0x40FF);

        CHECK_EQ(state.vRegisters[chip8::V0], 0);
        CHECK_EQ(state.pc, chip8::MinProgramAddress + 4);
    }

    SUBCASE("SE2")
    {
        chip8::execute_instruction(config, state, 0x5120);

        CHECK_EQ(state.vRegisters[chip8::V0], 0);
        CHECK_EQ(state.vRegisters[chip8::V1], 0);
        CHECK_EQ(state.pc, chip8::MinProgramAddress + 4);
    }

    SUBCASE("LD")
    {
        chip8::execute_instruction(config, state, 0x06042);

        CHECK_EQ(state.vRegisters[chip8::V0], 0x42);

        chip8::execute_instruction(config, state, 0x06A33);

        CHECK_EQ(state.vRegisters[chip8::VA], 0x33);
    }

    SUBCASE("ADD")
    {
        CHECK_EQ(state.vRegisters[chip8::V2], 0x00);

        chip8::execute_instruction(config, state, 0x7203);

        CHECK_EQ(state.vRegisters[chip8::V2], 0x03);

        chip8::execute_instruction(config, state, 0x7204);

        CHECK_EQ(state.vRegisters[chip8::V2], 0x07);
    }

    SUBCASE("LD2")
    {
        state.vRegisters[chip8::V3] = 32;

        chip8::execute_instruction(config, state, 0x8030);

        CHECK_EQ(state.vRegisters[chip8::V0], 32);
    }

    SUBCASE("OR")
    {
        state.vRegisters[chip8::VC] = 0xF0;
        state.vRegisters[chip8::VD] = 0x0F;

        chip8::execute_instruction(config, state, 0x8CD1);

        CHECK_EQ(state.vRegisters[chip8::VC], 0xFF);
    }

    SUBCASE("AND")
    {
        state.vRegisters[chip8::VC] = 0xF0;
        state.vRegisters[chip8::VD] = 0x0F;

        chip8::execute_instruction(config, state, 0x8CD2);

        CHECK_EQ(state.vRegisters[chip8::VC], 0x00);

        state.vRegisters[chip8::VC] = 0xF0;
        state.vRegisters[chip8::VD] = 0xFF;

        chip8::execute_instruction(config, state, 0x8CD2);

        CHECK_EQ(state.vRegisters[chip8::VC], 0xF0);
    }

    SUBCASE("XOR")
    {
        state.vRegisters[chip8::VC] = 0x10;
        state.vRegisters[chip8::VD] = 0x1F;

        chip8::execute_instruction(config, state, 0x8CD3);

        CHECK_EQ(state.vRegisters[chip8::VC], 0x0F);
    }

    SUBCASE("ADD")
    {
        state.vRegisters[chip8::V0] = 8;
        state.vRegisters[chip8::V1] = 8;

        chip8::execute_instruction(config, state, 0x8014);

        CHECK_EQ(state.vRegisters[chip8::V0], 16);
        CHECK_EQ(state.vRegisters[chip8::VF], 0);

        state.vRegisters[chip8::V0] = 128;
        state.vRegisters[chip8::V1] = 130;

        chip8::execute_instruction(config, state, 0x8014);

        CHECK_EQ(state.vRegisters[chip8::V0], 2);
        CHECK_EQ(state.vRegisters[chip8::VF], 1);
    }

    SUBCASE("SUB")
    {
        state.vRegisters[chip8::V0] = 8;
        state.vRegisters[chip8::V1] = 7;

        chip8::execute_instruction(config, state, 0x8015);

        CHECK_EQ(state.vRegisters[chip8::V0], 1);
        CHECK_EQ(state.vRegisters[chip8::VF], 1);

        state.vRegisters[chip8::V0] = 8;
        state.vRegisters[chip8::V1] = 9;

        chip8::execute_instruction(config, state, 0x8015);

        CHECK_EQ(state.vRegisters[chip8::V0], 255);
        CHECK_EQ(state.vRegisters[chip8::VF], 0);
    }

    SUBCASE("SHR")
    {
        state.vRegisters[chip8::V0] = 8;

        chip8::execute_instruction(config, state, 0x8016);

        CHECK_EQ(state.vRegisters[chip8::V0], 4);
        CHECK_EQ(state.vRegisters[chip8::VF], 0);

        chip8::execute_instruction(config, state, 0x8026);

        CHECK_EQ(state.vRegisters[chip8::V0], 2);
        CHECK_EQ(state.vRegisters[chip8::VF], 0);

        chip8::execute_instruction(config, state, 0x8026);

        CHECK_EQ(state.vRegisters[chip8::V0], 1);
        CHECK_EQ(state.vRegisters[chip8::VF], 0);

        chip8::execute_instruction(config, state, 0x8026);

        CHECK_EQ(state.vRegisters[chip8::V0], 0);
        CHECK_EQ(state.vRegisters[chip8::VF], 1);
    }

    SUBCASE("SUBN")
    {
        state.vRegisters[chip8::V0] = 7;
        state.vRegisters[chip8::V1] = 8;

        chip8::execute_instruction(config, state, 0x8017);

        CHECK_EQ(state.vRegisters[chip8::V0], 1);
        CHECK_EQ(state.vRegisters[chip8::VF], 1);

        state.vRegisters[chip8::V0] = 2;
        state.vRegisters[chip8::V1] = 1;

        chip8::execute_instruction(config, state, 0x8017);

        CHECK_EQ(state.vRegisters[chip8::V0], 255);
        CHECK_EQ(state.vRegisters[chip8::VF], 0);
    }

    SUBCASE("SHL")
    {
        state.vRegisters[chip8::V0] = 64;

        chip8::execute_instruction(config, state, 0x801E);

        CHECK_EQ(state.vRegisters[chip8::V0], 128);
        CHECK_EQ(state.vRegisters[chip8::VF], 0);

        chip8::execute_instruction(config, state, 0x801E);

        CHECK_EQ(state.vRegisters[chip8::V0], 0);
        CHECK_EQ(state.vRegisters[chip8::VF], 1);
    }

    SUBCASE("SNE2")
    {
        state.vRegisters[chip8::V9] = 64;
        state.vRegisters[chip8::VA] = 64;

        chip8::execute_instruction(config, state, 0x99A0);

        CHECK_EQ(state.pc, chip8::MinProgramAddress + 2);

        state.vRegisters[chip8::VA] = 0;
        chip8::execute_instruction(config, state, 0x99A0);

        CHECK_EQ(state.pc, chip8::MinProgramAddress + 6);
    }

    SUBCASE("LDI")
    {
        chip8::execute_instruction(config, state, 0xA242);

        CHECK_EQ(state.i, 0x0242);
    }

    SUBCASE("JP2")
    {
        state.vRegisters[chip8::V0] = 0x02;

        chip8::execute_instruction(config, state, 0xB240);

        CHECK_EQ(state.pc, 0x0242);
    }

    SUBCASE("RND")
    {
        chip8::execute_instruction(config, state, 0xC10F);

        CHECK_EQ(state.vRegisters[chip8::V1] & ~0x0F, 0);

        chip8::execute_instruction(config, state, 0xC1F0);

        CHECK_EQ(state.vRegisters[chip8::V1] & ~0xF0, 0);
    }

    SUBCASE("DRW")
    {
        // TODO
        // chip8::execute_instruction(config, state, 0x00E0); // Clear screen
        // state.vRegisters[chip8::V0] = 0x0F; // Set digit to print
        // state.vRegisters[chip8::V1] = 0x00; // Set digit to print
        // chip8::execute_instruction(config, state, 0xF029); // Load digit sprite address
        // chip8::execute_instruction(config, state, 0xD115); // Draw sprite
        // for (int i = 0; i < 10; i++)
        // {
        //     chip8::write_screen_pixel(state, chip8::ScreenWidth - i - 1, chip8::ScreenHeight - i - 1, 1);
        // }
    }

    SUBCASE("SKP")
    {
        state.vRegisters[chip8::VA] = 0x0F;
        state.keyState = 0x8000;

        chip8::execute_instruction(config, state, 0xEA9E);

        CHECK_EQ(state.pc, chip8::MinProgramAddress + 4); // Skipped

        chip8::execute_instruction(config, state, 0xEB9E);

        CHECK_EQ(state.pc, chip8::MinProgramAddress + 6); // Did not skip

    }

    SUBCASE("SKNP")
    {
        state.vRegisters[chip8::VA] = 0xF;
        state.keyState = 0x8000;

        chip8::execute_instruction(config, state, 0xEBA1);

        CHECK_EQ(state.pc, chip8::MinProgramAddress + 4); // Skipped

        chip8::execute_instruction(config, state, 0xEAA1);

        CHECK_EQ(state.pc, chip8::MinProgramAddress + 6); // Did not skip
    }

    SUBCASE("LDT")
    {
        state.delayTimer = 42;
        state.vRegisters[chip8::V4] = 0;

        chip8::execute_instruction(config, state, 0xF407);

        CHECK_EQ(state.vRegisters[chip8::V4], 42);
    }

    SUBCASE("LDK")
    {
        // TODO
    }

    SUBCASE("LDDT")
    {
        state.vRegisters[chip8::V5] = 66;

        chip8::execute_instruction(config, state, 0xF515);

        CHECK_EQ(state.delayTimer, 66);
    }

    SUBCASE("LDST")
    {
        state.vRegisters[chip8::V6] = 33;

        chip8::execute_instruction(config, state, 0xF618);

        CHECK_EQ(state.soundTimer, 33);
    }

    SUBCASE("ADDI")
    {
        state.vRegisters[chip8::V9] = 10;
        state.i = chip8::MinProgramAddress;

        chip8::execute_instruction(config, state, 0xF91E);

        CHECK_EQ(state.i, chip8::MinProgramAddress + 10);
    }

    SUBCASE("LDF")
    {
        state.vRegisters[chip8::V0] = 9;

        chip8::execute_instruction(config, state, 0xF029);

        CHECK_EQ(state.i, state.fontTableOffsets[9]);

        state.vRegisters[chip8::V0] = 0xF;

        chip8::execute_instruction(config, state, 0xF029);

        CHECK_EQ(state.i, state.fontTableOffsets[0xF]);
    }

    SUBCASE("LDB")
    {
        state.i = chip8::MinProgramAddress;
        state.vRegisters[chip8::V7] = 109;

        chip8::execute_instruction(config, state, 0xF733);

        CHECK_EQ(state.memory[state.i + 0], 1);
        CHECK_EQ(state.memory[state.i + 1], 0);
        CHECK_EQ(state.memory[state.i + 2], 9);

        state.vRegisters[chip8::V7] = 255;

        chip8::execute_instruction(config, state, 0xF733);

        CHECK_EQ(state.memory[state.i + 0], 2);
        CHECK_EQ(state.memory[state.i + 1], 5);
        CHECK_EQ(state.memory[state.i + 2], 5);
    }

    SUBCASE("LDAI")
    {
        state.i = chip8::MinProgramAddress;
        state.memory[state.i + 0] = 0xF4;
        state.memory[state.i + 1] = 0x33;
        state.memory[state.i + 2] = 0x82;
        state.memory[state.i + 3] = 0x73;

        state.vRegisters[chip8::V0] = 0xE4;
        state.vRegisters[chip8::V1] = 0x23;
        state.vRegisters[chip8::V2] = 0x00;

        state.vRegisters[chip8::VA] = 0x1;

        chip8::execute_instruction(config, state, 0xFA55);

        CHECK_EQ(state.memory[state.i + 0], 0xE4);
        CHECK_EQ(state.memory[state.i + 1], 0x23);
        CHECK_EQ(state.memory[state.i + 2], 0x82);
        CHECK_EQ(state.memory[state.i + 3], 0x73);
    }

    SUBCASE("LDM")
    {
        state.i = chip8::MinProgramAddress;
        state.vRegisters[chip8::V0] = 0xF4;
        state.vRegisters[chip8::V1] = 0x33;
        state.vRegisters[chip8::V2] = 0x82;
        state.vRegisters[chip8::V3] = 0x73;

        state.memory[state.i + 0] = 0xE4;
        state.memory[state.i + 1] = 0x23;
        state.memory[state.i + 2] = 0x00;

        state.vRegisters[chip8::VA] = 0x1;

        chip8::execute_instruction(config, state, 0xFA65);

        CHECK_EQ(state.vRegisters[chip8::V0], 0xE4);
        CHECK_EQ(state.vRegisters[chip8::V1], 0x23);
        CHECK_EQ(state.vRegisters[chip8::V2], 0x82);
        CHECK_EQ(state.vRegisters[chip8::V3], 0x73);
    }

    chip8::destroyCPUState(state);
}
