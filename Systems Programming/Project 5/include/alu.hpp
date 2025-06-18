#pragma once
#include <cstdint>
class ALU {
public:
    enum Op { ADD, SUB, AND, OR, XOR, SLL, SRL };
    struct Res { uint32_t val; bool zero,overflow; };
    static Res exec(uint32_t a,uint32_t b,Op op);
    static uint32_t signExt(int16_t imm);
};
