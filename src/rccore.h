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
    bool set_vtop(double);
    bool set_vbot(double);
    bool set_vmid(double);
    bool set_r1(double);
    bool set_r2(double);
    bool set_curr(double);
    bool set_vtop(Vals::dir);
    bool set_vbot(Vals::dir);
    bool set_vmid(Vals::dir);
    bool set_r1(Vals::dir);
    bool set_r2(Vals::dir);
    bool set_curr(Vals::dir);
    bool update();     // Given inst.vals0, sets and computes vals3; returns success t/f
    bool update(Vals); // Given Vals, sets and computes everything; returns success t/f
    Vals getInput() const;
    Vals getOutput() const;
    bool inputConstrainedType() const;

    void setBridge(std::shared_ptr<UIBridgeInterface> const &);
};

#endif // RCCORE_H
