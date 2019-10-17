#include <iostream>
#include <iomanip>
#include <cmath>
#include <list>
#include <algorithm>
#include <cassert>
#include "rccore.h"
#include "vals.h"

const double vul = 7;
const double vbl = -7;
const double rmax = 99.31415;
const double currmax = 4.3;


// PRIVATE
// For the list, keep at most 4 elements,
// and enable removing something from the middle
// No need for pop, as we'll access the list entirely
void RCCore::pushToList(vartype v) {
    // Move to front if it's already there
    latest4.remove(v);
    latest4.push_back(v);
    while (latest4.size() > 4)
        latest4.pop_front();
    for (auto &&i : latest4)
        std::cout << cstr(i) << " ";    
    std::cout << std::endl;
}
void RCCore::removeFromList(vartype v) {
    latest4.remove(v);
}
void RCCore::setBridge(const std::shared_ptr<UIBridgeInterface> & b) {
    ibridge = b;
}
Vals RCCore::swapInputs(Vals vals) const {
    // Set inputs according to list
    Vals ovals = vals;
    auto find = [&](vartype vt) {return std::find(latest4.begin(), latest4.end(), vt) != latest4.end();};
    ovals.vtopd = find(VTOP) ? Vals::INPUT : Vals::OUTPUT;
    ovals.vbotd = find(VBOT) ? Vals::INPUT : Vals::OUTPUT;
    ovals.vmidd = find(VMID) ? Vals::INPUT : Vals::OUTPUT;
    ovals.r1d   = find(R1)   ? Vals::INPUT : Vals::OUTPUT;
    ovals.r2d   = find(R2)   ? Vals::INPUT : Vals::OUTPUT;
    ovals.currd = find(CURR) ? Vals::INPUT : Vals::OUTPUT;
    return ovals;
}
Vals RCCore::clip(Vals ivals, int icode) const {
    // Clips values in vals.
    Vals ovals = ivals;
    if (icode & 0x01) ovals.curr = std::min(ovals.curr, currmax);
    if (icode & 0x02) ovals.r2   = std::min(ovals.r2, rmax);
    if (icode & 0x04) ovals.r1   = std::min(ovals.r1, rmax);
    if (icode & 0x08) ovals.vmid = std::max(std::min(ovals.vmid, vul), vbl);
    if (icode & 0x10) ovals.vbot = std::max(ovals.vbot, vbl);
    if (icode & 0x20) ovals.vtop = std::min(ovals.vtop, vul);
    return ovals;
}
Vals RCCore::calc_group(vartype vt, Vals invals) const {
    // Update one set of values from the previous set
    // double vtop  = invals.vtop;
    // double vbot  = invals.vbot;
    // double vmid  = invals.vmid;
    // double r1    = invals.r1;
    // double r2    = invals.r2;
    // double curr  = invals.curr;
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

    int icode = invals.incode();
    Vals out = clip(invals, icode);
    auto check_ctr = [](int ctr) {
        if (ctr >= 10) throw std::exception("Too many counts");};

    switch(icode) {
        case 0x0f: {
            // Vtop, Vbot = f(vmid, r1, r2, curr)
            out.vtopd = Vals::OUTPUT;
            out.vbotd = Vals::OUTPUT;
            auto compute = [&out]() {
                out.vtop = out.vmid + out.curr * out.r1;
                out.vbot = out.vmid - out.curr * out.r2; 
            };
            int ctr;
            for (ctr = 0; ctr < 10; ctr++) {
                compute();
                if      (out.vtop > vul && vt == RCCore::CURR) 
                    out.curr = (vul - out.vmid) / out.r1;
                else if (out.vtop < vbl && vt == RCCore::CURR) 
                    out.curr = 0.0;
                else if (out.vbot > vul && vt == RCCore::CURR) 
                    out.curr = 0.0;
                else if (out.vbot < vbl && vt == RCCore::CURR) 
                    out.curr = (out.vmid - vbl) / out.r2;
                else if (out.vtop > vul && vt == RCCore::VMID) 
                    out.vmid = vul - (out.curr * out.r1);
                else if (out.vtop < vbl && vt == RCCore::VMID) 
                    out.vmid = 0.0;
                else if (out.vbot > vul && vt == RCCore::VMID) 
                    out.vmid = 0.0;
                else if (out.vbot < vbl && vt == RCCore::VMID) 
                    out.vmid = vbl + (out.curr * out.r2);
                else if (out.vtop > vul && vt == RCCore::R1) 
                    out.r1 = (vul - out.vmid) / out.curr;
                else if (out.vtop < vbl && vt == RCCore::R1)  // won't happen
                    out.r1 = (vbl - out.vmid) / out.curr;
                else if (out.vbot > vul && vt == RCCore::R2)  // won't happen
                    out.r2 = -(vul - out.vmid) / out.curr;
                else if (out.vbot < vbl && vt == RCCore::R2) 
                    out.r2 = -(vbl - out.vmid) / out.curr;
                else break;
            }
            check_ctr(ctr);
            break;}
        case 0x17: {
            // Vtop, vmid = f(vbot, r1, r2, curr)
            out.vtopd = Vals::OUTPUT;
            out.vmidd = Vals::OUTPUT;
            auto compute = [&out]() {
                out.vtop = out.vbot + out.curr * (out.r1 + out.r2);
                out.vmid = out.vbot + out.curr * out.r2; 
            };
            int ctr;
            for (ctr = 0; ctr < 10; ctr++) {
                compute();
                if      (out.vtop > vul && vt == RCCore::VBOT)
                    out.vbot = vul - (out.curr * (out.r1 + out.r2));
                else if (out.vtop < vbl && vt == RCCore::VBOT)
                    out.vbot = vbl - (out.curr * (out.r1 + out.r2));
                else if (out.vmid > vul && vt == RCCore::VBOT)
                    out.vbot = vul - (out.curr * out.r2);
                else if (out.vmid < vbl && vt == RCCore::VBOT)
                    out.vbot = vbl - (out.curr * out.r2);
                else if (out.vtop > vul && vt == RCCore::R1)
                    out.r1 = ((vul - out.vbot) / out.curr) - out.r2;
                else if (out.vtop < vbl && vt == RCCore::R1)
                    out.r1 = (vbl - out.vbot) / out.curr - out.r2;
                else if (out.vmid > vul && vt == RCCore::R1); // won't happen
                else if (out.vmid < vbl && vt == RCCore::R1); // won't happen
                else if (out.vtop > vul && vt == RCCore::R2)
                    out.r2 = (vul - out.vbot) / out.curr - out.r1;
                else if (out.vtop < vbl && vt == RCCore::R2)
                    out.r2 = (vbl - out.vbot) / out.curr - out.r1;
                else if (out.vmid > vul && vt == RCCore::R2)
                    out.r2 = (vul - out.vbot) / out.curr;
                else if (out.vmid < vbl && vt == RCCore::R2)
                    out.r2 = (vbl - out.vbot) / out.curr;
                else if (out.vtop > vul && vt == RCCore::CURR)
                    out.curr = (vul - out.vbot) / (out.r1 + out.r2);
                else if (out.vtop < vbl && vt == RCCore::CURR)
                    out.curr = (vbl - out.vbot) / (out.r1 + out.r2);
                else if (out.vmid > vul && vt == RCCore::CURR)
                    out.curr = (vul - out.vbot) / out.r2;
                else if (out.vmid < vbl && vt == RCCore::CURR)
                    out.curr = (vbl - out.vbot) / out.r2;
                else break;
            }
            check_ctr(ctr);
            break;}
        case 0x1b:
            throw std::invalid_argument( "Error in logic" );
        case 0x1d: {
            // vtop, r2 = f(vbot, vmid, r1, curr)
            out.vtopd = Vals::OUTPUT;
            out.r2d   = Vals::OUTPUT;
            auto compute = [&out]() {
                out.vtop = out.vmid + out.curr * out.r1;
                out.r2   = (out.vmid - out.vbot) / out.curr;
            };
            int ctr;
            for (ctr = 0; ctr < 10; ctr++) {
                compute();
                if      (out.vtop > vul  && vt == RCCore::VBOT); // won't happen
                else if (out.vtop < vbl  && vt == RCCore::VBOT); // won't happen
                else if (out.r2   > rmax && vt == RCCore::VBOT)
                    out.vbot = -(rmax * out.curr - out.vmid);
                else if (out.r2   < 0.0  && vt == RCCore::VBOT)
                    out.vbot = out.vmid;
                else if (out.vtop > vul  && vt == RCCore::VMID)
                    out.vmid = vul - out.curr * out.r1;
                else if (out.vtop < vbl  && vt == RCCore::VMID)
                    out.vmid = vbl - out.curr * out.r1;
                else if (out.r2   > rmax && vt == RCCore::VMID)
                    out.vmid = rmax * out.curr + out.vbot;
                else if (out.r2   < 0.0  && vt == RCCore::VMID)
                    out.vmid = out.vbot;
                else if (out.vtop > vul  && vt == RCCore::R1)
                    out.r1 = (vul - out.vmid ) / out.curr;
                else if (out.vtop < vbl  && vt == RCCore::R1)
                    out.r1 = (vbl - out.vmid ) / out.curr;
                else if (out.r2   > rmax && vt == RCCore::R1); // won't happen
                else if (out.r2   < 0.0  && vt == RCCore::R1); // won't happen
                else if (out.vtop > vul  && vt == RCCore::CURR)
                    out.curr = (vul - out.vmid) / out.r1;
                else if (out.vtop < vbl  && vt == RCCore::CURR)
                    out.curr = (vbl - out.vmid) / out.r1;
                else if (out.r2   > rmax && vt == RCCore::CURR)
                    out.curr = (out.vmid - out.vbot) / rmax;
                else if (out.r2   < 0.0  && vt == RCCore::CURR)
                    // special case so we don't divide by zero
                    // conditions imply vmid < vbot, so just keep r2
                    out.r2 = out.r2;
                else break;
            }
            check_ctr(ctr);
            break;}
        case 0x1e: {
            // vtop, curr = f(vbot, vmid, r1, r2)
            out.vtopd = Vals::OUTPUT;
            out.currd = Vals::OUTPUT;
            auto compute = [&out]() {
                out.vtop = out.vmid + (out.r1 / out.r2) * (out.vmid - out.vbot);
                out.curr = (out.vmid - out.vbot) / out.r2;
            };
            int ctr;
            for (ctr = 0; ctr < 10; ctr++) {
                compute();
                if      (out.curr > currmax && vt == RCCore::VBOT)
                    out.vbot = -(currmax * out.r2 - out.vmid);
                else if (out.curr < 0.0     && vt == RCCore::VBOT)
                    out.vbot = out.vmid;
                else if (out.vtop > vul     && vt == RCCore::VBOT)
                    out.vbot = out.vmid - (vul - out.vmid) *  (out.r2 / out.r1);
                else if (out.vtop < vbl     && vt == RCCore::VBOT) // shouldn't happen
                    out.vbot = out.vmid - (vbl - out.vmid) *  (out.r2 / out.r1);
                else if (out.curr > currmax && vt == RCCore::VMID)
                    out.vmid = currmax * out.r2 + out.vbot;
                else if (out.curr < 0.0     && vt == RCCore::VMID)
                    out.vmid = out.vbot;
                else if (out.vtop > vul     && vt == RCCore::VMID)
                    out.vmid = (vul + (out.r1 / out.r2) * out.vbot)/(1.0 + out.r1 / out.r2);
                else if (out.vtop < vbl     && vt == RCCore::VMID) // shouldn't happen
                    out.vmid = (vbl + (out.r1 / out.r2) * out.vbot)/(1.0 + out.r1 / out.r2);
                else if (out.curr > currmax && vt == RCCore::R1); // won't happen
                else if (out.curr < 0.0     && vt == RCCore::R1); // won't happen
                else if (out.vtop > vul     && vt == RCCore::R1)
                    out.r1 = out.r2 * (vul - out.vmid) / (out.vmid - out.vbot);
                else if (out.vtop < vbl     && vt == RCCore::R1) // shouldn't happen
                    out.r1 = out.r2 * (vbl - out.vmid) / (out.vmid - out.vbot);
                else if (out.curr > currmax && vt == RCCore::R2)
                    out.r2 = (out.vmid - out.vbot) / currmax;
                else if (out.curr < 0.0     && vt == RCCore::R2)
                    // special case so not to divide by zero
                    out.r2 = out.r2;
                else if (out.vtop > vul     && vt == RCCore::R2)
                    out.r2 = out.r1 * (out.vmid - out.vbot) / (vul - out.vmid);
                else if (out.vtop < vbl     && vt == RCCore::R2) // shouldn't happen
                    out.r2 = out.r1 * (out.vmid - out.vbot) / (vbl - out.vmid);
                else break;                
            }
            check_ctr(ctr);
            break;}
        case 0x27: {
            // vbot, vmid = f(vtop, r1, r2 curr)
            out.vbotd = Vals::OUTPUT;
            out.vmidd = Vals::OUTPUT;
            auto compute = [&out]() {
                out.vbot = out.vtop - out.curr * (out.r1 + out.r2);
                out.vmid = out.vtop - out.curr * out.r1;
            };
            int ctr;
            for (ctr = 0; ctr < 10; ctr++) {
                compute();
                if      (out.vbot > vul && vt == RCCore::VTOP)
                    out.vtop = vul + out.curr * (out.r1 + out.r2);
                else if (out.vbot < vbl && vt == RCCore::VTOP)
                    out.vtop = vbl + out.curr * (out.r1 + out.r2);
                else if (out.vmid > vul && vt == RCCore::VTOP)
                    out.vtop = vul + out.curr * out.r1;
                else if (out.vmid < vbl && vt == RCCore::VTOP)
                    out.vtop = vbl + out.curr * out.r1;
                else if (out.vbot > vul && vt == RCCore::R1)
                    out.r1 = -(vul - out.vtop) / out.curr - out.r2;
                else if (out.vbot < vbl && vt == RCCore::R1)
                    out.r1 = -(vbl - out.vtop) / out.curr - out.r2;
                else if (out.vmid > vul && vt == RCCore::R1)
                    out.r1 = -(vul - out.vtop) / out.curr;
                else if (out.vmid < vbl && vt == RCCore::R1)
                    out.r1 = -(vbl - out.vtop) / out.curr;
                else if (out.vbot > vul && vt == RCCore::R2)
                    out.r2 = -(vul - out.vtop) / out.curr - out.r1;
                else if (out.vbot < vbl && vt == RCCore::R2)
                    out.r2 = -(vbl - out.vtop) / out.curr - out.r1;
                else if (out.vmid > vul && vt == RCCore::R2); // won't happen
                else if (out.vmid < vbl && vt == RCCore::R2); // won't happen
                else if (out.vbot > vul && vt == RCCore::CURR)
                    out.curr = -(vul - out.vtop) / (out.r1 + out.r2);
                else if (out.vbot < vbl && vt == RCCore::CURR)
                    out.curr = -(vbl - out.vtop) / (out.r1 + out.r2);
                else if (out.vmid > vul && vt == RCCore::CURR)
                    out.curr = -(vul - out.vtop) / out.r1;
                else if (out.vmid < vbl && vt == RCCore::CURR)
                    out.curr = -(vul - out.vtop) / out.r1;
                else break;
            }
            check_ctr(ctr);
            break;}
        case 0x2b: {
            // vbot, r1 = f(vtop, vmid, r2, curr)
            out.vbotd = Vals::OUTPUT;
            out.r1d   = Vals::OUTPUT;
            auto compute = [&out]() {
                out.vbot  = out.vmid - out.curr * out.r2;
                out.r1    = (out.vtop - out.vmid) / out.curr;
            };
            int ctr;
            for (ctr = 0; ctr < 10; ctr++) {
                compute();
                if      (out.vbot > vul  && vt == RCCore::VTOP); // won't happen
                else if (out.vbot < vbl  && vt == RCCore::VTOP); // won't happen
                else if (out.r1   > rmax && vt == RCCore::VTOP)
                    out.vtop = rmax * out.curr + out.vmid;
                else if (out.r1   < 0.0  && vt == RCCore::VTOP)
                    out.vtop = out.vmid;
                else if (out.vbot > vul  && vt == RCCore::VMID)
                    out.vmid = vul + out.curr * out.r2;
                else if (out.vbot < vbl  && vt == RCCore::VMID)
                    out.vmid = vbl + out.curr * out.r2;
                else if (out.r1   > rmax && vt == RCCore::VMID)
                    out.vmid = -(rmax * out.curr - out.vtop);
                else if (out.r1   < 0.0  && vt == RCCore::VMID)
                    out.vmid = out.vtop;
                else if (out.vbot > vul  && vt == RCCore::R2)
                    out.r2 = -(vul - out.vmid) / out.curr;
                else if (out.vbot < vbl  && vt == RCCore::R2)
                    out.r2 = -(vbl - out.vmid) / out.curr;
                else if (out.r1   > rmax && vt == RCCore::R2); // won't happen
                else if (out.r1   < 0.0  && vt == RCCore::R2); // won't happen
                else if (out.vbot > vul  && vt == RCCore::CURR)
                    out.curr = -(vul -out.vmid) / out.r2;
                else if (out.vbot < vbl  && vt == RCCore::CURR)
                    out.curr = -(vbl - out.vmid) / out.r2;
                else if (out.r1   > rmax && vt == RCCore::CURR)
                    out.curr = (out.vtop - out.vmid) / rmax;
                else if (out.r1   < 0.0  && vt == RCCore::CURR)
                    // special case for divide by zero
                    out.curr = out.curr;
                else break;
            }
            break; }
        case 0x2d:
            throw std::invalid_argument( "Error in logic" );
        case 0x2e:
            //out.vmid = std::min(vmid, vtop);
            out.curr = (out.vtop - out.vmid) / out.r1;
            out.vbot = out.vmid - out.curr * out.r2;
            out.currd = Vals::OUTPUT;
            out.vbotd = Vals::OUTPUT;
            break;
        case 0x33:
            out.vmid = out.vbot + out.curr * out.r2;
            out.r1   = (out.vtop - out.vmid) / out.curr;
            out.vmidd = Vals::OUTPUT;
            out.r1d   = Vals::OUTPUT;
            break;
        case 0x35:
            out.vmid = out.vtop - out.curr * out.r1;
            out.r2   = (out.vmid - out.vbot) / out.curr;
            out.vmidd = Vals::OUTPUT;
            out.r2d   = Vals::OUTPUT;
            break;
        case 0x36:
            out.curr = (out.vtop - out.vbot) / (out.r1 + out.r2);
            out.vmid = out.vbot + out.curr * out.r2;
            out.currd = Vals::OUTPUT;
            out.vmidd = Vals::OUTPUT;
            break;
        case 0x39:
            out.r1 = (out.vtop - out.vmid) / out.curr;
            out.r2 = (out.vmid - out.vbot) / out.curr;
            out.r1d = Vals::OUTPUT;
            out.r2d = Vals::OUTPUT;
            break;
        case 0x3a:
            out.curr = (out.vmid - out.vbot) / out.r2;
            out.r1   = (out.vtop - out.vmid) / out.curr;
            out.currd = Vals::OUTPUT;
            out.r1d   = Vals::OUTPUT;
            break;
        case 0x3c:
            out.curr = (out.vtop - out.vmid) / out.r1;
            out.r2   = (out.vmid - out.vbot) / out.curr;
            out.currd = Vals::OUTPUT;
            out.r2d   = Vals::OUTPUT;
            break;
        default:
            throw std::invalid_argument( "Error in logic" );

    }
    out.ratio = out.r1 / out.r2;
    out.ratiod = Vals::OUTPUT;
    return out;
}
Vals::constype RCCore::constraint(Vals vals) const {
    // Checks whether we have exactly 4 inputs, and of the right combination
    if (vals.numInputs() > 4) {
        std::cout << "precheck over" << std::endl;
        return Vals::OVER;
    }
    else if (vals.numInputs() < 4) {
        std::cout << "precheck under" << std::endl;
        return Vals::UNDER;
    }
    else // These two cases represent one loop overdefined and the other underdefined
        if (vals.incode() == 0x1b || vals.incode() == 0x2d) {
            std::cout << "precheck mixed" << std::endl;
            return Vals::MIXED;
        }
        else {
            return Vals::PROPER;
        }
}

bool RCCore::_update(vartype vt) {
    // Calculate a what-if.
    // If the what-if violates anything, don't update the output
    if (constraint(inVals) == Vals::PROPER) {
        outVals = calc_group(vt, inVals);
        // Setting inVals to outVals prevents things from jumping after recovering from a limit
        inVals = outVals;
        if (ibridge) ibridge->setState(outVals, constraint(outVals));
        return true;
    } else {
        std::cout << "Improper input constraint -- won't calculate" << std::endl;
        outVals = inVals;
        if (ibridge) ibridge->setState(outVals, constraint(outVals));
        return false;
    }
}



std::string cstr(RCCore::vartype vt) {
    assert (vt >= RCCore::VTOP && vt <= RCCore::CURR);
    switch (vt) {
        case RCCore::VTOP: return "VTOP";
        case RCCore::VBOT: return "VBOT";
        case RCCore::VMID: return "VMID";
        case RCCore::R1:   return "R1";
        case RCCore::R2:   return "R2";
        case RCCore::CURR: return "CURR";
        default: return "logic error";
    }
}

// Public
RCCore::RCCore() :
    inVals (Vals()),
    outVals (Vals())
    {}

bool RCCore::setVTop(Vals::dir dir) {
    removeFromList(VTOP);
    if (dir == Vals::INPUT) pushToList(VTOP);
    inVals = swapInputs(inVals);
    return update();
}
bool RCCore::setVBot(Vals::dir dir) {
    removeFromList(VBOT);
    if (dir == Vals::INPUT) pushToList(VBOT);
    inVals = swapInputs(inVals);
    return update();
}
bool RCCore::setVMid(Vals::dir dir) {
    removeFromList(VMID);
    if (dir == Vals::INPUT) pushToList(VMID);
    inVals = swapInputs(inVals);
    return update();
}
bool RCCore::setR1(Vals::dir dir) {
    removeFromList(R1);
    if (dir == Vals::INPUT) pushToList(R1);
    inVals = swapInputs(inVals);
    return update();
}
bool RCCore::setR2(Vals::dir dir) {
    removeFromList(R2);
    if (dir == Vals::INPUT) pushToList(R2);
    inVals = swapInputs(inVals);
    return update();
}
bool RCCore::setCurr(Vals::dir dir) {
    removeFromList(CURR);
    if (dir == Vals::INPUT) pushToList(CURR);
    inVals = swapInputs(inVals);
    return update();
}

bool RCCore::update(vartype vt, double in) {
    switch(vt) {
        case VTOP: inVals.vtop = in; break;
        case VBOT: inVals.vbot = in; break;
        case VMID: inVals.vmid = in; break;
        case R1:   inVals.r1 = in;   break;
        case R2:   inVals.r2 = in;   break;
        case CURR: inVals.curr = in; break;
        default: throw std::invalid_argument("whoops!");
    }
    pushToList(vt);
    inVals = swapInputs(inVals);
    return _update(vt);

}
bool RCCore::update() {
    return _update(NONE);
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











