#pragma once

#include <string>
#include <vector>

class Lexer
{
private:
    std::vector<std::string> tokenize_line(const std::string &line_content) const;

public:
    Lexer();

    std::vector<std::vector<std::string>> read_file(const std::string &file_path) const;
};