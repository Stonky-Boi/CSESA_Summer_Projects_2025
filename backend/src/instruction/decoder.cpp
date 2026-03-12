#include "instruction/decoder.hpp"
#include "instruction/arithmetic.hpp"
#include "instruction/logical.hpp"
#include "instruction/shift_rotate.hpp"
#include "instruction/comparison.hpp"
#include "instruction/memory_access.hpp"
#include "instruction/control_flow.hpp"
#include "instruction/bit_manipulation.hpp"
#include "instruction/address_generation.hpp"
#include "instruction/conditional_select.hpp"
#include "instruction/system.hpp"
#include <stdexcept>

Decoder::Decoder() {}

bool Decoder::execute_instruction(
    const Instruction &current_instruction,
    RegisterFile &register_file,
    Memory &system_memory,
    ProgramCounter &program_counter,
    SystemCoprocessor &system_coprocessor) const
{
    OperationType operation = current_instruction.get_operation();
    bool should_continue_execution = true;

    if (operation == OperationType::ADD || operation == OperationType::ADDI ||
        operation == OperationType::ADDU || operation == OperationType::SUB ||
        operation == OperationType::SUBU || operation == OperationType::MUL ||
        operation == OperationType::MUH || operation == OperationType::MULU ||
        operation == OperationType::MUHU || operation == OperationType::DIV ||
        operation == OperationType::DIVU || operation == OperationType::MOD ||
        operation == OperationType::MODU)
    {
        Arithmetic::execute(current_instruction, register_file);
        program_counter.increment_address();
    }
    else if (operation == OperationType::AND || operation == OperationType::ANDI ||
             operation == OperationType::OR || operation == OperationType::ORI ||
             operation == OperationType::XOR || operation == OperationType::XORI ||
             operation == OperationType::NOR || operation == OperationType::NAND)
    {
        Logical::execute(current_instruction, register_file);
        program_counter.increment_address();
    }
    else if (operation == OperationType::SLL || operation == OperationType::SRL ||
             operation == OperationType::SRA || operation == OperationType::SLLV ||
             operation == OperationType::SRLV || operation == OperationType::SRAV ||
             operation == OperationType::ROTL || operation == OperationType::ROTR ||
             operation == OperationType::ROTLV || operation == OperationType::ROTRV)
    {
        ShiftRotate::execute(current_instruction, register_file);
        program_counter.increment_address();
    }
    else if (operation == OperationType::SLT || operation == OperationType::SGT ||
             operation == OperationType::SLE || operation == OperationType::SGE ||
             operation == OperationType::SEQ || operation == OperationType::SNE ||
             operation == OperationType::SLTI || operation == OperationType::SGTI ||
             operation == OperationType::SLEI || operation == OperationType::SGEI ||
             operation == OperationType::SEQI || operation == OperationType::SNEI ||
             operation == OperationType::SLTU || operation == OperationType::SGTU ||
             operation == OperationType::SLEU || operation == OperationType::SGEU ||
             operation == OperationType::SLTIU || operation == OperationType::SGTIU ||
             operation == OperationType::SLEIU || operation == OperationType::SGEIU)
    {
        Comparison::execute(current_instruction, register_file);
        program_counter.increment_address();
    }
    else if (operation == OperationType::LB || operation == OperationType::LBU ||
             operation == OperationType::LH || operation == OperationType::LHU ||
             operation == OperationType::LW || operation == OperationType::LL ||
             operation == OperationType::SB || operation == OperationType::SH ||
             operation == OperationType::SW || operation == OperationType::SC)
    {
        MemoryAccess::execute(current_instruction, register_file, system_memory);
        program_counter.increment_address();
    }
    else if (operation == OperationType::CLO || operation == OperationType::CLZ ||
             operation == OperationType::EXT || operation == OperationType::INS ||
             operation == OperationType::SEB || operation == OperationType::SEH)
    {
        BitManipulation::execute(current_instruction, register_file);
        program_counter.increment_address();
    }
    else if (operation == OperationType::LUI || operation == OperationType::LSA)
    {
        AddressGeneration::execute(current_instruction, register_file);
        program_counter.increment_address();
    }
    else if (operation == OperationType::SELEQZ || operation == OperationType::SELNEZ)
    {
        ConditionalSelect::execute(current_instruction, register_file);
        program_counter.increment_address();
    }
    else if (operation == OperationType::J || operation == OperationType::JR ||
             operation == OperationType::JAL || operation == OperationType::JALR ||
             operation == OperationType::B || operation == OperationType::BAL ||
             operation == OperationType::BEQ || operation == OperationType::BNE ||
             operation == OperationType::BGEZ || operation == OperationType::BLEZ ||
             operation == OperationType::BGTZ || operation == OperationType::BLTZ)
    {
        // The PC is strictly modified inside the ControlFlow handler. Do not increment here.
        ControlFlow::execute(current_instruction, register_file, program_counter);
    }
    else if (operation == OperationType::NOP || operation == OperationType::BREAK ||
             operation == OperationType::TEQ || operation == OperationType::TNE)
    {
        System::execute(current_instruction, register_file);
        program_counter.increment_address();
    }
    else if (operation == OperationType::SYSCALL)
    {
        should_continue_execution = system_coprocessor.handle_syscall(register_file, system_memory);
        program_counter.increment_address();
    }
    else
        throw std::runtime_error("Unknown operation encountered during instruction decoding.");
    return should_continue_execution;
}