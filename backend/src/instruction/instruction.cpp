#include "instruction/instruction.hpp"
#include <stdexcept>

Instruction::Instruction()
{
    format = InstructionFormat::UNKNOWN_FORMAT;
    operation = OperationType::UNKNOWN_OPERATION;
    register_source = 0;
    register_target = 0;
    register_destination = 0;
    shift_amount = 0;
    immediate_value = 0;
    jump_address = 0;
    assembly_string = "";
}

void Instruction::set_format(InstructionFormat new_format)
{
    format = new_format;
}

InstructionFormat Instruction::get_format() const
{
    return format;
}

void Instruction::set_operation(OperationType new_operation)
{
    operation = new_operation;
}

OperationType Instruction::get_operation() const
{
    return operation;
}

void Instruction::set_register_source(int register_index)
{
    if (register_index < 0 || register_index > 31)
        throw std::out_of_range("Source register index is out of bounds.");
    register_source = register_index;
}

int Instruction::get_register_source() const
{
    return register_source;
}

void Instruction::set_register_target(int register_index)
{
    if (register_index < 0 || register_index > 31)
        throw std::out_of_range("Target register index is out of bounds.");
    register_target = register_index;
}

int Instruction::get_register_target() const
{
    return register_target;
}

void Instruction::set_register_destination(int register_index)
{
    if (register_index < 0 || register_index > 31)
        throw std::out_of_range("Destination register index is out of bounds.");
    register_destination = register_index;
}

int Instruction::get_register_destination() const
{
    return register_destination;
}

void Instruction::set_shift_amount(uint32_t amount)
{
    if (amount > 31)
        throw std::out_of_range("Shift amount exceeds 31.");
    shift_amount = amount;
}

uint32_t Instruction::get_shift_amount() const
{
    return shift_amount;
}

void Instruction::set_immediate_value(int32_t value)
{
    immediate_value = value;
}

int32_t Instruction::get_immediate_value() const
{
    return immediate_value;
}

void Instruction::set_jump_address(uint32_t address)
{
    jump_address = address;
}

uint32_t Instruction::get_jump_address() const
{
    return jump_address;
}

void Instruction::set_assembly_string(const std::string &string_value)
{
    assembly_string = string_value;
}

std::string Instruction::get_assembly_string() const
{
    return assembly_string;
}