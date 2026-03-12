#include "parser/lexer.hpp"
#include <fstream>
#include <stdexcept>
#include <cctype>

Lexer::Lexer() {}

std::vector<std::string> Lexer::tokenize_line(const std::string &line_content) const
{
    std::vector<std::string> tokens;
    std::string current_token = "";
    bool inside_string_literal = false;
    for (std::string::size_type character_index = 0; character_index < line_content.length(); ++character_index)
    {
        char current_character = line_content[character_index];
        if (inside_string_literal)
        {
            current_token += current_character;
            if (current_character == '"')
            {
                inside_string_literal = false;
                tokens.push_back(current_token);
                current_token = "";
            }
        }
        else
        {
            if (current_character == '#')
                break;
            else if (current_character == '"')
            {
                if (!current_token.empty())
                {
                    tokens.push_back(current_token);
                    current_token = "";
                }
                inside_string_literal = true;
                current_token += current_character;
            }
            else if (std::isspace(current_character) || current_character == ',')
            {
                if (!current_token.empty())
                {
                    tokens.push_back(current_token);
                    current_token = "";
                }
            }
            else
                current_token += current_character;
        }
    }
    if (inside_string_literal)
        throw std::runtime_error("Unterminated string literal encountered during tokenization.");
    if (!current_token.empty())
        tokens.push_back(current_token);
    return tokens;
}

std::vector<std::vector<std::string>> Lexer::read_file(const std::string &file_path) const
{
    std::ifstream file_stream(file_path);
    if (!file_stream.is_open())
        throw std::runtime_error("Failed to open assembly file: " + file_path);
    std::vector<std::vector<std::string>> parsed_lines;
    std::string current_line;
    while (std::getline(file_stream, current_line))
    {
        std::vector<std::string> tokenized_line = tokenize_line(current_line);
        if (!tokenized_line.empty())
            parsed_lines.push_back(tokenized_line);
    }
    file_stream.close();
    return parsed_lines;
}