////////////////////////////////////////////////////////////////////////////////
/// chip8-emu
///
/// Copyright (c) 2018 Thibault Schueller
/// This file is distributed under the MIT License
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Cpu.h"

namespace chip8
{
    void execute_cls(CPUState& state);
    void execute_ret(CPUState& state);
    void execute_sys(CPUState& state, u16 address);
    void execute_jp(CPUState& state, u16 address);
    void execute_call(CPUState& state, u16 address);
    void execute_se(CPUState& state, u8 registerName, u8 value);
    void execute_sne(CPUState& state, u8 registerName, u8 value);
    void execute_se2(CPUState& state, u8 registerLHS, u8 registerRHS);
    void execute_ld(CPUState& state, u8 registerName, u8 value);
    void execute_add(CPUState& state, u8 registerName, u8 value);
    void execute_ld2(CPUState& state, u8 registerLHS, u8 registerRHS);
    void execute_or(CPUState& state, u8 registerLHS, u8 registerRHS);
    void execute_and(CPUState& state, u8 registerLHS, u8 registerRHS);
    void execute_xor(CPUState& state, u8 registerLHS, u8 registerRHS);
    void execute_add2(CPUState& state, u8 registerLHS, u8 registerRHS);
    void execute_sub(CPUState& state, u8 registerLHS, u8 registerRHS);
    void execute_shr1(CPUState& state, u8 registerLHS, u8 registerRHS);
    void execute_subn(CPUState& state, u8 registerLHS, u8 registerRHS);
    void execute_shl1(CPUState& state, u8 registerLHS, u8 registerRHS);
    void execute_sne2(CPUState& state, u8 registerLHS, u8 registerRHS);
    void execute_ldi(CPUState& state, u16 address);
    void execute_jp2(CPUState& state, u16 baseAddress);
    void execute_rnd(CPUState& state, u8 registerName, u8 value);
    void execute_drw(CPUState& state, u8 registerLHS, u8 registerRHS, u8 size);
    void execute_skp(CPUState& state, u8 registerName);
    void execute_sknp(CPUState& state, u8 registerName);
    void execute_ldt(CPUState& state, u8 registerName);
    void execute_ldk(CPUState& state, u8 registerName);
    void execute_lddt(CPUState& state, u8 registerName);
    void execute_ldst(CPUState& state, u8 registerName);
    void execute_addi(CPUState& state, u8 registerName);
    void execute_ldf(CPUState& state, u8 registerName);
    void execute_ldb(CPUState& state, u8 registerName);
    void execute_ldai(CPUState& state, u8 registerName);
    void execute_ldm(CPUState& state, u8 registerName);
}
