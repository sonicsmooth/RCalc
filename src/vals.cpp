#include <iomanip>
#include <string>
#include <sstream>
#include <cassert>

#include "rctypes.h"
#include "vals.h"

Vals::Vals() : vtop(0.0), vbot(0.0), vmid(0.0), r1(0.0), r2(0.0), curr(0.0), ratio(0.0),
               vtopd(UNDEFINED), vbotd(UNDEFINED), vmidd(UNDEFINED), 
               r1d(UNDEFINED), r2d(UNDEFINED), currd(UNDEFINED), 
               ratiod(UNDEFINED) {}

int Vals::numInputs() const {
    return isin(vtopd) + isin(vbotd) + isin(vmidd) +
           isin(r1d)   + isin(r2d)   + isin(currd);
}
bool Vals::isDefined() const {
    return isdef(vtopd) && isdef(vbotd) && isdef(vmidd) &&
           isdef(r1d)   && isdef(r2d)   && isdef(currd) && isdef(ratiod);
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

bool isin(dir d) {return d == INPUT;};
bool isout(dir d) {return d == OUTPUT;};
bool isdef(dir d) {return d != UNDEFINED;};
std::string vstr(dir d) {
    assert (d >= UNDEFINED && d <= OUTPUT);
    switch (d) {
        case UNDEFINED: return "UNDEFINED";
        case INPUT: return "input";
        case OUTPUT: return "output";
        default: return "logic error";
    }
}
std::string vstr(constype c) {
    assert (c >= UNDER && c <= MIXED);
    switch (c) {
        case UNDER: return "under constrained";
        case PROPER: return "properly constrained";
        case OVER: return "over constrained";
        case MIXED: return "mixed constrained";
        default: return "logic error";
    }
}
