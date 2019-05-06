////////////////////////////////////////////////////////////////////////////////
/// chip8-emu
///
/// Copyright (c) 2018 Thibault Schueller
/// This file is distributed under the MIT License
////////////////////////////////////////////////////////////////////////////////

#include "Keyboard.h"

#include "core/Assert.h"

namespace chip8
{
    bool is_key_pressed(const CPUState& state, KeyID key)
    {
        Assert(key < KeyIDCount); // Invalid key

        return state.keyState & (1 << key);
    }

    // If multiple keys are pressed at the same time, only register one.
    KeyID get_key_pressed(u16 keyState)
    {
        Assert(keyState != 0);

        for (int i = 1; i < 16; i++)
        {
            if ((1 << i) & keyState)
                return i;
        }

        Assert(false);
        return 0x0;
    }

    void set_key_pressed(CPUState& state, KeyID key, bool pressedState)
    {
        Assert(key < KeyIDCount); // Invalid key

        const u16 keyMask = (1 << key);
        state.keyState = (state.keyState & ~keyMask) | (pressedState ? keyMask : 0);
    }
}
