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

    KeyID wait_for_key_press()
    {
        Assert(false); // FIXME
        return 0xA; // FIXME
    }

    void set_key_pressed(CPUState& state, KeyID key, bool pressedState)
    {
        Assert(key < KeyIDCount); // Invalid key

        const u16 keyMask = (1 << key);
        state.keyState = (state.keyState & ~keyMask) | (pressedState ? keyMask : 0);
    }
}
