#include "pipeline/hazard_unit.hpp"

HazardUnit::HazardUnit() {}

bool HazardUnit::is_load_instruction(OperationType operation) const
{
    return operation == OperationType::LB ||
           operation == OperationType::LBU ||
           operation == OperationType::LH ||
           operation == OperationType::LHU ||
           operation == OperationType::LW ||
           operation == OperationType::LL;
}

ForwardingSource HazardUnit::get_forwarding_source_a(
    const IdExLatch &id_ex_latch,
    const ExMemLatch &ex_mem_latch,
    const MemWbLatch &mem_wb_latch) const
{
    // EX Hazard
    if (ex_mem_latch.is_valid &&
        ex_mem_latch.write_register_index != 0 &&
        ex_mem_latch.write_register_index == id_ex_latch.register_source)
        return ForwardingSource::FORWARD_FROM_EX_MEM;
    // MEM Hazard (Only forward if EX is NOT already forwarding to the same register)
    if (mem_wb_latch.is_valid &&
        mem_wb_latch.write_register_index != 0 &&
        mem_wb_latch.write_register_index == id_ex_latch.register_source)
    {
        bool ex_is_forwarding = (ex_mem_latch.is_valid &&
                                 ex_mem_latch.write_register_index == id_ex_latch.register_source);
        if (!ex_is_forwarding)
            return ForwardingSource::FORWARD_FROM_MEM_WB;
    }
    return ForwardingSource::NO_FORWARDING;
}

ForwardingSource HazardUnit::get_forwarding_source_b(
    const IdExLatch &id_ex_latch,
    const ExMemLatch &ex_mem_latch,
    const MemWbLatch &mem_wb_latch) const
{
    // EX Hazard
    if (ex_mem_latch.is_valid &&
        ex_mem_latch.write_register_index != 0 &&
        ex_mem_latch.write_register_index == id_ex_latch.register_target)
        return ForwardingSource::FORWARD_FROM_EX_MEM;
    // MEM Hazard
    if (mem_wb_latch.is_valid &&
        mem_wb_latch.write_register_index != 0 &&
        mem_wb_latch.write_register_index == id_ex_latch.register_target)
    {
        bool ex_is_forwarding = (ex_mem_latch.is_valid &&
                                 ex_mem_latch.write_register_index == id_ex_latch.register_target);
        if (!ex_is_forwarding)
            return ForwardingSource::FORWARD_FROM_MEM_WB;
    }
    return ForwardingSource::NO_FORWARDING;
}

bool HazardUnit::requires_load_use_stall(
    const IfIdLatch &if_id_latch,
    const IdExLatch &id_ex_latch) const
{
    if (!id_ex_latch.is_valid || !if_id_latch.is_valid)
        return false;
    OperationType id_ex_operation = id_ex_latch.current_instruction.get_operation();
    if (is_load_instruction(id_ex_operation))
    {
        int id_ex_target = id_ex_latch.register_target;
        int if_id_source = if_id_latch.current_instruction.get_register_source();
        int if_id_target = if_id_latch.current_instruction.get_register_target();
        // If the instruction in ID needs to read the register that the instruction in EX is loading
        if (id_ex_target != 0 && (id_ex_target == if_id_source || id_ex_target == if_id_target))
            return true;
    }
    return false;
}