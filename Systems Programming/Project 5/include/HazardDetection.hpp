#ifndef HAZARD_DETECTION_HPP
#define HAZARD_DETECTION_HPP

#include "Instruction.hpp"
#include <vector>

enum class HazardType {
    NONE,
    DATA_HAZARD_RAW,  // Read After Write
    DATA_HAZARD_WAR,  // Write After Read
    DATA_HAZARD_WAW,  // Write After Write
    CONTROL_HAZARD,   // Branch/Jump
    STRUCTURAL_HAZARD // Resource conflict
};

struct ForwardingPath {
    bool enable;
    uint8_t sourceStage;  // 2=EX, 3=MEM, 4=WB
    uint8_t targetStage;  // 1=ID, 2=EX
    uint8_t reg;
    uint32_t value;
};

class HazardDetection {
private:
    // Hazard detection state
    std::vector<ForwardingPath> forwardingPaths;

    // Statistics
    uint64_t dataHazards;
    uint64_t controlHazards;
    uint64_t structuralHazards;
    uint64_t forwardingEvents;
    uint64_t stallsInserted;

public:
    HazardDetection();

    // Hazard detection
    HazardType detectHazard(const Instruction& current, 
                           const Instruction& inEX,
                           const Instruction& inMEM,
                           const Instruction& inWB);

    // Data hazard detection
    bool detectDataHazard(const Instruction& current, const Instruction& previous);
    bool needsForwarding(uint8_t reg, const Instruction& inEX, 
                        const Instruction& inMEM, const Instruction& inWB);

    // Control hazard detection
    bool detectControlHazard(const Instruction& instruction);

    // Forwarding control
    void setupForwarding(uint8_t reg, uint8_t sourceStage, uint32_t value);
    uint32_t getForwardedValue(uint8_t reg, uint32_t defaultValue);
    void clearForwarding();

    // Load-use hazard detection
    bool detectLoadUseHazard(const Instruction& load, const Instruction& use);

    // Stall/flush decisions
    bool shouldStall(const Instruction& current, 
                    const Instruction& inEX,
                    const Instruction& inMEM);
    bool shouldFlush(const Instruction& branch);

    // Statistics
    uint64_t getDataHazards() const { return dataHazards; }
    uint64_t getControlHazards() const { return controlHazards; }
    uint64_t getStructuralHazards() const { return structuralHazards; }
    uint64_t getForwardingEvents() const { return forwardingEvents; }
    uint64_t getStallsInserted() const { return stallsInserted; }

    std::string getStatistics() const;
};

#endif // HAZARD_DETECTION_HPP