#pragma once

#include <string>
#include <map>
#include <cstdint>

class RegisterFile
{
private:
    uint32_t general_purpose_registers[32];
    std::map<std::string, int> register_alias_map;

    void initialize_register_aliases();

public:
    RegisterFile();

    uint32_t read_register(int register_index) const;
    void write_register(int register_index, uint32_t write_value);

    int get_register_index(const std::string &register_name) const;
};