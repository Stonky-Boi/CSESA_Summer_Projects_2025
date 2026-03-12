#pragma once

#include "instruction/instruction.hpp"
#include "architecture/register_file.hpp"
#include "architecture/memory.hpp"
#include "architecture/program_counter.hpp"
#include "execution/system_coprocessor.hpp"

class Decoder
{
public:
    Decoder();

    bool execute_instruction(
        const Instruction &current_instruction,
        RegisterFile &register_file,
        Memory &system_memory,
        ProgramCounter &program_counter,
        SystemCoprocessor &system_coprocessor) const;
};