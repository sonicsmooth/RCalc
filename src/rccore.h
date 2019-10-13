#ifndef RCCORE_H
#define RCCORE_H

#include "uibridgeinterface.h"
#include "vals.h"



class RCCore
{

private:
    Vals vals0; // input from UI
    Vals vals1; // first set of calculations;
    Vals vals2; // second set of calculations;
    //Vals vals3; // final set of calculations;
    std::shared_ptr<UIBridgeInterface> ibridge;

    // void calc_vtop(Vals, double &, Vals::dir &) const;
    // void calc_vbot(Vals, double &, Vals::dir &) const;
    // void calc_vmid(Vals, double &, Vals::dir &) const;
    // void calc_r1(Vals, double &, Vals::dir &) const;
    // void calc_r2(Vals, double &, Vals::dir &) const;
    // void calc_ratio(Vals, double &, Vals::dir &) const;
    // void calc_curr(Vals, double &, Vals::dir &) const;
    // void verify_loops(Vals, bool &) const;
    Vals calc_group(Vals) const;
    void calc_constraint(Vals &) const;

public:
    RCCore();
    bool setVTop(double);
    bool setVBot(double);
    bool setVMid(double);
    bool setR1(double);
    bool setR2(double);
    bool setCurr(double);
    bool setVTop(Vals::dir);
    bool setVBot(Vals::dir);
    bool setVMid(Vals::dir);
    bool setR1(Vals::dir);
    bool setR2(Vals::dir);
    bool setCurr(Vals::dir);
    bool update();     // Given inst.vals0, sets and computes vals3; returns success t/f
    bool update(Vals); // Given Vals, sets and computes everything; returns success t/f
    void setInput(Vals);
    Vals getInput() const;
    Vals getOutput() const;
    bool inputConstrainedType() const;

    void setBridge(std::shared_ptr<UIBridgeInterface> const &);
};

#endif // RCCORE_H
