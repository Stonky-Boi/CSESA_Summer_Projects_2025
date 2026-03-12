#include "instruction/arithmetic.hpp"
#include <stdexcept>
#include <limits>

void Arithmetic::execute(const Instruction &current_instruction, RegisterFile &register_file)
{
    OperationType operation = current_instruction.get_operation();
    int register_source = current_instruction.get_register_source();
    int register_target = current_instruction.get_register_target();
    int register_destination = current_instruction.get_register_destination();
    int32_t immediate_value = current_instruction.get_immediate_value();
    if (operation == OperationType::ADD)
    {
        int32_t operand_one = static_cast<int32_t>(register_file.read_register(register_source));
        int32_t operand_two = static_cast<int32_t>(register_file.read_register(register_target));
        if ((operand_two > 0 && operand_one > std::numeric_limits<int32_t>::max() - operand_two) ||
            (operand_two < 0 && operand_one < std::numeric_limits<int32_t>::min() - operand_two))
            throw std::overflow_error("Integer overflow detected during ADD operation.");
        register_file.write_register(register_destination, static_cast<uint32_t>(operand_one + operand_two));
    }
    else if (operation == OperationType::ADDU)
    {
        uint32_t operand_one = register_file.read_register(register_source);
        uint32_t operand_two = register_file.read_register(register_target);
        register_file.write_register(register_destination, operand_one + operand_two);
    }
    else if (operation == OperationType::ADDI)
    {
        int32_t operand_one = static_cast<int32_t>(register_file.read_register(register_source));
        if ((immediate_value > 0 && operand_one > std::numeric_limits<int32_t>::max() - immediate_value) ||
            (immediate_value < 0 && operand_one < std::numeric_limits<int32_t>::min() - immediate_value))
            throw std::overflow_error("Integer overflow detected during ADDI operation.");
        register_file.write_register(register_target, static_cast<uint32_t>(operand_one + immediate_value));
    }
    else if (operation == OperationType::SUB)
    {
        int32_t operand_one = static_cast<int32_t>(register_file.read_register(register_source));
        int32_t operand_two = static_cast<int32_t>(register_file.read_register(register_target));
        if ((operand_two < 0 && operand_one > std::numeric_limits<int32_t>::max() + operand_two) ||
            (operand_two > 0 && operand_one < std::numeric_limits<int32_t>::min() + operand_two))
            throw std::overflow_error("Integer overflow detected during SUB operation.");
        register_file.write_register(register_destination, static_cast<uint32_t>(operand_one - operand_two));
    }
    else if (operation == OperationType::SUBU)
    {
        uint32_t operand_one = register_file.read_register(register_source);
        uint32_t operand_two = register_file.read_register(register_target);
        register_file.write_register(register_destination, operand_one - operand_two);
    }
    else if (operation == OperationType::MUL)
    {
        int32_t operand_one = static_cast<int32_t>(register_file.read_register(register_source));
        int32_t operand_two = static_cast<int32_t>(register_file.read_register(register_target));
        int64_t full_product = static_cast<int64_t>(operand_one) * static_cast<int64_t>(operand_two);
        register_file.write_register(register_destination, static_cast<uint32_t>(full_product & 0xFFFFFFFF));
    }
    else if (operation == OperationType::MUH)
    {
        int32_t operand_one = static_cast<int32_t>(register_file.read_register(register_source));
        int32_t operand_two = static_cast<int32_t>(register_file.read_register(register_target));
        int64_t full_product = static_cast<int64_t>(operand_one) * static_cast<int64_t>(operand_two);
        register_file.write_register(register_destination, static_cast<uint32_t>((full_product >> 32) & 0xFFFFFFFF));
    }
    else if (operation == OperationType::MULU)
    {
        uint32_t operand_one = register_file.read_register(register_source);
        uint32_t operand_two = register_file.read_register(register_target);
        uint64_t full_product = static_cast<uint64_t>(operand_one) * static_cast<uint64_t>(operand_two);
        register_file.write_register(register_destination, static_cast<uint32_t>(full_product & 0xFFFFFFFF));
    }
    else if (operation == OperationType::MUHU)
    {
        uint32_t operand_one = register_file.read_register(register_source);
        uint32_t operand_two = register_file.read_register(register_target);
        uint64_t full_product = static_cast<uint64_t>(operand_one) * static_cast<uint64_t>(operand_two);
        register_file.write_register(register_destination, static_cast<uint32_t>((full_product >> 32) & 0xFFFFFFFF));
    }
    else if (operation == OperationType::DIV || operation == OperationType::MOD)
    {
        int32_t dividend = static_cast<int32_t>(register_file.read_register(register_source));
        int32_t divisor = static_cast<int32_t>(register_file.read_register(register_target));
        if (divisor == 0)
            throw std::runtime_error("Division by zero encountered.");
        if (operation == OperationType::DIV)
            register_file.write_register(register_destination, static_cast<uint32_t>(dividend / divisor));
        else
            register_file.write_register(register_destination, static_cast<uint32_t>(dividend % divisor));
    }
    else if (operation == OperationType::DIVU || operation == OperationType::MODU)
    {
        uint32_t dividend = register_file.read_register(register_source);
        uint32_t divisor = register_file.read_register(register_target);
        if (divisor == 0)
            throw std::runtime_error("Unsigned division by zero encountered.");
        if (operation == OperationType::DIVU)
            register_file.write_register(register_destination, dividend / divisor);
        else
            register_file.write_register(register_destination, dividend % divisor);
    }
}