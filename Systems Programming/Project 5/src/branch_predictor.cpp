#include "branch_predictor.hpp"
#include <sstream>
#include <iomanip>

BranchPredictor::BranchPredictor(PredictorType type) 
    : predictor_type(type) {
    reset();
}

BranchPredictor::~BranchPredictor() {}

bool BranchPredictor::predict(uint32_t pc) {
    stats.total_predictions++;
    
    switch (predictor_type) {
        case STATIC_NOT_TAKEN:
            return false;
            
        case STATIC_TAKEN:
            return true;
            
        case DYNAMIC_1BIT:
            // 1-bit predictor: 0 = not taken, 1 = taken
            if (branch_history_table.find(pc) == branch_history_table.end()) {
                branch_history_table[pc] = 0; // Initialize as not taken
            }
            return branch_history_table[pc] == 1;
            
        case DYNAMIC_2BIT:
            // 2-bit predictor with 4 states
            if (branch_history_table.find(pc) == branch_history_table.end()) {
                branch_history_table[pc] = WEAKLY_NOT_TAKEN; // Initialize as weakly not taken
            }
            return (branch_history_table[pc] == WEAKLY_TAKEN || 
                    branch_history_table[pc] == STRONGLY_TAKEN);
            
        default:
            return false;
    }
}

void BranchPredictor::update(uint32_t pc, bool actual_outcome) {
    // Check if prediction was correct
    bool predicted_outcome = false;
    
    switch (predictor_type) {
        case STATIC_NOT_TAKEN:
            predicted_outcome = false;
            break;
            
        case STATIC_TAKEN:
            predicted_outcome = true;
            break;
            
        case DYNAMIC_1BIT:
            if (branch_history_table.find(pc) != branch_history_table.end()) {
                predicted_outcome = (branch_history_table[pc] == 1);
            }
            // Update 1-bit predictor
            branch_history_table[pc] = actual_outcome ? 1 : 0;
            break;
            
        case DYNAMIC_2BIT:
            if (branch_history_table.find(pc) != branch_history_table.end()) {
                uint8_t state = branch_history_table[pc];
                predicted_outcome = (state == WEAKLY_TAKEN || state == STRONGLY_TAKEN);
                
                // Update 2-bit predictor state machine
                if (actual_outcome) {
                    // Branch was taken
                    switch (state) {
                        case STRONGLY_NOT_TAKEN:
                            branch_history_table[pc] = WEAKLY_NOT_TAKEN;
                            break;
                        case WEAKLY_NOT_TAKEN:
                            branch_history_table[pc] = WEAKLY_TAKEN;
                            break;
                        case WEAKLY_TAKEN:
                            branch_history_table[pc] = STRONGLY_TAKEN;
                            break;
                        case STRONGLY_TAKEN:
                            // Stay in strongly taken
                            break;
                    }
                } else {
                    // Branch was not taken
                    switch (state) {
                        case STRONGLY_NOT_TAKEN:
                            // Stay in strongly not taken
                            break;
                        case WEAKLY_NOT_TAKEN:
                            branch_history_table[pc] = STRONGLY_NOT_TAKEN;
                            break;
                        case WEAKLY_TAKEN:
                            branch_history_table[pc] = WEAKLY_NOT_TAKEN;
                            break;
                        case STRONGLY_TAKEN:
                            branch_history_table[pc] = WEAKLY_TAKEN;
                            break;
                    }
                }
            } else {
                // Initialize entry
                branch_history_table[pc] = actual_outcome ? WEAKLY_TAKEN : WEAKLY_NOT_TAKEN;
            }
            break;
    }
    
    // Update statistics
    if (predicted_outcome == actual_outcome) {
        stats.correct_predictions++;
    } else {
        stats.incorrect_predictions++;
    }
    
    // Update accuracy
    if (stats.total_predictions > 0) {
        stats.accuracy = (double)stats.correct_predictions / stats.total_predictions * 100.0;
    }
}

void BranchPredictor::reset() {
    branch_history_table.clear();
    stats.total_predictions = 0;
    stats.correct_predictions = 0;
    stats.incorrect_predictions = 0;
    stats.accuracy = 0.0;
}

BranchPredictor::PredictionStats BranchPredictor::getStats() const {
    return stats;
}

std::string BranchPredictor::getStatsString() const {
    std::ostringstream oss;
    
    oss << "Branch Prediction Statistics:\n";
    oss << "============================\n";
    
    const char* type_names[] = {
        "Static Not Taken",
        "Static Taken", 
        "Dynamic 1-bit",
        "Dynamic 2-bit"
    };
    
    oss << "Predictor Type: " << type_names[predictor_type] << "\n";
    oss << "Total Predictions: " << stats.total_predictions << "\n";
    oss << "Correct Predictions: " << stats.correct_predictions << "\n";
    oss << "Incorrect Predictions: " << stats.incorrect_predictions << "\n";
    oss << "Accuracy: " << std::fixed << std::setprecision(2) << stats.accuracy << "%\n";
    
    if (predictor_type == DYNAMIC_1BIT || predictor_type == DYNAMIC_2BIT) {
        oss << "\nBranch History Table Entries: " << branch_history_table.size() << "\n";
        
        if (!branch_history_table.empty()) {
            oss << "Sample Entries:\n";
            int count = 0;
            for (const auto& entry : branch_history_table) {
                if (count >= 5) break; // Show only first 5 entries
                oss << "  PC: 0x" << std::hex << std::setw(8) << std::setfill('0') 
                    << entry.first << " -> State: " << std::dec << (int)entry.second << "\n";
                count++;
            }
        }
    }
    
    return oss.str();
}

void BranchPredictor::setPredictorType(PredictorType type) {
    predictor_type = type;
    reset();
}
