#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <list>
#include <algorithm>
#include <cassert>
#include <stdexcept>

#include "rctypes.h"
#include "vals.h"
#include "rccore.h"


void throw_line(std::string msg, const std::shared_ptr<UIBridgeInterface> b, int line) {
    std::ostringstream s;
    s << msg << " " << __FILE__ << ":" << line;
    std::string errstr = s.str();
    std::cout << errstr;
    if (b)
        b->errorMsg(errstr);
        throw std::logic_error(errstr);
}

#define throw_line(msg) throw_line(msg, ibridge, __LINE__);


//#define throw_line(msg) \
//    do { \
//        std::ostringstream s; \
//        s << msg << " " << __FILE__ << ":" << __LINE__; \
//        std::string errstr = s.str(); \
//        std::cout << errstr; \
//        if (ibridge) \
//            ibridge->errorMsg(errstr); \
//        throw std::logic_error(errstr); \
//    } while (0)
 
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
void RCCore::setEngaged(bool en) {
    engaged = en;
    update();

}
bool RCCore::getEngaged() const {
    return engaged;
}
Vals RCCore::swapInputs(Vals vals) const {
    // Set inputs according to list
    // If a vartype is in the argument vals, then it is being set by someone,
    // so it is considered an input
    Vals ovals = vals;
    auto find = [&](vartype vt) {return std::find(latest4.begin(), latest4.end(), vt) != latest4.end();};
    ovals.vtopd = find(VTOP) ? INPUT : OUTPUT;
    ovals.vbotd = find(VBOT) ? INPUT : OUTPUT;
    ovals.vmidd = find(VMID) ? INPUT : OUTPUT;
    ovals.r1d   = find(R1)   ? INPUT : OUTPUT;
    ovals.r2d   = find(R2)   ? INPUT : OUTPUT;
    ovals.currd = find(CURR) ? INPUT : OUTPUT;
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
    const double eps = 1e-8;
    return ((a - b) >= eps);
}
bool ltb(double a, double b) {
    // return true if a < b by a certain amount
    const double eps = 1e-8;
    return ((b - a) >= eps);
}

Vals RCCore::calc_group(vartype latestvt, Vals invals) const {
    // Update one set of values from the previous set
    /*      INPUTS
            vtop vbot vmid r1  r2 curr  comment
    0x0f      0    0    1   1   1   1	
    0x17      0    1    0   1   1   1	
    0x1b      0    1    1   0   1   1	mixed
    0x1d      0    1    1   1   0   1	
    0x1e      0    1    1   1   1   0	
    0x27      1    0    0   1   1   1
    0x2b      1    0    1   0   1   1	
    0x2d      1    0    1   1   0   1	mixed
    0x2e      1    0    1   1   1   0  
    0x33      1    1    0   0   1   1
    0x35      1    1    0   1   0   1	
    0x36      1    1    0   1   1   0	
    0x39      1    1    1   0   0   1	
    0x3a      1    1    1   0   1   0	
    0x3c      1    1    1   1   0   0	*/

    // Below we enumerate all the states from which a single selected
    // slider would lead to a mixed-constraint state.  A mixed constraint
    // state is one in which you can't compute one of the resistors.
    // Specifically there are two: vtop, vmid, r1, curr; and
    // vbot, vmid, r2, curr.  In the first, the top loop is overconstrained
    // and the bottom is underconstrained, and vice-versa for the other one.
    // We identify the otherwise valid states that could lead to an invalid
    // state and prevent the slider from being selected.

    // bit 2 down (r1), bit 0 up
    // From 1e    01 1110 // 1e -> r1 last, disable curr
    // to   1b    01 1011

    // bit 2 down (r1), bit 1 up
    // From 1d    01 1101 // 1d -> r1 last, disable r2
    // to   1b    01 1011

    // bit 2 down (r1), bit 3 up
    // From 17    01 0111 // 17 -> r1 last, disable vmid
    // to   1b    01 1011

    // bit 2 down (r1), bit 4 up
    // From 0f    00 1111 // 0f -> r1 last, disable vbot
    // to   1b    01 1011

    // bit 5 (vtop) down, bit 0 up
    // From 3a    11 1010 // 3a -> vtop last, disable curr
    // to   1b,   01 1011,

    // bit 5 down (vtop), bit 1 up
    // From 39    11 1001 // 39 -> vtop last, disable r2
    // to   1b,   01 1011,

    // bit 5 down (vtop), bit 3 up
    // From 33    11 0011 // 33 -> vtop last, disable vmid
    // to   1b,   01 1011,

    // bit 5 down (vtop), bit 4 up
    // From 2b    10 1011 // 2b -> vtop last, disable vbot
    // to   1b,   01 1011,


    // bit 1 down (r2), bit 0 up (curr)
    // from 2e    10 1110
    // to   2d    10 1101

    // bit 1 down (r2), bit 2 up (r1)
    // from 2b    10 1011
    // to   2d    10 1101

    // bit 1 down (r2), bit 3 up (vmid)
    // from 27    10 0111
    // to   2d    10 1101

    // bit 1 down (r2), bit 5 up (vtop)
    // from 0f    00 1111
    // to   2d    10 1101

    // bit 4 down (vbot), bit 0 up (curr)
    // from 3c    11 1100
    // to   2d    10 1101

    // bit 4 down (vbot), bit 2 up (r1)
    // from 39    11 1001
    // to   2d    10 1101

    // bit 4 down (vbot), bit 3 up (vmid)
    // from 35    11 0101
    // to   2d    10 1101

    // bit 4 down (vbot), bit 5 up (vtop)
    // from 1d    01 1101
    // to   2d    10 1101


    int icode = invals.incode();
    Vals out = clip(invals, icode);
    auto check_ctr = [this](int ctr) {
        if (ctr >= 10)
            throw_line("Too many counts");
    };

    switch(icode) {
        case 0x0f: {
            // Vtop, Vbot = f(vmid, r1, r2, curr)
            out.vtopd = OUTPUT;
            out.vbotd = OUTPUT;
            auto compute = [&]() {
                out.vtop = out.vmid + out.curr * out.r1;
                // out.vmid = out.vtop - out.curr * out.r1;
                // out.r1 = (out.vtop - out.vmid) / out.curr;
                // out.curr = (out.vtop - out.vmid) / out.r1;
                out.vbot = out.vmid - out.curr * out.r2;
                // out.vmid = out.vbot + out.curr * out.r2;
                // out.r2 = (out.vmid - out.vbot) / out.curr;
                // out.curr = (out.vbmid - out.vbot) / out.r2;
                out.disable = latest4.front() == R1 ? VBOT : 
                              latest4.front() == R2 ? VTOP : NONE;
            };
            int ctr;
            for (ctr = 0; ctr < 10; ctr++) {
                compute();
                if (gtb(out.vtop, vul)) {
                    if      (latestvt == VMID)
                        out.vmid = vul - out.curr * out.r1;
                    else if (latestvt == R1)
                        out.r1 = (vul - out.vmid) / out.curr;
                    else if (latestvt == CURR)
                        out.curr = (vul - out.vmid) / out.r1;
                    else if (latestvt == VMID)
                        out.vmid = vul - out.curr * out.r1;
                }
                else if (ltb(out.vtop, vbl)) {
                    if      (latestvt == VMID)
                        out.vmid = vbl - out.curr * out.r1;
                    else if (latestvt == R1)
                        out.r1 = (vbl - out.vmid) / out.curr;
                    else if (latestvt == CURR)
                        out.curr = (vbl - out.vmid) / out.r1;
                    else if (latestvt == VMID)
                        out.vmid = vbl - out.curr * out.r1;
                }
                else if (gtb(out.vbot, vul)) {
                    if      (latestvt == VMID)
                        out.vmid = vul + out.curr * out.r2;
                    else if (latestvt == R2)
                        out.r2 = (out.vmid - vul) / out.curr;
                    else if (latestvt == CURR)
                        out.curr = (out.vmid - vul) / out.r2;

                }
                else if (ltb(out.vbot, vbl)) {
                    if      (latestvt == VMID)
                        out.vmid = vbl + out.curr * out.r2;
                    else if (latestvt == R2)
                        out.r2 = (out.vmid - vbl) / out.curr;
                    else if (latestvt == CURR)
                        out.curr = (out.vmid - vbl) / out.r2;
                }
                else break;
            }
            check_ctr(ctr);
            break;}
        case 0x17: {
            // Vtop, vmid = f(vbot, r1, r2, curr)
            out.vtopd = OUTPUT;
            out.vmidd = OUTPUT;
            auto compute = [&]() {
                out.vtop = out.vbot + out.curr * (out.r1 + out.r2);
                // out.vbot = out.vtop - out.curr * (out.r1 + out.r2);
                // out.r1   = (out.vtop - out.vbot) / out.curr - out.r2;
                // out.r2   = (out.vtop - out.vbot) / out.curr - out.r1;
                // out.curr = (out.vtop - out.vbot) / (out.r1 + out.r2);
                out.vmid = out.vbot + out.curr * out.r2;
                // out.vbot = out.vmid - out.curr * out.r2;
                // out.r2   = (out.vmid - out.vbot) / out.curr;
                // out.curr = (out.vmid - out.vbot) / out.r2;
                out.disable = latest4.front() == R1 ? VMID : NONE;
            };
            int ctr;
            for (ctr = 0; ctr < 10; ctr++) {
                compute();
                if (gtb(out.vtop, vul)) {
                    if      (latestvt == VBOT)
                        out.vbot = vul - out.curr * (out.r1 + out.r2);
                    else if (latestvt == R1)
                        out.r1 = (vul - out.vbot) / out.curr - out.r2;
                    else if (latestvt == R2)
                        out.r2 = (vul - out.vbot) / out.curr - out.r1;
                    else if (latestvt == CURR)
                        out.curr = (vul - out.vbot) / (out.r1 + out.r2);
                }
                else if (ltb(out.vtop, vbl)) {
                    if      (latestvt == VBOT)
                        out.vbot = vbl - out.curr * (out.r1 + out.r2);
                    else if (latestvt == R1)
                        out.r1 = (vbl - out.vbot) / out.curr - out.r2;
                    else if (latestvt == R2)
                        out.r2 = (vbl - out.vbot) / out.curr - out.r1;
                    else if (latestvt == CURR)
                        out.curr = vbl - out.vbot / (out.r1 + out.r2);
                }
                else if (gtb(out.vmid, vul)) {
                    if      (latestvt == VBOT)
                        out.vbot = vul - out.curr * out.r2;
                    else if (latestvt == R2)
                        out.r2 = (vul - out.vbot) / out.curr;
                    else if (latestvt == CURR)
                        out.vbot = vul - out.curr * out.r2;
                }
                else if (ltb(out.vmid, vbl)) {
                    if      (latestvt == VBOT)
                        out.vbot = vbl - out.curr * out.r2;
                    else if (latestvt == R2)
                        out.vbot = vbl - out.curr * out.r2;
                    else if (latestvt == CURR)
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
            out.vtopd = OUTPUT;
            out.r2d   = OUTPUT;
            auto compute = [&]() {
                out.vtop = out.vmid + out.curr * out.r1;
                // out.vmid = out.vtop - out.curr * out.r1;
                // out.r1 = (out.vtop - out.vmid) / out.curr;
                // out.curr = (out.vtop - out.vmid) / out.r1;
                out.r2   = (out.vmid - out.vbot) / out.curr;
                // out.vbot = out.vmid - (out.r2 * out.curr);
                // out.vmid = (out.r2 * out.curr) + out.vbot;
                // out.curr = (out.vmid - out.vbot) / out.r2;
                out.disable = latest4.front() == R1   ? R2 : 
                              latest4.front() == VBOT ? VTOP : NONE;
            };
            int ctr;
            for (ctr = 0; ctr < 10; ctr++) {
                compute();
                if (gtb(out.vtop, vul)) {
                    if      (latestvt == VMID)
                        out.vmid = vul - out.curr * out.r1;
                    else if (latestvt == R1)
                        out.r1 = (vul - out.vmid) / out.curr;
                    else if (latestvt == CURR)
                        out.curr = (vul - out.vmid) / out.r1;
                }
                else if (ltb(out.vtop, vbl)) {
                    if      (latestvt == VMID)
                        out.vmid = vbl - out.curr * out.r1;
                    else if (latestvt == R1) 
                        out.r1 = (vbl - out.vmid) / out.curr;
                    else if (latestvt == CURR) 
                        out.curr = (vbl - out.vmid) / out.r1;
                }
                else if (gtb(out.r2, rmax)) {
                    if      (latestvt == VBOT) 
                        out.vbot = out.vmid - (rmax * out.curr);
                    else if (latestvt == VMID) 
                        out.vmid = (rmax * out.curr) + out.vbot;
                    else if (latestvt == CURR) 
                        out.curr = (out.vmid - out.vbot) / rmax;
                }
                else if (ltb(out.r2, 0.0)) {
                    if      (latestvt == VBOT) 
                        out.vbot = out.vmid - (0.0 * out.curr);
                    else if (latestvt == VMID) 
                        out.vmid = (0.0 * out.curr) + out.vbot;
                    //else if (latestvt == CURR)
                        //out.curr = (out.vmid - out.vbot) / 0.0;

                }
                else break;
            }
            check_ctr(ctr);
            break;}
        case 0x1e: {
            // vtop, curr = f(vbot, vmid, r1, r2)
            out.vtopd = OUTPUT;
            out.currd = OUTPUT;
            auto compute = [&]() {
                out.vtop = out.vmid * (1 + out.r1 / out.r2) - out.vbot * (out.r1 / out.r2);
                // out.vbot = out.vmid * (1 + out.r1 / out.r2) - out.vtop;
                // out.vmid = (out.vtop + out.vbot * (out.r1 / out.r2)) / (1 + out.r1 / out.r2);
                // out.r1 = out.r2 * (out.vtop - out.vmid) / (out.vmid - out.vbot);
                // out.r2 = out.r1 * (out.vmid - out.vbot) / (out.vtop - out.vmid);
                out.curr = (out.vmid - out.vbot) / out.r2;
                // out.vbot = out.vmid - out.curr * out.r2;
                // out.vmid = out.curr * out.r2 + out.vbot;
                // out.r2 = (out.vmid - out.vbot) / out.curr;
                out.disable = latest4.front() == R1 ? CURR : NONE;
            };
            int ctr;
            for (ctr = 0; ctr < 10; ctr++) {
                compute();
                if (gtb(out.vtop, vul)) {
                    if (latestvt == VBOT)
                        out.vbot = out.vmid * (1 + out.r1 / out.r2) - vul;
                    else if (latestvt == VMID)
                        out.vmid = (vul + out.vbot * (out.r1 / out.r2)) / (1 + out.r1 / out.r2);
                    else if (latestvt == R1)
                        out.r1 = out.r2 * (vul - out.vmid) / (out.vmid - out.vbot);
                    else if (latestvt == R2)
                        out.r2 = out.r1 * (out.vmid - out.vbot) / (vul - out.vmid);
                }
                else if (ltb(out.vtop, vbl)) {
                    if (latestvt == VBOT)
                        out.vbot = out.vmid * (1 + out.r1 / out.r2) - vbl;
                    else if (latestvt == VMID)
                        out.vmid = (vbl + out.vbot * (out.r1 / out.r2)) / (1 + out.r1 / out.r2);
                    else if (latestvt == R1)
                        out.r1 = out.r2 * (vbl - out.vmid) / (out.vmid - out.vbot);
                    else if (latestvt == R2)
                        out.r2 = out.r1 * (out.vmid - out.vbot) / (vbl - out.vmid);
                }
                else if (gtb(out.curr, currmax)) {
                    if (latestvt == VBOT)
                        out.vbot = out.vmid - currmax * out.r2;
                    else if (latestvt == VMID)
                        out.vmid = currmax * out.r2 + out.vbot;
                    else if (latestvt == R2)
                        out.r2 = (out.vmid - out.vbot) / currmax;
                }
                else if (ltb(out.curr, 0.0)) {
                    if (latestvt == VBOT)
                        out.vbot = out.vmid - 0.0 * out.r2;
                    else if (latestvt == VMID)
                        out.vmid = 0.0 * out.r2 + out.vbot;
                    //else if (latestvt == R2);
                        //out.r2 = (out.vmid - out.vbot) / 0.0;
                }
                else break;
            }
            check_ctr(ctr);
            break;}
        case 0x27: {
            // vbot, vmid = f(vtop, r1, r2 curr)
            out.vbotd = OUTPUT;
            out.vmidd = OUTPUT;
            auto compute = [&]() {
                out.vbot = out.vtop - out.curr * (out.r1 + out.r2);
                // out.vtop = out.vbot + out.curr * (out.r1 + out.r2);
                // out.r1 = ((out.vtop - out.vbot) / out.curr) - out.r2;
                // out.r2 = ((out.vtop - out.vbot) / out.curr) - out.r1;
                // out.curr = (out.vtop - out.vbot) / (out.r1 + out.r2)
                out.vmid = out.vtop - out.curr * out.r1;
                // out.vtop = out.vmid + out.curr * out.r1;
                // out.r1 = (out.vtop - out.vmid) / out.curr;
                // out.curr = (out.vtop - out.vmid) / out.r1;
                out.disable = latest4.front() == R2 ? VMID : NONE;
            };
            int ctr;
            for (ctr = 0; ctr < 10; ctr++) {
                compute();
                if (gtb(out.vbot, vul)) {
                    if (latestvt == VTOP)
                        out.vtop = vul + out.curr * (out.r1 + out.r2);
                    else if (latestvt == R1)
                        out.r1 = ((out.vtop - vul) / out.curr) - out.r2;
                    else if (latestvt == R2)
                        out.r2 = ((out.vtop - vul) / out.curr) - out.r1;
                    else if (latestvt == CURR)
                        out.curr = (out.vtop - vul) / (out.r1 + out.r2);
                }
                else if (ltb(out.vbot, vbl)) {
                    if (latestvt == VTOP)
                        out.vtop = vbl + out.curr * (out.r1 + out.r2);
                    else if (latestvt == R1)
                        out.r1 = ((out.vtop - vbl) / out.curr) - out.r2;
                    else if (latestvt == R2)
                        out.r2 = ((out.vtop - vbl) / out.curr) - out.r1;
                    else if (latestvt == CURR)
                        out.curr = (out.vtop - vbl) / (out.r1 + out.r2);
                } 
                else if (gtb(out.vmid, vul)) {
                    if (latestvt == VTOP)
                        out.vtop = vul + out.curr * out.r1;
                    else if (latestvt == R1)
                        out.r1 = (out.vtop - vul) / out.curr;
                    else if (latestvt == CURR)
                        out.curr = (out.vtop - vul) / out.r1;
                } 
                else if (ltb(out.vmid, vbl)) {
                    if (latestvt == VTOP)
                        out.vtop = vbl + out.curr * out.r1;
                    else if (latestvt == R1)
                        out.r1 = (out.vtop - vbl) / out.curr;
                    else if (latestvt == CURR)
                        out.curr = (out.vtop - vbl) / out.r1;
                } 
                else break;
            }
            check_ctr(ctr);
            break;}
        case 0x2b: {
            // vbot, r1 = f(vtop, vmid, r2, curr)
            out.vbotd = OUTPUT;
            out.r1d   = OUTPUT;
            auto compute = [&]() {
                out.vbot  = out.vmid - out.curr * out.r2;
                // out.vmid = out.vbot + out.curr * out.r2;
                // out.r2 = (out.vmid - out.vbot) / out.curr;
                // out.curr = (out.vmid - out.vbot) / out.r2;
                out.r1    = (out.vtop - out.vmid) / out.curr;
                // out.vtop = out.r1 * out.curr + out.vmid;
                // out.vmid = out.vtop - (out.r1 * out.curr);
                // out.curr = (out.vtop - out.vmid) / out.r1;
                out.disable = latest4.front() == VTOP ? VBOT : 
                              latest4.front() == R2 ? R1 : NONE;
            };
            int ctr;
            for (ctr = 0; ctr < 10; ctr++) {
                compute();
                if      (gtb(out.vbot, vul)){
                    if (latestvt == VMID)
                        out.vmid = vul + out.curr * out.r2;
                    else if (latestvt == R2)
                        out.r2 = (out.vmid - vul) / out.curr;
                    else if (latestvt == CURR)
                        out.curr = (out.vmid - vul) / out.r2;
                }
                else if (ltb(out.vbot, vbl)){
                    if (latestvt == VMID)
                        out.vmid = vbl + out.curr * out.r2;
                    else if (latestvt == R2)
                        out.r2 = (out.vmid - vbl) / out.curr;
                    else if (latestvt == CURR)
                        out.curr = (out.vmid - vbl) / out.r2;
                }
                else if (gtb(out.r1, rmax)) {
                    if (latestvt == VTOP)
                        out.vtop = rmax * out.curr + out.vmid;
                    else if (latestvt == VMID)
                        out.vmid = out.vtop - (rmax * out.curr);
                    else if (latestvt == CURR)
                        out.curr = (out.vtop - out.vmid) / rmax;
                }
                else if (ltb(out.r1, 0.0)){
                    if (latestvt == VTOP)
                        out.vtop = 0.0 * out.curr + out.vmid;
                    else if (latestvt == VMID)
                        out.vmid = out.vtop - (0.0 * out.curr);
                    //else if (latestvt == CURR);
                        //out.curr = (out.vtop - out.vmid) / 0.0;
                }
                else break;
            }
            check_ctr(ctr);
            break; }
        case 0x2d:
            throw_line( "Error in logic" );
        case 0x2e: {
            // vbot, curr = f(vtop, vmid, r1, r2)
            out.vbotd = OUTPUT;
            out.currd = OUTPUT;
            auto compute = [&]() {
                out.vbot = out.vmid - (out.r2 / out.r1) * (out.vtop - out.vmid);
                // out.vtop = (out.vmid - out.vbot) * (out.r1 / out.r2) + out.vmid;
                // out.vmid = (out.vbot + out.vtop * out.r2 / out.r1) / (1+ out.r2 / out.r1);
                // out.r1 = out.r2 * (out.vtop - out.vmid) / (out.vmid - out.vbot);
                // out.r2 = out.r1 * ((out.vmid - out.vbot)) / (out.vtop - out.vmid);
                out.curr = (out.vtop - out.vmid) / out.r1;
                // out.vtop = out.curr * out.r1 + out.vmid;
                // out.vmid = out.vtop - (out.curr * out.r1);
                // out.r1 = (out.vtop - out.vmid) / out.curr;
                out.disable = latest4.front() == R2 ? CURR : NONE;
            };
            int ctr;
            for (ctr = 0; ctr < 10; ctr++) {
                compute();
                if      (gtb(out.vbot, vul)) {
                    if (latestvt == VTOP)
                        out.vtop = (out.vmid - vul) * (out.r1 / out.r2) + out.vmid;
                    else if (latestvt == VMID)
                        out.vmid = (vul + out.vtop * out.r2 / out.r1) / (1+ out.r2 / out.r1);
                    else if (latestvt == R1)
                        out.r1 = out.r2 * (out.vtop - out.vmid) / (out.vmid - vul);
                    else if (latestvt == R2)
                        out.r2 = out.r1 * ((out.vmid - vul)) / (out.vtop - out.vmid);
                }
                else if (ltb(out.vbot, vbl)) {
                    if (latestvt == VTOP)
                        out.vtop = (out.vmid - vbl) * (out.r1 / out.r2) + out.vmid;
                    else if (latestvt == VMID)
                        out.vmid = (vbl + out.vtop * out.r2 / out.r1) / (1+ out.r2 / out.r1);
                    else if (latestvt == R1)
                        out.r1 = out.r2 * (out.vtop - out.vmid) / (out.vmid - vbl);
                    else if (latestvt == R2)
                        out.r2 = out.r1 * ((out.vmid - vbl)) / (out.vtop - out.vmid);
                }
                else if (gtb(out.curr, currmax)) {
                    if (latestvt == VTOP)
                        out.vtop = currmax * out.r1 + out.vmid;
                    else if (latestvt == VMID)
                        out.vmid = out.vtop - (currmax * out.r1);
                    else if (latestvt == R1)
                        out.r1 = (out.vtop - out.vmid) / currmax;
                }
                else if (ltb(out.curr, 0.0)) {
                    if (latestvt == VTOP)
                        out.vtop = 0.0 * out.r1 + out.vmid;
                    else if (latestvt == VMID)
                        out.vmid = out.vtop - (0.0 * out.r1);
                    //else if (latestvt == R1);
                        //out.r1 = (out.vtop - out.vmid) / 0.0;
                }
                else break;
            }
            check_ctr(ctr);
            break;}
        case 0x33: {
            // vmid, r1 = f(vtop, vbot, r2, curr)
            out.vmidd = OUTPUT;
            out.r1d   = OUTPUT;
            auto compute = [&]() {
                out.vmid = out.vbot + out.curr * out.r2;
                // out.vbot = out.vmid - out.curr * out.r2
                // out.r2   = (out.vmid - out.vbot) / out.curr;
                // out.curr = (out.vmid - out.vbot) / out. r2;
                out.r1   = (out.vtop - out.vbot - out.curr * out.r2) / out.curr;
                // out.vtop = (out.r1 * out.curr) + out.vbot + out.curr * out.r2;
                // out.vbot = out.vtop - (out.r1 * out.curr) - out.curr * out.r2;
                // out.r2   = (out.vtop - out.vbot) / out.curr + out.r1;
                // out.curr = (out.vtop - out.vbot) / (out.r1 + out.r2)
                out.disable = latest4.front() == VTOP ? VMID : NONE;
            };
            int ctr;
            for (ctr = 0; ctr < 10; ctr++) {
                compute();
                if (gtb(out.vmid, vul)) {
                    if      (latestvt == VBOT)
                        out.vbot = vul - out.curr * out.r2;
                    else if (latestvt == R2)
                        out.r2   = (vul - out.vbot) / out.curr;
                    else if (latestvt == CURR)
                		out.curr = (vul - out.vbot) / out. r2;
                    else break;}
                else if (ltb(out.vmid, vbl)) {
                    if      (latestvt == VBOT)
                		out.vbot = vbl - out.curr * out.r2;
                    else if (latestvt == R2)
                		out.r2   = (vbl - out.vbot) / out.curr;
                    else if (latestvt == CURR)
                		out.curr = (vbl - out.vbot) / out. r2;
                    else break;}
                else if (gtb(out.r1, rmax)) {
                    if      (latestvt == VTOP)
                		out.vtop = (rmax * out.curr) + out.vbot + out.curr * out.r2;
                    else if (latestvt == VBOT)
                		out.vbot = out.vtop - (rmax * out.curr) - out.curr * out.r2;
                    else if (latestvt == R2)
                		out.r2   = (out.vtop - out.vbot) / out.curr + rmax;
                    else if (latestvt == CURR)
                		out.curr = (out.vtop - out.vbot) / (rmax + out.r2);
                    else break;}
                else if (ltb(out.r1, 0.0)) {
                    if      (latestvt == VTOP)
                		out.vtop = (0.0 * out.curr) + out.vbot + out.curr * out.r2;
                    else if (latestvt == VBOT)
                		out.vbot = out.vtop - (0.0 * out.curr) - out.curr * out.r2;
                    else if (latestvt == R2)
                		out.r2   = (out.vtop - out.vbot) / out.curr + 0.0;
                    else if (latestvt == CURR)
                		out.curr = (out.vtop - out.vbot) / (0.0 + out.r2);
                    else break;}
                else break;
            }
            check_ctr(ctr);
            break;}
        case 0x35:{
            // vmid, r2 = f(vtop, vbot r1, curr)
            out.vmidd = OUTPUT;
            out.r2d   = OUTPUT;
            auto compute = [&]() {
                out.vmid = out.vtop - out.curr * out.r1;
                // out.vtop = out.vmid + out.curr * out.r1
                // out.r1 = (out.vtop - out.vmid) / out.curr
                // out.curr = (out.vtop - out.vmid) / out.r1
                out.r2 = (out.vtop - out.vbot) / out.curr - out.r1;
                // out.vtop = out.curr * (out.r1 + out.r2)  + out.vbot;
                // out.vbot = out.vtop - out.curr * (out.r1 + out.r2);
                // out.r1   = (out.vtop - out.vbot) / out.curr - out.r2;
                // out.curr = (out.vtop - out.vbot) / (out.r1 + out.r2);
                out.disable = latest4.front() == VBOT ? VMID : NONE;
                };
            int ctr;
            for (ctr = 0; ctr < 10; ctr++) {
                compute();
                if (gtb(out.vmid, vul)) {
                    if      (latestvt == VTOP)
                		out.vtop = vul + out.curr * out.r1;
                    else if (latestvt == R1)
                		out.r1 = (out.vtop - vul) / out.curr;
                    else if (latestvt == CURR)
                		out.curr = (out.vtop - vul) / out.r1;
                    else break;}
                else if (ltb(out.vmid, vbl)) {
                    if      (latestvt == VTOP)
                		out.vtop = vbl + out.curr * out.r1;
                    else if (latestvt == R1)
                        out.r1 = (out.vtop - vbl) / out.curr;
                    else if (latestvt == CURR)
                		out.curr = (out.vtop - vbl) / out.r1;
                    else break;}
                else if (gtb(out.r2, rmax))  {
                    if      (latestvt == VTOP)
                        out.vtop = out.curr * (out.r1 + rmax)  + out.vbot;
                    else if (latestvt == VBOT)
               		    out.vbot = out.vtop - out.curr * (out.r1 + rmax);
                    else if (latestvt == R1)
                        out.r1   = (out.vtop - out.vbot) / out.curr - rmax;
                    else if (latestvt == CURR)
                        out.curr = (out.vtop - out.vbot) / (out.r1 + rmax);
                    else break;}
                else if (ltb(out.r2, 0.0))   {
                    if      (latestvt == VTOP)
                		out.vtop = out.curr * (out.r1 + 0.0)  + out.vbot;
                    else if (latestvt == VBOT)
               		    out.vbot = out.vtop - out.curr * (out.r1 + 0.0);
                    else if (latestvt == R1)
                        out.r1   = (out.vtop - out.vbot) / out.curr - 0.0;
                    else if (latestvt == CURR)
                        out.curr = (out.vtop - out.vbot) / (out.r1 + 0.0);
                    else break;}
                else break;
            }
            check_ctr(ctr);
            break;}
        case 0x36:{
            // vmid, curr = f(vtop, vbot, r1, r2)
            out.vmidd = OUTPUT;
            out.currd = OUTPUT;
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
                    if      (latestvt == VTOP)
                        out.vtop = (vul * ( out.r1 + out.r2) - out.vbot / out.r2) + out.vbot;
                    else if (latestvt == VBOT)
                        out.vbot = vul - (out.r2 * (out.vtop - out.vbot)) / (out.r1 + out.r2);
                    else if (latestvt == R1)
                        out.r1 = (vul - out.r2 * (out.vtop - out.vbot) / out.vbot) - out.r2;
                    else if (latestvt == R2)
                        out.r2 = ((vul - out.vbot * out.r1)) / out.vtop;
                    else break;}
                else if (ltb(out.vmid, vbl))     {
                    if      (latestvt == VTOP)
                        out.vtop = (vbl * ( out.r1 + out.r2) - out.vbot / out.r2) + out.vbot;
                    else if (latestvt == VBOT)
                        out.vbot = vbl - (out.r2 * (out.vtop - out.vbot)) / (out.r1 + out.r2);
                    else if (latestvt == R1)
                        out.r1 = (vbl - out.r2 * (out.vtop - out.vbot) / out.vbot) - out.r2;
                    else if (latestvt == R2)
                        out.r2 = ((vbl - out.vbot * out.r1)) / out.vtop;
                    else break;}
                else if (gtb(out.curr, currmax)) {
                    if      (latestvt == VTOP)
                		out.vtop = currmax * (out.r1 + out.r2) + out.vbot;
                    else if (latestvt == VBOT)
                		out.vbot = out.vtop - currmax * (out.r1 + out.r2);
                    else if (latestvt == R1)
                		out.r1 = (out.vtop - out.vbot) / currmax - out.r2;
                    else if (latestvt == R2)
                		out.r2 = (out.vtop - out.vbot) / currmax - out.r1;
                    else break;}
                else if (ltb(out.curr, 0.0))     {
                    if      (latestvt == VTOP)
                		out.vtop = 0.0 * (out.r1 + out.r2) + out.vbot;
                    else if (latestvt == VBOT)
                		out.vbot = out.vtop - 0.0 * (out.r1 + out.r2);
                    else if (latestvt == R1);
                        //out.r1 = (out.vtop - out.vbot) / 0.0 - out.r2;
                    else if (latestvt == R2);
                        //out.r2 = (out.vtop - out.vbot) / 0.0 - out.r1;
                    else break;}
                else break;
            }
            check_ctr(ctr);
            break;}
        case 0x39:{
            // r1, r2 = f(vtop, vbot, vmid, curr)
            out.r1d = OUTPUT;
            out.r2d = OUTPUT;
            auto compute = [&]() {
                out.r1 = (out.vtop - out.vmid) / out.curr;
                // out.vtop = out.r1 * out.curr + out.vmid
                // out.vmid = out.vtop - out.r1 * out.curr
                // out.curr = (out.vtop -out.vmid) / out.r1
                out.r2 = (out.vmid - out.vbot) / out.curr;
                // out.vbot = out.vmid - out.r2 * out.curr
                // out.vmid = out.r2 * out.curr + out.vbot
                // out.curr = (out.vmid - out.vbot) / out.r2
                out.disable = latest4.front() == VTOP ? R2 : 
                              latest4.front() == VBOT ? R1 : NONE;                
                };
            int ctr;
            for (ctr = 0; ctr < 10; ctr++) {
                compute();
                if (gtb(out.r1, rmax)) {
                    if      (latestvt == VTOP)
                		   out.vtop = rmax * out.curr + out.vmid;
                    else if (latestvt == VMID)
                		out.vmid = out.vtop - rmax * out.curr;
                    else if (latestvt == CURR)
                		out.curr = (out.vtop - out.vmid) / rmax;
                    else break;}
                else if (ltb(out.r1, 0.0))  {
                    if      (latestvt == VTOP)
                		out.vtop = 0.0 * out.curr + out.vmid;
                    else if (latestvt == VMID)
                		out.vmid = out.vtop - 0.0 * out.curr;
                    else if (latestvt == CURR);
                        // out.curr = (out.vtop - out.vmid) / 0.0;
                    else break;}
                else if (gtb(out.r2, rmax)) {
                    if      (latestvt == VBOT)
                		out.vbot = out.vmid - rmax * out.curr;
                    else if (latestvt == VMID)
                		out.vmid = rmax * out.curr + out.vbot;
                    else if (latestvt == CURR)
                		out.curr = (out.vmid - out.vbot) / rmax;
                    else break;}
                else if (ltb(out.r2, 0.0))  {
                    if      (latestvt == VBOT)
                		out.vbot = out.vmid - 0.0 * out.curr;
                    else if (latestvt == VMID)
                		out.vmid = 0.0 * out.curr + out.vbot;
                    else if (latestvt == CURR);
                        // out.curr = (out.vmid - out.vbot) / 0.0;
                    else break;}
                else break;
            }
            check_ctr(ctr);
            break;}
        case 0x3a:{
            // r1, curr = f(vtop, vbot, vmid, r2)
            out.r1d   = OUTPUT;
            out.currd = OUTPUT;
            Vals lastout;
            auto compute = [&]() {
                out.r1   = out.r2 * (out.vtop - out.vmid) / (out.vmid - out.vbot);
                // out.vtop = (out.r1 / out.r2) * (out.vmid - out.vbot) + out.vmid;
                // out.vbot = out.vmid - (out.vtop - out.vmid) * (out.r2 / out.r1);
                // out.vmid = (out.vtop * out.r2 + out.vbot * out.r1) / (out.r1 + out.r2);
                // out.r2 = out.r1 * (out.vmid - out.vbot) / (out.vtop - out.vmid);
                out.curr = (out.vmid - out.vbot) / out.r2;
                // out.vbot = out.vmid - out.curr * out.r2
                // out.vmid = out.curr * out.r2 + out.vbot
                // out.r2 = (out.vmid - out.vbot) / out.curr
                out.disable = (latest4.front()) == VTOP ? CURR : NONE;
                };
            int ctr;
            for (ctr = 0; ctr < 10; ctr++) {
                if (out == lastout) break; // stop the obsession
                lastout = out;
                compute();
                // Special case at the root of r1<0, r2<0, or curr<0 problems
                if (out.vbot > out.vmid) {
                    if (latestvt == VBOT)
                        out.vbot = out.vmid - (out.vtop - out.vmid) * (out.r2 / rmax);
                    else if (latestvt == VMID)
                        out.vmid = (out.vtop * out.r2 + out.vbot * rmax) / (rmax + out.r2);
                    else
                        throw_line("Shouldn't get here");}
                else if (out.vtop < out.vmid) {
                    if (latestvt == VTOP)
                        out.vtop = out.vtop = (0.0 / out.r2) * (out.vmid - out.vbot) + out.vmid;
                    else if (latestvt == VMID)
                        out.vmid = (out.vtop * out.r2 + out.vbot * 0.0) / (0.0 + out.r2);
                    else 
                        throw_line("Shouldn't get here");}
                else if (gtb(out.r1, rmax))      {
                    if      (latestvt == VTOP)
                        out.vtop = (rmax / out.r2) * (out.vmid - out.vbot) + out.vmid;
                    else if (latestvt == VBOT)
                        out.vbot = out.vmid - (out.vtop - out.vmid) * (out.r2 / rmax);
                    else if (latestvt == VMID)
                        out.vmid = (out.vtop * out.r2 + out.vbot * rmax) / (rmax + out.r2);
                    else if (latestvt == R2)
                        out.r2 = rmax * (out.vmid - out.vbot) / (out.vtop - out.vmid);
                    else 
                        throw_line("Shouldn't get here");}
                else if (gtb(out.curr, currmax)) {
                    if      (latestvt == VBOT)
                		out.vbot = out.vmid - currmax * out.r2;
                    else if (latestvt == VMID)
                		out.vmid = currmax * out.r2 + out.vbot;
                    else if (latestvt == R2)
                        out.r2 = (out.vmid - out.vbot) / currmax;
                    else
                        throw_line("Shouldn't get here");
                    }
                else break;
            }
            check_ctr(ctr);
            break;}
        case 0x3c:{
            // TODO: DEBUG ERRORS IN 0x3C and 0x3A!!
            // TODO: Pause loop after a few rounds, but before breaking out of loop
            // r2, curr = f(vtop, vbot, vmid, r1)
            out.r2d   = OUTPUT;
            out.currd = OUTPUT;
            Vals lastout;
            auto compute = [&]() {
                out.r2   = out.r1 * (out.vmid - out.vbot) / (out.vtop - out.vmid);
                // out.vtop = (out.r1 / out.r2) * (out.vmid - out.vbot) + out.vmid;
                // out.vbot = out.vmid - (out.vtop - out.vmid) * (out.r2 / out.r1);
                // out.vmid = (out.vtop * out.r2 + out.vbot * out.r1) / (out.r1 + out.r2);
                // out.r1   = out.r2 * (out.vtop - out.vmid) / (out.vmid - out.vbot);
                out.curr = (out.vtop - out.vmid) / out.r1;
                // out.vtop = out.vmid + out.curr * out.r1;
                // out.vmid = out.vtop - out.curr * out.r1;
                // out.r1 = (out.vtop - out.vmid) / out.curr;
                out.disable = latest4.front() == VBOT ? CURR : NONE;
                };
            int ctr;
            for (ctr = 0; ctr < 10; ctr++) {
                if (out == lastout) break; // stop the obsession
                lastout = out;
                compute();
                // Special case at the root of r1<0, r2<0, or curr<0 problems
                if (out.vbot > out.vmid) {
                    if (latestvt == VBOT)
                        out.vbot = out.vmid - (out.vtop - out.vmid) * (0.0 / out.r1);
                    else if (latestvt == VMID)
                        out.vmid = (out.vtop * 0.0 + out.vbot * out.r1) / (out.r1 + 0.0);
                    else
                        throw_line("Shouldn't get here");}
                else if (out.vtop < out.vmid) {
                    if (latestvt == VTOP)
                        out.vtop = (out.r1 / rmax) * (out.vmid - out.vbot) + out.vmid;
                    else if (latestvt == VMID)
                        out.vmid = (out.vtop * rmax + out.vbot * out.r1) / (out.r1 + rmax);
                    else
                        throw_line("Shouldn't get here");}
                else if (gtb(out.r2, rmax))      {
                    if      (latestvt == VTOP)
                        out.vtop = (out.r1 / rmax) * (out.vmid - out.vbot) + out.vmid;
                    else if (latestvt == VBOT)
                        out.vbot = out.vmid - (out.vtop - out.vmid) * (rmax / out.r1);
                    else if (latestvt == VMID)
                        out.vmid = (out.vtop * rmax + out.vbot * out.r1) / (out.r1 + rmax);
                    else if (latestvt == R1)
                        out.r1   = rmax * (out.vtop - out.vmid) / (out.vmid - out.vbot);
                    else
                        throw_line("Shouldn't get here");}
                else if (gtb(out.curr, currmax)) {
                    if      (latestvt == VTOP)
                        out.vtop = out.vmid + currmax * out.r1;
                    else if (latestvt == VMID)
                        out.vmid = out.vtop - currmax * out.r1;
                    else if (latestvt == R1)
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
    out.ratiod = OUTPUT;
    return out;
}
constype RCCore::constraint(Vals vals) const {
    // Checks whether we have exactly 4 inputs, and of the right combination
    if (vals.numInputs() > 4) {
        std::cout << "precheck over" << std::endl;
        return OVER;
    }
    else if (vals.numInputs() < 4) {
        std::cout << "precheck under" << std::endl;
        return UNDER;
    }
    else // These two cases represent one loop overdefined and the other underdefined
        if (vals.incode() == 0x1b || vals.incode() == 0x2d) {
            std::cout << "precheck mixed" << std::endl;
            return MIXED;
        }
        else {
            return PROPER;
        }
}

bool RCCore::_update(vartype latestvt) {
    // Calculate a what-if.
    // If the what-if violates anything, don't update the output

    // If we are engaged, then check the constrainst, calculate, and update
    // If not engaged, then just update based on inputs

    if (ibridge)
        ibridge->setOutputEngaged(engaged);

    if (engaged && constraint(inVals) == PROPER) {
        outVals = calc_group(latestvt, inVals);
        inVals = outVals; // prevents jumping after recovering from a limit
        if (ibridge)
            ibridge->setOutputStates(outVals);
        return true;
    } else {
        outVals = inVals;
        if (ibridge)
            ibridge->setOutputStates(outVals);
        return false;
    }



//    if (constraint(inVals) == PROPER) {
//        if (engaged) {
//            outVals = calc_group(latestvt, inVals);
//            // Setting inVals to outVals prevents things from jumping after recovering from a limit
//            inVals = outVals;
//        }
//        if (ibridge)
//            ibridge->setOutputStates(outVals);
//        return true;
//    } else {
//        std::cout << "Improper input constraint!!!!!!!!!!! -- won't calculate" << std::endl;
//        outVals = inVals;
//        if (ibridge) ibridge->setOutputStates(outVals /*, constraint(outVals)*/);
//        return false;
//    }
}



std::string cstr(vartype vt) {
    assert (vt >= VTOP && vt <= CURR);
    switch (vt) {
        case VTOP: return "VTOP";
        case VBOT: return "VBOT";
        case VMID: return "VMID";
        case R1:   return "R1";
        case R2:   return "R2";
        case CURR: return "CURR";
        default: return "logic error";
    }
}

bool RCCore::setInput(vartype vt, double in) {
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

void RCCore::setInputs(Vals vals) {
    inVals = vals;
}
Vals RCCore::getInput() const {
    return inVals;
}
Vals RCCore::getOutput() const {
    return outVals;
}

double RCCore::VMax() const {return vul;}
double RCCore::VMin() const {return vbl;}
double RCCore::RMax() const {return rmax;}
double RCCore::CurrMax() const {return currmax;}
double RCCore::RatioMax() const {return ratiomax;}
void   RCCore::setVMax(double x) {vul = x;}
void   RCCore::setVMin(double x) {vbl = x; }
void   RCCore::setRMax(double x) {rmax = x;}
void   RCCore::setCurrMax(double x) {currmax = x; }
void   RCCore::setRatioMax(double x) {ratiomax = x;}
