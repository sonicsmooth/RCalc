#include <iomanip>
#include <string>
#include <sstream>
#include "vals.h"

int Vals::numInputs() const {
    return isin(vtopd) + isin(vbotd) + isin(vmidd) +
           isin(r1d)   + isin(r2d)   + isin(currd);
}
bool Vals::isDefined() const {
    return isdef(vtopd) && isdef(vbotd) && isdef(vmidd) &&
           isdef(r1d)   && isdef(r2d)   && isdef(currd) && isdef(ratiod);
}
Vals::constraint Vals::constrainedType() const {
    return constype;
}
std::string Vals::str() const {
    int nw = 10;
    int kw = 10;
    int vw = 15;
    std::stringstream sstr;

    sstr << std::boolalpha;
    sstr << std::fixed;
    sstr << std::setprecision(5);
    sstr.width(nw); sstr << std::left << "variable";
    sstr.width(kw); sstr << std::left << "direction";
    sstr.width(vw); sstr << std::left << "value" << std::endl;
    sstr << "---------------------------" << std::endl;
    sstr.width(nw); sstr << std::left << "Vtop";
    sstr.width(kw); sstr << std::left << vstr(vtopd);
    sstr.width(vw); sstr << std::left << vtop << std::endl;
    sstr.width(nw); sstr << std::left << "Vbot";
    sstr.width(kw); sstr << std::left << vstr(vbotd);
    sstr.width(vw); sstr << std::left << vbot << std::endl;
    sstr.width(nw); sstr << std::left << "Vmid";
    sstr.width(kw); sstr << std::left << vstr(vmidd);
    sstr.width(vw); sstr << std::left << vmid << std::endl;
    sstr.width(nw); sstr << std::left << "R1";
    sstr.width(kw); sstr << std::left << vstr(r1d);
    sstr.width(vw); sstr << std::left << r1 << std::endl;
    sstr.width(nw); sstr << std::left << "R2";
    sstr.width(kw); sstr << std::left << vstr(r2d);
    sstr.width(vw); sstr << std::left << r2 << std::endl;
    sstr.width(nw); sstr << std::left << "Curr";
    sstr.width(kw); sstr << std::left << vstr(currd);
    sstr.width(vw); sstr << std::left << curr << std::endl;
    sstr.width(nw); sstr << std::left << "Ratio";
    sstr.width(kw); sstr << std::left << vstr(ratiod);
    sstr.width(vw); sstr << std::left << ratio << std::endl;
    sstr << "Constrained? " << vstr(constrainedType()) << std::endl;
    sstr << "Code: 0x" <<std::hex << incode() << std::endl;
    return sstr.str();
}
 
int Vals::incode() const {
    return isin(vtopd) << 5 |
           isin(vbotd) << 4 |
           isin(vmidd) << 3 |
           isin(r1d)   << 2 |
           isin(r2d)   << 1 |
           isin(currd) << 0;
}

bool isin(Vals::dir d) {return d == Vals::INPUT;};
bool isout(Vals::dir d) {return d == Vals::OUTPUT;};
bool isdef(Vals::dir d) {return d != Vals::UNDEFINED;};
std::string vstr(Vals::dir d) {
    switch (d) {
        case Vals::UNDEFINED: return "undefined";
        case Vals::INPUT: return "input";
        case Vals::OUTPUT: return "output";
    }
}std::string vstr(Vals::constraint c) {
    switch (c) {
        case Vals::UNDER: return "under constrained";
        case Vals::PROPER: return "properly constrained";
        case Vals::OVER: return "over constrained";
        case Vals::MIXED: return "mixed constrained";
    }
}
