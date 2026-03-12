#include "architecture/register_file.hpp"
#include <stdexcept>

RegisterFile::RegisterFile()
{
    for (int register_index = 0; register_index < 32; ++register_index)
        general_purpose_registers[register_index] = 0;
    initialize_register_aliases();
}

void RegisterFile::initialize_register_aliases()
{
    const std::string abi_names[32] = {
        "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3",
        "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
        "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
        "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"};

    int current_index = 0;
    for (const std::string &register_name : abi_names)
    {
        register_alias_map[register_name] = current_index;
        std::string numeric_name = "r" + std::to_string(current_index);
        register_alias_map[numeric_name] = current_index;
        current_index++;
    }
}

uint32_t RegisterFile::read_register(int register_index) const
{
    if (register_index < 0 || register_index > 31)
        throw std::out_of_range("Register index out of bounds during read operation.");
    return general_purpose_registers[register_index];
}

void RegisterFile::write_register(int register_index, uint32_t write_value)
{
    if (register_index < 0 || register_index > 31)
        throw std::out_of_range("Register index out of bounds during write operation.");
    if (register_index == 0)
        return;
    general_purpose_registers[register_index] = write_value;
}

int RegisterFile::get_register_index(const std::string &register_name) const
{
    std::map<std::string, int>::const_iterator map_iterator = register_alias_map.find(register_name);
    if (map_iterator == register_alias_map.end())
        throw std::invalid_argument("Invalid register name encountered: " + register_name);
    return map_iterator->second;
}