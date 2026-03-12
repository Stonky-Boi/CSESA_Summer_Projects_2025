#include "pipeline/execute_stage.hpp"
#include <stdexcept>

ExecuteStage::ExecuteStage() {}

bool ExecuteStage::is_load_instruction(OperationType operation) const
{
    return operation == OperationType::LB || operation == OperationType::LBU ||
           operation == OperationType::LH || operation == OperationType::LHU ||
           operation == OperationType::LW || operation == OperationType::LL;
}

ExecuteResult ExecuteStage::execute(
    PipelineState &pipeline_state,
    const HazardUnit &hazard_unit) const
{
    ExecuteResult result;
    IdExLatch current_latch = pipeline_state.read_id_ex_latch();
    ExMemLatch next_latch;

    if (!current_latch.is_valid)
    {
        next_latch.is_valid = false;
        pipeline_state.write_ex_mem_latch(next_latch);
        return result;
    }

    ExMemLatch current_ex_mem = pipeline_state.read_ex_mem_latch();
    MemWbLatch current_mem_wb = pipeline_state.read_mem_wb_latch();

    ForwardingSource forward_a = hazard_unit.get_forwarding_source_a(current_latch, current_ex_mem, current_mem_wb);
    ForwardingSource forward_b = hazard_unit.get_forwarding_source_b(current_latch, current_ex_mem, current_mem_wb);

    uint32_t operand_one = current_latch.register_read_data_one;
    uint32_t operand_two = current_latch.register_read_data_two;

    // Resolve Forwarding Mux A
    if (forward_a == ForwardingSource::FORWARD_FROM_EX_MEM)
        operand_one = current_ex_mem.alu_result;
    else if (forward_a == ForwardingSource::FORWARD_FROM_MEM_WB)
    {
        if (is_load_instruction(current_mem_wb.current_instruction.get_operation()))
            operand_one = current_mem_wb.memory_read_data;
        else
            operand_one = current_mem_wb.alu_result;
    }

    // Resolve Forwarding Mux B
    if (forward_b == ForwardingSource::FORWARD_FROM_EX_MEM)
        operand_two = current_ex_mem.alu_result;
    else if (forward_b == ForwardingSource::FORWARD_FROM_MEM_WB)
    {
        if (is_load_instruction(current_mem_wb.current_instruction.get_operation()))
            operand_two = current_mem_wb.memory_read_data;
        else
            operand_two = current_mem_wb.alu_result;
    }

    OperationType operation = current_latch.current_instruction.get_operation();
    InstructionFormat format = current_latch.current_instruction.get_format();

    uint32_t alu_result = 0;
    int write_register_index = 0;

    // Determine Write Register
    if (format == InstructionFormat::R_TYPE)
        write_register_index = current_latch.register_destination;
    else if (format == InstructionFormat::I_TYPE)
        write_register_index = current_latch.register_target;

    // Basic ALU Execution (Arithmetic and Logic)
    if (operation == OperationType::ADD || operation == OperationType::ADDU)
        alu_result = operand_one + operand_two;
    else if (operation == OperationType::ADDI)
        alu_result = operand_one + static_cast<uint32_t>(current_latch.immediate_value);
    else if (operation == OperationType::SUB || operation == OperationType::SUBU)
        alu_result = operand_one - operand_two;
    else if (operation == OperationType::AND)
        alu_result = operand_one & operand_two;
    else if (operation == OperationType::ANDI)
        alu_result = operand_one & (static_cast<uint32_t>(current_latch.immediate_value) & 0x0000FFFF);
    else if (operation == OperationType::OR)
        alu_result = operand_one | operand_two;
    else if (operation == OperationType::ORI)
        alu_result = operand_one | (static_cast<uint32_t>(current_latch.immediate_value) & 0x0000FFFF);
    else if (operation == OperationType::XOR)
        alu_result = operand_one ^ operand_two;
    else if (operation == OperationType::XORI)
        alu_result = operand_one ^ (static_cast<uint32_t>(current_latch.immediate_value) & 0x0000FFFF);
    else if (operation == OperationType::LUI)
        alu_result = static_cast<uint32_t>(current_latch.immediate_value) << 16;
    // Memory Address Calculation
    else if (is_load_instruction(operation) || operation == OperationType::SB || operation == OperationType::SH || operation == OperationType::SW)
    {
        alu_result = operand_one + static_cast<uint32_t>(current_latch.immediate_value);
        write_register_index = (is_load_instruction(operation)) ? current_latch.register_target : 0;
    }
    // Branch Resolution
    else if (operation == OperationType::BEQ)
    {
        result.is_branch_instruction = true;
        result.branch_taken = (operand_one == operand_two);
        result.branch_target = current_latch.incremented_program_counter + static_cast<uint32_t>(current_latch.immediate_value);
        write_register_index = 0;
    }
    else if (operation == OperationType::BNE)
    {
        result.is_branch_instruction = true;
        result.branch_taken = (operand_one != operand_two);
        result.branch_target = current_latch.incremented_program_counter + static_cast<uint32_t>(current_latch.immediate_value);
        write_register_index = 0;
    }
    else if (operation == OperationType::J)
    {
        result.is_branch_instruction = true;
        result.branch_taken = true;
        result.branch_target = current_latch.current_instruction.get_jump_address();
        write_register_index = 0;
    }
    else
    {
        // Fallback for NOPs, unhandled complex ops, and system calls in this basic pipeline
        alu_result = 0;
    }

    next_latch.current_instruction = current_latch.current_instruction;
    next_latch.program_counter = current_latch.program_counter;
    next_latch.alu_result = alu_result;
    next_latch.store_data = operand_two; // Passed down for SB/SW
    next_latch.write_register_index = write_register_index;
    next_latch.is_valid = true;

    pipeline_state.write_ex_mem_latch(next_latch);
    return result;
}