#include "utils/command_line_parser.hpp"
#include "prediction/static/always_taken.hpp"
#include "prediction/static/always_not_taken.hpp"
#include "prediction/dynamic/one_bit.hpp"
#include "prediction/dynamic/two_bit_saturating.hpp"
#include "prediction/dynamic/local_history.hpp"
#include "prediction/dynamic/global_history.hpp"
#include "prediction/dynamic/two_level_adaptive.hpp"
#include <stdexcept>

CommandLineParser::CommandLineParser(int argument_count, char *argument_values[])
{
    if (argument_count < 2)
        throw std::invalid_argument("Usage: ember <path_to_assembly_file> [--mode=pipeline|interpreter] [--predictor=...] [--trace]");

    target_file_path = argument_values[1];
    execution_mode = "interpreter";
    predictor_type = "always_not_taken";
    enable_tracing = false;

    for (int index = 2; index < argument_count; ++index)
    {
        std::string argument = argument_values[index];
        if (argument.find("--mode=") == 0)
            execution_mode = argument.substr(7);
        else if (argument.find("--predictor=") == 0)
            predictor_type = argument.substr(12);
        else if (argument == "--trace")
            enable_tracing = true;
    }
}

std::string CommandLineParser::get_file_path() const
{
    return target_file_path;
}

std::string CommandLineParser::get_execution_mode() const
{
    return execution_mode;
}

bool CommandLineParser::is_tracing_enabled() const
{
    return enable_tracing;
}

BranchPredictor *CommandLineParser::instantiate_predictor() const
{
    if (predictor_type == "always_taken")
        return new AlwaysTaken();
    if (predictor_type == "always_not_taken")
        return new AlwaysNotTaken();
    if (predictor_type == "one_bit")
        return new OneBit();
    if (predictor_type == "two_bit")
        return new TwoBitSaturating();
    if (predictor_type == "local_history")
        return new LocalHistory();
    if (predictor_type == "global_history")
        return new GlobalHistory();
    if (predictor_type == "two_level")
        return new TwoLevelAdaptive();
        
    throw std::invalid_argument("Unknown predictor type specified: " + predictor_type);
}