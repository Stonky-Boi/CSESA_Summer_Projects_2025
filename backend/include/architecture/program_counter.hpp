#pragma once

#include <cstdint>

class ProgramCounter
{
private:
    uint32_t current_address;

public:
    ProgramCounter();

    uint32_t read_address() const;
    void write_address(uint32_t new_address);
    void increment_address();
};