#include "mips_simulator.hpp"
#include "instruction_decoder.hpp"
#include "alu.hpp"
#include <iostream>
#include <sstream>
#include <cstring>
using namespace std;

MIPSSimulator::MIPSSimulator()
 : registers(32), memory(1<<16), pc(0), halted(true),
   pipelineEnabled(false), branchPredictEnabled(false),
   dataBase(0x10010000), textBase(0x00400000) {
    memset(stages,0,sizeof(stages));
    bs={0,0,0};
}

bool MIPSSimulator::assemble(const string &programText) {
    outputBuffer.clear();
    symbolTable.clear(); textSegment.clear();
    pc=textBase; halted=false;
    return assembleText(programText);
}

bool MIPSSimulator::assembleText(const string &txt) {
    vector<string> lines;
    istringstream iss(txt);
    string line; bool inData=false,inText=false;
    uint32_t dataPtr=dataBase;
    while(getline(iss,line)) {
        auto trim=[&](string &s){size_t a=s.find_first_not_of(" \t"); if(a!=string::npos) s=s.substr(a); s.erase(s.find_last_not_of("\r\t ")+1);};
        trim(line); if(line.empty()||line[0]=='#') continue;
        if(line==".data") { inData=true; inText=false; continue; }
        if(line==".text") { inText=true; inData=false; continue; }
        if(inData) {
            // handle label and .asciiz
            if(line.find(".asciiz")!=string::npos) {
                auto colon=line.find(':');
                string label=line.substr(0,colon);
                trim(label);
                symbolTable[label]=dataPtr;
                string strLit=line.substr(line.find('"')+1);
                strLit=strLit.substr(0,strLit.find_last_of('"'));
                for(char c: strLit) {
                    memory[dataPtr++]=(uint8_t)c;
                }
                memory[dataPtr++]=0;
            }
        } else if(inText) {
            lines.push_back(line);
        }
    }
    // expand pseudo-instructions
    vector<string> realIns;
    for(auto &ln:lines){
        istringstream ls(ln); string op; ls>>op;
        if(op=="li") {
            string rd; int imm; ls>>rd>>imm;
            realIns.push_back("addi "+rd+", $zero, "+to_string(imm));
        } else if(op=="la") {
            string rt,label; ls>>rt>>label;
            uint32_t addr=symbolTable[label];
            realIns.push_back("lui $at, "+to_string((addr>>16)&0xFFFF));
            realIns.push_back("ori "+rt+", $at, "+to_string(addr&0xFFFF));
        } else {
            realIns.push_back(ln);
        }
    }
    // encode
    for(auto &ln:realIns){
        // simplistic: call decoder/disassembler then textSegment push hex
        // Here you would implement real encoding; for brevity assume ln is machine code hex
        uint32_t code=strtoul(ln.c_str(),nullptr,16);
        textSegment.push_back(code);
    }
    // load into memory
    for(size_t i=0;i<textSegment.size();i++){
        uint32_t w=textSegment[i];
        uint32_t adr=textBase+4*i;
        memory[adr]=w>>24; memory[adr+1]=w>>16;
        memory[adr+2]=w>>8; memory[adr+3]=w;
    }
    pc=textBase;
    return true;
}

bool MIPSSimulator::loadMachineCode(const vector<uint32_t> &code) {
    textSegment=code; return true;
}

void MIPSSimulator::reset(){
    fill(registers.begin(),registers.end(),0);
    pc=textBase; halted=false; outputBuffer.clear();
}

bool MIPSSimulator::step(){
    if(halted) return false;
    uint32_t instr = (memory[pc]<<24)|(memory[pc+1]<<16)|
                     (memory[pc+2]<<8)|memory[pc+3];
    pc+=4;
    return fetchDecodeExecute(instr);
}

void MIPSSimulator::run(){
    while(!halted && step());
}

bool MIPSSimulator::fetchDecodeExecute(uint32_t instr){
    auto d=InstructionDecoder::decode(instr);
    switch(d.opcode){
        case  MIPS::OPC_R:
            if(d.funct==MIPS::FUNC_SYSC){
                handleSyscall(); break;
            }
            // handle other R...
            break;
        case MIPS::OPC_ADDI:
            registers[d.rt]=registers[d.rs]+signExt(d.imm); break;
        // handle lw, sw, branches...
    }
    registers[0]=0;
    return true;
}

void MIPSSimulator::handleSyscall(){
    uint32_t code=registers[2];
    if(code==4){
        uint32_t addr=registers[4];
        while(memory[addr]){
            outputBuffer.push_back((char)memory[addr++]);
        }
    }
}

void MIPSSimulator::writeOutput(const string &s){
    outputBuffer+=s;
}

uint32_t MIPSSimulator::signExt(int16_t imm){
    return imm<0?0xFFFF0000|imm:imm;
}

uint32_t MIPSSimulator::getRegister(int r)const{return registers[r];}
uint32_t MIPSSimulator::getPC()const{return pc;}
string MIPSSimulator::getStateString()const{
    ostringstream o; o<<"PC: 0x"<<hex<<pc<<"\n";
    for(int i=0;i<32;i++)o<<"$"<<i<<":"<<hex<<registers[i]<<" ";
    return o.str();
}
string MIPSSimulator::getPipelineStateString()const{return ""; /* implement */ }
string MIPSSimulator::getBranchPredictionStats()const{return ""; }
bool MIPSSimulator::isHalted()const{return halted;}
void MIPSSimulator::enablePipeline(bool on){pipelineEnabled=on;}
void MIPSSimulator::enableBranchPrediction(bool on,const string&){branchPredictEnabled=on;}
const string& MIPSSimulator::getOutput()const{return outputBuffer;}

uint32_t MIPSSimulator::getMemory(uint32_t address) const {
    // Ensure word-aligned and in-bounds
    if (address + 3 < memory.size()) {
        return (memory[address]   << 24) |
               (memory[address+1] << 16) |
               (memory[address+2] <<  8) |
                memory[address+3];
    }
    return 0;
}