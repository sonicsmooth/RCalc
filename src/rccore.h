#ifndef RCCORE_H
#define RCCORE_H

#include <queue>
#include <list>
#include "rctypes.h"
#include "uibridgeinterface.h"
#include "vals.h"

class RCCore
{
private:
    Vals inVals; // input from UI
    Vals outVals; // second set of calculations;
    std::shared_ptr<UIBridgeInterface> ibridge;
    std::list<vartype> latest4; // list of 4 inputs
    double vul,vbl,rmax,currmax, ratiomax;

    void pushToList(vartype);
    void removeFromList(vartype);
    Vals swapInputs(Vals) const;
    Vals clip(Vals, int) const;
    Vals calc_group(vartype, Vals) const;
    constype constraint(Vals) const;
    bool _update(vartype);

public:
    bool setInput(vartype, double);
    bool update();     
    void setInputs(Vals);
    Vals getInput() const;
    Vals getOutput() const;
    double getVul() const;
    double getVbl() const;
    double getRmax() const;
    double getCurrmax() const;
    double getRatiomax() const;
    void setVul(double);
    void setVbl(double);
    void setRmax(double);
    void setCurrmax(double);
    void setRatiomax(double);
    void setBridge(std::shared_ptr<UIBridgeInterface> const &);
};

std::string cstr(vartype);

#endif // RCCORE_H
