#include <iostream>
#include <iomanip>
#include <cmath>
#include "rccore.h"
#include "vals.h"

// PRIVATE
    
// For the list, keep at most 4 elements,
// and enable removing something from the middle
// No need for pop, as we'll access the list entirely
void RCCore::pushToList(vartype v) {
    latest4.push_back(v);
    while (latest4.size() > 4)
        latest4.pop_front();
    std::cout << "list len from push: " << latest4.size() << std::endl;
}
void RCCore::removeFromList(vartype v) {
    latest4.remove(v);
    std::cout << "list len from remove: " << latest4.size() << std::endl;
}

void RCCore::setBridge(const std::shared_ptr<UIBridgeInterface> & b) {
    ibridge = b;
}

Vals RCCore::calc_group(Vals invals) const {
    // Update one set of values from the previous set
    Vals out {};
    double vtop  = invals.vtop;
    double vbot  = invals.vbot;
    double vmid  = invals.vmid;
    double r1    = invals.r1;
    double r2    = invals.r2;
    double curr  = invals.curr;
    /*      INPUTS
            vtop vbot vmid r1  r2 curr  comment
    0x0f      0    0    1   1   1   1	
    0x17      0    1    0   1   1   1	
    0x1b      0    1    1   0   1   1	under
    0x1d      0    1    1   1   0   1	
    0x1e      0    1    1   1   1   0	
    0x27      1    0    0   1   1   1	
    0x2b      1    0    1   0   1   1	
    0x2d      1    0    1   1   0   1	under
    0x2e      1    0    1   1   1   0  
    0x33      1    1    0   0   1   1	
    0x35      1    1    0   1   0   1	
    0x36      1    1    0   1   1   0	
    0x39      1    1    1   0   0   1	
    0x3a      1    1    1   0   1   0	
    0x3c      1    1    1   1   0   0	*/

    out = invals;
    switch(invals.incode()) {
        case 0x0f: 
            out.vtop = vmid + curr * r1; 
            out.vbot = vmid - curr * r2; 
            out.vtopd = Vals::OUTPUT;
            out.vbotd = Vals::OUTPUT;
            break;
        case 0x17:
            out.vtop = vbot + curr + (r1 + r2);
            out.vmid = vbot + curr * r2;
            out.vtopd = Vals::OUTPUT;
            out.vmidd = Vals::OUTPUT;
            break;
        case 0x1b:
            throw std::invalid_argument( "Error in logic" );
        case 0x1d:
            out.vtop = vmid + curr + r1;
            out.r2   = (vmid - vbot) / curr;
            out.vtopd = Vals::OUTPUT;
            out.vbotd = Vals::OUTPUT;
            break;
        case 0x1e:
            out.curr = (vmid-vbot)/r2;
            out.vtop = vmid + out.curr * r1;
            out.currd = Vals::OUTPUT;
            out.vtopd = Vals::OUTPUT;
            break;
        case 0x27:
            out.vmid = vtop - curr * r1;
            out.vbot = vmid - curr * r2;
            out.vmidd = Vals::OUTPUT;
            out.vbotd = Vals::OUTPUT;
            break;
        case 0x2b:
            out.vbot  = vmid - curr * r2;
            out.r1    = (vtop - vmid) / curr;
            out.vbotd = Vals::OUTPUT;
            out.r1d   = Vals::OUTPUT;
            break;
        case 0x2d:
            throw std::invalid_argument( "Error in logic" );
        case 0x2e:
            out.curr = (vtop - vmid) / r1;
            out.vbot = vmid - out.curr * r2;
            out.currd = Vals::OUTPUT;
            out.vbotd = Vals::OUTPUT;
            break;
        case 0x33:
            out.vmid = vbot + curr * r2;
            out.r1   = (vtop - out.vmid) / curr;
            out.vmidd = Vals::OUTPUT;
            out.r1d   = Vals::OUTPUT;
            break;
        case 0x35:
            out.vmid = vtop - curr * r1;
            out.r2   = (out.vmid - vbot) / curr;
            out.vmidd = Vals::OUTPUT;
            out.r2d   = Vals::OUTPUT;
            break;
        case 0x36:
            out.curr = (vtop - vbot) / (r1 + r2);
            out.vmid = vbot + out.curr * r2;
            out.currd = Vals::OUTPUT;
            out.vmidd = Vals::OUTPUT;
            break;
        case 0x39:
            out.r1 = (vtop - vmid) / curr;
            out.r2 = (vmid - vbot) / curr;
            out.r1d = Vals::OUTPUT;
            out.r2d = Vals::OUTPUT;
            break;
        case 0x3a:
            out.curr = (vmid - vbot) / r2;
            out.r1   = (vtop - vmid) / out.curr;
            out.currd = Vals::OUTPUT;
            out.r1d   = Vals::OUTPUT;
            break;
        case 0x3c:
            out.curr = (vtop - vmid) / r1;
            out.r2   = (vmid - vbot) / out.curr;
            out.currd = Vals::OUTPUT;
            out.r2d   = Vals::OUTPUT;
            break;
        default:
            throw std::invalid_argument( "Error in logic" );

    }
    out.ratio = out.r1 / out.r2;
    out.ratiod = Vals::OUTPUT;
    calc_constraint(out);
    return out;
}

void RCCore::calc_constraint(Vals & vals) const {
    if (vals.numInputs() > 4)
        vals.constraint = Vals::OVER;
    else if (vals.numInputs() < 4)
        vals.constraint = Vals::UNDER;
    else // These two cases represent one loop overdefined and the other underdefined
        if (vals.incode() == 0x1b || vals.incode() == 0x2d)
            vals.constraint = Vals::MIXED;
        else
            vals.constraint = Vals::PROPER;
}

// Public
RCCore::RCCore() :
    inVals (Vals()),
    outVals (Vals())
    {}

bool RCCore::setVTop(double in) {
    inVals.vtop = in;
    inVals.vtopd = Vals::INPUT;
    pushToList(VTOP);
    return update();
}
bool RCCore::setVBot(double in) {
    inVals.vbot = in;
    inVals.vbotd = Vals::INPUT;
    pushToList(VBOT);
    return update();
}
bool RCCore::setVMid(double in) {
    inVals.vmid = in;
    inVals.vmidd = Vals::INPUT;
    pushToList(VMID);
    return update();
}
bool RCCore::setR1(double in) {
    inVals.r1 = in;
    inVals.r1d = Vals::INPUT;
    pushToList(R1);
    return update();
}
bool RCCore::setR2(double in) {
    inVals.r2 = in;
    inVals.r2d = Vals::INPUT;
    pushToList(R2);
    return update();
}
bool RCCore::setCurr(double in) {
    inVals.curr = in;
    inVals.currd = Vals::INPUT;
    pushToList(CURR);
    return update();
}
bool RCCore::setVTop(Vals::dir dir) {
    inVals.vtopd = dir;
    if (dir == Vals::INPUT) pushToList(VTOP);
    else removeFromList(VTOP);
    return update();
}
bool RCCore::setVBot(Vals::dir dir) {
    inVals.vbotd = dir;
    if (dir == Vals::INPUT) pushToList(VBOT);
    else removeFromList(VBOT);
    return update();
}
bool RCCore::setVMid(Vals::dir dir) {
    inVals.vmidd = dir;
    if (dir == Vals::INPUT) pushToList(VMID);
    else removeFromList(VMID);
    return update();
}
bool RCCore::setR1(Vals::dir dir) {
    inVals.r1d = dir;
    if (dir == Vals::INPUT) pushToList(R1);
    else removeFromList(R1);
    return update();
}
bool RCCore::setR2(Vals::dir dir) {
    inVals.r2d = dir;
    if (dir == Vals::INPUT) pushToList(R2);
    else removeFromList(R2);
    return update();
}
bool RCCore::setCurr(Vals::dir dir) {
    inVals.currd = dir;
    if (dir == Vals::INPUT) pushToList(CURR);
    else removeFromList(CURR);
    return update();
}

bool RCCore::update() {
    // Go through all three calculations
    // keep intermediate results

    // Set everything to output, then set to input if in list
    inVals.vtopd = inVals.vbotd = inVals.vmidd = 
    inVals.r1d = inVals.r2d = inVals.currd = inVals.ratiod = Vals::OUTPUT;
    for (auto i = latest4.begin(); i != latest4.end(); i++) {
        switch(*i) {
            case VTOP: inVals.vtopd = Vals::INPUT; break;
            case VBOT: inVals.vbotd = Vals::INPUT; break;
            case VMID: inVals.vmidd = Vals::INPUT; break;
            case R1:   inVals.r1d   = Vals::INPUT; break;
            case R2:   inVals.r2d   = Vals::INPUT; break;
            case CURR: inVals.currd = Vals::INPUT; break;
        }
    }


    calc_constraint(inVals);

   if (inVals.constrainedType() != Vals::PROPER) {
       std::cout << "Improper input constraint -- won't calculate" << std::endl;
       outVals = inVals;
       if (ibridge) ibridge->setState(inVals);
       return false;
   } else {
        outVals = calc_group(inVals);
        if (ibridge) ibridge->setState(outVals);
        return true;
   }
}

bool RCCore::update(Vals in) {
    // Go through all three calculations
    // keep intermediate results
    inVals = in;
    return update();
}
void RCCore::setInput(Vals vals) {
    inVals = vals;
}
Vals RCCore::getInput() const {
    return inVals;
}
Vals RCCore::getOutput() const {
    return outVals;
}











