#ifndef UIBRIDGEINT_H
#define UIBRIDGEINT_H

#include <memory>
//#include "rccore.h"
#include "vals.h"


class UIBridgeInterface
{
public:
    virtual ~UIBridgeInterface() = 0;
    //virtual void setCore(std::shared_ptr<RCCore> &) = 0;
    virtual void setOutputs(Vals vals) = 0;
};

#endif // UIBRIDGEINT_H
