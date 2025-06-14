#include "alu.hpp"

ALU::Result ALU::execute(uint32_t operand1, uint32_t operand2, Operation op) {
    Result result;
    result.carry = false;
    result.overflow = false;
    
    switch (op) {
        case ADD:
            result.value = operand1 + operand2;
            result.overflow = detectOverflow(operand1, operand2, result.value, false);
            result.carry = (result.value < operand1);
            break;
            
        case SUB:
            result.value = operand1 - operand2;
            result.overflow = detectOverflow(operand1, operand2, result.value, true);
            result.carry = (operand1 < operand2);
            break;
            
        case AND:
            result.value = operand1 & operand2;
            break;
            
        case OR:
            result.value = operand1 | operand2;
            break;
            
        case XOR:
            result.value = operand1 ^ operand2;
            break;
            
        case NOR:
            result.value = ~(operand1 | operand2);
            break;
            
        case SLT:
            result.value = ((int32_t)operand1 < (int32_t)operand2) ? 1 : 0;
            break;
            
        case SLTU:
            result.value = (operand1 < operand2) ? 1 : 0;
            break;
            
        default:
            result.value = 0;
            break;
    }
    
    result.zero = (result.value == 0);
    return result;
}

ALU::Result ALU::execute(uint32_t operand1, uint32_t operand2, uint8_t shamt, Operation op) {
    Result result;
    result.carry = false;
    result.overflow = false;
    
    switch (op) {
        case SLL:
            result.value = operand2 << shamt;
            break;
            
        case SRL:
            result.value = operand2 >> shamt;
            break;
            
        case SRA:
            result.value = (int32_t)operand2 >> shamt;
            break;
            
        default:
            return execute(operand1, operand2, op);
    }
    
    result.zero = (result.value == 0);
    return result;
}

bool ALU::detectOverflow(uint32_t a, uint32_t b, uint32_t result, bool is_sub) {
    if (is_sub) {
        // For subtraction, convert to addition: a - b = a + (-b)
        b = ~b + 1;
    }
    
    // Overflow occurs when:
    // 1. Adding two positive numbers gives negative result
    // 2. Adding two negative numbers gives positive result
    bool a_sign = (a & 0x80000000) != 0;
    bool b_sign = (b & 0x80000000) != 0;
    bool result_sign = (result & 0x80000000) != 0;
    
    return (a_sign == b_sign) && (a_sign != result_sign);
}
