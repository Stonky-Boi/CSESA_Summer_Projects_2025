#include "instruction/conditional_select.hpp"

void ConditionalSelect::execute(const Instruction &current_instruction, RegisterFile &register_file)
{
    uint32_t operand_source = register_file.read_register(current_instruction.get_register_source());
    uint32_t operand_target = register_file.read_register(current_instruction.get_register_target());
    if (current_instruction.get_operation() == OperationType::SELEQZ)
    {
        if (operand_target == 0)
            register_file.write_register(current_instruction.get_register_destination(), operand_source);
        else
            register_file.write_register(current_instruction.get_register_destination(), 0);
    }
    else if (current_instruction.get_operation() == OperationType::SELNEZ)
    {
        if (operand_target != 0)
            register_file.write_register(current_instruction.get_register_destination(), operand_source);
        else
            register_file.write_register(current_instruction.get_register_destination(), 0);
    }
}