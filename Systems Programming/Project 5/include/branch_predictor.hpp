#pragma once
#include <cstdint>
#include <map>
class BranchPredictor {
public:
    enum Type { STATIC_NT,STATIC_T,BI2 };
    BranchPredictor(Type t=BI2);
    bool predict(uint32_t pc);
    void update(uint32_t pc,bool taken);
    std::string stats() const;
private:
    Type type;
    std::map<uint32_t,uint8_t> table;
    int total,correct,incorrect;
};
