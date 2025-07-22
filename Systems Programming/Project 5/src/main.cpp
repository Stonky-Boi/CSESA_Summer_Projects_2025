#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include "mips_simulator.hpp"

// Function to escape strings for JSON
std::string escape_json(const std::string &s) {
    std::ostringstream o;
    for (auto c = s.cbegin(); c != s.cend(); c++) {
        switch (*c) {
            case '"': o << "\\\""; break;
            case '\\': o << "\\\\"; break;
            case '\b': o << "\\b"; break;
            case '\f': o << "\\f"; break;
            case '\n': o << "\\n"; break;
            case '\r': o << "\\r"; break;
            case '\t': o << "\\t"; break;
            default:
                if ('\x00' <= *c && *c <= '\x1f') {
                    o << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)*c;
                } else {
                    o << *c;
                }
        }
    }
    return o.str();
}

void output_json_state(const MipsSimulator& sim) {
    std::cout << "{" << std::endl;

    // PC
    std::cout << "  \"pc\": " << sim.get_registers().get_pc() << "," << std::endl;
    
    // Registers
    std::cout << "  \"registers\": {" << std::endl;
    auto regs = sim.get_registers().get_registers();
    for (int i = 0; i < 32; ++i) {
        std::cout << "    \"" << RegisterFile::get_reg_name(i) << "\": " << regs[i] << (i == 31 ? "" : ",") << std::endl;
    }
    std::cout << "  }," << std::endl;
    
    // Console Output
    std::cout << "  \"console_output\": \"" << escape_json(sim.get_console_output()) << "\"," << std::endl;

    // Memory (only showing first 256 bytes for brevity)
    std::cout << "  \"memory\": {" << std::endl;
    auto mem = sim.get_memory().get_memory_state();
    std::cout << "    \"data_segment_hex\": \"";
    std::stringstream hex_stream;
    for(size_t i = 0; i < 256 && i < mem.size(); ++i) {
        hex_stream << std::hex << std::setw(2) << std::setfill('0') << (int)mem[i];
    }
    std::cout << hex_stream.str() << "\"" << std::endl;
    std::cout << "  }" << std::endl;


    std::cout << "}" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <filepath> [--run | --step]" << std::endl;
        return 1;
    }

    std::string filepath = argv[1];
    std::string mode = argv[2];

    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filepath << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string code = buffer.str();
    
    MipsSimulator simulator;
    try {
        simulator.load_program_from_string(code);
        if (mode == "--run") {
            simulator.run();
        } else if (mode == "--step") {
            // In a real step-by-step from web, we'd load state, step, and return.
            // This is a simplified CLI version.
            simulator.step();
        }
        output_json_state(simulator);

    } catch (const std::exception& e) {
        std::cerr << "{\"error\": \"" << escape_json(e.what()) << "\"}" << std::endl;
        return 1;
    }

    return 0;
}