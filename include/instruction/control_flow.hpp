#pragma once

#include "instruction/instruction.hpp"
#include "architecture/register_file.hpp"
#include "architecture/program_counter.hpp"

class ControlFlow
{
public:
    static void execute(const Instruction &current_instruction, RegisterFile &register_file, ProgramCounter &program_counter);
};