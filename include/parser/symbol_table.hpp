#pragma once

#include <string>
#include <map>
#include <cstdint>

class SymbolTable
{
private:
    std::map<std::string, uint32_t> label_address_map;

public:
    SymbolTable();

    void add_label(const std::string &label_name, uint32_t memory_address);
    uint32_t get_address(const std::string &label_name) const;
    bool contains_label(const std::string &label_name) const;
};