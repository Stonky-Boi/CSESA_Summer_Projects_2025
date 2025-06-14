#include "BranchPredictor.hpp"
#include <sstream>
#include <iomanip>

// Base BranchPredictor implementation
std::string BranchPredictor::getStatsString() const {
    std::stringstream ss;
    ss << "Branch Predictor Statistics:\n";
    ss << "Type: " << static_cast<int>(type) << "\n";
    ss << "Total Branches: " << stats.totalBranches << "\n";
    ss << "Correct Predictions: " << stats.correctPredictions << "\n";
    ss << "Mispredictions: " << stats.mispredictions << "\n";
    ss << "Accuracy: " << std::fixed << std::setprecision(2) << (stats.accuracy * 100) << "%\n";
    return ss.str();
}

// StaticPredictor implementation
StaticPredictor::StaticPredictor(PredictorType t, bool pred) 
    : BranchPredictor(t), prediction(pred) {}

bool StaticPredictor::predict(uint32_t pc, uint32_t targetAddress) {
    return prediction;
}

void StaticPredictor::update(uint32_t pc, bool taken, uint32_t targetAddress) {
    stats.totalBranches++;
    if (predict(pc, targetAddress) == taken) {
        stats.correctPredictions++;
    } else {
        stats.mispredictions++;
    }
    stats.accuracy = (double)stats.correctPredictions / stats.totalBranches;
}

void StaticPredictor::reset() {
    stats = BranchStats();
}

// BTFNPredictor implementation
BTFNPredictor::BTFNPredictor() : BranchPredictor(PredictorType::STATIC_BTFN) {}

bool BTFNPredictor::predict(uint32_t pc, uint32_t targetAddress) {
    // Backward taken, forward not taken
    return targetAddress < pc;
}

void BTFNPredictor::update(uint32_t pc, bool taken, uint32_t targetAddress) {
    stats.totalBranches++;
    if (predict(pc, targetAddress) == taken) {
        stats.correctPredictions++;
    } else {
        stats.mispredictions++;
    }
    stats.accuracy = (double)stats.correctPredictions / stats.totalBranches;
}

void BTFNPredictor::reset() {
    stats = BranchStats();
}

// BimodalPredictor implementation
BimodalPredictor::BimodalPredictor(PredictorType t, uint32_t bits) 
    : BranchPredictor(t), indexBits(bits), twoBit(t == PredictorType::BIMODAL_2BIT) {
    tableSize = 1 << indexBits;
    table.resize(tableSize, twoBit ? 1 : 0); // Initialize to weakly not taken or not taken
}

bool BimodalPredictor::predict(uint32_t pc, uint32_t targetAddress) {
    uint32_t index = getIndex(pc);
    if (twoBit) {
        return table[index] >= 2; // 2 or 3 means taken
    } else {
        return table[index] == 1; // 1 means taken
    }
}

void BimodalPredictor::update(uint32_t pc, bool taken, uint32_t targetAddress) {
    uint32_t index = getIndex(pc);

    stats.totalBranches++;
    if (predict(pc, targetAddress) == taken) {
        stats.correctPredictions++;
    } else {
        stats.mispredictions++;
    }
    stats.accuracy = (double)stats.correctPredictions / stats.totalBranches;

    // Update predictor state
    if (twoBit) {
        if (taken && table[index] < 3) {
            table[index]++;
        } else if (!taken && table[index] > 0) {
            table[index]--;
        }
    } else {
        table[index] = taken ? 1 : 0;
    }
}

void BimodalPredictor::reset() {
    stats = BranchStats();
    std::fill(table.begin(), table.end(), twoBit ? 1 : 0);
}

uint32_t BimodalPredictor::getIndex(uint32_t pc) const {
    return (pc >> 2) & ((1 << indexBits) - 1);
}

// GsharePredictor implementation
GsharePredictor::GsharePredictor(uint32_t histBits, uint32_t idxBits) 
    : BranchPredictor(PredictorType::GSHARE), globalHistory(0), 
      historyBits(histBits), indexBits(idxBits) {
    tableSize = 1 << indexBits;
    table.resize(tableSize, 1); // Initialize to weakly not taken
}

bool GsharePredictor::predict(uint32_t pc, uint32_t targetAddress) {
    uint32_t index = getIndex(pc);
    return table[index] >= 2; // 2 or 3 means taken
}

void GsharePredictor::update(uint32_t pc, bool taken, uint32_t targetAddress) {
    uint32_t index = getIndex(pc);

    stats.totalBranches++;
    if (predict(pc, targetAddress) == taken) {
        stats.correctPredictions++;
    } else {
        stats.mispredictions++;
    }
    stats.accuracy = (double)stats.correctPredictions / stats.totalBranches;

    // Update predictor state
    if (taken && table[index] < 3) {
        table[index]++;
    } else if (!taken && table[index] > 0) {
        table[index]--;
    }

    // Update global history
    globalHistory = ((globalHistory << 1) | (taken ? 1 : 0)) & ((1 << historyBits) - 1);
}

void GsharePredictor::reset() {
    stats = BranchStats();
    globalHistory = 0;
    std::fill(table.begin(), table.end(), 1);
}

uint32_t GsharePredictor::getIndex(uint32_t pc) const {
    uint32_t pcBits = (pc >> 2) & ((1 << indexBits) - 1);
    uint32_t historyMask = (1 << std::min(historyBits, indexBits)) - 1;
    return pcBits ^ (globalHistory & historyMask);
}

// LocalHistoryPredictor implementation
LocalHistoryPredictor::LocalHistoryPredictor(uint32_t localBits, uint32_t patternBits) 
    : BranchPredictor(PredictorType::LOCAL_HISTORY), 
      localHistoryBits(localBits), patternHistoryBits(patternBits) {
    localTableSize = 1 << localHistoryBits;
    patternTableSize = 1 << patternHistoryBits;
    localHistoryTable.resize(localTableSize, 0);
    patternHistoryTable.resize(patternTableSize, 1); // Initialize to weakly not taken
}

bool LocalHistoryPredictor::predict(uint32_t pc, uint32_t targetAddress) {
    uint32_t localIndex = getLocalIndex(pc);
    uint32_t localHistory = localHistoryTable[localIndex];
    uint32_t patternIndex = getPatternIndex(localHistory);
    return patternHistoryTable[patternIndex] >= 2;
}

void LocalHistoryPredictor::update(uint32_t pc, bool taken, uint32_t targetAddress) {
    uint32_t localIndex = getLocalIndex(pc);
    uint32_t localHistory = localHistoryTable[localIndex];
    uint32_t patternIndex = getPatternIndex(localHistory);

    stats.totalBranches++;
    if (predict(pc, targetAddress) == taken) {
        stats.correctPredictions++;
    } else {
        stats.mispredictions++;
    }
    stats.accuracy = (double)stats.correctPredictions / stats.totalBranches;

    // Update pattern history table
    if (taken && patternHistoryTable[patternIndex] < 3) {
        patternHistoryTable[patternIndex]++;
    } else if (!taken && patternHistoryTable[patternIndex] > 0) {
        patternHistoryTable[patternIndex]--;
    }

    // Update local history
    localHistoryTable[localIndex] = ((localHistory << 1) | (taken ? 1 : 0)) & 
                                   ((1 << patternHistoryBits) - 1);
}

void LocalHistoryPredictor::reset() {
    stats = BranchStats();
    std::fill(localHistoryTable.begin(), localHistoryTable.end(), 0);
    std::fill(patternHistoryTable.begin(), patternHistoryTable.end(), 1);
}

uint32_t LocalHistoryPredictor::getLocalIndex(uint32_t pc) const {
    return (pc >> 2) & ((1 << localHistoryBits) - 1);
}

uint32_t LocalHistoryPredictor::getPatternIndex(uint32_t localHistory) const {
    return localHistory & ((1 << patternHistoryBits) - 1);
}

// TournamentPredictor implementation
TournamentPredictor::TournamentPredictor(uint32_t choiceBits) 
    : BranchPredictor(PredictorType::TOURNAMENT), choiceTableSize(1 << choiceBits) {
    globalPredictor = std::make_unique<GsharePredictor>(12, 12);
    localPredictor = std::make_unique<LocalHistoryPredictor>(10, 10);
    choiceTable.resize(choiceTableSize, 1); // Initialize to weakly prefer global
}

bool TournamentPredictor::predict(uint32_t pc, uint32_t targetAddress) {
    uint32_t choiceIndex = getChoiceIndex(pc);

    if (choiceTable[choiceIndex] >= 2) {
        // Use global predictor
        return globalPredictor->predict(pc, targetAddress);
    } else {
        // Use local predictor
        return localPredictor->predict(pc, targetAddress);
    }
}

void TournamentPredictor::update(uint32_t pc, bool taken, uint32_t targetAddress) {
    uint32_t choiceIndex = getChoiceIndex(pc);

    bool globalPrediction = globalPredictor->predict(pc, targetAddress);
    bool localPrediction = localPredictor->predict(pc, targetAddress);
    bool chosenPrediction = predict(pc, targetAddress);

    stats.totalBranches++;
    if (chosenPrediction == taken) {
        stats.correctPredictions++;
    } else {
        stats.mispredictions++;
    }
    stats.accuracy = (double)stats.correctPredictions / stats.totalBranches;

    // Update choice table
    if (globalPrediction == taken && localPrediction != taken) {
        // Global was right, local was wrong - prefer global
        if (choiceTable[choiceIndex] < 3) choiceTable[choiceIndex]++;
    } else if (localPrediction == taken && globalPrediction != taken) {
        // Local was right, global was wrong - prefer local
        if (choiceTable[choiceIndex] > 0) choiceTable[choiceIndex]--;
    }

    // Update both predictors
    globalPredictor->update(pc, taken, targetAddress);
    localPredictor->update(pc, taken, targetAddress);
}

void TournamentPredictor::reset() {
    stats = BranchStats();
    globalPredictor->reset();
    localPredictor->reset();
    std::fill(choiceTable.begin(), choiceTable.end(), 1);
}

uint32_t TournamentPredictor::getChoiceIndex(uint32_t pc) const {
    return (pc >> 2) & (choiceTableSize - 1);
}

std::string TournamentPredictor::getDetailedStats() const {
    std::stringstream ss;
    ss << getStatsString();
    ss << "\nGlobal Predictor:\n" << globalPredictor->getStatsString();
    ss << "\nLocal Predictor:\n" << localPredictor->getStatsString();
    return ss.str();
}

// BranchPredictorFactory implementation
std::unique_ptr<BranchPredictor> BranchPredictorFactory::create(PredictorType type, 
                                                               uint32_t param1, 
                                                               uint32_t param2) {
    switch (type) {
        case PredictorType::STATIC_NOT_TAKEN:
            return std::make_unique<StaticPredictor>(type, false);
        case PredictorType::STATIC_TAKEN:
            return std::make_unique<StaticPredictor>(type, true);
        case PredictorType::STATIC_BTFN:
            return std::make_unique<BTFNPredictor>();
        case PredictorType::BIMODAL_1BIT:
            return std::make_unique<BimodalPredictor>(type, param1);
        case PredictorType::BIMODAL_2BIT:
            return std::make_unique<BimodalPredictor>(type, param1);
        case PredictorType::GSHARE:
            return std::make_unique<GsharePredictor>(param1, param2);
        case PredictorType::LOCAL_HISTORY:
            return std::make_unique<LocalHistoryPredictor>(param1, param2);
        case PredictorType::TOURNAMENT:
            return std::make_unique<TournamentPredictor>(param1);
        default:
            return std::make_unique<StaticPredictor>(PredictorType::STATIC_NOT_TAKEN, false);
    }
}