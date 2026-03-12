#include "instruction/memory_access.hpp"

void MemoryAccess::execute(const Instruction &current_instruction, RegisterFile &register_file, Memory &system_memory)
{
    OperationType operation = current_instruction.get_operation();
    int register_source = current_instruction.get_register_source();
    int register_target = current_instruction.get_register_target();
    int32_t immediate_value = current_instruction.get_immediate_value();
    uint32_t base_address = register_file.read_register(register_source);
    uint32_t effective_address = base_address + static_cast<uint32_t>(immediate_value);
    if (operation == OperationType::LB)
    {
        uint8_t byte_value = system_memory.read_byte(effective_address);
        int32_t sign_extended_value = static_cast<int8_t>(byte_value);
        register_file.write_register(register_target, static_cast<uint32_t>(sign_extended_value));
    }
    else if (operation == OperationType::LBU)
    {
        uint8_t byte_value = system_memory.read_byte(effective_address);
        register_file.write_register(register_target, static_cast<uint32_t>(byte_value));
    }
    else if (operation == OperationType::LH)
    {
        uint16_t halfword_value = system_memory.read_halfword(effective_address);
        int32_t sign_extended_value = static_cast<int16_t>(halfword_value);
        register_file.write_register(register_target, static_cast<uint32_t>(sign_extended_value));
    }
    else if (operation == OperationType::LHU)
    {
        uint16_t halfword_value = system_memory.read_halfword(effective_address);
        register_file.write_register(register_target, static_cast<uint32_t>(halfword_value));
    }
    else if (operation == OperationType::LW || operation == OperationType::LL)
    {
        uint32_t word_value = system_memory.read_word(effective_address);
        register_file.write_register(register_target, word_value);
    }
    else if (operation == OperationType::SB)
    {
        uint32_t register_value = register_file.read_register(register_target);
        system_memory.write_byte(effective_address, static_cast<uint8_t>(register_value & 0xFF));
    }
    else if (operation == OperationType::SH)
    {
        uint32_t register_value = register_file.read_register(register_target);
        system_memory.write_halfword(effective_address, static_cast<uint16_t>(register_value & 0xFFFF));
    }
    else if (operation == OperationType::SW)
    {
        uint32_t register_value = register_file.read_register(register_target);
        system_memory.write_word(effective_address, register_value);
    }
    else if (operation == OperationType::SC)
    {
        uint32_t register_value = register_file.read_register(register_target);
        system_memory.write_word(effective_address, register_value);
        register_file.write_register(register_target, 1);
    }
}