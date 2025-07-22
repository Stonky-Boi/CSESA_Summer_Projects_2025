#ifndef REGISTER_FILE_HPP
#define REGISTER_FILE_HPP

#include <cstdint>
#include <array>
#include <string>
#include <map>

class RegisterFile {
public:
    RegisterFile();
    void write(int reg_index, int32_t value);
    int32_t read(int reg_index) const;
    void set_pc(uint32_t address);
    uint32_t get_pc() const;
    void increment_pc();
    static int get_reg_index(const std::string& reg_name);
    static std::string get_reg_name(int index);
    std::array<int32_t, 32> get_registers() const;

private:
    std::array<int32_t, 32> registers;
    uint32_t pc; // Program Counter
    static const std::map<std::string, int> reg_map;
};

#endif // REGISTER_FILE_HPP