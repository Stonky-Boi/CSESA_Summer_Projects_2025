#include "instruction.hpp"
#include <sstream>
#include <algorithm>

Instruction::Instruction(std::string line, uint32_t addr) : original_line(line), address(addr) {
    parse();
}

void Instruction::parse() {
    // Sanitize: remove comments and trim whitespace
    auto comment_pos = original_line.find('#');
    if (comment_pos != std::string::npos) {
        original_line = original_line.substr(0, comment_pos);
    }
    original_line.erase(0, original_line.find_first_not_of(" \t\r\n"));
    original_line.erase(original_line.find_last_not_of(" \t\r\n") + 1);

    if (original_line.empty()) return;

    // A label might exist
    auto label_pos = original_line.find(':');
    if (label_pos != std::string::npos) {
        // For now, we assume labels are handled by the simulator's parser
        // This parser just focuses on the instruction part
        original_line = original_line.substr(label_pos + 1);
        original_line.erase(0, original_line.find_first_not_of(" \t"));
    }
    
    std::stringstream ss(original_line);
    ss >> operation;

    std::string arg;
    while (ss >> arg) {
        // Remove commas
        arg.erase(std::remove(arg.begin(), arg.end(), ','), arg.end());
        if (!arg.empty()) {
            args.push_back(arg);
        }
    }
}