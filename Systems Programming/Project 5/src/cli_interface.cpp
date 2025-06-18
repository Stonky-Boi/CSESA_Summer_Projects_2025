#include <iostream>
#include <sstream>
#include <string>
#include <fstream>        // For std::ifstream  [1]
#include <vector>
#include <cstdint>
#include "mips_simulator.hpp"

void printHelp() {
    std::cout << "Available Commands:\n"
                 "  load <file>     - Load MIPS assembly file and assemble\n"
                 "  loadhex         - Load raw hex instructions interactively\n"
                 "  step (s)        - Execute one instruction\n"
                 "  run (r)         - Execute program to completion\n"
                 "  reset           - Reset simulator state\n"
                 "  registers (reg) - Show register values\n"
                 "  memory <addr>   - Show memory at address\n"
                 "  help (h)        - Show this help\n"
                 "  quit (q)        - Exit\n";
}

std::vector<uint32_t> parseHexLines(const std::string &lines) {
    std::vector<uint32_t> code;
    std::istringstream iss(lines);
    std::string line;
    while (std::getline(iss, line)) {
        if (line.empty() || line[0] == '#') continue;
        uint32_t word = std::stoul(line, nullptr, 16);  // Parse hex string[2]
        code.push_back(word);
    }
    return code;
}

int main() {
    MIPSSimulator sim;
    std::string input, cmd;

    std::cout << "MIPS Simulator CLI (type 'help')\n";

    while (true) {
        std::cout << "MIPS> ";
        if (!std::getline(std::cin, input)) break;
        std::istringstream iss(input);
        iss >> cmd;

        if (cmd == "help" || cmd == "h") {
            printHelp();
        }
        else if (cmd == "load") {
            std::string filename;
            iss >> filename;
            std::ifstream file(filename);
            if (!file.is_open()) {            // Use .is_open() on ifstream [1]
                std::cerr << "Error: Cannot open file " << filename << "\n";
                continue;
            }
            std::ostringstream buf;
            buf << file.rdbuf();
            file.close();
            if (sim.assemble(buf.str())) {
                std::cout << "Program assembled successfully.\n";
            } else {
                std::cerr << "Error: Assembly failed.\n";
            }
        }
        else if (cmd == "loadhex") {
            std::cout << "Enter hex instructions (blank line to finish):\n";
            std::ostringstream buf;
            while (true) {
                std::string line;
                std::getline(std::cin, line);
                if (line.empty()) break;
                buf << line << "\n";
            }
            auto code = parseHexLines(buf.str());
            sim.reset();
            sim.loadMachineCode(code);        // loadMachineCode expects vector<uint32_t>
            std::cout << "Hex instructions loaded.\n";
        }
        else if (cmd == "step" || cmd == "s") {
            if (!sim.step()) {
                std::cout << "Simulation halted or error occurred.\n";
            } else {
                std::cout << sim.getStateString();
            }
        }
        else if (cmd == "run" || cmd == "r") {
            sim.run();
            std::cout << sim.getStateString();
            std::cout << sim.getOutput();
        }
        else if (cmd == "reset") {
            sim.reset();
            std::cout << "Simulator reset.\n";
        }
        else if (cmd == "registers" || cmd == "reg") {
            std::cout << sim.getStateString();
        }
        else if (cmd == "memory" || cmd == "mem") {
            std::string addrStr;
            iss >> addrStr;
            try {
                uint32_t addr = std::stoul(addrStr, nullptr, 0);
                uint32_t val = sim.getMemory(addr);  // New method getMemory() [3]
                std::cout << "Memory[0x" << std::hex << addr
                          << "] = 0x" << val << std::dec << "\n";
            } catch (...) {
                std::cerr << "Error: Invalid address format.\n";
            }
        }
        else if (cmd == "quit" || cmd == "q") {
            break;
        }
        else {
            std::cerr << "Unknown command: " << cmd << ". Type 'help'.\n";
        }
    }
    return 0;
}