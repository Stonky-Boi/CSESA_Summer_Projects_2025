#include "instruction/shift_rotate.hpp"

void ShiftRotate::execute(const Instruction &current_instruction, RegisterFile &register_file)
{
    OperationType operation = current_instruction.get_operation();
    int register_source = current_instruction.get_register_source();
    int register_target = current_instruction.get_register_target();
    int register_destination = current_instruction.get_register_destination();
    uint32_t shift_amount = current_instruction.get_shift_amount();
    if (operation == OperationType::SLL)
    {
        uint32_t operand = register_file.read_register(register_target);
        register_file.write_register(register_destination, operand << shift_amount);
    }
    else if (operation == OperationType::SRL)
    {
        uint32_t operand = register_file.read_register(register_target);
        register_file.write_register(register_destination, operand >> shift_amount);
    }
    else if (operation == OperationType::SRA)
    {
        int32_t signed_operand = static_cast<int32_t>(register_file.read_register(register_target));
        register_file.write_register(register_destination, static_cast<uint32_t>(signed_operand >> shift_amount));
    }
    else if (operation == OperationType::SLLV)
    {
        uint32_t operand = register_file.read_register(register_target);
        uint32_t variable_shift_amount = register_file.read_register(register_source) & 0x1F;
        register_file.write_register(register_destination, operand << variable_shift_amount);
    }
    else if (operation == OperationType::SRLV)
    {
        uint32_t operand = register_file.read_register(register_target);
        uint32_t variable_shift_amount = register_file.read_register(register_source) & 0x1F;
        register_file.write_register(register_destination, operand >> variable_shift_amount);
    }
    else if (operation == OperationType::SRAV)
    {
        int32_t signed_operand = static_cast<int32_t>(register_file.read_register(register_target));
        uint32_t variable_shift_amount = register_file.read_register(register_source) & 0x1F;
        register_file.write_register(register_destination, static_cast<uint32_t>(signed_operand >> variable_shift_amount));
    }
    else if (operation == OperationType::ROTL)
    {
        uint32_t operand = register_file.read_register(register_target);
        uint32_t result = (operand << shift_amount) | (operand >> (32 - shift_amount));
        register_file.write_register(register_destination, result);
    }
    else if (operation == OperationType::ROTR)
    {
        uint32_t operand = register_file.read_register(register_target);
        uint32_t result = (operand >> shift_amount) | (operand << (32 - shift_amount));
        register_file.write_register(register_destination, result);
    }
    else if (operation == OperationType::ROTLV)
    {
        uint32_t operand = register_file.read_register(register_target);
        uint32_t variable_shift_amount = register_file.read_register(register_source) & 0x1F;
        uint32_t result = (operand << variable_shift_amount) | (operand >> (32 - variable_shift_amount));
        register_file.write_register(register_destination, result);
    }
    else if (operation == OperationType::ROTRV)
    {
        uint32_t operand = register_file.read_register(register_target);
        uint32_t variable_shift_amount = register_file.read_register(register_source) & 0x1F;
        uint32_t result = (operand >> variable_shift_amount) | (operand << (32 - variable_shift_amount));
        register_file.write_register(register_destination, result);
    }
}