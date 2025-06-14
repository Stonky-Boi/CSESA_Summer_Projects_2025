#include "HazardDetection.hpp"
#include <sstream>

HazardDetection::HazardDetection() : dataHazards(0), controlHazards(0), 
                                   structuralHazards(0), forwardingEvents(0), stallsInserted(0) {}

HazardType HazardDetection::detectHazard(const Instruction& current, 
                                        const Instruction& inEX,
                                        const Instruction& inMEM,
                                        const Instruction& inWB) {

    // Check for data hazards (RAW)
    if (detectDataHazard(current, inEX) || 
        detectDataHazard(current, inMEM) || 
        detectDataHazard(current, inWB)) {
        dataHazards++;
        return HazardType::DATA_HAZARD_RAW;
    }

    // Check for control hazards
    if (detectControlHazard(current)) {
        controlHazards++;
        return HazardType::CONTROL_HAZARD;
    }

    return HazardType::NONE;
}

bool HazardDetection::detectDataHazard(const Instruction& current, const Instruction& previous) {
    if (!previous.valid) return false;

    // Check if previous instruction writes to a register that current instruction reads
    if (previous.writesRD() || previous.writesRT()) {
        uint8_t writeReg = previous.writesRD() ? previous.rd : previous.rt;

        if (current.usesRS() && current.rs == writeReg) return true;
        if (current.usesRT() && current.rt == writeReg) return true;
    }

    return false;
}

bool HazardDetection::needsForwarding(uint8_t reg, const Instruction& inEX, 
                                     const Instruction& inMEM, const Instruction& inWB) {

    // Check EX stage for forwarding opportunity
    if (inEX.valid && (inEX.writesRD() || inEX.writesRT())) {
        uint8_t writeReg = inEX.writesRD() ? inEX.rd : inEX.rt;
        if (writeReg == reg && writeReg != 0) {
            setupForwarding(reg, 2, 0); // Forward from EX stage
            forwardingEvents++;
            return true;
        }
    }

    // Check MEM stage for forwarding opportunity
    if (inMEM.valid && (inMEM.writesRD() || inMEM.writesRT())) {
        uint8_t writeReg = inMEM.writesRD() ? inMEM.rd : inMEM.rt;
        if (writeReg == reg && writeReg != 0) {
            setupForwarding(reg, 3, 0); // Forward from MEM stage
            forwardingEvents++;
            return true;
        }
    }

    return false;
}

bool HazardDetection::detectControlHazard(const Instruction& instruction) {
    return instruction.isBranch() || instruction.isJump();
}

void HazardDetection::setupForwarding(uint8_t reg, uint8_t sourceStage, uint32_t value) {
    ForwardingPath path;
    path.enable = true;
    path.sourceStage = sourceStage;
    path.targetStage = 2; // Usually forward to EX stage
    path.reg = reg;
    path.value = value;

    forwardingPaths.push_back(path);
}

uint32_t HazardDetection::getForwardedValue(uint8_t reg, uint32_t defaultValue) {
    for (const auto& path : forwardingPaths) {
        if (path.enable && path.reg == reg) {
            return path.value;
        }
    }
    return defaultValue;
}

void HazardDetection::clearForwarding() {
    forwardingPaths.clear();
}

bool HazardDetection::detectLoadUseHazard(const Instruction& load, const Instruction& use) {
    if (!load.isLoad()) return false;

    uint8_t loadReg = load.rt;

    if (use.usesRS() && use.rs == loadReg) return true;
    if (use.usesRT() && use.rt == loadReg) return true;

    return false;
}

bool HazardDetection::shouldStall(const Instruction& current, 
                                 const Instruction& inEX,
                                 const Instruction& inMEM) {

    // Load-use hazard requires stall
    if (detectLoadUseHazard(inEX, current)) {
        stallsInserted++;
        return true;
    }

    return false;
}

bool HazardDetection::shouldFlush(const Instruction& branch) {
    return branch.isBranch() || branch.isJump();
}

std::string HazardDetection::getStatistics() const {
    std::stringstream ss;
    ss << "Hazard Detection Statistics:\n";
    ss << "Data Hazards: " << dataHazards << "\n";
    ss << "Control Hazards: " << controlHazards << "\n";
    ss << "Structural Hazards: " << structuralHazards << "\n";
    ss << "Forwarding Events: " << forwardingEvents << "\n";
    ss << "Stalls Inserted: " << stallsInserted << "\n";
    return ss.str();
}