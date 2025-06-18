#pragma once
#include <vector>
#include <string>
class Pipeline {
public:
    struct Reg { uint32_t instr,pc; bool valid; } IFID,IDEX,EXMEM,MEMWB;
    Pipeline();
    void reset();
    void advance(bool stall);
    std::string toString() const;
};
