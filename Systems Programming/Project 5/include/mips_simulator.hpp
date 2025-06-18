#pragma once
#include <vector>
#include <map>
#include <string>
#include <cstdint>

class MIPSSimulator {
public:
    MIPSSimulator();
    ~MIPSSimulator();

    // Assembler modes
    bool assemble(const std::string &program);
    bool loadMachineCode(const std::vector<uint32_t> &code);

    // Execution controls
    void reset();
    bool step();
    void run();
    bool isHalted() const;

    // State inspection
    uint32_t getRegister(int reg) const;
    uint32_t getPC() const;
    uint32_t getMemory(uint32_t address) const;
    std::string getStateString() const;
    std::string getPipelineStateString() const;
    std::string getBranchPredictionStats() const;

    // Configuration
    void enablePipeline(bool on);
    void enableBranchPrediction(bool on, const std::string &type="2bit");

    // Syscall I/O
    const std::string &getOutput() const;

private:
    // Core state
    std::vector<uint32_t> registers;
    std::vector<uint8_t> memory;
    uint32_t pc;
    bool halted;

    // Pipeline
    bool pipelineEnabled;
    struct Stage { uint32_t instr, pc; bool valid; };
    Stage stages[5];

    // Branch predictor
    bool branchPredictEnabled;
    std::map<uint32_t,uint8_t> bht; // 2-bit predictor
    struct {int total,correct,incorrect;} bs;

    // Output buffer
    std::string outputBuffer;

    // Assembler support
    bool assembleText(const std::string &txt);
    std::map<std::string,uint32_t> symbolTable;
    uint32_t dataBase, textBase;
    std::vector<uint32_t> textSegment;

    bool decodeAndLoad(const std::vector<std::string> &lines);

    // Execution helpers
    bool fetchDecodeExecute(uint32_t instr);
    uint32_t signExt(int16_t imm);
    void handleSyscall();

    // Utility
    void writeOutput(const std::string &s);
};