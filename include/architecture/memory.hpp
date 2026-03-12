#pragma once

#include <cstdint>
#include <map>

class Memory
{
private:
    std::map<uint32_t, uint8_t> memory_store;

public:
    const uint32_t text_segment_start = 0x00400000;
    const uint32_t data_segment_start = 0x10010000;

    Memory() = default;

    uint8_t read_byte(uint32_t memory_address) const;
    void write_byte(uint32_t memory_address, uint8_t write_value);

    uint16_t read_halfword(uint32_t memory_address) const;
    void write_halfword(uint32_t memory_address, uint16_t write_value);

    uint32_t read_word(uint32_t memory_address) const;
    void write_word(uint32_t memory_address, uint32_t write_value);
};