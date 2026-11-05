// SPDX-FileCopyrightText: Copyright 2026 Eden Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include <algorithm>
#include <array>
#include <catch2/catch_test_macros.hpp>

#include "shader_recompiler/frontend/ampere/lower_to_maxwell.h"
#include "shader_recompiler/frontend/ampere/program_capture.h"

namespace {
using Shader::Ampere::Instruction;
using Shader::Ampere::LowerToMaxwell;
using Shader::Ampere::ProgramCaptureState;

constexpr Instruction EXIT{{0x0000794d, 0, 0x03800000, 0}};

// Lowered Maxwell words whose top bits (above `shift`) are `opcode`.
template <typename Program>
size_t CountMaxwell(const Program& lowered, u64 opcode, u32 shift) {
    return static_cast<size_t>(std::count_if(lowered.instructions.begin(),
                                             lowered.instructions.end(),
                                             [&](u64 word) { return (word >> shift) == opcode; }));
}

template <typename Program>
u64 FindMaxwell(const Program& lowered, u64 opcode, u32 shift) {
    const auto it = std::find_if(lowered.instructions.begin(), lowered.instructions.end(),
                                 [&](u64 word) { return (word >> shift) == opcode; });
    return it != lowered.instructions.end() ? *it : 0;
}

TEST_CASE("Ampere BRA.CONV always branches and BRA.DIV never does", "[video_core][ampere]") {
    // BRA.CONV UR6 / BRA.DIV ~URZ to the second EXIT: the mode is in the offset's low two bits.
    const std::array<Instruction, 3> convergent{{
        {{0x06007947, 0x10 | 3, 0x0b800000, 0}},
        EXIT,
        EXIT,
    }};
    ProgramCaptureState capture;
    REQUIRE(capture.Observe(convergent[0], 0) == ProgramCaptureState::Result::Continue);
    const auto taken = LowerToMaxwell(convergent);
    REQUIRE(static_cast<bool>(taken));
    REQUIRE(CountMaxwell(taken, 0xe24, 52) == 1);

    const std::array<Instruction, 3> divergent{{
        {{0xff007947, 0x10 | 2, 0x0b800000, 0}},
        EXIT,
        EXIT,
    }};
    const auto dropped = LowerToMaxwell(divergent);
    REQUIRE(static_cast<bool>(dropped));
    REQUIRE(CountMaxwell(dropped, 0xe24, 52) == 0);
}

TEST_CASE("Bananza captured BRA.U preserves its predicate and relative target",
          "[video_core][ampere]") {
    // 040074A01BF12000 fragment abb4ff47..., code offset 0x5b0.
    constexpr Instruction Nop{{0x00007918, 0, 0, 0x000fc000}};
    std::array<Instruction, 133> code;
    code.fill(Nop);
    code[91] = Instruction{{0x00008947, 0x00000281, 0x03800000, 0x000fea00}};
    code.back() = EXIT;
    const auto lowered = LowerToMaxwell(code);
    INFO(lowered.error);
    REQUIRE(lowered);
    REQUIRE(lowered.instructions.size() == code.size());
    CHECK(lowered.instructions[91] == 0xe24000001b08000fULL);

    const auto backward = LowerToMaxwell(std::array{
        Nop, Instruction{{0x00008947, 0xffffffe1, 0x0383ffff, 0x000fea00}}, EXIT});
    REQUIRE(backward);
    CHECK(backward.instructions[1] == 0xe2400fffff08000fULL);

    const auto guarded = LowerToMaxwell(std::array{
        Instruction{{0x00008947, 0x11, 0x04800000, 0x000fea00}}, Nop, EXIT});
    REQUIRE(guarded);
    REQUIRE(guarded.instructions.size() == 4);
    CHECK(guarded.instructions[0] == 0xe24000000100000fULL);
    CHECK(guarded.instructions[1] == 0xe24000000109000fULL);
}

TEST_CASE("Ampere BRA.U rejects unsupported fields and invalid targets", "[video_core][ampere]") {
    const Instruction branch{{0x00008947, 0x1, 0x03800000, 0x000fea00}};
    for (const u32 bit : {82U, 83U, 84U, 85U, 86U, 91U, 92U, 93U, 94U, 95U,
                          96U, 97U, 98U, 99U, 100U, 101U, 102U, 103U, 104U}) {
        auto unsupported = branch;
        unsupported.words[bit / 32] |= 1U << (bit % 32);
        const auto lowered = LowerToMaxwell(std::array{unsupported, EXIT});
        INFO(bit);
        REQUIRE_FALSE(lowered);
        CHECK(lowered.error == "native branch convergence mode");
    }
    for (const u32 offset : {5U, 0x11U, 0xffffffe1U}) {
        auto invalid = branch;
        invalid.words[1] = offset;
        if (offset == 0xffffffe1U) invalid.words[2] |= 0x3ffff;
        REQUIRE_FALSE(LowerToMaxwell(std::array{invalid, EXIT}));
    }
}

TEST_CASE("Ampere TLD.SCR.LZ takes its handle from a uniform register", "[video_core][ampere]") {
    // TLD.SCR.LZ R5, R8, R4, R5, UR4, 0x0, 2D, 0x7 -> Maxwell TLD.B.
    const std::array<Instruction, 2> code{{
        {{0x04087f66, 0x30000405, 0x089e0705, 0x001f4200}},
        EXIT,
    }};
    const auto lowered = LowerToMaxwell(code);
    REQUIRE(static_cast<bool>(lowered));
    const u64 tld = FindMaxwell(lowered, 0xdd, 56);
    REQUIRE(tld != 0);
    REQUIRE(((tld >> 28) & 7) == 2); // 2D
    REQUIRE(((tld >> 31) & 0xf) == 7);
}

TEST_CASE("Ampere I2F converts signed 16-bit sources", "[video_core][ampere]") {
    // I2F.S16 R8, R8
    const std::array<Instruction, 2> code{{
        {{0x00087306, 0x00000008, 0x00101400, 0x000fe200}},
        EXIT,
    }};
    const auto lowered = LowerToMaxwell(code);
    REQUIRE(static_cast<bool>(lowered));
    const u64 i2f = FindMaxwell(lowered, 0x5cb, 52);
    REQUIRE(((i2f >> 10) & 3) == 1); // 16-bit source
    REQUIRE(((i2f >> 13) & 1) == 1); // signed
}

TEST_CASE("Ampere integer-to-float conversions take immediates", "[video_core][ampere]") {
    // I2F.U32.RP R7, 0x20; I2FP.F32.U32.RZ R4, 0x100; WARPSYNC R7
    const std::array<Instruction, 4> code{{
        {{0x00077906, 0x00000020, 0x00209000, 0x000e2200}},
        {{0x00047845, 0x00000100, 0x0020d000, 0x000fe200}},
        {{0x00007348, 0x00000007, 0x03800000, 0x000fe800}},
        EXIT,
    }};
    REQUIRE(static_cast<bool>(LowerToMaxwell(code)));
}

TEST_CASE("Ampere SUST.P keeps its channel mask", "[video_core][ampere]") {
    // SUST.P.2D.STRONG.SM.R.IGN [R6], R4, 0x0, 0x59
    const std::array<Instruction, 2> code{{
        {{0x06007b9b, 0x60005904, 0x0010a100, 0x000fe200}},
        EXIT,
    }};
    const auto lowered = LowerToMaxwell(code);
    REQUIRE(static_cast<bool>(lowered));
    REQUIRE(((FindMaxwell(lowered, 0xeb2, 52) >> 20) & 0xf) == 1);
}

TEST_CASE("Ampere SHFL with a register lane keeps its mode", "[video_core][ampere]") {
    // SHFL.DOWN PT, R7, R0, R5, 0x1f
    const std::array<Instruction, 2> code{{
        {{0x00077589, 0x08001f05, 0x000e0000, 0x000fe200}},
        EXIT,
    }};
    const auto lowered = LowerToMaxwell(code);
    REQUIRE(static_cast<bool>(lowered));
    REQUIRE(((FindMaxwell(lowered, 0xef1, 52) >> 30) & 3) == 2);
}

TEST_CASE("Ampere calls find a return address set ahead of them", "[video_core][ampere]") {
    // MOV R9, 0x30; MOV R4, 0x5; CALL.REL.NOINC; EXIT; RET.REL.NODEC R9
    std::array<Instruction, 5> code{{
        {{0x00097802, 0x30, 0x00000f00, 0}},
        {{0x00047802, 0x5, 0x00000f00, 0}},
        {{0x00007944, 0x10, 0x03c00000, 0}},
        EXIT,
        {{0x09007950, 0xffffffb0, 0x03c3ffff, 0}},
    }};
    REQUIRE(static_cast<bool>(LowerToMaxwell(code)));

    // The link register is written again between its MOV and the call.
    code[1] = {{0x00097802, 0x5, 0x00000f00, 0}};
    const auto lowered = LowerToMaxwell(code);
    REQUIRE(!static_cast<bool>(lowered));
    REQUIRE(lowered.error == "native call linkage mode");
}

TEST_CASE("Ampere programs go on past an EXIT that a branch jumps over", "[video_core][ampere]") {
    // A call, then a branch over an EXIT to more of the program, with another call there.
    const std::array<Instruction, 8> code{{
        {{0x00047802, 0x20, 0x00000f00, 0}},
        {{0x00007944, 0x50, 0x03c00000, 0}},
        {{0x00007947, 0x10, 0x03800000, 0}},
        EXIT,
        {{0x00047802, 0x60, 0x00000f00, 0}},
        {{0x00007944, 0x10, 0x03c00000, 0}},
        EXIT,
        {{0x04007950, 0xffffff80, 0x03c3ffff, 0}},
    }};
    const auto lowered = LowerToMaxwell(code);
    REQUIRE(static_cast<bool>(lowered));
    REQUIRE(lowered.source_size == sizeof(code));
}

TEST_CASE("Ocarina UIMAD.WIDE.U32 with an immediate writes the uniform pair", "[video_core][ampere]") {
    // UIMAD.WIDE.U32 UR4, UR6, 0x88888889, URZ (0400486024DEC000 compute): a divide by 15.
    const auto lowered = LowerToMaxwell(
        std::array{Instruction{{0x060478a5, 0x88888889, 0x0f8e003f, 0x000fc400}}, EXIT});
    INFO(lowered.error);
    REQUIRE(static_cast<bool>(lowered));
    // UR4 and UR5 live in R220 and R221; the immediate goes through scratch R248.
    size_t low = 0, high = 0;
    for (const u64 word : lowered.instructions) {
        if ((word >> 52) != 0x5c1) continue;
        low += (word & 0xff) == 220;
        high += (word & 0xff) == 221;
    }
    CHECK(low == 1);
    CHECK(high == 1);
    CHECK(CountMaxwell(lowered, 0x0100000000000000ULL >> 52, 52) >= 1);
}

TEST_CASE("Ocarina ISETP.EX combines with an input predicate other than PT", "[video_core][ampere]") {
    // ISETP.NE.U32.AND.EX P1, PT, R0, UR9, P1, P2 (0400486024DEC000): P1 = (hi != OR P2) AND P1.
    const auto lowered = LowerToMaxwell(
        std::array{Instruction{{0xff007c0c, 0x00000009, 0x08f25120, 0x000fe400}}, EXIT});
    INFO(lowered.error);
    REQUIRE(static_cast<bool>(lowered));
    // ISETP.NE into the free temporary P0, OR the low words' P2.
    const u64 isetp = FindMaxwell(lowered, 0x5b6, 52);
    REQUIRE(isetp != 0);
    CHECK(((isetp >> 3) & 7) == 0);
    CHECK(((isetp >> 39) & 7) == 2);
    CHECK(((isetp >> 45) & 3) == 1);
    CHECK(((isetp >> 49) & 7) == 5);
    // PSETP P1 = P0 AND P1, the original predicate saved and restored around it.
    const u64 psetp = FindMaxwell(lowered, 0x509, 52);
    REQUIRE(psetp != 0);
    CHECK(((psetp >> 3) & 7) == 1);
    CHECK(((psetp >> 12) & 7) == 0);
    CHECK(((psetp >> 39) & 7) == 1);
    CHECK(((psetp >> 45) & 3) == 0);
    CHECK(CountMaxwell(lowered, 0x38e8000000070000ULL >> 44, 44) == 1);
    CHECK(CountMaxwell(lowered, 0x38f0000000070000ULL >> 44, 44) == 1);
}

} // namespace
