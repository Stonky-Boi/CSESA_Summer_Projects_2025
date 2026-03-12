#include "instruction/system.hpp"
#include <stdexcept>

void System::execute(const Instruction &current_instruction, RegisterFile &register_file)
{
    OperationType operation = current_instruction.get_operation();
    if (operation == OperationType::NOP)
    {
        // NOP does nothing
        return;
    }
    else if (operation == OperationType::TEQ)
    {
        uint32_t operand_source = register_file.read_register(current_instruction.get_register_source());
        uint32_t operand_target = register_file.read_register(current_instruction.get_register_target());
        if (operand_source == operand_target)
            throw std::runtime_error("Trap exception triggered by TEQ instruction.");
    }
    else if (operation == OperationType::TNE)
    {
        uint32_t operand_source = register_file.read_register(current_instruction.get_register_source());
        uint32_t operand_target = register_file.read_register(current_instruction.get_register_target());
        if (operand_source != operand_target)
            throw std::runtime_error("Trap exception triggered by TNE instruction.");
    }
    else if (operation == OperationType::BREAK)
        throw std::runtime_error("Breakpoint exception triggered by BREAK instruction.");
    else if (operation == OperationType::SYSCALL)
    {
        // This acts as a placeholder. The Decoder will be updated to route SYSCALL to the SystemCoprocessor.
        throw std::runtime_error("SYSCALL must be handled by the SystemCoprocessor, not the instruction execution logic.");
    }
}