#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include <string>
#include <vector>
#include <cstdint>

struct Instruction {
    std::string original_line;
    std::string operation;
    std::vector<std::string> args;
    uint32_t address;

    Instruction(std::string line = "", uint32_t addr = 0);
    void parse();
};

#endif // INSTRUCTION_HPP