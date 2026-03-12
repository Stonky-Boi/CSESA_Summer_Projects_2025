#include "instruction/control_flow.hpp"

void ControlFlow::execute(const Instruction &current_instruction, RegisterFile &register_file, ProgramCounter &program_counter)
{
    OperationType operation = current_instruction.get_operation();
    int register_source = current_instruction.get_register_source();
    int register_target = current_instruction.get_register_target();
    int register_destination = current_instruction.get_register_destination();
    uint32_t current_pc = program_counter.read_address();
    uint32_t return_address = current_pc + 4;
    int32_t branch_offset = current_instruction.get_immediate_value();
    if (operation == OperationType::J)
        program_counter.write_address(current_instruction.get_jump_address());
    else if (operation == OperationType::JAL)
    {
        register_file.write_register(31, return_address);
        program_counter.write_address(current_instruction.get_jump_address());
    }
    else if (operation == OperationType::JR)
        program_counter.write_address(register_file.read_register(register_source));
    else if (operation == OperationType::JALR)
    {
        uint32_t target_address = register_file.read_register(register_source);
        register_file.write_register(register_destination, return_address);
        program_counter.write_address(target_address);
    }
    else if (operation == OperationType::B)
        program_counter.write_address(current_pc + 4 + static_cast<uint32_t>(branch_offset));
    else if (operation == OperationType::BAL)
    {
        register_file.write_register(31, return_address);
        program_counter.write_address(current_pc + 4 + static_cast<uint32_t>(branch_offset));
    }
    else if (operation == OperationType::BEQ)
    {
        if (register_file.read_register(register_source) == register_file.read_register(register_target))
            program_counter.write_address(current_pc + 4 + static_cast<uint32_t>(branch_offset));
        else
            program_counter.increment_address();
    }
    else if (operation == OperationType::BNE)
    {
        if (register_file.read_register(register_source) != register_file.read_register(register_target))
            program_counter.write_address(current_pc + 4 + static_cast<uint32_t>(branch_offset));
        else
            program_counter.increment_address();
    }
    else if (operation == OperationType::BGEZ)
    {
        int32_t signed_source = static_cast<int32_t>(register_file.read_register(register_source));
        if (signed_source >= 0)
            program_counter.write_address(current_pc + 4 + static_cast<uint32_t>(branch_offset));
        else
            program_counter.increment_address();
    }
    else if (operation == OperationType::BLEZ)
    {
        int32_t signed_source = static_cast<int32_t>(register_file.read_register(register_source));
        if (signed_source <= 0)
            program_counter.write_address(current_pc + 4 + static_cast<uint32_t>(branch_offset));
        else
            program_counter.increment_address();
    }
    else if (operation == OperationType::BGTZ)
    {
        int32_t signed_source = static_cast<int32_t>(register_file.read_register(register_source));
        if (signed_source > 0)
            program_counter.write_address(current_pc + 4 + static_cast<uint32_t>(branch_offset));
        else
            program_counter.increment_address();
    }
    else if (operation == OperationType::BLTZ)
    {
        int32_t signed_source = static_cast<int32_t>(register_file.read_register(register_source));
        if (signed_source < 0)
            program_counter.write_address(current_pc + 4 + static_cast<uint32_t>(branch_offset));
        else
            program_counter.increment_address();
    }
}