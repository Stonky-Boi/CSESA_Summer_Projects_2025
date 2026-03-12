#pragma once

#include "pipeline/pipeline_state.hpp"
#include "instruction/instruction.hpp"

enum class ForwardingSource
{
    NO_FORWARDING,
    FORWARD_FROM_EX_MEM,
    FORWARD_FROM_MEM_WB
};

class HazardUnit
{
private:
    bool is_load_instruction(OperationType operation) const;

public:
    HazardUnit();

    ForwardingSource get_forwarding_source_a(
        const IdExLatch &id_ex_latch,
        const ExMemLatch &ex_mem_latch,
        const MemWbLatch &mem_wb_latch) const;

    ForwardingSource get_forwarding_source_b(
        const IdExLatch &id_ex_latch,
        const ExMemLatch &ex_mem_latch,
        const MemWbLatch &mem_wb_latch) const;

    bool requires_load_use_stall(
        const IfIdLatch &if_id_latch,
        const IdExLatch &id_ex_latch) const;
};