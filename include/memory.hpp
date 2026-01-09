#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <vector>
#include <cstdint>
#include <string>

class Memory {
public:
    Memory(size_t size_bytes = 4096); // 4KB memory

    void write_half(uint32_t address, int16_t value);
    int16_t read_half(uint32_t address) const;

    void write_byte(uint32_t address, int8_t value);
    int8_t read_byte(uint32_t address) const;

    void write_word(uint32_t address, int32_t value);
    int32_t read_word(uint32_t address) const;

    uint32_t store_string(const std::string& str);
    std::string load_string(uint32_t address);

    std::vector<uint8_t> get_memory_state() const;
    
private:
    std::vector<uint8_t> mem;
    uint32_t next_data_address;
};

#endif // MEMORY_HPP