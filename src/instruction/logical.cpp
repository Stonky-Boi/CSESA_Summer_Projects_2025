#include "instruction/logical.hpp"

void Logical::execute(const Instruction &current_instruction, RegisterFile &register_file)
{
    OperationType operation = current_instruction.get_operation();
    int register_source = current_instruction.get_register_source();
    int register_target = current_instruction.get_register_target();
    int register_destination = current_instruction.get_register_destination();
    int32_t immediate_value = current_instruction.get_immediate_value();
    if (operation == OperationType::AND)
    {
        uint32_t operand_one = register_file.read_register(register_source);
        uint32_t operand_two = register_file.read_register(register_target);
        register_file.write_register(register_destination, operand_one & operand_two);
    }
    else if (operation == OperationType::ANDI)
    {
        uint32_t operand_one = register_file.read_register(register_source);
        uint32_t zero_extended_immediate = static_cast<uint32_t>(immediate_value & 0x0000FFFF);
        register_file.write_register(register_target, operand_one & zero_extended_immediate);
    }
    else if (operation == OperationType::OR)
    {
        uint32_t operand_one = register_file.read_register(register_source);
        uint32_t operand_two = register_file.read_register(register_target);
        register_file.write_register(register_destination, operand_one | operand_two);
    }
    else if (operation == OperationType::ORI)
    {
        uint32_t operand_one = register_file.read_register(register_source);
        uint32_t zero_extended_immediate = static_cast<uint32_t>(immediate_value & 0x0000FFFF);
        register_file.write_register(register_target, operand_one | zero_extended_immediate);
    }
    else if (operation == OperationType::XOR)
    {
        uint32_t operand_one = register_file.read_register(register_source);
        uint32_t operand_two = register_file.read_register(register_target);
        register_file.write_register(register_destination, operand_one ^ operand_two);
    }
    else if (operation == OperationType::XORI)
    {
        uint32_t operand_one = register_file.read_register(register_source);
        uint32_t zero_extended_immediate = static_cast<uint32_t>(immediate_value & 0x0000FFFF);
        register_file.write_register(register_target, operand_one ^ zero_extended_immediate);
    }
    else if (operation == OperationType::NOR)
    {
        uint32_t operand_one = register_file.read_register(register_source);
        uint32_t operand_two = register_file.read_register(register_target);
        register_file.write_register(register_destination, ~(operand_one | operand_two));
    }
    else if (operation == OperationType::NAND)
    {
        uint32_t operand_one = register_file.read_register(register_source);
        uint32_t operand_two = register_file.read_register(register_target);
        register_file.write_register(register_destination, ~(operand_one & operand_two));
    }
}