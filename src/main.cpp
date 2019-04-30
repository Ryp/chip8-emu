////////////////////////////////////////////////////////////////////////////////
/// chip8-emu
///
/// Copyright (c) 2018 Thibault Schueller
/// This file is distributed under the MIT License
////////////////////////////////////////////////////////////////////////////////

#include "core/Assert.h"
#include "core/Types.h"

#include "chip8/Config.h"
#include "chip8/Cpu.h"
#include "chip8/Execution.h"

#include "sdl2/SDL2Backend.h"

#include <iostream>
#include <fstream>
#include <vector>

int main(int ac, char** av)
{
    if (ac != 2)
    {
        std::cerr << "error: missing rom file" << std::endl;
        return 1;
    }

    chip8::EmuConfig config = {};
    config.debugMode = true;
    config.palette.primary = { 1.f, 1.f, 1.f };
    config.palette.secondary = { 0.f, 0.f, 0.f };
    config.screenScale = 8;

    chip8::CPUState state = chip8::createCPUState();

    // Load program in chip8 memory
    {
        const char* programPath = av[1];
        Assert(programPath != nullptr);

        std::cout << "[INFO] loading program: " << programPath << std::endl;

        std::ifstream programFile(programPath, std::ios::binary | std::ios::ate);
        const std::ifstream::pos_type programSizeInBytes = programFile.tellg();

        std::cout << "[INFO] rom size: " << programSizeInBytes << std::endl;

        std::vector<char> programContent(programSizeInBytes);

        programFile.seekg(0, std::ios::beg);
        programFile.read(&(programContent[0]), programSizeInBytes);

        std::cout << "[INFO] program loaded" << std::endl;

        chip8::load_program(state, reinterpret_cast<const u8*>(programContent.data()), programSizeInBytes);
    }

    sdl2::execute_main_loop(state, config);

    return 0;
}
