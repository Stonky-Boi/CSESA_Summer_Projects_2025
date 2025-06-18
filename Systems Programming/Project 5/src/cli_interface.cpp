#include "mips_simulator.hpp"
#include "instruction_decoder.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>

class CLIInterface {
private:
    MIPSSimulator simulator;
    bool running;
    
public:
    CLIInterface() : running(true) {}
    
    void run() {
        printWelcome();
        
        while (running) {
            printPrompt();
            std::string command;
            std::getline(std::cin, command);
            
            if (command.empty()) continue;
            
            processCommand(command);
        }
    }
    
private:
    void printWelcome() {
        std::cout << "╔══════════════════════════════════════╗\n";
        std::cout << "║          MIPS Simulator CLI          ║\n";
        std::cout << "║       Interactive Interface          ║\n";
        std::cout << "╚══════════════════════════════════════╝\n\n";
        std::cout << "Type 'help' for available commands.\n\n";
    }
    
    void printPrompt() {
        std::cout << "MIPS> ";
    }
    
    void processCommand(const std::string& command) {
        std::istringstream iss(command);
        std::string cmd;
        iss >> cmd;
        
        if (cmd == "help" || cmd == "h") {
            printHelp();
        } else if (cmd == "load" || cmd == "l") {
            std::string filename;
            iss >> filename;
            loadProgram(filename);
        } else if (cmd == "loadhex" || cmd == "lh") {
            loadHexProgram();
        } else if (cmd == "step" || cmd == "s") {
            step();
        } else if (cmd == "run" || cmd == "r") {
            run_simulation();
        } else if (cmd == "reset") {
            reset();
        } else if (cmd == "state" || cmd == "st") {
            printState();
        } else if (cmd == "registers" || cmd == "reg") {
            printRegisters();
        } else if (cmd == "memory" || cmd == "mem") {
            std::string addr_str;
            iss >> addr_str;
            printMemory(addr_str);
        } else if (cmd == "setreg" || cmd == "sr") {
            std::string reg_str, val_str;
            iss >> reg_str >> val_str;
            setRegister(reg_str, val_str);
        } else if (cmd == "setmem" || cmd == "sm") {
            std::string addr_str, val_str;
            iss >> addr_str >> val_str;
            setMemory(addr_str, val_str);
        } else if (cmd == "pipeline" || cmd == "pipe") {
            std::string mode;
            iss >> mode;
            togglePipeline(mode);
        } else if (cmd == "branch" || cmd == "bp") {
            std::string mode, type;
            iss >> mode >> type;
            configureBranchPrediction(mode, type);
        } else if (cmd == "stats") {
            printStats();
        } else if (cmd == "disasm" || cmd == "d") {
            std::string addr_str;
            iss >> addr_str;
            disassemble(addr_str);
        } else if (cmd == "quit" || cmd == "q" || cmd == "exit") {
            running = false;
            std::cout << "Goodbye!\n";
        } else {
            std::cout << "Unknown command: " << cmd << ". Type 'help' for available commands.\n";
        }
    }
    
    void printHelp() {
        std::cout << "\nAvailable Commands:\n";
        std::cout << "==================\n";
        std::cout << "Program Control:\n";
        std::cout << "  load <file>     - Load program from file\n";
        std::cout << "  loadhex         - Load program from hex input\n";
        std::cout << "  step (s)        - Execute one instruction\n";
        std::cout << "  run (r)         - Run until completion\n";
        std::cout << "  reset           - Reset simulator state\n";
        std::cout << "\nState Inspection:\n";
        std::cout << "  state (st)      - Show complete system state\n";
        std::cout << "  registers (reg) - Show register values\n";
        std::cout << "  memory <addr>   - Show memory contents at address\n";
        std::cout << "  disasm <addr>   - Disassemble instruction at address\n";
        std::cout << "\nState Modification:\n";
        std::cout << "  setreg <reg> <val> - Set register value\n";
        std::cout << "  setmem <addr> <val> - Set memory value\n";
        std::cout << "\nAdvanced Features:\n";
        std::cout << "  pipeline <on/off>   - Enable/disable pipeline\n";
        std::cout << "  branch <on/off> [type] - Configure branch prediction\n";
        std::cout << "  stats              - Show performance statistics\n";
        std::cout << "\nGeneral:\n";
        std::cout << "  help (h)        - Show this help\n";
        std::cout << "  quit (q)        - Exit simulator\n\n";
    }
    
    void loadProgram(const std::string& filename) {
        if (filename.empty()) {
            std::cout << "Error: No filename specified.\n";
            return;
        }
        
        if (simulator.loadProgram(filename)) {
            std::cout << "Program loaded successfully from: " << filename << "\n";
        } else {
            std::cout << "Error: Could not load program from: " << filename << "\n";
        }
    }
    
    void loadHexProgram() {
        std::cout << "Enter MIPS machine code in hex format (one instruction per line, empty line to finish):\n";
        
        std::string program;
        std::string line;
        
        while (std::getline(std::cin, line)) {
            if (line.empty()) break;
            program += line + "\n";
        }
        
        if (simulator.loadProgramFromString(program)) {
            std::cout << "Program loaded successfully.\n";
        } else {
            std::cout << "Error: Invalid program format.\n";
        }
    }
    
    void step() {
        if (simulator.step()) {
            std::cout << "Instruction executed. PC = 0x" << std::hex << std::setw(8) 
                     << std::setfill('0') << simulator.getPC() << std::dec << "\n";
        } else {
            if (simulator.isHalted()) {
                std::cout << "Simulation halted.\n";
            } else {
                std::cout << "Error executing instruction.\n";
            }
        }
    }
    
    void run_simulation() {
        std::cout << "Running simulation...\n";
        
        int instructions = 0;
        while (!simulator.isHalted() && simulator.step()) {
            instructions++;
            if (instructions > 10000) {
                std::cout << "Warning: Simulation stopped after 10000 instructions to prevent infinite loop.\n";
                break;
            }
        }
        
        std::cout << "Simulation completed. Executed " << instructions << " instructions.\n";
        std::cout << "Final PC = 0x" << std::hex << std::setw(8) << std::setfill('0') 
                 << simulator.getPC() << std::dec << "\n";
    }
    
    void reset() {
        simulator.reset();
        std::cout << "Simulator reset to initial state.\n";
    }
    
    void printState() {
        std::cout << "\n" << simulator.getStateString() << "\n";
    }
    
    void printRegisters() {
        std::cout << "\nRegister Values:\n";
        std::cout << "================\n";
        
        const std::string reg_names[] = {
            "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3",
            "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
            "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
            "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"
        };
        
        for (int i = 0; i < 32; i += 4) {
            for (int j = 0; j < 4; j++) {
                int reg = i + j;
                std::cout << std::setw(5) << reg_names[reg] << ": 0x" 
                         << std::hex << std::setw(8) << std::setfill('0') 
                         << simulator.getRegister(reg) << "  ";
            }
            std::cout << "\n";
        }
        std::cout << std::dec << "\n";
    }
    
    void printMemory(const std::string& addr_str) {
        if (addr_str.empty()) {
            std::cout << "Error: No address specified.\n";
            return;
        }
        
        try {
            uint32_t addr = std::stoul(addr_str, nullptr, 0);
            uint32_t value = simulator.getMemory(addr);
            
            std::cout << "Memory[0x" << std::hex << std::setw(8) << std::setfill('0') 
                     << addr << "] = 0x" << std::setw(8) << std::setfill('0') 
                     << value << std::dec << "\n";
        } catch (const std::exception& e) {
            std::cout << "Error: Invalid address format.\n";
        }
    }
    
    void setRegister(const std::string& reg_str, const std::string& val_str) {
        if (reg_str.empty() || val_str.empty()) {
            std::cout << "Error: Missing register or value.\n";
            return;
        }
        
        try {
            int reg = std::stoi(reg_str);
            uint32_t value = std::stoul(val_str, nullptr, 0);
            
            simulator.setRegister(reg, value);
            std::cout << "Register $" << reg << " set to 0x" << std::hex 
                     << std::setw(8) << std::setfill('0') << value << std::dec << "\n";
        } catch (const std::exception& e) {
            std::cout << "Error: Invalid register number or value format.\n";
        }
    }
    
    void setMemory(const std::string& addr_str, const std::string& val_str) {
        if (addr_str.empty() || val_str.empty()) {
            std::cout << "Error: Missing address or value.\n";
            return;
        }
        
        try {
            uint32_t addr = std::stoul(addr_str, nullptr, 0);
            uint32_t value = std::stoul(val_str, nullptr, 0);
            
            simulator.setMemory(addr, value);
            std::cout << "Memory[0x" << std::hex << std::setw(8) << std::setfill('0') 
                     << addr << "] set to 0x" << std::setw(8) << std::setfill('0') 
                     << value << std::dec << "\n";
        } catch (const std::exception& e) {
            std::cout << "Error: Invalid address or value format.\n";
        }
    }
    
    void togglePipeline(const std::string& mode) {
        if (mode == "on" || mode == "enable" || mode == "1") {
            simulator.enablePipeline(true);
            std::cout << "Pipeline simulation enabled.\n";
        } else if (mode == "off" || mode == "disable" || mode == "0") {
            simulator.enablePipeline(false);
            std::cout << "Pipeline simulation disabled.\n";
        } else {
            std::cout << "Usage: pipeline <on|off>\n";
        }
    }
    
    void configureBranchPrediction(const std::string& mode, const std::string& type) {
        if (mode == "on" || mode == "enable" || mode == "1") {
            std::string pred_type = type.empty() ? "static" : type;
            simulator.enableBranchPrediction(true, pred_type);
            std::cout << "Branch prediction enabled (" << pred_type << ").\n";
        } else if (mode == "off" || mode == "disable" || mode == "0") {
            simulator.enableBranchPrediction(false);
            std::cout << "Branch prediction disabled.\n";
        } else {
            std::cout << "Usage: branch <on|off> [static|dynamic]\n";
        }
    }
    
    void printStats() {
        std::cout << "\n" << simulator.getBranchPredictionStats();
        std::cout << simulator.getPipelineStateString() << "\n";
    }
    
    void disassemble(const std::string& addr_str) {
        if (addr_str.empty()) {
            std::cout << "Error: No address specified.\n";
            return;
        }
        
        try {
            uint32_t addr = std::stoul(addr_str, nullptr, 0);
            uint32_t instruction = simulator.getMemory(addr);
            
            std::cout << "0x" << std::hex << std::setw(8) << std::setfill('0') << addr 
                     << ": 0x" << std::setw(8) << std::setfill('0') << instruction 
                     << "  " << InstructionDecoder::disassemble(instruction) << std::dec << "\n";
        } catch (const std::exception& e) {
            std::cout << "Error: Invalid address format.\n";
        }
    }
};

int main() {
    CLIInterface cli;
    cli.run();
    return 0;
}
