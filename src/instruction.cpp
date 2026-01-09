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

    // Handle labels
    auto label_pos = original_line.find(':');
    if (label_pos != std::string::npos) {
        original_line = original_line.substr(label_pos + 1);
        original_line.erase(0, original_line.find_first_not_of(" \t"));
    }
    
    if (original_line.empty()) return;
    
    // Split by spaces first to get operation and argument string
    std::stringstream ss(original_line);
    ss >> operation;
    
    // Get the rest of the line as arguments
    std::string rest_of_line;
    std::getline(ss, rest_of_line);
    
    // Trim leading whitespace from arguments
    rest_of_line.erase(0, rest_of_line.find_first_not_of(" \t"));
    
    if (rest_of_line.empty()) return;
    
    // Split arguments by comma
    std::stringstream arg_stream(rest_of_line);
    std::string arg;
    
    while (std::getline(arg_stream, arg, ',')) {
        // Trim whitespace from each argument
        arg.erase(0, arg.find_first_not_of(" \t"));
        arg.erase(arg.find_last_not_of(" \t") + 1);
        
        if (!arg.empty()) {
            args.push_back(arg);
        }
    }
}