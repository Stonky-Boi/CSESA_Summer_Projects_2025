#pragma once
#include <vector>
#include <map>
#include <string>
#include <cstdint>
#include <memory>

class MIPSSimulator {
public:
    // Constructor and destructor
    MIPSSimulator();
    ~MIPSSimulator();
    
    // Main execution methods
    bool loadProgram(const std::string& filename);
    bool loadProgramFromString(const std::string& program);
    void reset();
    bool step();
    void run();
    bool isHalted() const;
    
    // State access methods
    uint32_t getRegister(int reg) const;
    void setRegister(int reg, uint32_t value);
    uint32_t getMemory(uint32_t address) const;
    void setMemory(uint32_t address, uint32_t value);
    uint32_t getPC() const;
    void setPC(uint32_t pc);
    
    // Pipeline and statistics
    void enablePipeline(bool enable);
    void enableBranchPrediction(bool enable, const std::string& type = "static");
    std::string getStateString() const;
    std::string getPipelineStateString() const;
    std::string getBranchPredictionStats() const;
    
    // Execution modes
    void setStepMode(bool step_mode);
    bool getStepMode() const;

private:
    // Core components
    std::vector<uint32_t> registers;
    std::vector<uint8_t> memory;
    uint32_t pc;
    bool halted;
    bool step_mode;
    
    // Pipeline components
    bool pipeline_enabled;
    struct PipelineStage {
        uint32_t instruction;
        uint32_t pc;
        bool valid;
        bool stall;
        bool flush;
    };
    
    std::vector<PipelineStage> pipeline_stages;
    
    // Branch prediction
    bool branch_prediction_enabled;
    std::string prediction_type;
    std::map<uint32_t, bool> branch_history_table;
    struct BranchStats {
        int total_branches;
        int correct_predictions;
        int incorrect_predictions;
    } branch_stats;
    
    // Instruction processing
    struct Instruction {
        uint32_t raw;
        uint8_t opcode;
        uint8_t rs, rt, rd;
        uint16_t immediate;
        uint32_t jump_addr;
        uint8_t funct;
        uint8_t shamt;
        std::string type; // "R", "I", "J"
    };
    
    Instruction decodeInstruction(uint32_t instruction);
    bool executeInstruction(const Instruction& instr);
    
    // Pipeline methods
    void initializePipeline();
    void advancePipeline();
    bool detectHazards();
    void handleHazards();
    
    // Branch prediction methods
    bool predictBranch(uint32_t pc);
    void updateBranchPredictor(uint32_t pc, bool taken);
    
    // Helper methods
    uint32_t signExtend16(uint16_t value);
    bool isValidAddress(uint32_t address) const;
    void printInstruction(const Instruction& instr) const;
};