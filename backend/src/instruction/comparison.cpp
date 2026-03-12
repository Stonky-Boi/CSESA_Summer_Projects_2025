#include "instruction/comparison.hpp"

void Comparison::execute(const Instruction &current_instruction, RegisterFile &register_file)
{
    OperationType operation = current_instruction.get_operation();
    int register_source = current_instruction.get_register_source();
    int register_target = current_instruction.get_register_target();
    int register_destination = current_instruction.get_register_destination();
    int32_t immediate_value = current_instruction.get_immediate_value();
    if (operation == OperationType::SLT || operation == OperationType::SGT ||
        operation == OperationType::SLE || operation == OperationType::SGE ||
        operation == OperationType::SEQ || operation == OperationType::SNE)
    {
        int32_t operand_one = static_cast<int32_t>(register_file.read_register(register_source));
        int32_t operand_two = static_cast<int32_t>(register_file.read_register(register_target));
        uint32_t result = 0;
        if (operation == OperationType::SLT)
            result = (operand_one < operand_two) ? 1 : 0;
        else if (operation == OperationType::SGT)
            result = (operand_one > operand_two) ? 1 : 0;
        else if (operation == OperationType::SLE)
            result = (operand_one <= operand_two) ? 1 : 0;
        else if (operation == OperationType::SGE)
            result = (operand_one >= operand_two) ? 1 : 0;
        else if (operation == OperationType::SEQ)
            result = (operand_one == operand_two) ? 1 : 0;
        else if (operation == OperationType::SNE)
            result = (operand_one != operand_two) ? 1 : 0;
        register_file.write_register(register_destination, result);
    }
    else if (operation == OperationType::SLTU || operation == OperationType::SGTU ||
             operation == OperationType::SLEU || operation == OperationType::SGEU)
    {
        uint32_t operand_one = register_file.read_register(register_source);
        uint32_t operand_two = register_file.read_register(register_target);
        uint32_t result = 0;
        if (operation == OperationType::SLTU)
            result = (operand_one < operand_two) ? 1 : 0;
        else if (operation == OperationType::SGTU)
            result = (operand_one > operand_two) ? 1 : 0;
        else if (operation == OperationType::SLEU)
            result = (operand_one <= operand_two) ? 1 : 0;
        else if (operation == OperationType::SGEU)
            result = (operand_one >= operand_two) ? 1 : 0;
        register_file.write_register(register_destination, result);
    }
    else if (operation == OperationType::SLTI || operation == OperationType::SGTI ||
             operation == OperationType::SLEI || operation == OperationType::SGEI ||
             operation == OperationType::SEQI || operation == OperationType::SNEI)
    {
        int32_t operand_one = static_cast<int32_t>(register_file.read_register(register_source));
        uint32_t result = 0;
        if (operation == OperationType::SLTI)
            result = (operand_one < immediate_value) ? 1 : 0;
        else if (operation == OperationType::SGTI)
            result = (operand_one > immediate_value) ? 1 : 0;
        else if (operation == OperationType::SLEI)
            result = (operand_one <= immediate_value) ? 1 : 0;
        else if (operation == OperationType::SGEI)
            result = (operand_one >= immediate_value) ? 1 : 0;
        else if (operation == OperationType::SEQI)
            result = (operand_one == immediate_value) ? 1 : 0;
        else if (operation == OperationType::SNEI)
            result = (operand_one != immediate_value) ? 1 : 0;
        register_file.write_register(register_target, result);
    }
    else if (operation == OperationType::SLTIU || operation == OperationType::SGTIU ||
             operation == OperationType::SLEIU || operation == OperationType::SGEIU)
    {
        uint32_t operand_one = register_file.read_register(register_source);
        uint32_t unsigned_immediate = static_cast<uint32_t>(immediate_value);
        uint32_t result = 0;
        if (operation == OperationType::SLTIU)
            result = (operand_one < unsigned_immediate) ? 1 : 0;
        else if (operation == OperationType::SGTIU)
            result = (operand_one > unsigned_immediate) ? 1 : 0;
        else if (operation == OperationType::SLEIU)
            result = (operand_one <= unsigned_immediate) ? 1 : 0;
        else if (operation == OperationType::SGEIU)
            result = (operand_one >= unsigned_immediate) ? 1 : 0;
        register_file.write_register(register_target, result);
    }
}