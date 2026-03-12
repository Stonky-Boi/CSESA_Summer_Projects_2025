#include "architecture/register_file.hpp"
#include "architecture/memory.hpp"
#include "architecture/program_counter.hpp"
#include "parser/assembler.hpp"
#include "execution/interpreter.hpp"
#include "pipeline/pipeline_coordinator.hpp"
#include "utils/command_line_parser.hpp"
#include "utils/statistics_tracker.hpp"
#include "utils/execution_tracer.hpp"
#include "prediction/branch_predictor.hpp"
#include <iostream>
#include <string>
#include <stdexcept>
#include <map>

int main(int argument_count, char *argument_values[])
{
    try
    {
        CommandLineParser command_line_parser(argument_count, argument_values);

        std::string file_path = command_line_parser.get_file_path();
        std::string execution_mode = command_line_parser.get_execution_mode();

        RegisterFile register_file;
        Memory system_memory;
        ProgramCounter program_counter;

        Assembler program_assembler(register_file);
        std::map<uint32_t, Instruction> instruction_memory = program_assembler.assemble_program(file_path, system_memory);

        if (execution_mode == "pipeline")
        {
            BranchPredictor *branch_predictor = command_line_parser.instantiate_predictor();
            StatisticsTracker statistics_tracker;
            ExecutionTracer execution_tracer(command_line_parser.is_tracing_enabled());

            PipelineCoordinator pipeline_coordinator(
                register_file,
                system_memory,
                program_counter,
                instruction_memory,
                branch_predictor,
                statistics_tracker,
                execution_tracer);

            pipeline_coordinator.execute_program();
            statistics_tracker.print_statistics_report();

            delete branch_predictor;
        }
        else if (execution_mode == "interpreter")
        {
            Interpreter program_interpreter(register_file, system_memory, program_counter, instruction_memory);
            program_interpreter.execute_program();
        }
        else
            throw std::invalid_argument("Unknown execution mode specified: " + execution_mode);
    }
    catch (const std::exception &error)
    {
        std::cerr << "Fatal Error: " << error.what() << std::endl;
        return 1;
    }
    return 0;
}