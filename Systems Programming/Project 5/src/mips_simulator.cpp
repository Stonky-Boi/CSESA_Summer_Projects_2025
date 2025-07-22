#include "mips_simulator.hpp"
#include <stdexcept>
#include <cctype>
#include <limits>
#include <sstream>
#include <iostream>
#include <cstring>

static int32_t HI = 0;
static int32_t LO = 0;

MipsSimulator::MipsSimulator() : mem(4096) {
    // Arithmetic R-type
    instruction_map["add"] = [this](const Instruction& i) {
        int rs = RegisterFile::get_reg_index(i.args[1]);
        int rt = RegisterFile::get_reg_index(i.args[2]);
        int rd = RegisterFile::get_reg_index(i.args[0]);
        int64_t sum = (int64_t)regs.read(rs) + (int64_t)regs.read(rt);
        if (sum > std::numeric_limits<int32_t>::max() || sum < std::numeric_limits<int32_t>::min())
            throw std::runtime_error("Arithmetic overflow in add");
        regs.write(rd, (int32_t)sum);
    };
    instruction_map["addu"] = [this](const Instruction& i) {
        int rs = RegisterFile::get_reg_index(i.args[1]);
        int rt = RegisterFile::get_reg_index(i.args[2]);
        int rd = RegisterFile::get_reg_index(i.args[0]);
        uint32_t sum = (uint32_t)regs.read(rs) + (uint32_t)regs.read(rt);
        regs.write(rd, (int32_t)sum);
    };
    instruction_map["sub"] = [this](const Instruction& i) {
        int rs = RegisterFile::get_reg_index(i.args[1]);
        int rt = RegisterFile::get_reg_index(i.args[2]);
        int rd = RegisterFile::get_reg_index(i.args[0]);
        int64_t diff = (int64_t)regs.read(rs) - (int64_t)regs.read(rt);
        if (diff > std::numeric_limits<int32_t>::max() || diff < std::numeric_limits<int32_t>::min())
            throw std::runtime_error("Arithmetic overflow in sub");
        regs.write(rd, (int32_t)diff);
    };
    instruction_map["subu"] = [this](const Instruction& i) {
        int rs = RegisterFile::get_reg_index(i.args[1]);
        int rt = RegisterFile::get_reg_index(i.args[2]);
        int rd = RegisterFile::get_reg_index(i.args[0]);
        uint32_t diff = (uint32_t)regs.read(rs) - (uint32_t)regs.read(rt);
        regs.write(rd, (int32_t)diff);
    };

    // Logical R-type instructions
    instruction_map["and"] = [this](const Instruction& i) {
        regs.write(RegisterFile::get_reg_index(i.args[0]),
                   regs.read(RegisterFile::get_reg_index(i.args[1])) &
                   regs.read(RegisterFile::get_reg_index(i.args[2])));
    };
    instruction_map["or"] = [this](const Instruction& i) {
        regs.write(RegisterFile::get_reg_index(i.args[0]),
                   regs.read(RegisterFile::get_reg_index(i.args[1])) |
                   regs.read(RegisterFile::get_reg_index(i.args[2])));
    };
    instruction_map["xor"] = [this](const Instruction& i) {
        regs.write(RegisterFile::get_reg_index(i.args[0]),
                   regs.read(RegisterFile::get_reg_index(i.args[1])) ^
                   regs.read(RegisterFile::get_reg_index(i.args[2])));
    };
    instruction_map["nor"] = [this](const Instruction& i) {
        regs.write(RegisterFile::get_reg_index(i.args[0]),
                   ~(regs.read(RegisterFile::get_reg_index(i.args[1])) |
                     regs.read(RegisterFile::get_reg_index(i.args[2]))));
    };

    // Set on less than instructions
    instruction_map["slt"] = [this](const Instruction& i) {
        regs.write(RegisterFile::get_reg_index(i.args[0]),
                   regs.read(RegisterFile::get_reg_index(i.args[1])) <
                   regs.read(RegisterFile::get_reg_index(i.args[2])) ? 1 : 0);
    };
    instruction_map["sltu"] = [this](const Instruction& i) {
        regs.write(RegisterFile::get_reg_index(i.args[0]),
          (uint32_t)regs.read(RegisterFile::get_reg_index(i.args[1])) <
          (uint32_t)regs.read(RegisterFile::get_reg_index(i.args[2])) ? 1 : 0);
    };

    // Shift instructions
    instruction_map["sll"] = [this](const Instruction& i) {
        int shamt = std::stoi(i.args[2]);
        regs.write(RegisterFile::get_reg_index(i.args[0]),
                   (uint32_t)regs.read(RegisterFile::get_reg_index(i.args[1])) << shamt);
    };
    instruction_map["srl"] = [this](const Instruction& i) {
        int shamt = std::stoi(i.args[2]);
        regs.write(RegisterFile::get_reg_index(i.args[0]),
                   (uint32_t)regs.read(RegisterFile::get_reg_index(i.args[1])) >> shamt);
    };
    instruction_map["sra"] = [this](const Instruction& i) {
        int shamt = std::stoi(i.args[2]);
        regs.write(RegisterFile::get_reg_index(i.args[0]),
                   regs.read(RegisterFile::get_reg_index(i.args[1])) >> shamt);
    };
    instruction_map["sllv"] = [this](const Instruction& i) {
        uint32_t shamt = regs.read(RegisterFile::get_reg_index(i.args[1])) & 0x1F;
        regs.write(RegisterFile::get_reg_index(i.args[0]),
                   (uint32_t)regs.read(RegisterFile::get_reg_index(i.args[2])) << shamt);
    };
    instruction_map["srlv"] = [this](const Instruction& i) {
        uint32_t shamt = regs.read(RegisterFile::get_reg_index(i.args[1])) & 0x1F;
        regs.write(RegisterFile::get_reg_index(i.args[0]),
                   (uint32_t)regs.read(RegisterFile::get_reg_index(i.args[2])) >> shamt);
    };
    instruction_map["srav"] = [this](const Instruction& i) {
        uint32_t shamt = regs.read(RegisterFile::get_reg_index(i.args[1])) & 0x1F;
        regs.write(RegisterFile::get_reg_index(i.args[0]),
                   regs.read(RegisterFile::get_reg_index(i.args[2])) >> shamt);
    };

    // Move pseudo instructions
    instruction_map["move"] = [this](const Instruction& i) {
        regs.write(RegisterFile::get_reg_index(i.args[0]),
                   regs.read(RegisterFile::get_reg_index(i.args[1])));
    };
    instruction_map["not"] = [this](const Instruction& i) {
        regs.write(RegisterFile::get_reg_index(i.args[0]),
                   ~regs.read(RegisterFile::get_reg_index(i.args[1])));
    };
    instruction_map["neg"] = [this](const Instruction& i) {
        regs.write(RegisterFile::get_reg_index(i.args[0]),
                   -regs.read(RegisterFile::get_reg_index(i.args[1])));
    };
    instruction_map["abs"] = [this](const Instruction& i) {
        int32_t val = regs.read(RegisterFile::get_reg_index(i.args[1]));
        regs.write(RegisterFile::get_reg_index(i.args[0]), val < 0 ? -val : val);
    };

    // Multiply and divide
    instruction_map["mult"] = [this](const Instruction& i) {
        int32_t rsval = regs.read(RegisterFile::get_reg_index(i.args[0]));
        int32_t rtval = regs.read(RegisterFile::get_reg_index(i.args[1]));
        int64_t mult_result = (int64_t)rsval * (int64_t)rtval;
        HI = (int32_t)(mult_result >> 32);
        LO = (int32_t)(mult_result & 0xFFFFFFFF);
    };
    instruction_map["multu"] = [this](const Instruction& i) {
        uint32_t rsval = (uint32_t)regs.read(RegisterFile::get_reg_index(i.args[0]));
        uint32_t rtval = (uint32_t)regs.read(RegisterFile::get_reg_index(i.args[1]));
        uint64_t mult_result = (uint64_t)rsval * (uint64_t)rtval;
        HI = (int32_t)(mult_result >> 32);
        LO = (int32_t)(mult_result & 0xFFFFFFFF);
    };
    instruction_map["div"] = [this](const Instruction& i) {
        int32_t dividend = regs.read(RegisterFile::get_reg_index(i.args[0]));
        int32_t divisor = regs.read(RegisterFile::get_reg_index(i.args[1]));
        if(divisor == 0) throw std::runtime_error("Division by zero");
        LO = dividend / divisor;
        HI = dividend % divisor;
    };
    instruction_map["divu"] = [this](const Instruction& i) {
        uint32_t dividend = (uint32_t)regs.read(RegisterFile::get_reg_index(i.args[0]));
        uint32_t divisor = (uint32_t)regs.read(RegisterFile::get_reg_index(i.args[1]));
        if(divisor == 0) throw std::runtime_error("Division by zero");
        LO = dividend / divisor;
        HI = dividend % divisor;
    };
    instruction_map["mfhi"] = [this](const Instruction& i) {
        regs.write(RegisterFile::get_reg_index(i.args[0]), HI);
    };
    instruction_map["mflo"] = [this](const Instruction& i) {
        regs.write(RegisterFile::get_reg_index(i.args[0]), LO);
    };
    instruction_map["mthi"] = [this](const Instruction& i) {
        HI = regs.read(RegisterFile::get_reg_index(i.args[0]));
    };
    instruction_map["mtlo"] = [this](const Instruction& i) {
        LO = regs.read(RegisterFile::get_reg_index(i.args[0]));
    };

    // Immediate arithmetic/logical
    instruction_map["addi"] = [this](const Instruction& i) {
        int32_t imm = std::stoi(i.args[2]);
        int rs = RegisterFile::get_reg_index(i.args[1]);
        int rt = RegisterFile::get_reg_index(i.args[0]);
        int64_t val = (int64_t)regs.read(rs) + (int64_t)imm;
        if (val > std::numeric_limits<int32_t>::max() || val < std::numeric_limits<int32_t>::min())
            throw std::runtime_error("Arithmetic overflow in addi");
        regs.write(rt, (int32_t)val);
    };
    instruction_map["addiu"] = [this](const Instruction& i) {
        int32_t imm = std::stoi(i.args[2]);
        int rs = RegisterFile::get_reg_index(i.args[1]);
        int rt = RegisterFile::get_reg_index(i.args[0]);
        regs.write(rt, regs.read(rs) + imm);
    };
    instruction_map["andi"] = [this](const Instruction& i) {
        uint32_t imm = (uint16_t)std::stoi(i.args[2]);
        int rs = RegisterFile::get_reg_index(i.args[1]);
        int rt = RegisterFile::get_reg_index(i.args[0]);
        regs.write(rt, regs.read(rs) & imm);
    };
    instruction_map["ori"] = [this](const Instruction& i) {
        uint32_t imm = (uint16_t)std::stoi(i.args[2]);
        int rs = RegisterFile::get_reg_index(i.args[1]);
        int rt = RegisterFile::get_reg_index(i.args[0]);
        regs.write(rt, regs.read(rs) | imm);
    };
    instruction_map["xori"] = [this](const Instruction& i) {
        uint32_t imm = (uint16_t)std::stoi(i.args[2]);
        int rs = RegisterFile::get_reg_index(i.args[1]);
        int rt = RegisterFile::get_reg_index(i.args[0]);
        regs.write(rt, regs.read(rs) ^ imm);
    };
    instruction_map["lui"] = [this](const Instruction& i) {
        uint32_t imm = (uint16_t)std::stoi(i.args[1]);
        int rt = RegisterFile::get_reg_index(i.args[0]);
        regs.write(rt, imm << 16);
    };
    instruction_map["slti"] = [this](const Instruction& i) {
        int32_t imm = std::stoi(i.args[2]);
        int rs = RegisterFile::get_reg_index(i.args[1]);
        int rt = RegisterFile::get_reg_index(i.args[0]);
        regs.write(rt, regs.read(rs) < imm ? 1 : 0);
    };
    instruction_map["sltiu"] = [this](const Instruction& i) {
        uint32_t imm = (uint16_t)std::stoi(i.args[2]);
        int rs = RegisterFile::get_reg_index(i.args[1]);
        int rt = RegisterFile::get_reg_index(i.args[0]);
        regs.write(rt, (uint32_t)regs.read(rs) < imm ? 1 : 0);
    };

    // Extended memory argument parser supporting label or offset(reg)
    auto parse_mem_arg = [this](const std::string &arg, int16_t &offset, int &base, bool &is_label, uint32_t &label_addr) {
        size_t open = arg.find('(');
        if(open == std::string::npos) {
            // label form
            is_label = true;
            auto it = label_map.find(arg);
            if(it == label_map.end()) throw std::runtime_error("Label not found: " + arg);
            label_addr = it->second;
            offset = 0;
            base = 0;
        }
        else {
            is_label = false;
            size_t close = arg.find(')');
            if(close == std::string::npos)
                throw std::runtime_error("Malformed memory argument: " + arg);
            offset = static_cast<int16_t>(std::stoi(arg.substr(0, open)));
            std::string reg_name = arg.substr(open+1, close - open - 1);
            base = RegisterFile::get_reg_index(reg_name);
            label_addr = 0;
        }
    };

    // Memory instructions with label or offset(reg) support:
    instruction_map["lb"] = [this, parse_mem_arg](const Instruction& i) {
        int rt = RegisterFile::get_reg_index(i.args[0]);
        int16_t offset; int base; bool is_label; uint32_t label_addr;
        parse_mem_arg(i.args[1], offset, base, is_label, label_addr);
        uint32_t addr = is_label ? label_addr : (regs.read(base) + offset);
        int8_t val = mem.read_byte(addr);
        regs.write(rt, val);
    };
    instruction_map["lbu"] = [this, parse_mem_arg](const Instruction& i) {
        int rt = RegisterFile::get_reg_index(i.args[0]);
        int16_t offset; int base; bool is_label; uint32_t label_addr;
        parse_mem_arg(i.args[1], offset, base, is_label, label_addr);
        uint32_t addr = is_label ? label_addr : (regs.read(base) + offset);
        uint8_t val = (uint8_t)mem.read_byte(addr);
        regs.write(rt, val);
    };
    instruction_map["lh"] = [this, parse_mem_arg](const Instruction& i) {
        int rt = RegisterFile::get_reg_index(i.args[0]);
        int16_t offset; int base; bool is_label; uint32_t label_addr;
        parse_mem_arg(i.args[1], offset, base, is_label, label_addr);
        uint32_t addr = is_label ? label_addr : (regs.read(base) + offset);
        int16_t val = (int16_t)((mem.read_byte(addr + 1) << 8) | (uint8_t)mem.read_byte(addr));
        regs.write(rt, val);
    };
    instruction_map["lhu"] = [this, parse_mem_arg](const Instruction& i) {
        int rt = RegisterFile::get_reg_index(i.args[0]);
        int16_t offset; int base; bool is_label; uint32_t label_addr;
        parse_mem_arg(i.args[1], offset, base, is_label, label_addr);
        uint32_t addr = is_label ? label_addr : (regs.read(base) + offset);
        uint16_t val = ((uint16_t)mem.read_byte(addr + 1) << 8) | (uint8_t)mem.read_byte(addr);
        regs.write(rt, val);
    };
    instruction_map["lw"] = [this, parse_mem_arg](const Instruction& i) {
        int rt = RegisterFile::get_reg_index(i.args[0]);
        int16_t offset; int base; bool is_label; uint32_t label_addr;
        parse_mem_arg(i.args[1], offset, base, is_label, label_addr);
        uint32_t addr = is_label ? label_addr : (regs.read(base) + offset);
        int32_t val = mem.read_word(addr);
        regs.write(rt, val);
    };
    instruction_map["sb"] = [this, parse_mem_arg](const Instruction& i) {
        int rt = RegisterFile::get_reg_index(i.args[0]);
        int16_t offset; int base; bool is_label; uint32_t label_addr;
        parse_mem_arg(i.args[1], offset, base, is_label, label_addr);
        uint32_t addr = is_label ? label_addr : (regs.read(base) + offset);
        mem.write_byte(addr, regs.read(rt) & 0xFF);
    };
    instruction_map["sh"] = [this, parse_mem_arg](const Instruction& i) {
        int rt = RegisterFile::get_reg_index(i.args[0]);
        int16_t offset; int base; bool is_label; uint32_t label_addr;
        parse_mem_arg(i.args[1], offset, base, is_label, label_addr);
        uint32_t addr = is_label ? label_addr : (regs.read(base) + offset);
        int16_t val = regs.read(rt) & 0xFFFF;
        mem.write_byte(addr, val & 0xFF);
        mem.write_byte(addr + 1, (val >> 8) & 0xFF);
    };
    instruction_map["sw"] = [this, parse_mem_arg](const Instruction& i) {
        int rt = RegisterFile::get_reg_index(i.args[0]);
        int16_t offset; int base; bool is_label; uint32_t label_addr;
        parse_mem_arg(i.args[1], offset, base, is_label, label_addr);
        uint32_t addr = is_label ? label_addr : (regs.read(base) + offset);
        mem.write_word(addr, regs.read(rt));
    };

    // Branches
    auto branch_if = [this](bool cond, const Instruction& i) {
        if(cond) regs.set_pc(label_map.at(i.args.back()));
        else regs.increment_pc();
    };
    instruction_map["beq"] = [this, branch_if](const Instruction& i) {
        int rs = RegisterFile::get_reg_index(i.args[0]);
        int rt = RegisterFile::get_reg_index(i.args[1]);
        branch_if(regs.read(rs) == regs.read(rt), i);
    };
    instruction_map["bne"] = [this, branch_if](const Instruction& i) {
        int rs = RegisterFile::get_reg_index(i.args[0]);
        int rt = RegisterFile::get_reg_index(i.args[1]);
        branch_if(regs.read(rs) != regs.read(rt), i);
    };
    instruction_map["blez"] = [this, branch_if](const Instruction& i) {
        int rs = RegisterFile::get_reg_index(i.args[0]);
        branch_if(regs.read(rs) <= 0, i);
    };
    instruction_map["bgtz"] = [this, branch_if](const Instruction& i) {
        int rs = RegisterFile::get_reg_index(i.args[0]);
        branch_if(regs.read(rs) > 0, i);
    };
    instruction_map["bltz"] = [this, branch_if](const Instruction& i) {
        int rs = RegisterFile::get_reg_index(i.args[0]);
        branch_if(regs.read(rs) < 0, i);
    };
    instruction_map["bgez"] = [this, branch_if](const Instruction& i) {
        int rs = RegisterFile::get_reg_index(i.args[0]);
        branch_if(regs.read(rs) >= 0, i);
    };

    // Pseudo branches (added to fix your example 4 ble)
    instruction_map["ble"] = [this](const Instruction& i) {
        int rs = RegisterFile::get_reg_index(i.args[0]);
        int rt = RegisterFile::get_reg_index(i.args[1]);
        if (regs.read(rs) <= regs.read(rt))
            regs.set_pc(label_map.at(i.args[2]));
        else
            regs.increment_pc();
    };
    instruction_map["bge"] = [this](const Instruction& i) {
        int rs = RegisterFile::get_reg_index(i.args[0]);
        int rt = RegisterFile::get_reg_index(i.args[1]);
        if (regs.read(rs) >= regs.read(rt))
            regs.set_pc(label_map.at(i.args[2]));
        else
            regs.increment_pc();
    };

    // Jump instructions
    instruction_map["j"] = [this](const Instruction& i) {
        regs.set_pc(label_map.at(i.args[0]));
    };
    instruction_map["jal"] = [this](const Instruction& i) {
        regs.write(31, regs.get_pc() + 4);
        regs.set_pc(label_map.at(i.args[0]));
    };
    instruction_map["jr"] = [this](const Instruction& i) {
        regs.set_pc(regs.read(RegisterFile::get_reg_index(i.args[0])));
    };
    instruction_map["jalr"] = [this](const Instruction& i) {
        int rs = RegisterFile::get_reg_index(i.args[0]);
        int rd = 31;
        if(i.args.size() > 1) rd = RegisterFile::get_reg_index(i.args[1]);
        regs.write(rd, regs.get_pc() + 4);
        regs.set_pc(regs.read(rs));
    };

    // Pseudo instructions
    instruction_map["li"] = [this](const Instruction& i) {
        int rt = RegisterFile::get_reg_index(i.args[0]);
        int32_t imm = std::stoi(i.args[1]);
        regs.write(rt, imm);
    };
    instruction_map["la"] = [this](const Instruction& i) {
        int rt = RegisterFile::get_reg_index(i.args[0]);
        regs.write(rt, label_map.at(i.args[1]));
    };

    // Syscall handler
    instruction_map["syscall"] = [this](const Instruction&) {
        int32_t v0 = regs.read(2);
        switch(v0) {
            case 1: // print_int
                console_output += std::to_string(regs.read(4));
                break;
            case 4: // print_string
                console_output += mem.load_string(regs.read(4));
                break;
            case 5: { // read_int
                int32_t n;
                std::cout << "Input integer: ";
                std::cin >> n;
                regs.write(2, n);
                break;
            }
            case 8: { // read_string
                int32_t buf = regs.read(4);
                int32_t len = regs.read(5);
                std::cin.ignore();
                std::string input;
                std::getline(std::cin, input);
                if((int)input.size() >= len) input.resize(len - 1);
                for(size_t i=0; i < input.size(); ++i)
                    mem.write_byte(buf + (uint32_t)i, input[i]);
                mem.write_byte(buf + (uint32_t)input.size(), '\0');
                break;
            }
            case 10: // exit
                regs.set_pc(0x00400000 + 4 * (uint32_t)text_segment.size());
                break;
            default:
                throw std::runtime_error("Unhandled syscall code: "+std::to_string(v0));
        }
    };
}

void MipsSimulator::load_program_from_string(const std::string& code) {
    text_segment.clear();
    label_map.clear();
    console_output.clear();
    regs = RegisterFile();
    mem = Memory(4096);
    HI = 0; LO = 0;
    parse_code(code);
    regs.set_pc(0x00400000);
}

bool MipsSimulator::is_finished() const {
    uint32_t pc = regs.get_pc();
    return pc >= 0x00400000 + 4 * (uint32_t)text_segment.size();
}

void MipsSimulator::step() {
    if(is_finished()) return;
    uint32_t pc = regs.get_pc();
    uint32_t index = (pc - 0x00400000) / 4;
    if(index >= text_segment.size()) {
        regs.set_pc(0x00400000 + 4 * (uint32_t)text_segment.size());
        return;
    }
    const Instruction& instr = text_segment[index];
    uint32_t old_pc = regs.get_pc();

    try {
        execute_instruction(instr);
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Error at ") + instr.original_line + ": " + e.what());
    }
    if(regs.get_pc() == old_pc) regs.increment_pc();
}

void MipsSimulator::run() {
    while(!is_finished()) step();
}

void MipsSimulator::parse_code(const std::string& code) {
    std::stringstream ss(code);
    std::string line;
    bool in_data = false, in_text = false;
    uint32_t current_text_addr = 0x00400000;
    uint32_t current_data_addr = 0x10010000;

    while(std::getline(ss,line)) {
        auto comment_pos = line.find('#');
        if(comment_pos != std::string::npos) line = line.substr(0,comment_pos);
        line.erase(0,line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n")+1);
        if(line.empty()) continue;

        if(line==".data") { in_data = true; in_text = false; continue; }
        if(line==".text") { in_data = false; in_text = true; continue; }

        if(in_data) {
            size_t colon_pos = line.find(':');
            std::string label, rest;
            if(colon_pos!=std::string::npos) {
                label = line.substr(0,colon_pos);
                rest = line.substr(colon_pos+1);
                rest.erase(0,rest.find_first_not_of(" \t"));
            } else rest = line;

            std::stringstream rest_ss(rest);
            std::string directive;
            rest_ss >> directive;
            if(directive==".asciiz" || directive==".ascii") {
                std::string str;
                std::getline(rest_ss,str);
                size_t quote1 = str.find('"');
                size_t quote2 = str.rfind('"');
                if(quote1==std::string::npos||quote2==std::string::npos||quote1==quote2) throw std::runtime_error("Malformed string literal");
                std::string literal = str.substr(quote1+1,quote2-quote1-1);
                std::string processed;
                for(size_t i=0;i<literal.size();i++) {
                    if(literal[i]=='\\'&&i+1<literal.size()) {
                        char n = literal[++i];
                        if(n=='n') processed.push_back('\n');
                        else if(n=='t') processed.push_back('\t');
                        else if(n=='r') processed.push_back('\r');
                        else processed.push_back(n);
                    } else processed.push_back(literal[i]);
                }
                uint32_t addr = mem.store_string(processed);
                if(!label.empty()) label_map[label]=addr;
            }
            else if(directive==".word") {
                while(!rest_ss.eof()) {
                    int32_t val; rest_ss>>val;
                    if(rest_ss.fail()) break;
                    mem.write_word(current_data_addr,val);
                    if(!label.empty()) { label_map[label]=current_data_addr; label.clear(); }
                    current_data_addr+=4;
                }
            }
            else if(directive==".byte") {
                while(!rest_ss.eof()) {
                    int val; rest_ss>>val;
                    if(rest_ss.fail()) break;
                    mem.write_byte(current_data_addr,(int8_t)val);
                    if(!label.empty()) { label_map[label]=current_data_addr; label.clear(); }
                    current_data_addr++;
                }
            }
            else if(directive==".half") {
                while(!rest_ss.eof()) {
                    int val; rest_ss>>val;
                    if(rest_ss.fail()) break;
                    mem.write_byte(current_data_addr,val&0xFF);
                    mem.write_byte(current_data_addr+1,(val>>8)&0xFF);
                    if(!label.empty()) { label_map[label]=current_data_addr; label.clear(); }
                    current_data_addr+=2;
                }
            }
            else if(directive==".space") {
                int spaceCount; rest_ss >> spaceCount;
                if(!label.empty()) { label_map[label]=current_data_addr; label.clear(); }
                current_data_addr += spaceCount;
            }
            else if(directive==".align") {
                int alignVal; rest_ss >> alignVal;
                uint32_t align_bytes=1<<alignVal;
                if((current_data_addr%align_bytes)!=0)
                    current_data_addr += (align_bytes-(current_data_addr%align_bytes));
            }
            else {
                throw std::runtime_error("Unknown or unsupported directive: " + directive);
            }
        }
        else if(in_text) {
            size_t colon_pos = line.find(':');
            if(colon_pos!=std::string::npos) {
                std::string lbl = line.substr(0,colon_pos);
                label_map[lbl] = current_text_addr;
                line=line.substr(colon_pos+1);
                line.erase(0,line.find_first_not_of(" \t"));
                if(line.empty()) continue;
            }
            Instruction instr(line,current_text_addr);
            if(!instr.operation.empty()) {
                text_segment.push_back(instr);
                current_text_addr+=4;
            }
        }
    }
}

void MipsSimulator::execute_instruction(const Instruction& instr) {
    if(instr.operation.empty()) return;

    auto it = instruction_map.find(instr.operation);
    if(it == instruction_map.end()) {
        throw std::runtime_error("Unknown instruction: "+instr.operation);
    }
    it->second(instr);
}