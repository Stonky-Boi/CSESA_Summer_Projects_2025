#include "utils/statistics_tracker.hpp"
#include <iostream>
#include <iomanip>

StatisticsTracker::StatisticsTracker()
{
    total_cycles = 0;
    total_instructions = 0;
    pipeline_stalls = 0;
    pipeline_flushes = 0;
    branches_executed = 0;
    branches_taken = 0;
    branches_mispredicted = 0;
}

void StatisticsTracker::increment_total_cycles() { total_cycles++; }
uint64_t StatisticsTracker::get_total_cycles() const { return total_cycles; }

void StatisticsTracker::increment_total_instructions() { total_instructions++; }
void StatisticsTracker::increment_pipeline_stalls() { pipeline_stalls++; }
void StatisticsTracker::increment_pipeline_flushes() { pipeline_flushes++; }
void StatisticsTracker::increment_branches_executed() { branches_executed++; }
void StatisticsTracker::increment_branches_taken() { branches_taken++; }
void StatisticsTracker::increment_branches_mispredicted() { branches_mispredicted++; }

void StatisticsTracker::print_statistics_report() const
{
    double cycles_per_instruction = 0.0;
    if (total_instructions > 0)
        cycles_per_instruction = static_cast<double>(total_cycles) / static_cast<double>(total_instructions);

    double prediction_accuracy = 0.0;
    if (branches_executed > 0)
    {
        uint64_t correct_predictions = branches_executed - branches_mispredicted;
        prediction_accuracy = (static_cast<double>(correct_predictions) / static_cast<double>(branches_executed)) * 100.0;
    }

    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "         EMBER EXECUTION REPORT         " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Total Instructions : " << total_instructions << std::endl;
    std::cout << "Total Cycles       : " << total_cycles << std::endl;
    std::cout << "Overall CPI        : " << std::fixed << std::setprecision(3) << cycles_per_instruction << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Load-Use Stalls    : " << pipeline_stalls << std::endl;
    std::cout << "Control Flushes    : " << pipeline_flushes << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Branches Executed  : " << branches_executed << std::endl;
    std::cout << "Branches Taken     : " << branches_taken << std::endl;
    std::cout << "Mispredictions     : " << branches_mispredicted << std::endl;
    std::cout << "Predictor Accuracy : " << std::fixed << std::setprecision(2) << prediction_accuracy << "%" << std::endl;
    std::cout << "========================================" << std::endl;
}