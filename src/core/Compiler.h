////////////////////////////////////////////////////////////////////////////////
/// Reaper
///
/// Copyright (c) 2015-2018 Thibault Schueller
/// This file is distributed under the MIT License
////////////////////////////////////////////////////////////////////////////////

#pragma once

// Detect compiler
#if defined(__INTEL_COMPILER) || defined(__ICC)
#    define CHIP8EMU_COMPILER_ICC
#elif defined(__GNUC__)
#    define CHIP8EMU_COMPILER_GCC
#elif defined(__clang__)
#    define CHIP8EMU_COMPILER_CLANG
#elif defined(_MSC_VER)
#    define CHIP8EMU_COMPILER_MSVC
#else
#    error "Compiler not recognised!"
#endif

// Define symbol export/import macros
#if defined(CHIP8EMU_COMPILER_GCC) || defined(CHIP8EMU_COMPILER_CLANG)
#    define CHIP8EMU_EXPORT __attribute__((visibility("default")))
#    define CHIP8EMU_IMPORT
#elif defined(CHIP8EMU_COMPILER_MSVC)
#    define CHIP8EMU_EXPORT __declspec(dllexport)
#    define CHIP8EMU_IMPORT __declspec(dllimport)
#else
#    error "Could not define import and export macros !"
#endif
