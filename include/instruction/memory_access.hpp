#pragma once

#include "instruction/instruction.hpp"
#include "architecture/register_file.hpp"
#include "architecture/memory.hpp"

class MemoryAccess
{
public:
    static void execute(const Instruction &current_instruction, RegisterFile &register_file, Memory &system_memory);
};