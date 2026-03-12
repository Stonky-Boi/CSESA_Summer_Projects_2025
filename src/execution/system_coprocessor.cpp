#include "execution/system_coprocessor.hpp"
#include <iostream>
#include <string>
#include <stdexcept>

SystemCoprocessor::SystemCoprocessor() {}

bool SystemCoprocessor::handle_syscall(RegisterFile &register_file, Memory &system_memory)
{
    uint32_t syscall_code = register_file.read_register(2); // $v0
    if (syscall_code == 1)
    {
        // Print Integer
        int32_t integer_value = static_cast<int32_t>(register_file.read_register(4)); // $a0
        std::cout << integer_value;
    }
    else if (syscall_code == 4)
    {
        // Print String
        uint32_t memory_address = register_file.read_register(4); // $a0
        std::string string_to_print = "";
        while (true)
        {
            uint8_t character_byte = system_memory.read_byte(memory_address);
            if (character_byte == 0)
                break;
            string_to_print += static_cast<char>(character_byte);
            memory_address++;
        }
        std::cout << string_to_print;
    }
    else if (syscall_code == 5)
    {
        // Read Integer
        int32_t integer_input;
        std::cin >> integer_input;
        // Clear trailing newline from the input buffer to prevent it from skipping subsequent string reads
        if (std::cin.peek() == '\n')
            std::cin.ignore();
        register_file.write_register(2, static_cast<uint32_t>(integer_input)); // Write back to $v0
    }
    else if (syscall_code == 8)
    {
        // Read String
        uint32_t memory_address = register_file.read_register(4); // $a0
        uint32_t maximum_length = register_file.read_register(5); // $a1
        std::string string_input;
        std::getline(std::cin, string_input);
        uint32_t bytes_to_write = static_cast<uint32_t>(string_input.length());
        if (bytes_to_write >= maximum_length)
            bytes_to_write = maximum_length - 1;
        for (uint32_t index = 0; index < bytes_to_write; ++index)
            system_memory.write_byte(memory_address + index, static_cast<uint8_t>(string_input[index]));
        // Always null-terminate
        system_memory.write_byte(memory_address + bytes_to_write, 0);
    }
    else if (syscall_code == 10)
    {
        // Exit Program
        return false;
    }
    else if (syscall_code == 11)
    {
        // Print Character
        uint32_t character_value = register_file.read_register(4); // $a0
        std::cout << static_cast<char>(character_value & 0xFF);
    }
    else if (syscall_code == 12)
    {
        // Read Character
        char character_input;
        std::cin.get(character_input);
        register_file.write_register(2, static_cast<uint32_t>(character_input)); // Write back to $v0
    }
    else
        throw std::runtime_error("Unsupported or invalid SYSCALL code: " + std::to_string(syscall_code));
    return true; // Continue execution unless SYSCALL 10 is hit
}