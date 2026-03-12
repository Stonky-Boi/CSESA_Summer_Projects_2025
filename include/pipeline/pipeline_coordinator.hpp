#pragma once

#include "architecture/register_file.hpp"
#include "architecture/memory.hpp"
#include "architecture/program_counter.hpp"
#include "pipeline/pipeline_state.hpp"
#include "pipeline/fetch_stage.hpp"
#include "pipeline/decode_stage.hpp"
#include "pipeline/execute_stage.hpp"
#include "pipeline/memory_stage.hpp"
#include "pipeline/writeback_stage.hpp"
#include "pipeline/hazard_unit.hpp"
#include "execution/system_coprocessor.hpp"
#include "prediction/branch_predictor.hpp"
#include "utils/statistics_tracker.hpp"
#include "utils/execution_tracer.hpp"
#include "instruction/instruction.hpp"
#include <map>
#include <cstdint>

class PipelineCoordinator
{
private:
    RegisterFile &register_file;
    Memory &system_memory;
    ProgramCounter &program_counter;
    std::map<uint32_t, Instruction> instruction_memory;

    PipelineState pipeline_state;
    FetchStage fetch_stage;
    DecodeStage decode_stage;
    ExecuteStage execute_stage;
    MemoryStage memory_stage;
    WritebackStage writeback_stage;
    HazardUnit hazard_unit;
    SystemCoprocessor system_coprocessor;

    BranchPredictor *branch_predictor;
    std::map<uint32_t, uint32_t> branch_target_buffer;

    StatisticsTracker &statistics_tracker;
    ExecutionTracer &execution_tracer;

    bool check_if_pipeline_is_empty() const;
    bool handle_system_calls_in_writeback();

public:
    PipelineCoordinator(
        RegisterFile &registers,
        Memory &memory,
        ProgramCounter &pc,
        const std::map<uint32_t, Instruction> &program_instructions,
        BranchPredictor *predictor,
        StatisticsTracker &stats,
        ExecutionTracer &tracer);

    void execute_program();
};