#include "instruction/bit_manipulation.hpp"

void BitManipulation::execute(const Instruction &current_instruction, RegisterFile &register_file)
{
    OperationType operation = current_instruction.get_operation();
    int register_source = current_instruction.get_register_source();
    int register_target = current_instruction.get_register_target();
    int register_destination = current_instruction.get_register_destination();
    if (operation == OperationType::CLO)
    {
        uint32_t operand = register_file.read_register(register_source);
        uint32_t leading_ones_count = 0;
        for (int bit_index = 31; bit_index >= 0; --bit_index)
        {
            if ((operand >> bit_index) & 1)
                leading_ones_count++;
            else
                break;
        }
        register_file.write_register(register_destination, leading_ones_count);
    }
    else if (operation == OperationType::CLZ)
    {
        uint32_t operand = register_file.read_register(register_source);
        uint32_t leading_zeros_count = 0;
        for (int bit_index = 31; bit_index >= 0; --bit_index)
        {
            if (((operand >> bit_index) & 1) == 0)
                leading_zeros_count++;
            else
                break;
        }
        register_file.write_register(register_destination, leading_zeros_count);
    }
    else if (operation == OperationType::SEB)
    {
        uint32_t operand = register_file.read_register(register_target);
        int32_t sign_extended_byte = static_cast<int8_t>(operand & 0xFF);
        register_file.write_register(register_destination, static_cast<uint32_t>(sign_extended_byte));
    }
    else if (operation == OperationType::SEH)
    {
        uint32_t operand = register_file.read_register(register_target);
        int32_t sign_extended_halfword = static_cast<int16_t>(operand & 0xFFFF);
        register_file.write_register(register_destination, static_cast<uint32_t>(sign_extended_halfword));
    }
    else if (operation == OperationType::EXT)
    {
        uint32_t operand = register_file.read_register(register_source);
        uint32_t position = current_instruction.get_shift_amount();
        uint32_t size = static_cast<uint32_t>(current_instruction.get_immediate_value());
        uint32_t extracted_value = (operand >> position) & ((1 << size) - 1);
        register_file.write_register(register_target, extracted_value);
    }
    else if (operation == OperationType::INS)
    {
        uint32_t operand_source = register_file.read_register(register_source);
        uint32_t operand_target = register_file.read_register(register_target);
        uint32_t position = current_instruction.get_shift_amount();
        uint32_t size = static_cast<uint32_t>(current_instruction.get_immediate_value());
        uint32_t mask = ((1 << size) - 1) << position;
        uint32_t inserted_value = (operand_source & ((1 << size) - 1)) << position;
        uint32_t result = (operand_target & ~mask) | inserted_value;
        register_file.write_register(register_target, result);
    }
}