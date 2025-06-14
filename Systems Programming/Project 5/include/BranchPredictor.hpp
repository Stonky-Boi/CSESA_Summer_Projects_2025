#ifndef BRANCH_PREDICTOR_HPP
#define BRANCH_PREDICTOR_HPP

#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include <memory>

enum class PredictorType {
    STATIC_NOT_TAKEN,
    STATIC_TAKEN,
    STATIC_BTFN,      // Backward Taken, Forward Not taken
    BIMODAL_1BIT,
    BIMODAL_2BIT,
    GSHARE,
    LOCAL_HISTORY,
    TOURNAMENT
};

struct BranchStats {
    uint64_t totalBranches;
    uint64_t correctPredictions;
    uint64_t mispredictions;
    double accuracy;

    BranchStats() : totalBranches(0), correctPredictions(0), mispredictions(0), accuracy(0.0) {}
};

// Base class for branch predictors
class BranchPredictor {
protected:
    PredictorType type;
    BranchStats stats;

public:
    BranchPredictor(PredictorType t) : type(t) {}
    virtual ~BranchPredictor() = default;

    virtual bool predict(uint32_t pc, uint32_t targetAddress) = 0;
    virtual void update(uint32_t pc, bool taken, uint32_t targetAddress) = 0;
    virtual void reset() = 0;

    BranchStats getStats() const { return stats; }
    std::string getStatsString() const;
    PredictorType getType() const { return type; }
};

// Static predictors
class StaticPredictor : public BranchPredictor {
private:
    bool prediction;

public:
    StaticPredictor(PredictorType t, bool pred);
    bool predict(uint32_t pc, uint32_t targetAddress) override;
    void update(uint32_t pc, bool taken, uint32_t targetAddress) override;
    void reset() override;
};

// BTFN Predictor
class BTFNPredictor : public BranchPredictor {
public:
    BTFNPredictor();
    bool predict(uint32_t pc, uint32_t targetAddress) override;
    void update(uint32_t pc, bool taken, uint32_t targetAddress) override;
    void reset() override;
};

// Bimodal predictor (1-bit and 2-bit)
class BimodalPredictor : public BranchPredictor {
private:
    std::vector<uint8_t> table;
    uint32_t indexBits;
    uint32_t tableSize;
    bool twoBit;

public:
    BimodalPredictor(PredictorType t, uint32_t bits);
    bool predict(uint32_t pc, uint32_t targetAddress) override;
    void update(uint32_t pc, bool taken, uint32_t targetAddress) override;
    void reset() override;

private:
    uint32_t getIndex(uint32_t pc) const;
};

// Gshare predictor
class GsharePredictor : public BranchPredictor {
private:
    std::vector<uint8_t> table;
    uint32_t globalHistory;
    uint32_t historyBits;
    uint32_t indexBits;
    uint32_t tableSize;

public:
    GsharePredictor(uint32_t histBits, uint32_t idxBits);
    bool predict(uint32_t pc, uint32_t targetAddress) override;
    void update(uint32_t pc, bool taken, uint32_t targetAddress) override;
    void reset() override;

private:
    uint32_t getIndex(uint32_t pc) const;
};

// Local history predictor
class LocalHistoryPredictor : public BranchPredictor {
private:
    std::vector<uint32_t> localHistoryTable;
    std::vector<uint8_t> patternHistoryTable;
    uint32_t localHistoryBits;
    uint32_t patternHistoryBits;
    uint32_t localTableSize;
    uint32_t patternTableSize;

public:
    LocalHistoryPredictor(uint32_t localBits, uint32_t patternBits);
    bool predict(uint32_t pc, uint32_t targetAddress) override;
    void update(uint32_t pc, bool taken, uint32_t targetAddress) override;
    void reset() override;

private:
    uint32_t getLocalIndex(uint32_t pc) const;
    uint32_t getPatternIndex(uint32_t localHistory) const;
};

// Tournament predictor (hybrid)
class TournamentPredictor : public BranchPredictor {
private:
    std::unique_ptr<BranchPredictor> globalPredictor;
    std::unique_ptr<BranchPredictor> localPredictor;
    std::vector<uint8_t> choiceTable;
    uint32_t choiceTableSize;

public:
    TournamentPredictor(uint32_t choiceBits);
    bool predict(uint32_t pc, uint32_t targetAddress) override;
    void update(uint32_t pc, bool taken, uint32_t targetAddress) override;
    void reset() override;

    std::string getDetailedStats() const;

private:
    uint32_t getChoiceIndex(uint32_t pc) const;
};

// Factory for creating predictors
class BranchPredictorFactory {
public:
    static std::unique_ptr<BranchPredictor> create(PredictorType type, 
                                                  uint32_t param1 = 10, 
                                                  uint32_t param2 = 10);
};

#endif // BRANCH_PREDICTOR_HPP