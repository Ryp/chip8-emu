////////////////////////////////////////////////////////////////////////////////
/// chip8-emu
///
/// Copyright (c) 2018 Thibault Schueller
/// This file is distributed under the MIT License
////////////////////////////////////////////////////////////////////////////////

#include "Instruction.h"

#include "Memory.h"
#include "Keyboard.h"
#include "Display.h"

#include "core/Assert.h"

#include <cstdlib>
#include <cstring>

namespace chip8
{
    // Clear the display.
    void execute_cls(CPUState& state)
    {
        const u32 screenSizeInBytes = ScreenHeight * ScreenLineSizeInBytes;

        std::memset(&state.screen[0][0], 0x0, screenSizeInBytes);
    }

    // Return from a subroutine.
    // The interpreter sets the program counter to the address at the top of the stack,
    // then subtracts 1 from the stack pointer.
    void execute_ret(CPUState& state)
    {
        Assert(state.sp > 0); // Stack Underflow

        const u16 nextPC = state.stack[state.sp] + 2;
        Assert(is_valid_memory_range(nextPC, 2, MemoryUsage::Execute));

        state.pc = nextPC;
        state.sp = (state.sp > 0) ? (state.sp - 1) : state.sp;
    }

    // Jump to a machine code routine at nnn.
    // This instruction is only used on the old computers on which Chip-8 was originally implemented.
    // NOTE: We choose to ignore it since we don't load any code into system memory.
    void execute_sys(CPUState& state, u16 address)
    {
        // noop
    }

    // Jump to location nnn.
    // The interpreter sets the program counter to nnn.
    void execute_jp(CPUState& state, u16 address)
    {
        Assert((address & 0x0001) == 0); // Unaligned address
        Assert(is_valid_memory_range(address, 2, MemoryUsage::Execute));

        state.pc = address;
    }

    // Call subroutine at nnn.
    // The interpreter increments the stack pointer, then puts the current PC on the top of the stack.
    // The PC is then set to nnn.
    void execute_call(CPUState& state, u16 address)
    {
        Assert((address & 0x0001) == 0); // Unaligned address
        Assert(is_valid_memory_range(address, 2, MemoryUsage::Execute));

        Assert(state.sp < StackSize); // Stack overflow

        state.sp = (state.sp < StackSize) ? (state.sp + 1) : state.sp; // Increment sp
        state.stack[state.sp] = state.pc; // Put PC on top of the stack
        state.pc = address; // Set PC to new address
    }

    // Skip next instruction if Vx = kk.
    // The interpreter compares register Vx to kk, and if they are equal,
    // increments the program counter by 2.
    void execute_se(CPUState& state, u8 registerName, u8 value)
    {
        Assert((registerName & ~0x0F) == 0); // Invalid register
        Assert(is_valid_memory_range(state.pc, 6, MemoryUsage::Execute));

        const u8 registerValue = state.vRegisters[registerName];

        if (registerValue == value)
            state.pc += 4;
    }

    // Skip next instruction if Vx != kk.
    // The interpreter compares register Vx to kk, and if they are not equal,
    // increments the program counter by 2.
    void execute_sne(CPUState& state, u8 registerName, u8 value)
    {
        const u8 registerValue = state.vRegisters[registerName];

        Assert((registerName & ~0x0F) == 0); // Invalid register
        Assert(is_valid_memory_range(state.pc, 6, MemoryUsage::Execute));

        if (registerValue != value)
            state.pc += 4;
    }

    // Skip next instruction if Vx = Vy.
    // The interpreter compares register Vx to register Vy, and if they are equal,
    // increments the program counter by 2.
    void execute_se2(CPUState& state, u8 registerLHS, u8 registerRHS)
    {
        Assert((registerLHS & ~0x0F) == 0); // Invalid register
        Assert((registerRHS & ~0x0F) == 0); // Invalid register
        Assert(is_valid_memory_range(state.pc, 6, MemoryUsage::Execute));

        const u8 registerValueLHS = state.vRegisters[registerLHS];
        const u8 registerValueRHS = state.vRegisters[registerRHS];

        if (registerValueLHS == registerValueRHS)
            state.pc += 4;
    }

    // Set Vx = kk.
    // The interpreter puts the value kk into register Vx.
    void execute_ld(CPUState& state, u8 registerName, u8 value)
    {
        Assert((registerName & ~0x0F) == 0); // Invalid register

        state.vRegisters[registerName] = value;
    }

    // Set Vx = Vx + kk.
    // Adds the value kk to the value of register Vx, then stores the result in Vx.
    // NOTE: Carry in NOT set.
    // NOTE: Overflows will just wrap the value around.
    void execute_add(CPUState& state, u8 registerName, u8 value)
    {
        Assert((registerName & ~0x0F) == 0); // Invalid register

        const u8 registerValue = state.vRegisters[registerName];
        const u8 sum = registerValue + value;

        state.vRegisters[registerName] = sum;
    }

    // Set Vx = Vy.
    // Stores the value of register Vy in register Vx.
    void execute_ld2(CPUState& state, u8 registerLHS, u8 registerRHS)
    {
        Assert((registerLHS & ~0x0F) == 0); // Invalid register
        Assert((registerRHS & ~0x0F) == 0); // Invalid register

        state.vRegisters[registerLHS] = state.vRegisters[registerRHS];
    }

    // Set Vx = Vx OR Vy.
    // Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx.
    // A bitwise OR compares the corrseponding bits from two values, and if either bit is 1,
    // then the same bit in the result is also 1. Otherwise, it is 0.
    void execute_or(CPUState& state, u8 registerLHS, u8 registerRHS)
    {
        Assert((registerLHS & ~0x0F) == 0); // Invalid register
        Assert((registerRHS & ~0x0F) == 0); // Invalid register

        state.vRegisters[registerLHS] |= state.vRegisters[registerRHS];
    }

    // Set Vx = Vx AND Vy.
    // Performs a bitwise AND on the values of Vx and Vy, then stores the result in Vx.
    // A bitwise AND compares the corrseponding bits from two values, and if both bits are 1,
    // then the same bit in the result is also 1. Otherwise, it is 0.
    void execute_and(CPUState& state, u8 registerLHS, u8 registerRHS)
    {
        Assert((registerLHS & ~0x0F) == 0); // Invalid register
        Assert((registerRHS & ~0x0F) == 0); // Invalid register

        state.vRegisters[registerLHS] &= state.vRegisters[registerRHS];
    }

    // Set Vx = Vx XOR Vy.
    // Performs a bitwise exclusive OR on the values of Vx and Vy, then stores the result in Vx.
    // An exclusive OR compares the corrseponding bits from two values, and if the bits are not both the same,
    // then the corresponding bit in the result is set to 1.  Otherwise, it is 0.
    void execute_xor(CPUState& state, u8 registerLHS, u8 registerRHS)
    {
        Assert((registerLHS & ~0x0F) == 0); // Invalid register
        Assert((registerRHS & ~0x0F) == 0); // Invalid register

        state.vRegisters[registerLHS] = state.vRegisters[registerLHS] ^ state.vRegisters[registerRHS];
    }

    // Set Vx = Vx + Vy, set VF = carry.
    // The values of Vx and Vy are added together.
    // If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0.
    // Only the lowest 8 bits of the result are kept, and stored in Vx.
    void execute_add2(CPUState& state, u8 registerLHS, u8 registerRHS)
    {
        Assert((registerLHS & ~0x0F) == 0); // Invalid register
        Assert((registerRHS & ~0x0F) == 0); // Invalid register

        const u8 valueLHS = state.vRegisters[registerLHS];
        const u8 valueRHS = state.vRegisters[registerRHS];
        const u8 result = valueLHS + valueRHS;

        state.vRegisters[registerLHS] = result;
        state.vRegisters[VF] = (result > valueLHS) ? 0 : 1; // Set carry
    }

    // Set Vx = Vx - Vy, set VF = NOT borrow.
    // If Vx > Vy, then VF is set to 1, otherwise 0.
    // Then Vy is subtracted from Vx, and the results stored in Vx.
    void execute_sub(CPUState& state, u8 registerLHS, u8 registerRHS)
    {
        Assert((registerLHS & ~0x0F) == 0); // Invalid register
        Assert((registerRHS & ~0x0F) == 0); // Invalid register

        const u8 valueLHS = state.vRegisters[registerLHS];
        const u8 valueRHS = state.vRegisters[registerRHS];
        const u8 result = valueLHS - valueRHS;

        state.vRegisters[registerLHS] = result;
        state.vRegisters[VF] = (valueLHS > valueRHS) ? 1 : 0; // Set carry
    }

    // Set Vx = Vx SHR 1.
    // If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0.
    // Then Vx is divided by 2.
    // NOTE: registerRHS is just ignored apparently.
    void execute_shr1(CPUState& state, u8 registerLHS, u8 registerRHS)
    {
        Assert((registerLHS & ~0x0F) == 0); // Invalid register
        Assert((registerRHS & ~0x0F) == 0); // Invalid register

        const u8 valueLHS = state.vRegisters[registerLHS];

        state.vRegisters[registerLHS] = valueLHS >> 1;
        state.vRegisters[VF] = valueLHS & 0x01; // Set carry
    }

    // Set Vx = Vy - Vx, set VF = NOT borrow.
    // If Vy > Vx, then VF is set to 1, otherwise 0.
    // Then Vx is subtracted from Vy, and the results stored in Vx.
    void execute_subn(CPUState& state, u8 registerLHS, u8 registerRHS)
    {
        Assert((registerLHS & ~0x0F) == 0); // Invalid register
        Assert((registerRHS & ~0x0F) == 0); // Invalid register

        const u8 valueLHS = state.vRegisters[registerLHS];
        const u8 valueRHS = state.vRegisters[registerRHS];
        const u8 result = valueRHS - valueLHS;

        state.vRegisters[registerLHS] = result;
        state.vRegisters[VF] = (valueRHS > valueLHS) ? 1 : 0; // Set carry
    }

    // Set Vx = Vx SHL 1.
    // If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.
    // NOTE: registerRHS is just ignored apparently.
    void execute_shl1(CPUState& state, u8 registerLHS, u8 registerRHS)
    {
        Assert((registerLHS & ~0x0F) == 0); // Invalid register
        Assert((registerRHS & ~0x0F) == 0); // Invalid register

        const u8 valueLHS = state.vRegisters[registerLHS];

        state.vRegisters[registerLHS] = valueLHS << 1;
        state.vRegisters[VF] = (valueLHS & 0x80) ? 1 : 0; // Set carry
    }

    // Skip next instruction if Vx != Vy.
    // The values of Vx and Vy are compared, and if they are not equal, the program counter is increased by 2.
    void execute_sne2(CPUState& state, u8 registerLHS, u8 registerRHS)
    {
        Assert((registerLHS & ~0x0F) == 0); // Invalid register
        Assert((registerRHS & ~0x0F) == 0); // Invalid register
        Assert(is_valid_memory_range(state.pc, 6, MemoryUsage::Execute));

        const u8 valueLHS = state.vRegisters[registerLHS];
        const u8 valueRHS = state.vRegisters[registerRHS];

        if (valueLHS != valueRHS)
            state.pc += 4;
    }

    // Set I = nnn.
    // The value of register I is set to nnn.
    void execute_ldi(CPUState& state, u16 address)
    {
        state.i = address;
    }

    // Jump to location nnn + V0.
    // The program counter is set to nnn plus the value of V0.
    void execute_jp2(CPUState& state, u16 baseAddress)
    {
        const u16 offset = state.vRegisters[V0];
        const u16 targetAddress = baseAddress + offset;

        Assert((targetAddress & 0x0001) == 0); // Unaligned address
        Assert(is_valid_memory_range(targetAddress, 2, MemoryUsage::Execute));

        state.pc = targetAddress;
    }

    // Set Vx = random byte AND kk.
    // The interpreter generates a random number from 0 to 255, which is then ANDed with the value kk.
    // The results are stored in Vx. See instruction 8xy2 for more information on AND.
    void execute_rnd(CPUState& state, u8 registerName, u8 value)
    {
        Assert((registerName & ~0x0F) == 0); // Invalid register

        const u8 randomValue = std::rand();
        state.vRegisters[registerName] = randomValue & value;
    }

    // Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
    // The interpreter reads n bytes from memory, starting at the address stored in I.
    // These bytes are then displayed as sprites on screen at coordinates (Vx, Vy).
    // Sprites are XORed onto the existing screen. If this causes any pixels to be erased, VF is set to 1,
    // otherwise it is set to 0.
    // If the sprite is positioned so part of it is outside the coordinates of the display,
    // it wraps around to the opposite side of the screen. See instruction 8xy3 for more information on XOR,
    // and section 2.4, Display, for more information on the Chip-8 screen and sprites.
    void execute_drw(CPUState& state, u8 registerLHS, u8 registerRHS, u8 size)
    {
        Assert((registerLHS & ~0x0F) == 0); // Invalid register
        Assert((registerRHS & ~0x0F) == 0); // Invalid register
        Assert(is_valid_memory_range(state.i, size, MemoryUsage::Read));

        const int spriteStartX = state.vRegisters[registerLHS];
        const int spriteStartY = state.vRegisters[registerRHS];

        bool collision = false;

        // Sprites are made of rows of 1 byte each.
        for (int rowIndex = 0; rowIndex < size; rowIndex++)
        {
            const u8 spriteRow = state.memory[state.i + rowIndex];
            const u8 screenY = (spriteStartY + rowIndex) % ScreenHeight;

            for (int pixelIndex = 0; pixelIndex < 8; pixelIndex++)
            {
                const u8 spritePixelValue = (spriteRow >> (7 - pixelIndex)) & 0x1;
                const u8 screenX = (spriteStartX + pixelIndex) % ScreenWidth;

                const u8 screenPixelValue = read_screen_pixel(state, screenX, screenY);

                const u8 result = screenPixelValue ^ spritePixelValue;

                if (result != spritePixelValue)
                    collision = true;

                write_screen_pixel(state, screenX, screenY, result);
            }
        }

        state.vRegisters[VF] = collision ? 1 : 0;
    }

    // Skip next instruction if key with the value of Vx is pressed.
    // Checks the keyboard, and if the key corresponding to the value of Vx is currently in the down position,
    // PC is increased by 2.
    void execute_skp(CPUState& state, u8 registerName)
    {
        Assert((registerName & ~0x0F) == 0); // Invalid register
        Assert(is_valid_memory_range(state.pc, 6, MemoryUsage::Execute));

        const u8 keyID = state.vRegisters[registerName];

        if (is_key_pressed(state, keyID))
            state.pc += 4;
    }

    // Skip next instruction if key with the value of Vx is not pressed.
    // Checks the keyboard, and if the key corresponding to the value of Vx is currently in the up position,
    // PC is increased by 2.
    void execute_sknp(CPUState& state, u8 registerName)
    {
        Assert((registerName & ~0x0F) == 0); // Invalid register
        Assert(is_valid_memory_range(state.pc, 6, MemoryUsage::Execute));

        const KeyID key = state.vRegisters[registerName];

        if (!is_key_pressed(state, key))
            state.pc += 4;
    }

    // Set Vx = delay timer value.
    // The value of DT is placed into Vx.
    void execute_ldt(CPUState& state, u8 registerName)
    {
        Assert((registerName & ~0x0F) == 0); // Invalid register

        state.vRegisters[registerName] = state.delayTimer;
    }

    // Wait for a key press, store the value of the key in Vx.
    // All execution stops until a key is pressed, then the value of that key is stored in Vx.
    void execute_ldk(CPUState& state, u8 registerName)
    {
        Assert((registerName & ~0x0F) == 0); // Invalid register

        const KeyID key = wait_for_key_press();

        state.vRegisters[registerName] = key;
    }

    // Set delay timer = Vx.
    // DT is set equal to the value of Vx.
    void execute_lddt(CPUState& state, u8 registerName)
    {
        Assert((registerName & ~0x0F) == 0); // Invalid register

        state.delayTimer = state.vRegisters[registerName];
    }

    // Set sound timer = Vx.
    // ST is set equal to the value of Vx.
    void execute_ldst(CPUState& state, u8 registerName)
    {
        Assert((registerName & ~0x0F) == 0); // Invalid register

        state.soundTimer = state.vRegisters[registerName];
    }

    // Set I = I + Vx.
    // The values of I and Vx are added, and the results are stored in I.
    // NOTE: Carry in NOT set.
    // NOTE: Overflows will just wrap the value around.
    void execute_addi(CPUState& state, u8 registerName)
    {
        Assert((registerName & ~0x0F) == 0); // Invalid register

        const u16 registerValue = state.vRegisters[registerName];
        const u16 iValue = state.i;
        const u16 sum = iValue + registerValue;

        Assert(sum >= iValue); // Overflow

        state.i = sum;
    }

    // Set I = location of sprite for digit Vx.
    // The value of I is set to the location for the hexadecimal sprite corresponding to the value of Vx.
    // See section 2.4, Display, for more information on the Chip-8 hexadecimal font.
    void execute_ldf(CPUState& state, u8 registerName)
    {
        Assert((registerName & ~0x0F) == 0); // Invalid register

        const u8 glyphIndex = state.vRegisters[registerName];

        Assert((glyphIndex & ~0x0F) == 0); // Invalid index

        state.i = state.fontTableOffsets[glyphIndex];
    }

    // Store BCD representation of Vx in memory locations I, I+1, and I+2.
    // The interpreter takes the decimal value of Vx, and places the hundreds digit in memory at location in I,
    // the tens digit at location I+1, and the ones digit at location I+2.
    void execute_ldb(CPUState& state, u8 registerName)
    {
        Assert((registerName & ~0x0F) == 0); // Invalid register
        Assert(is_valid_memory_range(state.i, 3, MemoryUsage::Write));

        const u8 registerValue = state.vRegisters[registerName];

        state.memory[state.i + 0] = (registerValue / 100) % 10;
        state.memory[state.i + 1] = (registerValue / 10) % 10;
        state.memory[state.i + 2] = (registerValue) % 10;
    }

    // Store registers V0 through Vx in memory starting at location I.
    // The interpreter copies the values of registers V0 through Vx into memory,
    // starting at the address in I.
    void execute_ldai(CPUState& state, u8 registerName)
    {
        Assert((registerName & ~0x0F) == 0); // Invalid register

        const u8 registerIndexMax = state.vRegisters[registerName];

        Assert((registerIndexMax & ~0x0F) == 0); // Invalid register
        Assert(is_valid_memory_range(state.i, registerIndexMax + 1, MemoryUsage::Write));

        for (u8 index = 0; index <= registerIndexMax; index++)
            state.memory[state.i + index] = state.vRegisters[index];
    }

    // Read registers V0 through Vx from memory starting at location I.
    // The interpreter reads values from memory starting at location I into registers V0 through Vx.
    void execute_ldm(CPUState& state, u8 registerName)
    {
        Assert((registerName & ~0x0F) == 0); // Invalid register

        const u8 registerIndexMax = state.vRegisters[registerName];

        Assert((registerIndexMax & ~0x0F) == 0); // Invalid register
        Assert(is_valid_memory_range(state.i, registerIndexMax + 1, MemoryUsage::Read));

        for (u8 index = 0; index <= registerIndexMax; index++)
            state.vRegisters[index] = state.memory[state.i + index];
    }
}
