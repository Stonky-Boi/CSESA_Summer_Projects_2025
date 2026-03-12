#pragma once

#include "prediction/branch_predictor.hpp"
#include <string>

class CommandLineParser
{
private:
    std::string target_file_path;
    std::string execution_mode;
    std::string predictor_type;
    bool enable_tracing;

public:
    CommandLineParser(int argument_count, char *argument_values[]);

    std::string get_file_path() const;
    std::string get_execution_mode() const;
    bool is_tracing_enabled() const;

    // Caller is responsible for deleting the returned pointer
    BranchPredictor *instantiate_predictor() const;
};