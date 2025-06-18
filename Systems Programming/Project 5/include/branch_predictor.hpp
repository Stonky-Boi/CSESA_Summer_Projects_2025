#pragma once
#include <cstdint>
#include <map>
#include <string>
#include <vector>

class BranchPredictor {
public:
    enum PredictorType {
        STATIC_NOT_TAKEN,
        STATIC_TAKEN,
        DYNAMIC_1BIT,
        DYNAMIC_2BIT
    };
    
    struct PredictionStats {
        int total_predictions;
        int correct_predictions;
        int incorrect_predictions;
        double accuracy;
    };
    
    BranchPredictor(PredictorType type = STATIC_NOT_TAKEN);
    ~BranchPredictor();
    
    bool predict(uint32_t pc);
    void update(uint32_t pc, bool actual_outcome);
    void reset();
    
    PredictionStats getStats() const;
    std::string getStatsString() const;
    void setPredictorType(PredictorType type);
    
private:
    PredictorType predictor_type;
    std::map<uint32_t, uint8_t> branch_history_table;
    PredictionStats stats;
    
    // 2-bit predictor states
    enum State2Bit {
        STRONGLY_NOT_TAKEN = 0,
        WEAKLY_NOT_TAKEN = 1,
        WEAKLY_TAKEN = 2,
        STRONGLY_TAKEN = 3
    };
};
