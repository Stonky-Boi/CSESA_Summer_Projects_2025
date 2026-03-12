#include "pipeline/pipeline_state.hpp"

PipelineState::PipelineState() {}

IfIdLatch PipelineState::read_if_id_latch() const
{
    return current_if_id;
}

void PipelineState::write_if_id_latch(const IfIdLatch &new_latch)
{
    next_if_id = new_latch;
}

void PipelineState::flush_if_id_latch()
{
    next_if_id.is_valid = false;
}

IfIdLatch PipelineState::read_next_if_id_latch() const
{
    return next_if_id;
}

void PipelineState::inject_prediction_to_if_id(bool taken, uint32_t target)
{
    next_if_id.was_predicted_taken = taken;
    next_if_id.predicted_target = target;
}

IdExLatch PipelineState::read_id_ex_latch() const
{
    return current_id_ex;
}

void PipelineState::write_id_ex_latch(const IdExLatch &new_latch)
{
    next_id_ex = new_latch;
}

void PipelineState::flush_id_ex_latch()
{
    next_id_ex.is_valid = false;
}

ExMemLatch PipelineState::read_ex_mem_latch() const
{
    return current_ex_mem;
}

void PipelineState::write_ex_mem_latch(const ExMemLatch &new_latch)
{
    next_ex_mem = new_latch;
}

void PipelineState::flush_ex_mem_latch()
{
    next_ex_mem.is_valid = false;
}

MemWbLatch PipelineState::read_mem_wb_latch() const
{
    return current_mem_wb;
}

void PipelineState::write_mem_wb_latch(const MemWbLatch &new_latch)
{
    next_mem_wb = new_latch;
}

void PipelineState::flush_mem_wb_latch()
{
    next_mem_wb.is_valid = false;
}

void PipelineState::update_clock_cycle()
{
    current_if_id = next_if_id;
    current_id_ex = next_id_ex;
    current_ex_mem = next_ex_mem;
    current_mem_wb = next_mem_wb;
}