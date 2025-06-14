#pragma once
#include <cstdint>

class ALU {
public:
    enum Operation {
        ADD = 0,
        SUB = 1,
        AND = 2,
        OR = 3,
        XOR = 4,
        NOR = 5,
        SLT = 6,
        SLTU = 7,
        SLL = 8,
        SRL = 9,
        SRA = 10
    };
    
    struct Result {
        uint32_t value;
        bool zero;
        bool overflow;
        bool carry;
    };
    
    static Result execute(uint32_t operand1, uint32_t operand2, Operation op);
    static Result execute(uint32_t operand1, uint32_t operand2, uint8_t shamt, Operation op);
    
private:
    static bool detectOverflow(uint32_t a, uint32_t b, uint32_t result, bool is_sub);
};
