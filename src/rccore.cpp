#include <iostream>
#include <iomanip>
#include <cmath>
#include "rccore.h"
#include "vals.h"

// PRIVATE
// void RCCore::calc_vtop(Vals invals, Vals & outvals) const {
//     // INPUTS                             
//     // vtop vbot vmid r1  r2 curr  comment
//     //   0    0    1   1   1   1	
//     //   0    1    0   1   1   1	
//     //   0    1    1   0   1   1	under
//     //   0    1    1   1   0   1	
//     //   0    1    1   1   1   0
//     double vtop  = invals.vtop;  Vals::dir vtopd  = invals.vtopd;
//     double vbot  = invals.vbot;  Vals::dir vbotd  = invals.vbotd;
//     double vmid  = invals.vmid;  Vals::dir vmidd  = invals.vmidd;
//     double r1    = invals.r1;    Vals::dir r1d    = invals.r1d;
//     double r2    = invals.r2;    Vals::dir r2d    = invals.r2d;
//     double curr  = invals.curr;  Vals::dir currd  = invals.currd;
//     double ratio = invals.ratio; Vals::dir ratiod = invals.ratiod;

//     outvals.vtopd == Vals::OUTPUT;
//     if (!isin(vtopd) && !isin(vbotd) && isin(vmidd) && isin(r1d) && isin(r2d) && isin(currd))

// }

// void RCCore::calc_vbot(Vals invals, double & out, Vals::dir & d) const {
//     double vtop  = invals.vtop;  Vals::dir vtopd  = invals.vtopd;
//     double vbot  = invals.vbot;  Vals::dir vbotd  = invals.vbotd;
//     double vmid  = invals.vmid;  Vals::dir vmidd  = invals.vmidd;
//     double r1    = invals.r1;    Vals::dir r1d    = invals.r1d;
//     double r2    = invals.r2;    Vals::dir r2d    = invals.r2d;
//     double curr  = invals.curr;  Vals::dir currd  = invals.currd;
//     double ratio = invals.ratio; Vals::dir ratiod = invals.ratiod;
//     k = true;
//     if (vbotk)
//         out = vbot;
//     else if (vmidk && currk && r2k)
//         out = vmid - curr * r2;
//     else 
//         k = false;
// }

// void RCCore::calc_vmid(Vals invals, double & out, Vals::dir & d) const {
//     double vtop  = invals.vtop;  Vals::dir vtopd  = invals.vtopd;
//     double vbot  = invals.vbot;  Vals::dir vbotd  = invals.vbotd;
//     double vmid  = invals.vmid;  Vals::dir vmidd  = invals.vmidd;
//     double r1    = invals.r1;    Vals::dir r1d    = invals.r1d;
//     double r2    = invals.r2;    Vals::dir r2d    = invals.r2d;
//     double curr  = invals.curr;  Vals::dir currd  = invals.currd;
//     double ratio = invals.ratio; Vals::dir ratiod = invals.ratiod;
//     k = true;
//     if (vmidk)
//         out = vmid;
//     else if (vtopk && currk && r1k)
//         out = vtop - curr * r1;
//     else if (vbotk && currk && r2k)
//         out = vbot + curr * r2;
//     else if (vtopk && vbotk && ratiok && currk)
//         out = (vtop + curr * ratio * vbot) / (curr * ratio + 1.0);
//     else 
//         k = false;
// }

// void RCCore::calc_r1(Vals invals, double & out, Vals::dir & d) const {
//     double vtop  = invals.vtop;  Vals::dir vtopd  = invals.vtopd;
//     double vbot  = invals.vbot;  Vals::dir vbotd  = invals.vbotd;
//     double vmid  = invals.vmid;  Vals::dir vmidd  = invals.vmidd;
//     double r1    = invals.r1;    Vals::dir r1d    = invals.r1d;
//     double r2    = invals.r2;    Vals::dir r2d    = invals.r2d;
//     double curr  = invals.curr;  Vals::dir currd  = invals.currd;
//     double ratio = invals.ratio; Vals::dir ratiod = invals.ratiod;
//     k = true;
//     if (r1k)
//         out = r1;
//     else if (ratiok && r2k)
//         out = ratio * r2;
//     else if (vtopk && vmidk && currk)
//         out = (vtop - vmid) / curr;
//     else 
//         k = false;
// }

// void RCCore::calc_r2(Vals invals, double & out, Vals::dir & d) const {
//     double vtop  = invals.vtop;  Vals::dir vtopd  = invals.vtopd;
//     double vbot  = invals.vbot;  Vals::dir vbotd  = invals.vbotd;
//     double vmid  = invals.vmid;  Vals::dir vmidd  = invals.vmidd;
//     double r1    = invals.r1;    Vals::dir r1d    = invals.r1d;
//     double r2    = invals.r2;    Vals::dir r2d    = invals.r2d;
//     double curr  = invals.curr;  Vals::dir currd  = invals.currd;
//     double ratio = invals.ratio; Vals::dir ratiod = invals.ratiod;
//     k = true;
//     if (r2k)
//         out = r2;
//     else if (ratiok && r1k)
//         out = r1 / ratio;
//     else if (vmidk && vbotk && currk)
//         out = (vmid - vbot) / curr;
//     else 
//         k = false;
// }

// void RCCore::calc_ratio(Vals invals, double & out, Vals::dir & d) const {
//     double vtop  = invals.vtop;  Vals::dir vtopd  = invals.vtopd;
//     double vbot  = invals.vbot;  Vals::dir vbotd  = invals.vbotd;
//     double vmid  = invals.vmid;  Vals::dir vmidd  = invals.vmidd;
//     double r1    = invals.r1;    Vals::dir r1d    = invals.r1d;
//     double r2    = invals.r2;    Vals::dir r2d    = invals.r2d;
//     double curr  = invals.curr;  Vals::dir currd  = invals.currd;
//     double ratio = invals.ratio; Vals::dir ratiod = invals.ratiod;
//     k = true;
//     //if (ratiok)
//     //    out = ratio;
//     /*else*/ if (r1k && r2k)
//         out = r1 / r2;
//     //else 
//     //    k = false;
// }

// void RCCore::calc_curr(Vals invals, double & out, Vals::dir & d) const {
//     double vtop  = invals.vtop;  Vals::dir vtopd  = invals.vtopd;
//     double vbot  = invals.vbot;  Vals::dir vbotd  = invals.vbotd;
//     double vmid  = invals.vmid;  Vals::dir vmidd  = invals.vmidd;
//     double r1    = invals.r1;    Vals::dir r1d    = invals.r1d;
//     double r2    = invals.r2;    Vals::dir r2d    = invals.r2d;
//     double curr  = invals.curr;  Vals::dir currd  = invals.currd;
//     double ratio = invals.ratio; Vals::dir ratiod = invals.ratiod;
//     k = true;
//     if (currk)
//         out = curr;
//     else if (vtopk && vmidk && r1k)
//         out = (vtop - vmid) / r1;
//     else if (vmidk && vbotk && r2k)
//         out = (vmid - vbot) / r2;
//     else if (vtopk && vbotk && r1k && r2k)
//         out = (vtop - vbot) / (r1 + r2);
//     else 

//         k = false;
// }

// void RCCore::verify_loops(Vals invals, bool & out) const {
//     // Check for mathematical consistency among defined values
//     // Returns true in out var when one or more 
//     // calcs are good and none are bad
//     int passctr = 0, failctr = 0;
//     double eps = 1e-12;
//     double vtop  = invals.vtop;  Vals::dir vtopd  = invals.vtopd;
//     double vbot  = invals.vbot;  Vals::dir vbotd  = invals.vbotd;
//     double vmid  = invals.vmid;  Vals::dir vmidd  = invals.vmidd;
//     double r1    = invals.r1;    Vals::dir r1d    = invals.r1d;
//     double r2    = invals.r2;    Vals::dir r2d    = invals.r2d;
//     double curr  = invals.curr;  Vals::dir currd  = invals.currd;
//     double ratio = invals.ratio; Vals::dir ratiod = invals.ratiod;
//     auto passfail = [eps, &passctr, &failctr] (double v1, double v2) -> bool {
//         if (std::abs(v1 - v2) < eps) {
//             passctr++;
//             return true;
//             }
//         else {
//             failctr++;
//             return false;
//         }
//     };

//     // Voltage, resistance, current
//     if (vtopk && vmidk && r1k && currk)        passfail(vtop - curr * r1, vmid);
//     if (vbotk && vmidk && r2k && currk)        passfail(vbot + curr * r2, vmid);
//     if (vtopk && vbotk && r1k && r2k && currk) passfail((vtop - vbot) / (r1 + r2), curr);
//     if (vtopk && vmidk && r1k && currk)        passfail((vtop - vmid) / r1, curr);
//     if (vbotk && vmidk && r2k && currk)        passfail((vmid - vbot) / r2, curr);

//     // Voltage, resistance, infer current
//     if (vtopk && vbotk && vmidk && r1k && r2k) {
//         double iinfer1 = (vtop - vmid) / r1;
//         double iinfer2 = (vmid - vbot) / r2;
//         passfail(vtop - iinfer2 * r1, vmid);
//         passfail(vbot + iinfer1 * r2, vmid);
//         passfail(iinfer1, iinfer2);
//     }

//     // Resistors, ratio
//     if (r1k && r2k && ratiok)                  passfail(r1 / r2, ratio);
//     if (vtopk && vbotk && vmidk && ratiok)     passfail ((vtop - vmid) / (vmid - vbot), ratio);
//     if (vtopk && vbotk && vmidk && r1k && r2k) passfail ((vtop - vmid) / (vmid - vbot), r1 / r2);

//     out = (passctr >= 1 && failctr == 0);

//     }
    

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



    // calc_vtop(in, out.vtop, out.vtopd);
    // calc_vbot(in, out.vbot, out.vbotd);
    // calc_vmid(in, out.vmid, out.vmidd);
    // calc_r1(in, out.r1, out.r1d);
    // calc_r2(in, out.r2, out.r2d);
    // calc_ratio(in, out.ratio, out.ratiod);
    // calc_curr(in, out.curr, out.currd);
    //verify_loops(in, out.consistent);

void RCCore::calc_constraint(Vals & vals) const {
    if (vals.numInputs() > 4)
        vals.constype = Vals::OVER;
    else if (vals.numInputs() < 4)
        vals.constype = Vals::UNDER;
    else // These two cases represent one loop overdefined and the other underdefined
        if (vals.incode() == 0x1b || vals.incode() == 0x2d)
            vals.constype = Vals::MIXED;
        else
            vals.constype = Vals::PROPER;
}

// Public
RCCore::RCCore() :
    vals0 (Vals()),
    vals1 (Vals()),
    vals2 (Vals())
    {}

bool RCCore::set_vtop(double in) {
    vals0.vtop = in;
    vals0.vtopd = Vals::INPUT;
    return update();
}
bool RCCore::set_vbot(double in) {
    vals0.vbot = in;
    vals0.vbotd = Vals::INPUT;
    return update();
}
bool RCCore::set_vmid(double in) {
    vals0.vmid = in;
    vals0.vmidd = Vals::INPUT;
    return update();
}
bool RCCore::set_r1(double in) {
    vals0.r1 = in;
    vals0.r1d = Vals::INPUT;
    return update();
}
bool RCCore::set_r2(double in) {
    vals0.r2 = in;
    vals0.r2d = Vals::INPUT;
    return update();
}
bool RCCore::set_curr(double in) {
    vals0.curr = in;
    vals0.currd = Vals::INPUT;
    return update();
}
bool RCCore::set_vtop(Vals::dir dir) {
    vals0.vtopd = dir;
    return update();
}
bool RCCore::set_vbot(Vals::dir dir) {
    vals0.vbotd = dir;
    return update();
}
bool RCCore::set_vmid(Vals::dir dir) {
    vals0.vmidd = dir;
    return update();
}
bool RCCore::set_r1(Vals::dir dir) {
    vals0.r1d = dir;
    return update();
}
bool RCCore::set_r2(Vals::dir dir) {
    vals0.r2d = dir;
    return update();
}
bool RCCore::set_curr(Vals::dir dir) {
    vals0.currd = dir;
    return update();
}

bool RCCore::update() {
    // Go through all three calculations
    // keep intermediate results
//    std::cout << "code: 0x" << std::hex << vals2.incode() << std::endl;

    calc_constraint(vals0);

   if (vals0.constrainedType() != Vals::PROPER) {
       std::cout << "Improper input constraint -- won't calculate" << std::endl;
       vals2 = vals1 = vals0;
       ibridge->setOutputs(vals0);
       return false;
   }
    
    vals2 = vals1 = calc_group(vals0);
    ibridge->setOutputs(vals2);
    return true;
}

bool RCCore::update(Vals in) {
    // Go through all three calculations
    // keep intermediate results
    vals0 = in;
    return update();
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










