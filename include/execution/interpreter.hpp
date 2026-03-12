#pragma once

#include "architecture/register_file.hpp"
#include "architecture/memory.hpp"
#include "architecture/program_counter.hpp"
#include "instruction/instruction.hpp"
#include "instruction/decoder.hpp"
#include "execution/system_coprocessor.hpp"
#include <map>
#include <cstdint>

class Interpreter
{
private:
    RegisterFile &register_file;
    Memory &system_memory;
    ProgramCounter &program_counter;
    std::map<uint32_t, Instruction> instruction_memory;
    Decoder instruction_decoder;
    SystemCoprocessor system_coprocessor;

public:
    Interpreter(
        RegisterFile &registers,
        Memory &memory,
        ProgramCounter &pc,
        const std::map<uint32_t, Instruction> &program_instructions);

    void execute_program();
};