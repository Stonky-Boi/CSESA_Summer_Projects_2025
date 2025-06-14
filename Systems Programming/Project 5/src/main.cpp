#include "mips_simulator.hpp"
#include <iostream>
#include <string>
#include <fstream>

void printUsage(const char* program_name) {
    std::cout << "Usage: " << program_name << " <program_file> [options]\n";
    std::cout << "\nOptions:\n";
    std::cout << "  --step           Enable step-by-step execution\n";
    std::cout << "  --pipeline       Enable 5-stage pipeline simulation\n";
    std::cout << "  --branch-pred    Enable branch prediction\n";
    std::cout << "  --pred-type TYPE Set branch predictor type (static|1bit|2bit)\n";
    std::cout << "  --help           Show this help message\n";
    std::cout << "\nExample:\n";
    std::cout << "  " << program_name << " program.txt --pipeline --branch-pred --pred-type 2bit\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string program_file = argv[1];
    bool step_mode = false;
    bool pipeline_enabled = false;
    bool branch_prediction = false;
    std::string predictor_type = "static";
    
    // Parse command line arguments
    for (int i = 2; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "--step") {
            step_mode = true;
        } else if (arg == "--pipeline") {
            pipeline_enabled = true;
        } else if (arg == "--branch-pred") {
            branch_prediction = true;
        } else if (arg == "--pred-type" && i + 1 < argc) {
            predictor_type = argv[++i];
        } else {
            std::cerr << "Unknown option: " << arg << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }
    
    // Create and configure simulator
    MIPSSimulator simulator;
    simulator.setStepMode(step_mode);
    simulator.enablePipeline(pipeline_enabled);
    simulator.enableBranchPrediction(branch_prediction, predictor_type);
    
    // Load program
    if (!simulator.loadProgram(program_file)) {
        std::cerr << "Error: Could not load program file: " << program_file << std::endl;
        return 1;
    }
    
    std::cout << "MIPS Simulator\n";
    std::cout << "==============\n";
    std::cout << "Program: " << program_file << "\n";
    std::cout << "Step Mode: " << (step_mode ? "Enabled" : "Disabled") << "\n";
    std::cout << "Pipeline: " << (pipeline_enabled ? "Enabled" : "Disabled") << "\n";
    std::cout << "Branch Prediction: " << (branch_prediction ? "Enabled (" + predictor_type + ")" : "Disabled") << "\n";
    std::cout << "\n";
    
    if (step_mode) {
        std::string input;
        int cycle = 0;
        
        while (!simulator.isHalted()) {
            std::cout << "\n--- Cycle " << ++cycle << " ---\n";
            std::cout << simulator.getStateString();
            
            if (pipeline_enabled) {
                std::cout << "\n" << simulator.getPipelineStateString();
            }
            
            std::cout << "\nPress Enter to continue (or 'q' to quit): ";
            std::getline(std::cin, input);
            
            if (input == "q" || input == "quit") {
                break;
            }
            
            if (!simulator.step()) {
                std::cout << "\nSimulation completed or error occurred.\n";
                break;
            }
        }
    } else {
        // Run simulation
        simulator.run();
        
        std::cout << "Simulation completed.\n\n";
        std::cout << "Final State:\n";
        std::cout << simulator.getStateString();
        
        if (pipeline_enabled) {
            std::cout << "\n" << simulator.getPipelineStateString();
        }
    }
    
    if (branch_prediction) {
        std::cout << "\n" << simulator.getBranchPredictionStats();
    }
    
    return 0;
}
