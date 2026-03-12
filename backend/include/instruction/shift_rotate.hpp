#pragma once

#include "instruction/instruction.hpp"
#include "architecture/register_file.hpp"

class ShiftRotate
{
public:
    static void execute(const Instruction &current_instruction, RegisterFile &register_file);
};