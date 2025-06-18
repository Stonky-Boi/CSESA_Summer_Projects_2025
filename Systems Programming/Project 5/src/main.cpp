#include "mips_simulator.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

void usage(){
    cout<<"Usage: mips_simulator <file> [--pipeline] [--branch-pred]\n";
}

int main(int argc,char**argv){
    if(argc<2){usage(); return 1;}
    string file=argv[1];
    bool pipe=false, bp=false;
    for(int i=2;i<argc;i++){
        if(string(argv[i])=="--pipeline") pipe=true;
        if(string(argv[i])=="--branch-pred") bp=true;
    }
    ifstream in(file); if(!in){cerr<<"Cannot open "<<file<<"\n"; return 1;}
    string line,prog;
    while(getline(in,line)) prog+=line+"\n";
    MIPSSimulator sim;
    sim.enablePipeline(pipe);
    sim.enableBranchPrediction(bp);
    if(!sim.assemble(prog)){cerr<<"Assemble error\n"; return 1;}
    sim.run();
    cout<<sim.getOutput();
    return 0;
}
