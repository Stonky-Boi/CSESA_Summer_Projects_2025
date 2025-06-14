#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include "Instruction.hpp"
#include "HazardDetection.hpp"
#include <queue>

struct PipelineStage {
    bool valid;
    Instruction instruction;
    uint32_t pc;

    // Stage-specific data
    uint32_t aluResult;
    uint32_t memoryData;
    uint32_t writeData;
    uint8_t writeReg;
    bool regWrite;
    bool memRead;
    bool memWrite;
    bool branch;
    bool jump;

    PipelineStage() : valid(false), regWrite(false), memRead(false), 
                     memWrite(false), branch(false), jump(false) {}
};

class Pipeline {
private:
    // Pipeline registers
    PipelineStage IF_ID;
    PipelineStage ID_EX;
    PipelineStage EX_MEM;
    PipelineStage MEM_WB;

    // Hazard detection and forwarding
    HazardDetection hazardUnit;

    // Stall and flush control
    bool stall;
    bool flush;

    // Statistics
    uint64_t stallCycles;
    uint64_t flushCycles;

public:
    Pipeline();

    // Pipeline execution
    void tick(const Instruction& fetchedInstruction, uint32_t pc,
              std::vector<uint32_t>& registers, std::vector<uint8_t>& memory);

    // Pipeline control
    void stallPipeline();
    void flushPipeline();
    bool isStalled() const { return stall; }

    // Forwarding
    uint32_t getForwardedValue(uint8_t reg, uint32_t originalValue);

    // State access
    std::string getState() const;
    PipelineStage getStage(int stage) const;

    // Statistics
    uint64_t getStallCycles() const { return stallCycles; }
    uint64_t getFlushCycles() const { return flushCycles; }

private:
    // Individual stage execution
    void executeIF(const Instruction& instruction, uint32_t pc);
    void executeID(std::vector<uint32_t>& registers);
    void executeEX();
    void executeMEM(std::vector<uint8_t>& memory);
    void executeWB(std::vector<uint32_t>& registers);

    // ALU operations
    uint32_t performALU(uint32_t op1, uint32_t op2, InstructionFormat format);
};

#endif // PIPELINE_HPP