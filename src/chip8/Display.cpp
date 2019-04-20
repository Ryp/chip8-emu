////////////////////////////////////////////////////////////////////////////////
/// chip8-emu
///
/// Copyright (c) 2018 Thibault Schueller
/// This file is distributed under the MIT License
////////////////////////////////////////////////////////////////////////////////

#include "Display.h"

#include "Config.h"
#include "Cpu.h"
#include "Keyboard.h"
#include "Execution.h"

#include "core/Assert.h"

#include <iostream>
#include <cstring>

namespace chip8
{
    u8 read_screen_pixel(const CPUState& state, u32 x, u32 y)
    {
        const u32 screenOffsetByte = x / 8;
        const u32 screenOffsetBit = x % 8;

        const u8 value = (state.screen[y][screenOffsetByte] >> screenOffsetBit) & 0x1;

        Assert(value == 0 || value == 1);

        return value;
    }

    void write_screen_pixel(CPUState& state, u32 x, u32 y, u8 value)
    {
        Assert(value == 0 || value == 1);

        const u32 screenOffsetByte = x / 8;
        const u32 screenOffsetBit = x % 8;

        const u8 mask = static_cast<u8>(1 << screenOffsetBit);
        const u8 screenByteValue = state.screen[y][screenOffsetByte];

        state.screen[y][screenOffsetByte] = static_cast<u8>(screenByteValue & ~mask) | static_cast<u8>(value << screenOffsetBit);
    }
}

namespace chip8
{
    static const u32 pixelFormatBGRASizeInBytes = 4;

    namespace
    {
        void fill_image_buffer(u8* imageOutput, const CPUState& state, const Palette& palette, int scale)
        {
            const u8 primaryColorBGRA[4] = {
                static_cast<u8>(palette.primary.b * 255.f),
                static_cast<u8>(palette.primary.g * 255.f),
                static_cast<u8>(palette.primary.r * 255.f),
                255
            };
            const u8 secondaryColorBGRA[4] = {
                static_cast<u8>(palette.secondary.b * 255.f),
                static_cast<u8>(palette.secondary.g * 255.f),
                static_cast<u8>(palette.secondary.r * 255.f),
                255
            };

            for (unsigned int j = 0; j < ScreenHeight * scale; j++)
            {
                for (unsigned int i = 0; i < ScreenWidth * scale; i++)
                {
                    const unsigned int pixelIndexFlatDst = j * ScreenWidth * scale + i;
                    const unsigned int pixelOutputOffsetInBytes = pixelIndexFlatDst * pixelFormatBGRASizeInBytes;
                    const u8 pixelValue = read_screen_pixel(state, i / scale, j / scale);

                    if (pixelValue)
                    {
                        imageOutput[pixelOutputOffsetInBytes + 0] = primaryColorBGRA[0];
                        imageOutput[pixelOutputOffsetInBytes + 1] = primaryColorBGRA[1];
                        imageOutput[pixelOutputOffsetInBytes + 2] = primaryColorBGRA[2];
                        imageOutput[pixelOutputOffsetInBytes + 3] = primaryColorBGRA[3];
                    }
                    else
                    {
                        imageOutput[pixelOutputOffsetInBytes + 0] = secondaryColorBGRA[0];
                        imageOutput[pixelOutputOffsetInBytes + 1] = secondaryColorBGRA[1];
                        imageOutput[pixelOutputOffsetInBytes + 2] = secondaryColorBGRA[2];
                        imageOutput[pixelOutputOffsetInBytes + 3] = secondaryColorBGRA[3];
                    }
                }
            }
        }
    }

    void test(CPUState& state, const EmuConfig& config)
    {
        std::cout << "[INFO] enter main loop" << std::endl;

        const unsigned int scale = config.screenScale;
        const unsigned int width = ScreenWidth * scale;
        const unsigned int height = ScreenHeight * scale;
        //const int stride = width * pixelFormatBGRASizeInBytes; // No extra space between lines
        //const int size = stride * ScreenHeight * scale;


        std::cout << "[INFO] exit main loop" << std::endl;
    }
}
