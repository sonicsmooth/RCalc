#ifndef VALSTRUCT_H
#define VALSTRUCT_H
#include <string>
#include "rctypes.h"

class Vals {
    public:
        double vtop, vbot, vmid, r1, r2, curr, ratio;
        dir vtopd, vbotd, vmidd, r1d, r2d, currd, ratiod;
        vartype disable; // which one, if any, to disable
        
        Vals();
        int numInputs() const;
        bool isDefined() const;
        std::string str() const;
        int incode() const;
        bool operator==(const Vals &);
};

bool isin(dir);
bool isin(int, vartype); // parses int as incode instead of just checking dir with ==
bool isout(dir);
bool isdef(dir);
std::string vstr(dir);    
std::string vstr(constype);    


#endif
