////////////////////////////////////////////////////////////////////////////////
/// Reaper
///
/// Copyright (c) 2015-2018 Thibault Schueller
/// This file is distributed under the MIT License
////////////////////////////////////////////////////////////////////////////////

// Generated by CMake

#pragma once

#include "core/Compiler.h"

#if defined(CHIP8EMU_BUILD_SHARED)
#    if defined(chip8emu_sdl2_EXPORTS)
#        define CHIP8EMU_SDL2_API CHIP8EMU_EXPORT
#    else
#        define CHIP8EMU_SDL2_API CHIP8EMU_IMPORT
#    endif
#elif defined(CHIP8EMU_BUILD_STATIC)
#    define CHIP8EMU_SDL2_API
#else
#    error "Build type must be defined"
#endif
