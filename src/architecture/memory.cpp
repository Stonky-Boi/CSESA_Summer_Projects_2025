#include "architecture/memory.hpp"
#include <stdexcept>

uint8_t Memory::read_byte(uint32_t memory_address) const
{
    std::map<uint32_t, uint8_t>::const_iterator map_iterator = memory_store.find(memory_address);
    if (map_iterator == memory_store.end())
        return 0;
    return map_iterator->second;
}

void Memory::write_byte(uint32_t memory_address, uint8_t write_value)
{
    memory_store[memory_address] = write_value;
}

uint16_t Memory::read_halfword(uint32_t memory_address) const
{
    if (memory_address % 2 != 0)
        throw std::runtime_error("Unaligned halfword read access at memory address.");
    uint16_t byte_zero = read_byte(memory_address);
    uint16_t byte_one = read_byte(memory_address + 1);
    return byte_zero | (byte_one << 8);
}

void Memory::write_halfword(uint32_t memory_address, uint16_t write_value)
{
    if (memory_address % 2 != 0)
        throw std::runtime_error("Unaligned halfword write access at memory address.");
    write_byte(memory_address, write_value & 0xFF);
    write_byte(memory_address + 1, (write_value >> 8) & 0xFF);
}

uint32_t Memory::read_word(uint32_t memory_address) const
{
    if (memory_address % 4 != 0)
        throw std::runtime_error("Unaligned word read access at memory address.");
    uint32_t byte_zero = read_byte(memory_address);
    uint32_t byte_one = read_byte(memory_address + 1);
    uint32_t byte_two = read_byte(memory_address + 2);
    uint32_t byte_three = read_byte(memory_address + 3);
    return byte_zero | (byte_one << 8) | (byte_two << 16) | (byte_three << 24);
}

void Memory::write_word(uint32_t memory_address, uint32_t write_value)
{
    if (memory_address % 4 != 0)
        throw std::runtime_error("Unaligned word write access at memory address.");
    write_byte(memory_address, write_value & 0xFF);
    write_byte(memory_address + 1, (write_value >> 8) & 0xFF);
    write_byte(memory_address + 2, (write_value >> 16) & 0xFF);
    write_byte(memory_address + 3, (write_value >> 24) & 0xFF);
}