#include "instruction/address_generation.hpp"

void AddressGeneration::execute(const Instruction &current_instruction, RegisterFile &register_file)
{
    if (current_instruction.get_operation() == OperationType::LUI)
    {
        uint32_t immediate_value = static_cast<uint32_t>(current_instruction.get_immediate_value());
        register_file.write_register(current_instruction.get_register_target(), immediate_value << 16);
    }
    else if (current_instruction.get_operation() == OperationType::LSA)
    {
        uint32_t operand_source = register_file.read_register(current_instruction.get_register_source());
        uint32_t operand_target = register_file.read_register(current_instruction.get_register_target());
        uint32_t shift_amount = current_instruction.get_shift_amount();
        uint32_t result = (operand_source << shift_amount) + operand_target;
        register_file.write_register(current_instruction.get_register_destination(), result);
    }
}