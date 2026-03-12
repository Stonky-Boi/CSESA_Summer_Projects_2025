#pragma once

#include "architecture/register_file.hpp"
#include "architecture/memory.hpp"

class SystemCoprocessor
{
public:
    SystemCoprocessor();

    bool handle_syscall(RegisterFile &register_file, Memory &system_memory);
};