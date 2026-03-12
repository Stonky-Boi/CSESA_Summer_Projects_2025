#include "parser/symbol_table.hpp"
#include <stdexcept>

SymbolTable::SymbolTable() {}

void SymbolTable::add_label(const std::string &label_name, uint32_t memory_address)
{
    if (contains_label(label_name))
        throw std::runtime_error("Duplicate label definition encountered: " + label_name);
    label_address_map[label_name] = memory_address;
}

uint32_t SymbolTable::get_address(const std::string &label_name) const
{
    std::map<std::string, uint32_t>::const_iterator map_iterator = label_address_map.find(label_name);
    if (map_iterator == label_address_map.end())
        throw std::invalid_argument("Attempted to access undefined label: " + label_name);
    return map_iterator->second;
}

bool SymbolTable::contains_label(const std::string &label_name) const
{
    return label_address_map.find(label_name) != label_address_map.end();
}