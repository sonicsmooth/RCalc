#include <iostream>
#include <iomanip>
#include <cmath>
#include "rccore.h"
#include "vals.h"

// PRIVATE
    

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
    vals0 (Vals()),
    vals1 (Vals()),
    vals2 (Vals())
    {}

bool RCCore::setVTop(double in) {
    vals0.vtop = in;
    vals0.vtopd = Vals::INPUT;
    return update();
}
bool RCCore::setVBot(double in) {
    vals0.vbot = in;
    vals0.vbotd = Vals::INPUT;
    return update();
}
bool RCCore::setVMid(double in) {
    vals0.vmid = in;
    vals0.vmidd = Vals::INPUT;
    return update();
}
bool RCCore::setR1(double in) {
    vals0.r1 = in;
    vals0.r1d = Vals::INPUT;
    return update();
}
bool RCCore::setR2(double in) {
    vals0.r2 = in;
    vals0.r2d = Vals::INPUT;
    return update();
}
bool RCCore::setCurr(double in) {
    vals0.curr = in;
    vals0.currd = Vals::INPUT;
    return update();
}
bool RCCore::setVTop(Vals::dir dir) {
    vals0.vtopd = dir;
    return update();
}
bool RCCore::setVBot(Vals::dir dir) {
    vals0.vbotd = dir;
    return update();
}
bool RCCore::setVMid(Vals::dir dir) {
    vals0.vmidd = dir;
    return update();
}
bool RCCore::setR1(Vals::dir dir) {
    vals0.r1d = dir;
    return update();
}
bool RCCore::setR2(Vals::dir dir) {
    vals0.r2d = dir;
    return update();
}
bool RCCore::setCurr(Vals::dir dir) {
    vals0.currd = dir;
    return update();
}

bool RCCore::update() {
    // Go through all three calculations
    // keep intermediate results

    calc_constraint(vals0);

   if (vals0.constrainedType() != Vals::PROPER) {
       std::cout << "Improper input constraint -- won't calculate" << std::endl;
       vals2 = vals1 = vals0;
       if (ibridge) ibridge->setState(vals0);
       return false;
   }
    
    vals2 = vals1 = calc_group(vals0);
    if (ibridge) ibridge->setState(vals2);
    return true;
}

bool RCCore::update(Vals in) {
    // Go through all three calculations
    // keep intermediate results
    vals0 = in;
    return update();
}
void RCCore::setInput(Vals vals) {
    vals0 = vals;
}
Vals RCCore::getInput() const {
    return vals0;
}
Vals RCCore::getOutput() const {
    return vals2;
}
void RCCore::setBridge(const std::shared_ptr<UIBridgeInterface> & b) {
    ibridge = b;
}










