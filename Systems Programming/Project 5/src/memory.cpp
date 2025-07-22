#include "memory.hpp"
#include <stdexcept>

Memory::Memory(size_t size_bytes) : next_data_address(0x10010000) {
    mem.resize(size_bytes, 0);
}

void Memory::write_byte(uint32_t address, int8_t value) {
    uint32_t offset = (address >= 0x10010000) ? address - 0x10010000 : address;
    if (offset < mem.size()) {
        mem[offset] = value;
    } else {
        throw std::out_of_range("Memory write out of bounds");
    }
}

int8_t Memory::read_byte(uint32_t address) const {
    uint32_t offset = (address >= 0x10010000) ? address - 0x10010000 : address;
    if (offset < mem.size()) {
        return mem[offset];
    }
    throw std::out_of_range("Memory read out of bounds");
}

void Memory::write_word(uint32_t address, int32_t value) {
    for (int i = 0; i < 4; ++i) {
        write_byte(address + i, (value >> (i * 8)) & 0xFF);
    }
}

int32_t Memory::read_word(uint32_t address) const {
    int32_t value = 0;
    for (int i = 0; i < 4; ++i) {
        value |= (static_cast<int32_t>(read_byte(address + i)) & 0xFF) << (i * 8);
    }
    return value;
}

uint32_t Memory::store_string(const std::string& str) {
    uint32_t start_address = next_data_address;
    for (char c : str) {
        write_byte(next_data_address++, c);
    }
    write_byte(next_data_address++, '\0'); // Null terminator
    return start_address;
}

std::string Memory::load_string(uint32_t address) {
    std::string result;
    char c;
    while ((c = read_byte(address++)) != '\0') {
        result += c;
    }
    return result;
}

std::vector<uint8_t> Memory::get_memory_state() const {
    return mem;
}

void Memory::write_half(uint32_t address, int16_t value) {
    write_byte(address, value & 0xFF);
    write_byte(address+1, (value >> 8) & 0xFF);
}

int16_t Memory::read_half(uint32_t address) const {
    return (int16_t)((read_byte(address + 1) << 8) | (uint8_t)read_byte(address));
}