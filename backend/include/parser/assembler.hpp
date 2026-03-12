#pragma once

#include "parser/lexer.hpp"
#include "parser/symbol_table.hpp"
#include "instruction/instruction.hpp"
#include "architecture/memory.hpp"
#include "architecture/register_file.hpp"
#include <string>
#include <vector>
#include <map>
#include <cstdint>

class Assembler
{
private:
    Lexer lexer;
    SymbolTable symbol_table;
    RegisterFile &register_reference;

    void perform_first_pass(const std::vector<std::vector<std::string>> &parsed_lines);

    std::map<uint32_t, Instruction> perform_second_pass(
        const std::vector<std::vector<std::string>> &parsed_lines,
        Memory &system_memory);

    Instruction parse_instruction(
        const std::vector<std::string> &tokens,
        uint32_t current_address,
        size_t start_index);

public:
    Assembler(RegisterFile &registers);

    std::map<uint32_t, Instruction> assemble_program(const std::string &file_path, Memory &system_memory);
};