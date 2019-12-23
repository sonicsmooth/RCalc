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
    bool engaged;

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
    double VMax() const;
    double VMin() const;
    double RMax() const;
    double CurrMax() const;
    double RatioMax() const;
    void setVMax(double);
    void setVMin(double);
    void setRMax(double);
    void setCurrMax(double);
    void setRatioMax(double);
    void setBridge(std::shared_ptr<UIBridgeInterface> const &);
    void setEngaged(bool);
    bool getEngaged() const;
};

std::string cstr(vartype);

#endif // RCCORE_H
