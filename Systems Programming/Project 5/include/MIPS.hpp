#ifndef MIPS_HPP
#define MIPS_HPP

#include <vector>
#include <map>
#include <string>
#include <cstdint>
#include "Instruction.hpp"
#include "Pipeline.hpp"
#include "BranchPredictor.hpp"

class MIPS {
private:
    // Registers
    std::vector<uint32_t> registers;
    uint32_t pc;
    uint32_t hi, lo;
    
    // Memory
    std::vector<uint8_t> memory;
    
    // Components
    Pipeline pipeline;
    BranchPredictor branchPredictor;
    
    // Instruction cache
    std::vector<Instruction> instructions;
    
    // Execution modes
    bool stepMode;
    bool pipelineEnabled;
    
    // Statistics
    uint64_t cycleCount;
    uint64_t instructionCount;
    
public:
    MIPS();
    ~MIPS();
    
    // Core functionality
    void loadProgram(const std::vector<std::string>& assembly);
    void reset();
    void step();
    void run();
    
    // State access
    uint32_t getRegister(int reg) const;
    void setRegister(int reg, uint32_t value);
    uint32_t getPC() const;
    void setPC(uint32_t value);
    uint32_t getMemoryWord(uint32_t address) const;
    void setMemoryWord(uint32_t address, uint32_t value);
    uint8_t getMemoryByte(uint32_t address) const;
    void setMemoryByte(uint32_t address, uint8_t value);
    
    // Configuration
    void enablePipeline(bool enable);
    void setStepMode(bool enable);
    
    // Status information
    std::string getRegisterState() const;
    std::string getMemoryState(uint32_t start, uint32_t length) const;
    std::string getPipelineState() const;
    std::string getBranchPredictorStats() const;
    
    // Statistics
    uint64_t getCycleCount() const { return cycleCount; }
    uint64_t getInstructionCount() const { return instructionCount; }
    double getCPI() const { return (double)cycleCount / instructionCount; }
    
    // Pipeline control
    bool hasPipelineHazard() const;
    void flushPipeline();
};

#endif // MIPS_HPP