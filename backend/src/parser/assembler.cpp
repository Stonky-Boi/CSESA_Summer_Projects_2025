#include "parser/assembler.hpp"
#include <stdexcept>
#include <iostream>

Assembler::Assembler(RegisterFile &registers) : register_reference(registers) {}

void Assembler::perform_first_pass(const std::vector<std::vector<std::string>> &parsed_lines)
{
    uint32_t current_text_address = 0x00400000;
    uint32_t current_data_address = 0x10010000;
    bool inside_text_section = true;
    for (const std::vector<std::string> &current_line : parsed_lines)
    {
        if (current_line.empty())
            continue;
        std::string first_token = current_line[0];
        if (first_token == ".text")
        {
            inside_text_section = true;
            continue;
        }
        else if (first_token == ".data")
        {
            inside_text_section = false;
            continue;
        }
        std::string possible_label = first_token;
        if (possible_label.back() == ':')
        {
            possible_label.pop_back();
            if (inside_text_section)
                symbol_table.add_label(possible_label, current_text_address);
            else
                symbol_table.add_label(possible_label, current_data_address);
            if (current_line.size() == 1)
                continue;
        }
        if (inside_text_section)
        {
            if (first_token.back() != ':' || current_line.size() > 1)
                current_text_address += 4;
        }
        else
        {
            size_t token_start_index = 0;
            if (first_token.back() == ':')
                token_start_index = 1;
            if (token_start_index >= current_line.size())
                continue;
            std::string data_directive = current_line[token_start_index];
            if (data_directive == ".word")
                current_data_address += 4 * (current_line.size() - token_start_index - 1);
            else if (data_directive == ".half")
                current_data_address += 2 * (current_line.size() - token_start_index - 1);
            else if (data_directive == ".byte")
                current_data_address += 1 * (current_line.size() - token_start_index - 1);
            else if (data_directive == ".asciiz")
            {
                std::string string_literal = current_line[token_start_index + 1];
                current_data_address += string_literal.length() - 1;
            }
            else
                throw std::runtime_error("Unknown data directive encountered: " + data_directive);
        }
    }
}

std::map<uint32_t, Instruction> Assembler::perform_second_pass(
    const std::vector<std::vector<std::string>> &parsed_lines,
    Memory &system_memory)
{
    std::map<uint32_t, Instruction> instruction_memory;
    uint32_t current_text_address = 0x00400000;
    uint32_t current_data_address = 0x10010000;
    bool inside_text_section = true;
    for (const std::vector<std::string> &current_line : parsed_lines)
    {
        if (current_line.empty())
            continue;
        std::string first_token = current_line[0];
        if (first_token == ".text")
        {
            inside_text_section = true;
            continue;
        }
        else if (first_token == ".data")
        {
            inside_text_section = false;
            continue;
        }
        size_t token_start_index = 0;
        if (first_token.back() == ':')
        {
            token_start_index = 1;
            if (current_line.size() == 1)
                continue;
        }
        if (inside_text_section)
        {
            Instruction generated_instruction = parse_instruction(current_line, current_text_address, token_start_index);
            instruction_memory[current_text_address] = generated_instruction;
            current_text_address += 4;
        }
        else
        {
            std::string data_directive = current_line[token_start_index];
            if (data_directive == ".word")
            {
                for (size_t index = token_start_index + 1; index < current_line.size(); ++index)
                {
                    uint32_t value = std::stoul(current_line[index]);
                    system_memory.write_word(current_data_address, value);
                    current_data_address += 4;
                }
            }
            else if (data_directive == ".asciiz")
            {
                std::string string_literal = current_line[token_start_index + 1];
                for (size_t character_index = 1; character_index < string_literal.length() - 1; ++character_index)
                {
                    system_memory.write_byte(current_data_address, static_cast<uint8_t>(string_literal[character_index]));
                    current_data_address += 1;
                }
                system_memory.write_byte(current_data_address, 0);
                current_data_address += 1;
            }
        }
    }
    return instruction_memory;
}

Instruction Assembler::parse_instruction(const std::vector<std::string> &tokens, uint32_t current_address, size_t start_index)
{
    Instruction new_instruction;
    std::string operation_mnemonic = tokens[start_index];
    std::string full_assembly_string = "";
    for (size_t index = start_index; index < tokens.size(); ++index)
    {
        full_assembly_string += tokens[index];
        if (index != tokens.size() - 1)
            full_assembly_string += " ";
    }
    new_instruction.set_assembly_string(full_assembly_string);
    OperationType operation_type = OperationType::UNKNOWN_OPERATION;

    if (operation_mnemonic == "ADD")
        operation_type = OperationType::ADD;
    else if (operation_mnemonic == "ADDI")
        operation_type = OperationType::ADDI;
    else if (operation_mnemonic == "ADDU")
        operation_type = OperationType::ADDU;
    else if (operation_mnemonic == "SUB")
        operation_type = OperationType::SUB;
    else if (operation_mnemonic == "SUBU")
        operation_type = OperationType::SUBU;
    else if (operation_mnemonic == "MUL")
        operation_type = OperationType::MUL;
    else if (operation_mnemonic == "MUH")
        operation_type = OperationType::MUH;
    else if (operation_mnemonic == "MULU")
        operation_type = OperationType::MULU;
    else if (operation_mnemonic == "MUHU")
        operation_type = OperationType::MUHU;
    else if (operation_mnemonic == "DIV")
        operation_type = OperationType::DIV;
    else if (operation_mnemonic == "DIVU")
        operation_type = OperationType::DIVU;
    else if (operation_mnemonic == "MOD")
        operation_type = OperationType::MOD;
    else if (operation_mnemonic == "MODU")
        operation_type = OperationType::MODU;

    else if (operation_mnemonic == "AND")
        operation_type = OperationType::AND;
    else if (operation_mnemonic == "ANDI")
        operation_type = OperationType::ANDI;
    else if (operation_mnemonic == "OR")
        operation_type = OperationType::OR;
    else if (operation_mnemonic == "ORI")
        operation_type = OperationType::ORI;
    else if (operation_mnemonic == "XOR")
        operation_type = OperationType::XOR;
    else if (operation_mnemonic == "XORI")
        operation_type = OperationType::XORI;
    else if (operation_mnemonic == "NOR")
        operation_type = OperationType::NOR;
    else if (operation_mnemonic == "NAND")
        operation_type = OperationType::NAND;

    else if (operation_mnemonic == "SLL")
        operation_type = OperationType::SLL;
    else if (operation_mnemonic == "SRL")
        operation_type = OperationType::SRL;
    else if (operation_mnemonic == "SRA")
        operation_type = OperationType::SRA;
    else if (operation_mnemonic == "SLLV")
        operation_type = OperationType::SLLV;
    else if (operation_mnemonic == "SRLV")
        operation_type = OperationType::SRLV;
    else if (operation_mnemonic == "SRAV")
        operation_type = OperationType::SRAV;
    else if (operation_mnemonic == "ROTL")
        operation_type = OperationType::ROTL;
    else if (operation_mnemonic == "ROTR")
        operation_type = OperationType::ROTR;
    else if (operation_mnemonic == "ROTLV")
        operation_type = OperationType::ROTLV;
    else if (operation_mnemonic == "ROTRV")
        operation_type = OperationType::ROTRV;

    else if (operation_mnemonic == "SLT")
        operation_type = OperationType::SLT;
    else if (operation_mnemonic == "SGT")
        operation_type = OperationType::SGT;
    else if (operation_mnemonic == "SLE")
        operation_type = OperationType::SLE;
    else if (operation_mnemonic == "SGE")
        operation_type = OperationType::SGE;
    else if (operation_mnemonic == "SEQ")
        operation_type = OperationType::SEQ;
    else if (operation_mnemonic == "SNE")
        operation_type = OperationType::SNE;
    else if (operation_mnemonic == "SLTI")
        operation_type = OperationType::SLTI;
    else if (operation_mnemonic == "SGTI")
        operation_type = OperationType::SGTI;
    else if (operation_mnemonic == "SLEI")
        operation_type = OperationType::SLEI;
    else if (operation_mnemonic == "SGEI")
        operation_type = OperationType::SGEI;
    else if (operation_mnemonic == "SEQI")
        operation_type = OperationType::SEQI;
    else if (operation_mnemonic == "SNEI")
        operation_type = OperationType::SNEI;
    else if (operation_mnemonic == "SLTU")
        operation_type = OperationType::SLTU;
    else if (operation_mnemonic == "SGTU")
        operation_type = OperationType::SGTU;
    else if (operation_mnemonic == "SLEU")
        operation_type = OperationType::SLEU;
    else if (operation_mnemonic == "SGEU")
        operation_type = OperationType::SGEU;
    else if (operation_mnemonic == "SLTIU")
        operation_type = OperationType::SLTIU;
    else if (operation_mnemonic == "SGTIU")
        operation_type = OperationType::SGTIU;
    else if (operation_mnemonic == "SLEIU")
        operation_type = OperationType::SLEIU;
    else if (operation_mnemonic == "SGEIU")
        operation_type = OperationType::SGEIU;

    else if (operation_mnemonic == "CLO")
        operation_type = OperationType::CLO;
    else if (operation_mnemonic == "CLZ")
        operation_type = OperationType::CLZ;
    else if (operation_mnemonic == "EXT")
        operation_type = OperationType::EXT;
    else if (operation_mnemonic == "INS")
        operation_type = OperationType::INS;
    else if (operation_mnemonic == "SEB")
        operation_type = OperationType::SEB;
    else if (operation_mnemonic == "SEH")
        operation_type = OperationType::SEH;

    else if (operation_mnemonic == "SELEQZ")
        operation_type = OperationType::SELEQZ;
    else if (operation_mnemonic == "SELNEZ")
        operation_type = OperationType::SELNEZ;

    else if (operation_mnemonic == "LUI")
        operation_type = OperationType::LUI;
    else if (operation_mnemonic == "LSA")
        operation_type = OperationType::LSA;

    else if (operation_mnemonic == "LB")
        operation_type = OperationType::LB;
    else if (operation_mnemonic == "LBU")
        operation_type = OperationType::LBU;
    else if (operation_mnemonic == "LH")
        operation_type = OperationType::LH;
    else if (operation_mnemonic == "LHU")
        operation_type = OperationType::LHU;
    else if (operation_mnemonic == "LW")
        operation_type = OperationType::LW;
    else if (operation_mnemonic == "LL")
        operation_type = OperationType::LL;

    else if (operation_mnemonic == "SB")
        operation_type = OperationType::SB;
    else if (operation_mnemonic == "SH")
        operation_type = OperationType::SH;
    else if (operation_mnemonic == "SW")
        operation_type = OperationType::SW;
    else if (operation_mnemonic == "SC")
        operation_type = OperationType::SC;

    else if (operation_mnemonic == "J")
        operation_type = OperationType::J;
    else if (operation_mnemonic == "JR")
        operation_type = OperationType::JR;
    else if (operation_mnemonic == "JAL")
        operation_type = OperationType::JAL;
    else if (operation_mnemonic == "JALR")
        operation_type = OperationType::JALR;

    else if (operation_mnemonic == "B")
        operation_type = OperationType::B;
    else if (operation_mnemonic == "BAL")
        operation_type = OperationType::BAL;
    else if (operation_mnemonic == "BEQ")
        operation_type = OperationType::BEQ;
    else if (operation_mnemonic == "BNE")
        operation_type = OperationType::BNE;
    else if (operation_mnemonic == "BGEZ")
        operation_type = OperationType::BGEZ;
    else if (operation_mnemonic == "BLEZ")
        operation_type = OperationType::BLEZ;
    else if (operation_mnemonic == "BGTZ")
        operation_type = OperationType::BGTZ;
    else if (operation_mnemonic == "BLTZ")
        operation_type = OperationType::BLTZ;

    else if (operation_mnemonic == "NOP")
        operation_type = OperationType::NOP;
    else if (operation_mnemonic == "BREAK")
        operation_type = OperationType::BREAK;
    else if (operation_mnemonic == "SYSCALL")
        operation_type = OperationType::SYSCALL;
    else if (operation_mnemonic == "TEQ")
        operation_type = OperationType::TEQ;
    else if (operation_mnemonic == "TNE")
        operation_type = OperationType::TNE;

    else
        throw std::runtime_error("Unsupported or invalid instruction mnemonic: " + operation_mnemonic);

    new_instruction.set_operation(operation_type);

    if (operation_type == OperationType::ADD || operation_type == OperationType::ADDU ||
        operation_type == OperationType::SUB || operation_type == OperationType::SUBU ||
        operation_type == OperationType::MUL || operation_type == OperationType::MUH ||
        operation_type == OperationType::MULU || operation_type == OperationType::MUHU ||
        operation_type == OperationType::DIV || operation_type == OperationType::DIVU ||
        operation_type == OperationType::MOD || operation_type == OperationType::MODU ||
        operation_type == OperationType::AND || operation_type == OperationType::OR ||
        operation_type == OperationType::XOR || operation_type == OperationType::NOR ||
        operation_type == OperationType::NAND || operation_type == OperationType::SLT ||
        operation_type == OperationType::SGT || operation_type == OperationType::SLE ||
        operation_type == OperationType::SGE || operation_type == OperationType::SEQ ||
        operation_type == OperationType::SNE || operation_type == OperationType::SLTU ||
        operation_type == OperationType::SGTU || operation_type == OperationType::SLEU ||
        operation_type == OperationType::SGEU || operation_type == OperationType::SELEQZ ||
        operation_type == OperationType::SELNEZ)
    {
        if (tokens.size() < start_index + 4)
            throw std::runtime_error("Insufficient arguments for: " + operation_mnemonic);
        new_instruction.set_format(InstructionFormat::R_TYPE);
        new_instruction.set_register_destination(register_reference.get_register_index(tokens[start_index + 1]));
        new_instruction.set_register_source(register_reference.get_register_index(tokens[start_index + 2]));
        new_instruction.set_register_target(register_reference.get_register_index(tokens[start_index + 3]));
    }

    else if (operation_type == OperationType::SLLV || operation_type == OperationType::SRLV ||
             operation_type == OperationType::SRAV || operation_type == OperationType::ROTLV ||
             operation_type == OperationType::ROTRV)
    {
        if (tokens.size() < start_index + 4)
            throw std::runtime_error("Insufficient arguments for: " + operation_mnemonic);
        new_instruction.set_format(InstructionFormat::R_TYPE);
        new_instruction.set_register_destination(register_reference.get_register_index(tokens[start_index + 1]));
        new_instruction.set_register_target(register_reference.get_register_index(tokens[start_index + 2]));
        new_instruction.set_register_source(register_reference.get_register_index(tokens[start_index + 3]));
    }

    else if (operation_type == OperationType::ADDI || operation_type == OperationType::ANDI ||
             operation_type == OperationType::ORI || operation_type == OperationType::XORI ||
             operation_type == OperationType::SLTI || operation_type == OperationType::SGTI ||
             operation_type == OperationType::SLEI || operation_type == OperationType::SGEI ||
             operation_type == OperationType::SEQI || operation_type == OperationType::SNEI ||
             operation_type == OperationType::SLTIU || operation_type == OperationType::SGTIU ||
             operation_type == OperationType::SLEIU || operation_type == OperationType::SGEIU)
    {
        if (tokens.size() < start_index + 4)
            throw std::runtime_error("Insufficient arguments for: " + operation_mnemonic);
        new_instruction.set_format(InstructionFormat::I_TYPE);
        new_instruction.set_register_target(register_reference.get_register_index(tokens[start_index + 1]));
        new_instruction.set_register_source(register_reference.get_register_index(tokens[start_index + 2]));
        new_instruction.set_immediate_value(std::stoi(tokens[start_index + 3]));
    }

    else if (operation_type == OperationType::SLL || operation_type == OperationType::SRL ||
             operation_type == OperationType::SRA || operation_type == OperationType::ROTL ||
             operation_type == OperationType::ROTR)
    {
        if (tokens.size() < start_index + 4)
            throw std::runtime_error("Insufficient arguments for: " + operation_mnemonic);
        new_instruction.set_format(InstructionFormat::R_TYPE);
        new_instruction.set_register_destination(register_reference.get_register_index(tokens[start_index + 1]));
        new_instruction.set_register_target(register_reference.get_register_index(tokens[start_index + 2]));
        new_instruction.set_shift_amount(std::stoul(tokens[start_index + 3]));
    }

    else if (operation_type == OperationType::CLO || operation_type == OperationType::CLZ)
    {
        if (tokens.size() < start_index + 3)
            throw std::runtime_error("Insufficient arguments for: " + operation_mnemonic);
        new_instruction.set_format(InstructionFormat::R_TYPE);
        new_instruction.set_register_destination(register_reference.get_register_index(tokens[start_index + 1]));
        new_instruction.set_register_source(register_reference.get_register_index(tokens[start_index + 2]));
    }

    else if (operation_type == OperationType::SEB || operation_type == OperationType::SEH)
    {
        if (tokens.size() < start_index + 3)
            throw std::runtime_error("Insufficient arguments for: " + operation_mnemonic);
        new_instruction.set_format(InstructionFormat::R_TYPE);
        new_instruction.set_register_destination(register_reference.get_register_index(tokens[start_index + 1]));
        new_instruction.set_register_target(register_reference.get_register_index(tokens[start_index + 2]));
    }

    else if (operation_type == OperationType::LUI)
    {
        if (tokens.size() < start_index + 3)
            throw std::runtime_error("Insufficient arguments for: " + operation_mnemonic);
        new_instruction.set_format(InstructionFormat::I_TYPE);
        new_instruction.set_register_target(register_reference.get_register_index(tokens[start_index + 1]));
        new_instruction.set_immediate_value(std::stoi(tokens[start_index + 2]));
    }

    else if (operation_type == OperationType::LB || operation_type == OperationType::LBU ||
             operation_type == OperationType::LH || operation_type == OperationType::LHU ||
             operation_type == OperationType::LW || operation_type == OperationType::LL ||
             operation_type == OperationType::SB || operation_type == OperationType::SH ||
             operation_type == OperationType::SW || operation_type == OperationType::SC)
    {
        if (tokens.size() < start_index + 3)
            throw std::runtime_error("Insufficient arguments for: " + operation_mnemonic);
        new_instruction.set_format(InstructionFormat::I_TYPE);
        new_instruction.set_register_target(register_reference.get_register_index(tokens[start_index + 1]));
        std::string memory_operand = tokens[start_index + 2];
        size_t open_paren_index = memory_operand.find('(');
        size_t close_paren_index = memory_operand.find(')');
        if (open_paren_index == std::string::npos || close_paren_index == std::string::npos)
            throw std::runtime_error("Invalid memory operand format: " + memory_operand);
        std::string offset_string = memory_operand.substr(0, open_paren_index);
        std::string base_register_string = memory_operand.substr(open_paren_index + 1, close_paren_index - open_paren_index - 1);
        new_instruction.set_immediate_value(std::stoi(offset_string));
        new_instruction.set_register_source(register_reference.get_register_index(base_register_string));
    }

    else if (operation_type == OperationType::BEQ || operation_type == OperationType::BNE)
    {
        if (tokens.size() < start_index + 4)
            throw std::runtime_error("Insufficient arguments for: " + operation_mnemonic);
        new_instruction.set_format(InstructionFormat::I_TYPE);
        new_instruction.set_register_source(register_reference.get_register_index(tokens[start_index + 1]));
        new_instruction.set_register_target(register_reference.get_register_index(tokens[start_index + 2]));
        std::string branch_label = tokens[start_index + 3];
        if (!symbol_table.contains_label(branch_label))
            throw std::runtime_error("Undefined label in branch instruction: " + branch_label);
        uint32_t target_address = symbol_table.get_address(branch_label);
        new_instruction.set_immediate_value(static_cast<int32_t>(target_address) - static_cast<int32_t>(current_address + 4));
    }

    else if (operation_type == OperationType::BGEZ || operation_type == OperationType::BLEZ ||
             operation_type == OperationType::BGTZ || operation_type == OperationType::BLTZ)
    {
        if (tokens.size() < start_index + 3)
            throw std::runtime_error("Insufficient arguments for: " + operation_mnemonic);
        new_instruction.set_format(InstructionFormat::I_TYPE);
        new_instruction.set_register_source(register_reference.get_register_index(tokens[start_index + 1]));
        std::string branch_label = tokens[start_index + 2];
        if (!symbol_table.contains_label(branch_label))
            throw std::runtime_error("Undefined label in branch instruction: " + branch_label);
        uint32_t target_address = symbol_table.get_address(branch_label);
        new_instruction.set_immediate_value(static_cast<int32_t>(target_address) - static_cast<int32_t>(current_address + 4));
    }

    else if (operation_type == OperationType::B || operation_type == OperationType::BAL)
    {
        if (tokens.size() < start_index + 2)
            throw std::runtime_error("Insufficient arguments for: " + operation_mnemonic);
        new_instruction.set_format(InstructionFormat::I_TYPE);
        std::string branch_label = tokens[start_index + 1];
        if (!symbol_table.contains_label(branch_label))
            throw std::runtime_error("Undefined label in branch instruction: " + branch_label);
        uint32_t target_address = symbol_table.get_address(branch_label);
        new_instruction.set_immediate_value(static_cast<int32_t>(target_address) - static_cast<int32_t>(current_address + 4));
    }

    else if (operation_type == OperationType::J || operation_type == OperationType::JAL)
    {
        if (tokens.size() < start_index + 2)
            throw std::runtime_error("Insufficient arguments for: " + operation_mnemonic);
        new_instruction.set_format(InstructionFormat::J_TYPE);
        std::string jump_label = tokens[start_index + 1];
        if (!symbol_table.contains_label(jump_label))
            throw std::runtime_error("Undefined label in jump instruction: " + jump_label);
        new_instruction.set_jump_address(symbol_table.get_address(jump_label));
    }

    else if (operation_type == OperationType::JR)
    {
        if (tokens.size() < start_index + 2)
            throw std::runtime_error("Insufficient arguments for: " + operation_mnemonic);
        new_instruction.set_format(InstructionFormat::R_TYPE);
        new_instruction.set_register_source(register_reference.get_register_index(tokens[start_index + 1]));
    }

    else if (operation_type == OperationType::JALR)
    {
        if (tokens.size() < start_index + 2)
            throw std::runtime_error("Insufficient arguments for: " + operation_mnemonic);
        new_instruction.set_format(InstructionFormat::R_TYPE);
        if (tokens.size() == start_index + 3)
        {
            new_instruction.set_register_destination(register_reference.get_register_index(tokens[start_index + 1]));
            new_instruction.set_register_source(register_reference.get_register_index(tokens[start_index + 2]));
        }
        else
        {
            new_instruction.set_register_destination(31);
            new_instruction.set_register_source(register_reference.get_register_index(tokens[start_index + 1]));
        }
    }

    else if (operation_type == OperationType::LSA)
    {
        if (tokens.size() < start_index + 5)
            throw std::runtime_error("Insufficient arguments for: " + operation_mnemonic);
        new_instruction.set_format(InstructionFormat::R_TYPE);
        new_instruction.set_register_destination(register_reference.get_register_index(tokens[start_index + 1]));
        new_instruction.set_register_source(register_reference.get_register_index(tokens[start_index + 2]));
        new_instruction.set_register_target(register_reference.get_register_index(tokens[start_index + 3]));
        new_instruction.set_shift_amount(std::stoul(tokens[start_index + 4]));
    }

    else if (operation_type == OperationType::EXT || operation_type == OperationType::INS)
    {
        if (tokens.size() < start_index + 5)
            throw std::runtime_error("Insufficient arguments for: " + operation_mnemonic);
        new_instruction.set_format(InstructionFormat::R_TYPE);
        new_instruction.set_register_target(register_reference.get_register_index(tokens[start_index + 1]));
        new_instruction.set_register_source(register_reference.get_register_index(tokens[start_index + 2]));
        new_instruction.set_shift_amount(std::stoul(tokens[start_index + 3]));
        new_instruction.set_immediate_value(std::stoi(tokens[start_index + 4]));
    }

    else if (operation_type == OperationType::TEQ || operation_type == OperationType::TNE)
    {
        if (tokens.size() < start_index + 3)
            throw std::runtime_error("Insufficient arguments for: " + operation_mnemonic);
        new_instruction.set_format(InstructionFormat::R_TYPE);
        new_instruction.set_register_source(register_reference.get_register_index(tokens[start_index + 1]));
        new_instruction.set_register_target(register_reference.get_register_index(tokens[start_index + 2]));
    }

    else if (operation_type == OperationType::NOP || operation_type == OperationType::BREAK || operation_type == OperationType::SYSCALL)
        new_instruction.set_format(InstructionFormat::R_TYPE);

    return new_instruction;
}

std::map<uint32_t, Instruction> Assembler::assemble_program(const std::string &file_path, Memory &system_memory)
{
    std::vector<std::vector<std::string>> parsed_lines = lexer.read_file(file_path);
    perform_first_pass(parsed_lines);
    return perform_second_pass(parsed_lines, system_memory);
}