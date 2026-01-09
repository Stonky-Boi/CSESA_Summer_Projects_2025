#ifndef MIPS_SIMULATOR_HPP
#define MIPS_SIMULATOR_HPP

#include "register_file.hpp"
#include "memory.hpp"
#include "instruction.hpp"
#include <vector>
#include <string>
#include <map>
#include <functional>

class MipsSimulator {
public:
    MipsSimulator();
    void load_program_from_string(const std::string& code);
    void step();
    void run();
    
    // Getters for state
    const RegisterFile& get_registers() const { return regs; }
    const Memory& get_memory() const { return mem; }
    const std::vector<Instruction>& get_instructions() const { return text_segment; }
    std::string get_console_output() const { return console_output; }
    bool is_finished() const;

private:
    void parse_code(const std::string& code);
    void execute_instruction(const Instruction& instr);

    // Instruction handlers
    void execute_r_type(const Instruction& instr);
    void execute_i_type(const Instruction& instr);
    void execute_j_type(const Instruction& instr);
    void execute_syscall();

    RegisterFile regs;
    Memory mem;
    std::vector<Instruction> text_segment;
    std::map<std::string, uint32_t> label_map;
    std::string console_output;

    std::map<std::string, std::function<void(const Instruction&)>> instruction_map;
};

#endif // MIPS_SIMULATOR_HPP