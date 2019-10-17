#ifndef RCCORE_H
#define RCCORE_H

#include <queue>
#include <list>
#include "uibridgeinterface.h"
#include "vals.h"



class RCCore
{
public:
    typedef enum {NONE, VTOP, VBOT, VMID, R1, R2, CURR} vartype;

private:
    Vals inVals; // input from UI
    Vals outVals; // second set of calculations;
    std::shared_ptr<UIBridgeInterface> ibridge;
    std::list<vartype> latest4; // list of 4 inputs

    void pushToList(vartype);
    void removeFromList(vartype);
    Vals swapInputs(Vals) const;
    Vals clip(Vals, int) const;
    Vals calc_group(vartype, Vals) const;
    Vals::constype constraint(Vals) const;
    bool _update(vartype);

public:
    RCCore();
    bool setVTop(Vals::dir);
    bool setVBot(Vals::dir);
    bool setVMid(Vals::dir);
    bool setR1(Vals::dir);
    bool setR2(Vals::dir);
    bool setCurr(Vals::dir);
    bool update(vartype, double);
    bool update();     
    void setInput(Vals);
    Vals getInput() const;
    Vals getOutput() const;
    bool inputConstrainedType() const;
    

    void setBridge(std::shared_ptr<UIBridgeInterface> const &);
};

std::string cstr(RCCore::vartype);

#endif // RCCORE_H
