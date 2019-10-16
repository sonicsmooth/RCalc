#ifndef VALSTRUCT_H
#define VALSTRUCT_H
#include <string>


class Vals {
    public:
        typedef enum {UNDEFINED, INPUT, OUTPUT} dir;
        typedef enum {UNDER, OVER, PROPER, MIXED} constype;
        double vtop, vbot, vmid, r1, r2, curr, ratio;
        dir vtopd, vbotd, vmidd, r1d, r2d, currd, ratiod;
        //constype _constraint;
        
        Vals();
        int numInputs() const;
        bool isDefined() const;
        //constype constraint() const;
        std::string str() const;
        int incode() const;
};

bool isin(Vals::dir);
bool isout(Vals::dir);
bool isdef(Vals::dir);
std::string vstr(Vals::dir);    
std::string vstr(Vals::constype);    


#endif