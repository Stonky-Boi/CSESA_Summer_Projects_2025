#include "register_file.hpp"
#include <stdexcept>

const std::map<std::string, int> RegisterFile::reg_map = {
    {"$zero", 0}, {"$at", 1}, {"$v0", 2}, {"$v1", 3},
    {"$a0", 4}, {"$a1", 5}, {"$a2", 6}, {"$a3", 7},
    {"$t0", 8}, {"$t1", 9}, {"$t2", 10}, {"$t3", 11},
    {"$t4", 12}, {"$t5", 13}, {"$t6", 14}, {"$t7", 15},
    {"$s0", 16}, {"$s1", 17}, {"$s2", 18}, {"$s3", 19},
    {"$s4", 20}, {"$s5", 21}, {"$s6", 22}, {"$s7", 23},
    {"$t8", 24}, {"$t9", 25}, {"$k0", 26}, {"$k1", 27},
    {"$gp", 28}, {"$sp", 29}, {"$fp", 30}, {"$ra", 31}
};

RegisterFile::RegisterFile() : pc(0x00400000) {
    registers.fill(0);
    // Set stack pointer to the top of memory
    registers[29] = 0x10000000 + 4096 - 4;
}

void RegisterFile::write(int reg_index, int32_t value) {
    if (reg_index > 0 && reg_index < 32) {
        registers[reg_index] = value;
    }
}

int32_t RegisterFile::read(int reg_index) const {
    if (reg_index >= 0 && reg_index < 32) {
        return registers[reg_index];
    }
    return 0; // Should throw an error
}

void RegisterFile::set_pc(uint32_t address) {
    pc = address;
}

uint32_t RegisterFile::get_pc() const {
    return pc;
}

void RegisterFile::increment_pc() {
    pc += 4;
}

int RegisterFile::get_reg_index(const std::string& reg_name) {
    auto it = reg_map.find(reg_name);
    if (it != reg_map.end()) {
        return it->second;
    }
    throw std::runtime_error("Invalid register name: " + reg_name);
}

std::string RegisterFile::get_reg_name(int index) {
    for (const auto& pair : reg_map) {
        if (pair.second == index) {
            return pair.first;
        }
    }
    return "UNKNOWN";
}

std::array<int32_t, 32> RegisterFile::get_registers() const {
    return registers;
}