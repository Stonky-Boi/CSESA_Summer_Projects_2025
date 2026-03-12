#include "execution/interpreter.hpp"
#include <stdexcept>

Interpreter::Interpreter(
    RegisterFile &registers,
    Memory &memory,
    ProgramCounter &pc,
    const std::map<uint32_t, Instruction> &program_instructions) : register_file(registers),
                                                                   system_memory(memory),
                                                                   program_counter(pc),
                                                                   instruction_memory(program_instructions) {}

void Interpreter::execute_program()
{
    bool continue_execution = true;
    while (continue_execution)
    {
        uint32_t current_address = program_counter.read_address();
        std::map<uint32_t, Instruction>::const_iterator map_iterator = instruction_memory.find(current_address);
        if (map_iterator == instruction_memory.end())
            throw std::runtime_error("Program counter reached an unmapped instruction memory address: " + std::to_string(current_address));
        const Instruction &current_instruction = map_iterator->second;
        continue_execution = instruction_decoder.execute_instruction(
            current_instruction,
            register_file,
            system_memory,
            program_counter,
            system_coprocessor);
    }
}