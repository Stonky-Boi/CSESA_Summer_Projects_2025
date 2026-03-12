#include "architecture/program_counter.hpp"
#include <stdexcept>

ProgramCounter::ProgramCounter()
{
    current_address = 0x00400000;
}

uint32_t ProgramCounter::read_address() const
{
    return current_address;
}

void ProgramCounter::write_address(uint32_t new_address)
{
    if (new_address % 4 != 0)
        throw std::runtime_error("Program counter misaligned. Address must be a multiple of 4.");
    current_address = new_address;
}

void ProgramCounter::increment_address()
{
    current_address += 4;
}