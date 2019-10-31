#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <list>
#include <algorithm>
#include <cassert>
#include <stdexcept>
#include "rccore.h"
#include "vals.h"

// const double vul = 20;
// const double vbl = -5;
// const double rmax = 500000;
// const double currmax = 2.5;


#define throw_line(msg) \
    do { \
        std::ostringstream s; \
        s << msg << " " << __FILE__ << ":" << __LINE__; \
        std::cout << s.str(); \
        throw std::logic_error(s.str()); \
    } while (0)
 
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
    if (icode & 0x10) ovals.vbot = std::max(std::min(ovals.vbot, vul), vbl);
    if (icode & 0x20) ovals.vtop = std::max(std::min(ovals.vtop, vul), vbl);
    return ovals;
}

bool gtb(double a, double b) {
    // return true if a > b by a certain amount
    const double eps = 1e-12;
    return ((a - b) >= eps);
}
bool ltb(double a, double b) {
    // return true if a < b by a certain amount
    const double eps = 1e-12;
    return ((b - a) >= eps);
}

Vals RCCore::calc_group(vartype vt, Vals invals) const {
    // Update one set of values from the previous set
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
        if (ctr >= 10)
            throw_line("Too many counts");
    };

    switch(icode) {
        case 0x0f: {
            // Vtop, Vbot = f(vmid, r1, r2, curr)
            out.vtopd = Vals::OUTPUT;
            out.vbotd = Vals::OUTPUT;
            auto compute = [&out]() {
                out.vtop = out.vmid + out.curr * out.r1;
                // out.vmid = out.vtop - out.curr * out.r1;
                // out.r1 = (out.vtop - out.vmid) / out.curr;
                // out.curr = (out.vtop - out.vmid) / out.r1;
                out.vbot = out.vmid - out.curr * out.r2;
                // out.vmid = out.vbot + out.curr * out.r2;
                // out.r2 = (out.vmid - out.vbot) / out.curr;
                // out.curr = (out.vbmid - out.vbot) / out.r2;
            };
            int ctr;
            for (ctr = 0; ctr < 10; ctr++) {
                compute();
                if (gtb(out.vtop, vul)) {
                    if      (vt == RCCore::VMID)
                        out.vmid = vul - out.curr * out.r1;
                    else if (vt == RCCore::R1)
                        out.r1 = (vul - out.vmid) / out.curr;
                    else if (vt == RCCore::CURR)
                        out.curr = (vul - out.vmid) / out.r1;
                    else if (vt == RCCore::VMID)
                        out.vmid = vul - out.curr * out.r1;
                }
                else if (ltb(out.vtop, vbl)) {
                    if      (vt == RCCore::VMID)
                        out.vmid = vbl - out.curr * out.r1;
                    else if (vt == RCCore::R1)
                        out.r1 = (vbl - out.vmid) / out.curr;
                    else if (vt == RCCore::CURR)
                        out.curr = (vbl - out.vmid) / out.r1;
                    else if (vt == RCCore::VMID)
                        out.vmid = vbl - out.curr * out.r1;
                }
                else if (gtb(out.vbot, vul)) {
                    if      (vt == RCCore::VMID)
                        out.vmid = vul + out.curr * out.r2;
                    else if (vt == RCCore::R2)
                        out.r2 = (out.vmid - vul) / out.curr;
                    else if (vt == RCCore::CURR)
                        out.curr = (out.vmid - vul) / out.r2;

                }
                else if (ltb(out.vbot, vbl)) {
                    if      (vt == RCCore::VMID)
                        out.vmid = vbl + out.curr * out.r2;
                    else if (vt == RCCore::R2)
                        out.r2 = (out.vmid - vbl) / out.curr;
                    else if (vt == RCCore::CURR)
                        out.curr = (out.vmid - vbl) / out.r2;
                }
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
                // out.vbot = out.vtop - out.curr * (out.r1 + out.r2);
                // out.r1   = (out.vtop - out.vbot) / out.curr - out.r2;
                // out.r2   = (out.vtop - out.vbot) / out.curr - out.r1;
                // out.curr = (out.vtop - out.vbot) / (out.r1 + out.r2);
                out.vmid = out.vbot + out.curr * out.r2;
                // out.vbot = out.vmid - out.curr * out.r2;
                // out.r2   = (out.vmid - out.vbot) / out.curr;
                // out.curr = (out.vmid - out.vbot) / out.r2;
            };
            int ctr;
            for (ctr = 0; ctr < 10; ctr++) {
                compute();
                if (gtb(out.vtop, vul)) {
                    if      (vt == RCCore::VBOT)
                        out.vbot = vul - out.curr * (out.r1 + out.r2);
                    else if (vt == RCCore::R1)
                        out.r1 = (vul - out.vbot) / out.curr - out.r2;
                    else if (vt == RCCore::R2)
                        out.r2 = (vul - out.vbot) / out.curr - out.r1;
                    else if (vt == RCCore::CURR)
                        out.curr = (vul - out.vbot) / (out.r1 + out.r2);
                }
                else if (ltb(out.vtop, vbl)) {
                    if      (vt == RCCore::VBOT)
                        out.vbot = vbl - out.curr * (out.r1 + out.r2);
                    else if (vt == RCCore::R1)
                        out.r1 = (vbl - out.vbot) / out.curr - out.r2;
                    else if (vt == RCCore::R2)
                        out.r2 = (vbl - out.vbot) / out.curr - out.r1;
                    else if (vt == RCCore::CURR)
                        out.curr = vbl - out.vbot / (out.r1 + out.r2);
                }
                else if (gtb(out.vmid, vul)) {
                    if      (vt == RCCore::VBOT)
                        out.vbot = vul - out.curr * out.r2;
                    else if (vt == RCCore::R2)
                        out.r2 = (vul - out.vbot) / out.curr;
                    else if (vt == RCCore::CURR)
                        out.vbot = vul - out.curr * out.r2;
                }
                else if (ltb(out.vmid, vbl)) {
                    if      (vt == RCCore::VBOT)
                        out.vbot = vbl - out.curr * out.r2;
                    else if (vt == RCCore::R2)
                        out.vbot = vbl - out.curr * out.r2;
                    else if (vt == RCCore::CURR)
                        out.curr = (vbl - out.vbot) / out.r2;
                }
                else break;
            }
            check_ctr(ctr);
            break;}
        case 0x1b:
            throw_line( "Error in logic" );
        case 0x1d: {
            // vtop, r2 = f(vbot, vmid, r1, curr)
            out.vtopd = Vals::OUTPUT;
            out.r2d   = Vals::OUTPUT;
            auto compute = [&out]() {
                out.vtop = out.vmid + out.curr * out.r1;
                // out.vmid = out.vtop - out.curr * out.r1;
                // out.r1 = (out.vtop - out.vmid) / out.curr;
                // out.curr = (out.vtop - out.vmid) / out.r1;
                out.r2   = (out.vmid - out.vbot) / out.curr;
                // out.vbot = out.vmid - (out.r2 * out.curr);
                // out.vmid = (out.r2 * out.curr) + out.vbot;
                // out.curr = (out.vmid - out.vbot) / out.r2;
            };
            int ctr;
            for (ctr = 0; ctr < 10; ctr++) {
                compute();
                if (gtb(out.vtop, vul)) {
                    if      (vt == RCCore::VMID)
                        out.vmid = vul - out.curr * out.r1;
                    else if (vt == RCCore::R1)
                        out.r1 = (vul - out.vmid) / out.curr;
                    else if (vt == RCCore::CURR)
                        out.curr = (vul - out.vmid) / out.r1;
                }
                else if (ltb(out.vtop, vbl)) {
                    if      (vt == RCCore::VMID)
                        out.vmid = vbl - out.curr * out.r1;
                    else if (vt == RCCore::R1) 
                        out.r1 = (vbl - out.vmid) / out.curr;
                    else if (vt == RCCore::CURR) 
                        out.curr = (vbl - out.vmid) / out.r1;
                }
                else if (gtb(out.r2, rmax)) {
                    if      (vt == RCCore::VBOT) 
                        out.vbot = out.vmid - (rmax * out.curr);
                    else if (vt == RCCore::VMID) 
                        out.vmid = (rmax * out.curr) + out.vbot;
                    else if (vt == RCCore::CURR) 
                        out.curr = (out.vmid - out.vbot) / rmax;
                }
                else if (ltb(out.r2, 0.0)) {
                    if      (vt == RCCore::VBOT) 
                        out.vbot = out.vmid - (0.0 * out.curr);
                    else if (vt == RCCore::VMID) 
                        out.vmid = (0.0 * out.curr) + out.vbot;
                    else if (vt == RCCore::CURR) 
                        out.curr = (out.vmid - out.vbot) / 0.0;

                }
                else break;
            }
            check_ctr(ctr);
            break;}
        case 0x1e: {
            // vtop, curr = f(vbot, vmid, r1, r2)
            out.vtopd = Vals::OUTPUT;
            out.currd = Vals::OUTPUT;
            auto compute = [&out]() {
                out.vtop = out.vmid * (1 + out.r1 / out.r2) - out.vbot * (out.r1 / out.r2);
                // out.vbot = out.vmid * (1 + out.r1 / out.r2) - out.vtop;
                // out.vmid = (out.vtop + out.vbot * (out.r1 / out.r2)) / (1 + out.r1 / out.r2);
                // out.r1 = out.r2 * (out.vtop - out.vmid) / (out.vmid - out.vbot);
                // out.r2 = out.r1 * (out.vmid - out.vbot) / (out.vtop - out.vmid);
                out.curr = (out.vmid - out.vbot) / out.r2;
                // out.vbot = out.vmid - out.curr * out.r2;
                // out.vmid = out.curr * out.r2 + out.vbot;
                // out.r2 = (out.vmid - out.vbot) / out.curr;
            };
            int ctr;
            for (ctr = 0; ctr < 10; ctr++) {
                compute();
                if (gtb(out.vtop, vul)) {
                    if (vt == RCCore::VBOT)
                        out.vbot = out.vmid * (1 + out.r1 / out.r2) - vul;
                    else if (vt == RCCore::VMID)
                        out.vmid = (vul + out.vbot * (out.r1 / out.r2)) / (1 + out.r1 / out.r2);
                    else if (vt == RCCore::R1)
                        out.r1 = out.r2 * (vul - out.vmid) / (out.vmid - out.vbot);
                    else if (vt == RCCore::R2)
                        out.r2 = out.r1 * (out.vmid - out.vbot) / (vul - out.vmid);
                }
                else if (ltb(out.vtop, vbl)) {
                    if (vt == RCCore::VBOT)
                        out.vbot = out.vmid * (1 + out.r1 / out.r2) - vbl;
                    else if (vt == RCCore::VMID)
                        out.vmid = (vbl + out.vbot * (out.r1 / out.r2)) / (1 + out.r1 / out.r2);
                    else if (vt == RCCore::R1)
                        out.r1 = out.r2 * (vbl - out.vmid) / (out.vmid - out.vbot);
                    else if (vt == RCCore::R2)
                        out.r2 = out.r1 * (out.vmid - out.vbot) / (vbl - out.vmid);
                }
                else if (gtb(out.curr, currmax)) {
                    if (vt == RCCore::VBOT)
                        out.vbot = out.vmid - currmax * out.r2;
                    else if (vt == RCCore::VMID)
                        out.vmid = currmax * out.r2 + out.vbot;
                    else if (vt == RCCore::R2)
                        out.r2 = (out.vmid - out.vbot) / currmax;
                }
                else if (ltb(out.curr, 0.0)) {
                    if (vt == RCCore::VBOT)
                        out.vbot = out.vmid - 0.0 * out.r2;
                    else if (vt == RCCore::VMID)
                        out.vmid = 0.0 * out.r2 + out.vbot;
                    else if (vt == RCCore::R2)
                        out.r2 = (out.vmid - out.vbot) / 0.0;
                }
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
                // out.vtop = out.vbot + out.curr * (out.r1 + out.r2);
                // out.r1 = ((out.vtop - out.vbot) / out.curr) - out.r2;
                // out.r2 = ((out.vtop - out.vbot) / out.curr) - out.r1;
                // out.curr = (out.vtop - out.vbot) / (out.r1 + out.r2)
                out.vmid = out.vtop - out.curr * out.r1;
                // out.vtop = out.vmid + out.curr * out.r1;
                // out.r1 = (out.vtop - out.vmid) / out.curr;
                // out.curr = (out.vtop - out.vmid) / out.r1;
            };
            int ctr;
            for (ctr = 0; ctr < 10; ctr++) {
                compute();
                if (gtb(out.vbot, vul)) {
                    if (vt == RCCore::VTOP)
                        out.vtop = vul + out.curr * (out.r1 + out.r2);
                    else if (vt == RCCore::R1)
                        out.r1 = ((out.vtop - vul) / out.curr) - out.r2;
                    else if (vt == RCCore::R2)
                        out.r2 = ((out.vtop - vul) / out.curr) - out.r1;
                    else if (vt == RCCore::CURR)
                        out.curr = (out.vtop - vul) / (out.r1 + out.r2);
                }
                else if (ltb(out.vbot, vbl)) {
                    if (vt == RCCore::VTOP)
                        out.vtop = vbl + out.curr * (out.r1 + out.r2);
                    else if (vt == RCCore::R1)
                        out.r1 = ((out.vtop - vbl) / out.curr) - out.r2;
                    else if (vt == RCCore::R2)
                        out.r2 = ((out.vtop - vbl) / out.curr) - out.r1;
                    else if (vt == RCCore::CURR)
                        out.curr = (out.vtop - vbl) / (out.r1 + out.r2);
                } 
                else if (gtb(out.vmid, vul)) {
                    if (vt == RCCore::VTOP)
                        out.vtop = vul + out.curr * out.r1;
                    else if (vt == RCCore::R1)
                        out.r1 = (out.vtop - vul) / out.curr;
                    else if (vt == RCCore::CURR)
                        out.curr = (out.vtop - vul) / out.r1;
                } 
                else if (ltb(out.vmid, vbl)) {
                    if (vt == RCCore::VTOP)
                        out.vtop = vbl + out.curr * out.r1;
                    else if (vt == RCCore::R1)
                        out.r1 = (out.vtop - vbl) / out.curr;
                    else if (vt == RCCore::CURR)
                        out.curr = (out.vtop - vbl) / out.r1;
                } 
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
                // out.vmid = out.vbot + out.curr * out.r2;
                // out.r2 = (out.vmid - out.vbot) / out.curr;
                // out.curr = (out.vmid - out.vbot) / out.r2;
                out.r1    = (out.vtop - out.vmid) / out.curr;
                // out.vtop = out.r1 * out.curr + out.vmid;
                // out.vmid = out.vtop - (out.r1 * out.curr);
                // out.curr = (out.vtop - out.vmid) / out.r1;
            };
            int ctr;
            for (ctr = 0; ctr < 10; ctr++) {
                compute();
                if      (gtb(out.vbot, vul)){
                    if (vt == RCCore::VMID)
                        out.vmid = vul + out.curr * out.r2;
                    else if (vt == RCCore::R2)
                        out.r2 = (out.vmid - vul) / out.curr;
                    else if (vt == RCCore::CURR)
                        out.curr = (out.vmid - vul) / out.r2;
                }
                else if (ltb(out.vbot, vbl)){
                    if (vt == RCCore::VMID)
                        out.vmid = vbl + out.curr * out.r2;
                    else if (vt == RCCore::R2)
                        out.r2 = (out.vmid - vbl) / out.curr;
                    else if (vt == RCCore::CURR)
                        out.curr = (out.vmid - vbl) / out.r2;
                }
                else if (gtb(out.r1, rmax)) {
                    if (vt == RCCore::VTOP)
                        out.vtop = rmax * out.curr + out.vmid;
                    else if (vt == RCCore::VMID)
                        out.vmid = out.vtop - (rmax * out.curr);
                    else if (vt == RCCore::CURR)
                        out.curr = (out.vtop - out.vmid) / rmax;
                }
                else if (ltb(out.r1, 0.0)){
                    if (vt == RCCore::VTOP)
                        out.vtop = 0.0 * out.curr + out.vmid;
                    else if (vt == RCCore::VMID)
                        out.vmid = out.vtop - (0.0 * out.curr);
                    else if (vt == RCCore::CURR)
                        out.curr = (out.vtop - out.vmid) / 0.0;
                }
                else break;
            }
            check_ctr(ctr);
            break; }
        case 0x2d:
            throw_line( "Error in logic" );
        case 0x2e: {
            // vbot, curr = f(vtop, vmid, r1, r2)
            out.vbotd = Vals::OUTPUT;
            out.currd = Vals::OUTPUT;
            auto compute = [&out]() {
                out.vbot = out.vmid - (out.r2 / out.r1) * (out.vtop - out.vmid);
                // out.vtop = (out.vmid - out.vbot) * (out.r1 / out.r2) + out.vmid;
                // out.vmid = (out.vbot + out.vtop * out.r2 / out.r1) / (1+ out.r2 / out.r1);
                // out.r1 = out.r2 * (out.vtop - out.vmid) / (out.vmid - out.vbot);
                // out.r2 = out.r1 * ((out.vmid - out.vbot)) / (out.vtop - out.vmid);
                out.curr = (out.vtop - out.vmid) / out.r1;
                // out.vtop = out.curr * out.r1 + out.vmid;
                // out.vmid = out.vtop - (out.curr * out.r1);
                // out.r1 = (out.vtop - out.vmid) / out.curr;

            };
            int ctr;
            for (ctr = 0; ctr < 10; ctr++) {
                compute();
                if      (gtb(out.vbot, vul)) {
                    if (vt == RCCore::VTOP)
                        out.vtop = (out.vmid - vul) * (out.r1 / out.r2) + out.vmid;
                    else if (vt == RCCore::VMID)
                        out.vmid = (vul + out.vtop * out.r2 / out.r1) / (1+ out.r2 / out.r1);
                    else if (vt == RCCore::R1)
                        out.r1 = out.r2 * (out.vtop - out.vmid) / (out.vmid - vul);
                    else if (vt == RCCore ::R2)
                        out.r2 = out.r1 * ((out.vmid - vul)) / (out.vtop - out.vmid);
                }
                else if (ltb(out.vbot, vbl)) {
                    if (vt == RCCore::VTOP)
                        out.vtop = (out.vmid - vbl) * (out.r1 / out.r2) + out.vmid;
                    else if (vt == RCCore::VMID)
                        out.vmid = (vbl + out.vtop * out.r2 / out.r1) / (1+ out.r2 / out.r1);
                    else if (vt == RCCore::R1)
                        out.r1 = out.r2 * (out.vtop - out.vmid) / (out.vmid - vbl);
                    else if (vt == RCCore ::R2)
                        out.r2 = out.r1 * ((out.vmid - vbl)) / (out.vtop - out.vmid);
                }
                else if (gtb(out.curr, currmax)) {
                    if (vt == RCCore::VTOP)
                        out.vtop = currmax * out.r1 + out.vmid;
                    else if (vt == RCCore::VMID)
                        out.vmid = out.vtop - (currmax * out.r1);
                    else if (vt == RCCore::R1)
                        out.r1 = (out.vtop - out.vmid) / currmax;
                }
                else if (ltb(out.curr, 0.0)) {
                    if (vt == RCCore::VTOP)
                        out.vtop = 0.0 * out.r1 + out.vmid;
                    else if (vt == RCCore::VMID)
                        out.vmid = out.vtop - (0.0 * out.r1);
                    else if (vt == RCCore::R1)
                        out.r1 = (out.vtop - out.vmid) / 0.0;
                }
                else break;
            }
            check_ctr(ctr);
            break;}
        case 0x33: {
            // vmid, r1 = f(vtop, vbot, r2, curr)
            out.vmidd = Vals::OUTPUT;
            out.r1d   = Vals::OUTPUT;
            auto compute = [&out]() {
                out.vmid = out.vbot + out.curr * out.r2;
                // out.vbot = out.vmid - out.curr * out.r2
                // out.r2   = (out.vmid - out.vbot) / out.curr;
                // out.curr = (out.vmid - out.vbot) / out. r2;
                out.r1   = (out.vtop - out.vbot - out.curr * out.r2) / out.curr;
                // out.vtop = (out.r1 * out.curr) + out.vbot + out.curr * out.r2;
                // out.vbot = out.vtop - (out.r1 * out.curr) - out.curr * out.r2;
                // out.r2   = (out.vtop - out.vbot) / out.curr + out.r1;
                // out.curr = (out.vtop - out.vbot) / (out.r1 + out.r2)
            };
            int ctr;
            for (ctr = 0; ctr < 10; ctr++) {
                compute();
                if (gtb(out.vmid, vul)) {
                    if      (vt == RCCore::VBOT)
                        out.vbot = vul - out.curr * out.r2;
                    else if (vt == RCCore::R2)
                        out.r2   = (vul - out.vbot) / out.curr;
                    else if (vt == RCCore::CURR)
                		out.curr = (vul - out.vbot) / out. r2;
                    else break;}
                else if (ltb(out.vmid, vbl)) {
                    if      (vt == RCCore::VBOT)
                		out.vbot = vbl - out.curr * out.r2;
                    else if (vt == RCCore::R2)
                		out.r2   = (vbl - out.vbot) / out.curr;
                    else if (vt == RCCore::CURR)
                		out.curr = (vbl - out.vbot) / out. r2;
                    else break;}
                else if (gtb(out.r1, rmax)) {
                    if      (vt == RCCore::VTOP)
                		out.vtop = (rmax * out.curr) + out.vbot + out.curr * out.r2;
                    else if (vt == RCCore::VBOT)
                		out.vbot = out.vtop - (rmax * out.curr) - out.curr * out.r2;
                    else if (vt == RCCore::R2)
                		out.r2   = (out.vtop - out.vbot) / out.curr + rmax;
                    else if (vt == RCCore::CURR)
                		out.curr = (out.vtop - out.vbot) / (rmax + out.r2);
                    else break;}
                else if (ltb(out.r1, 0.0)) {
                    if      (vt == RCCore::VTOP)
                		out.vtop = (0.0 * out.curr) + out.vbot + out.curr * out.r2;
                    else if (vt == RCCore::VBOT)
                		out.vbot = out.vtop - (0.0 * out.curr) - out.curr * out.r2;
                    else if (vt == RCCore::R2)
                		out.r2   = (out.vtop - out.vbot) / out.curr + 0.0;
                    else if (vt == RCCore::CURR)
                		out.curr = (out.vtop - out.vbot) / (0.0 + out.r2);
                    else break;}
                else break;
            }
            check_ctr(ctr);
            break;}
        case 0x35:{
            // vmid, r2 = f(vtop, vbot r1, curr)
            out.vmidd = Vals::OUTPUT;
            out.r2d   = Vals::OUTPUT;
            auto compute = [&out]() {
                out.vmid = out.vtop - out.curr * out.r1;
                // out.vtop = out.vmid + out.curr * out.r1
                // out.r1 = (out.vtop - out.vvmid) / out.curr
                // out.curr = (out.vtop - out.vmid) / out.r1
                out.r2 = (out.vtop - out.vbot) / out.curr - out.r1;
                // out.vtop = out.curr * (out.r1 + out.r2)  + out.vbot;
                // out.vbot = out.vtop - out.curr * (out.r1 + out.r2);
                // out.r1   = (out.vtop - out.vbot) / out.curr - out.r2;
                // out.curr = (out.vtop - out.vbot) / (out.r1 + out.r2);
                };
            int ctr;
            for (ctr = 0; ctr < 10; ctr++) {
                compute();
                if (gtb(out.vmid, vul)) {
                    if      (vt == RCCore::VTOP)
                		out.vtop = vul + out.curr * out.r1;
                    else if (vt == RCCore::R1)
                		out.r1 = (out.vtop - vul) / out.curr;
                    else if (vt == RCCore::CURR)
                		out.curr = (out.vtop - vul) / out.r1;
                    else break;}
                else if (ltb(out.vmid, vbl)) {
                    if      (vt == RCCore::VTOP)
                		out.vtop = vbl + out.curr * out.r1;
                    else if (vt == RCCore::R1)
                		out.r1 = (out.vtop - out.vmid) / out.curr;
                    else if (vt == RCCore::CURR)
                		out.curr = (out.vtop - vbl) / out.r1;
                    else break;}
                else if (gtb(out.r2, rmax))  {
                    if      (vt == RCCore::VTOP)
                        out.vtop = out.curr * (out.r1 + rmax)  + out.vbot;
                    else if (vt == RCCore::VBOT)
               		    out.vbot = out.vtop - out.curr * (out.r1 + rmax);
                    else if (vt == RCCore::R1)
                        out.r1   = (out.vtop - out.vbot) / out.curr - rmax;
                    else if (vt == RCCore::CURR)
                        out.curr = (out.vtop - out.vbot) / (out.r1 + rmax);
                    else break;}
                else if (ltb(out.r2, 0.0))   {
                    if      (vt == RCCore::VTOP)
                		out.vtop = out.curr * (out.r1 + 0.0)  + out.vbot;
                    else if (vt == RCCore::VBOT)
               		    out.vbot = out.vtop - out.curr * (out.r1 + 0.0);
                    else if (vt == RCCore::R1)
                        out.r1   = (out.vtop - out.vbot) / out.curr - 0.0;
                    else if (vt == RCCore::CURR)
                        out.curr = (out.vtop - out.vbot) / (out.r1 + 0.0);
                    else break;}
                else break;
            }
            check_ctr(ctr);
            break;}
        case 0x36:{
            // vmid, curr = f(vtop, vbot, r1, r2)
            out.vmidd = Vals::OUTPUT;
            out.currd = Vals::OUTPUT;
            auto compute = [&out]() {
                out.vmid = out.vbot + (out.vtop - out.vbot) / (out.r1 + out.r2) * out.r2;
                // out.vtop = (out.vm* ( out.r1 + out.r2) - out.vbot / out.r2) + out.vbot;
                // out.vbot = out.vmid - (out.r2 * (out.vtop - out.vbot)) / (out.r1 + out.r2);
                // out.r1 = (out.vmid - out.r2 * (out.vtop - out.vbot) / out.vbot) - out.r2;
                // out.r2 = ((out.vmid - out.vbot * out.r1)) / out.vtop;
                out.curr = (out.vtop - out.vbot) / (out.r1 + out.r2);
                // out.vtop = out.curr * (out.r1 + out.r2) + out.vbot
                // out.vbot = out.vtop - out.curr * (out.r1 + out.r2)
                // out.r1 = (out.vtop - out.vbot) / out.curr - out.r2
                // out.r2 = (out.vtop - out.vbot) / out.curr - out.r1
                };
            int ctr;
            for (ctr = 0; ctr < 10; ctr++) {
                compute();
                if (gtb(out.vmid, vul))     {
                    if      (vt == RCCore::VTOP)
                        out.vtop = (vul * ( out.r1 + out.r2) - out.vbot / out.r2) + out.vbot;
                    else if (vt == RCCore::VBOT)
                        out.vbot = vul - (out.r2 * (out.vtop - out.vbot)) / (out.r1 + out.r2);
                    else if (vt == RCCore::R1)
                        out.r1 = (vul - out.r2 * (out.vtop - out.vbot) / out.vbot) - out.r2;
                    else if (vt == RCCore::R2)
                        out.r2 = ((vul - out.vbot * out.r1)) / out.vtop;
                    else break;}
                else if (ltb(out.vmid, vbl))     {
                    if      (vt == RCCore::VTOP)
                        out.vtop = (vbl * ( out.r1 + out.r2) - out.vbot / out.r2) + out.vbot;
                    else if (vt == RCCore::VBOT)
                        out.vbot = vbl - (out.r2 * (out.vtop - out.vbot)) / (out.r1 + out.r2);
                    else if (vt == RCCore::R1)
                        out.r1 = (vbl - out.r2 * (out.vtop - out.vbot) / out.vbot) - out.r2;
                    else if (vt == RCCore::R2)
                        out.r2 = ((vbl - out.vbot * out.r1)) / out.vtop;
                    else break;}
                else if (gtb(out.curr, currmax)) {
                    if      (vt == RCCore::VTOP)
                		out.vtop = currmax * (out.r1 + out.r2) + out.vbot;
                    else if (vt == RCCore::VBOT)
                		out.vbot = out.vtop - currmax * (out.r1 + out.r2);
                    else if (vt == RCCore::R1)
                		out.r1 = (out.vtop - out.vbot) / currmax - out.r2;
                    else if (vt == RCCore::R2)
                		out.r2 = (out.vtop - out.vbot) / currmax - out.r1;
                    else break;}
                else if (ltb(out.curr, 0.0))     {
                    if      (vt == RCCore::VTOP)
                		out.vtop = 0.0 * (out.r1 + out.r2) + out.vbot;
                    else if (vt == RCCore::VBOT)
                		out.vbot = out.vtop - 0.0 * (out.r1 + out.r2);
                    else if (vt == RCCore::R1)
                		out.r1 = (out.vtop - out.vbot) / 0.0 - out.r2;
                    else if (vt == RCCore::R2)
                		out.r2 = (out.vtop - out.vbot) / 0.0 - out.r1;
                    else break;}
                else break;
            }
            check_ctr(ctr);
            break;}
        case 0x39:{
            // r1, r2 = f(vtop, vbot, vmid, curr)
            out.r1d = Vals::OUTPUT;
            out.r2d = Vals::OUTPUT;
            auto compute = [&out]() {
                out.r1 = (out.vtop - out.vmid) / out.curr;
                // out.vtop = out.r1 * out.curr + out.vmid
                // out.vmid = out.vtop - out.r1 * out.curr
                // out.curr = (out.vtop -out.vmid) / out.r1
                out.r2 = (out.vmid - out.vbot) / out.curr;
                // out.vbot = out.vmid - out.r2 * out.curr
                // out.vmid = out.r2 * out.curr + out.vbot
                // out.curr = (out.vmid - out.vbot) / out.r2
                };
            int ctr;
            for (ctr = 0; ctr < 10; ctr++) {
                compute();
                if (gtb(out.r1, rmax)) {
                    if      (vt == RCCore::VTOP)
                		   out.vtop = rmax * out.curr + out.vmid;
                    else if (vt == RCCore::VMID)
                		out.vmid = out.vtop - rmax * out.curr;
                    else if (vt == RCCore::CURR)
                		out.curr = (out.vtop - out.vmid) / rmax;
                    else break;}
                else if (ltb(out.r1, 0.0))  {
                    if      (vt == RCCore::VTOP)
                		out.vtop = 0.0 * out.curr + out.vmid;
                    else if (vt == RCCore::VMID)
                		out.vmid = out.vtop - 0.0 * out.curr;
                    else if (vt == RCCore::CURR)
                		out.curr = (out.vtop - out.vmid) / 0.0;
                    else break;}
                else if (gtb(out.r2, rmax)) {
                    if      (vt == RCCore::VBOT)
                		out.vbot = out.vmid - rmax * out.curr;
                    else if (vt == RCCore::VMID)
                		out.vmid = rmax * out.curr + out.vbot;
                    else if (vt == RCCore::CURR)
                		out.curr = (out.vmid - out.vbot) / rmax;
                    else break;}
                else if (ltb(out.r2, 0.0))  {
                    if      (vt == RCCore::VBOT)
                		out.vbot = out.vmid - 0.0 * out.curr;
                    else if (vt == RCCore::VMID)
                		out.vmid = 0.0 * out.curr + out.vbot;
                    else if (vt == RCCore::CURR)
                		out.curr = (out.vmid - out.vbot) / 0.0;
                    else break;}
                else break;
            }
            check_ctr(ctr);
            break;}
        case 0x3a:{
            // r1, curr = f(vtop, vbot, vmid, r2)
            out.r1d   = Vals::OUTPUT;
            out.currd = Vals::OUTPUT;
            auto compute = [&out]() {
                out.r1   = out.r2 * (out.vtop - out.vmid) / (out.vmid - out.vbot);
                // out.vtop = (out.r1 / out.r2) * (out.vmid - out.vbot) + out.vmid;
                // out.vbot = out.vmid - (out.vtop - out.vmid) * (out.r2 / out.r1);
                // out.vmid = (out.vtop * out.r2 + out.vbot * out.r1) / (out.r1 + out.r2);
                // out.r2 = out.r1 * (out.vmid - out.vbot) / (out.vtop - out.vmid);
                out.curr = (out.vmid - out.vbot) / out.r2;
                // out.vbot = out.vmid - out.curr * out.r2
                // out.vmid = out.curr * out.r2 + out.vbot
                // out.r2 = (out.vmid - out.vbot) / out.curr
                };
            int ctr;
            for (ctr = 0; ctr < 10; ctr++) {
                compute();
                // Special case at the root of r1<0, r2<0, or curr<0 problems
                if (out.vbot > out.vmid) {
                    if (vt == RCCore::VBOT)
                        out.vbot = out.vmid - (out.vtop - out.vmid) * (out.r2 / rmax);
                    else if (vt == RCCore::VMID)
                        out.vmid = (out.vtop * out.r2 + out.vbot * rmax) / (rmax + out.r2);
                    else
                        throw_line("Shouldn't get here");}
                else if (out.vtop < out.vmid) {
                    if (vt == RCCore::VTOP)
                        out.vtop = out.vtop = (0.0 / out.r2) * (out.vmid - out.vbot) + out.vmid;
                    else if (vt == RCCore::VMID)
                        out.vmid = (out.vtop * out.r2 + out.vbot * 0.0) / (0.0 + out.r2);
                    else 
                        throw_line("Shouldn't get here");}
                else if (gtb(out.r1, rmax))      {
                    if      (vt == RCCore::VTOP)
                        out.vtop = (rmax / out.r2) * (out.vmid - out.vbot) + out.vmid;
                    else if (vt == RCCore::VBOT)
                        out.vbot = out.vmid - (out.vtop - out.vmid) * (out.r2 / rmax);
                    else if (vt == RCCore::VMID)
                        out.vmid = (out.vtop * out.r2 + out.vbot * rmax) / (rmax + out.r2);
                    else if (vt == RCCore::R2)
                        out.r2 = rmax * (out.vmid - out.vbot) / (out.vtop - out.vmid);
                    else 
                        throw_line("Shouldn't get here");}
                else if (gtb(out.curr, currmax)) {
                    if      (vt == RCCore::VBOT)
                		out.vbot = out.vmid - currmax * out.r2;
                    else if (vt == RCCore::VMID)
                		out.vmid = currmax * out.r2 + out.vbot;
                    else if (vt == RCCore::R2)
                        out.r2 = (out.vmid - out.vbot) / currmax;
                    else
                        throw_line("Shouldn't get here");
                    }
                else break;
            }
            check_ctr(ctr);
            break;}
        case 0x3c:{
            // r2, curr = f(vtop, vbot, vmid, r1)
            out.r2d   = Vals::OUTPUT;
            out.currd = Vals::OUTPUT;
            auto compute = [&out]() {
                out.r2   = out.r1 * (out.vmid - out.vbot) / (out.vtop - out.vmid);
                // out.vtop = (out.r1 / out.r2) * (out.vmid - out.vbot) + out.vmid;
                // out.vbot = out.vmid - (out.vtop - out.vmid) * (out.r2 / out.r1);
                // out.vmid = (out.vtop * out.r2 + out.vbot * out.r1) / (out.r1 + out.r2);
                // out.r1   = out.r2 * (out.vtop - out.vmid) / (out.vmid - out.vbot);
                out.curr = (out.vtop - out.vmid) / out.r1;
                // out.vtop = out.vmid + out.curr * out.r1;
                // out.vmid = out.vtop - out.curr * out.r1;
                // out.r1 = (out.vtop - out.vmid) / out.curr;
                };
            int ctr;
            for (ctr = 0; ctr < 10; ctr++) {
                compute();
                // Special case at the root of r1<0, r2<0, or curr<0 problems
                if (out.vbot > out.vmid) {
                    if (vt == RCCore::VBOT)
                        out.vbot = out.vmid - (out.vtop - out.vmid) * (0.0 / out.r1);
                    else if (vt == RCCore::VMID)
                        out.vmid = (out.vtop * 0.0 + out.vbot * out.r1) / (out.r1 + 0.0);
                    else
                        throw_line("Shouldn't get here");}
                else if (out.vtop < out.vmid) {
                    if (vt == RCCore::VTOP)
                        out.vtop = out.vtop = (out.r1 / rmax) * (out.vmid - out.vbot) + out.vmid;
                    else if (vt == RCCore::VMID)
                        out.vmid = (out.vtop * rmax + out.vbot * out.r1) / (out.r1 + rmax);
                    else
                        throw_line("Shouldn't get here");}
                else if (gtb(out.r2, rmax))      {
                    if      (vt == RCCore::VTOP)
                        out.vtop = (out.r1 / rmax) * (out.vmid - out.vbot) + out.vmid;
                    else if (vt == RCCore::VBOT)
                        out.vbot = out.vmid - (out.vtop - out.vmid) * (rmax / out.r1);
                    else if (vt == RCCore::VMID)
                        out.vmid = (out.vtop * rmax + out.vbot * out.r1) / (out.r1 + rmax);
                    else if (vt == RCCore::R1)
                        out.r1   = rmax * (out.vtop - out.vmid) / (out.vmid - out.vbot);
                    else
                        throw_line("Shouldn't get here");}
                else if (gtb(out.curr, currmax)) {
                    if      (vt == RCCore::VTOP)
                        out.vtop = out.vmid + currmax * out.r1;
                    else if (vt == RCCore::VMID)
                        out.vmid = out.vtop - currmax * out.r1;
                    else if (vt == RCCore::R1)
                        out.r1 = (out.vtop - out.vmid) / currmax;
                    else
                        throw_line("Shouldn't get here");}
                else break;
            }
            check_ctr(ctr);
            break;}
        default:
            throw_line( "Error in logic" );

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
// RCCore::RCCore() :
//     inVals (Vals()),
//     outVals (Vals())
//     {}

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
        default: throw_line("whoops!");
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

double RCCore::getVul() const {return vul;}
double RCCore::getVbl() const {return vbl;}
double RCCore::getRmax() const {return rmax;}
double RCCore::getCurrmax() const {return currmax;}
void   RCCore::setVul(double x) {vul = x;};
void   RCCore::setVbl(double x) {vbl = x; };
void   RCCore::setRmax(double x) {rmax = x;};
void   RCCore::setCurrmax(double x) {currmax = x; };










