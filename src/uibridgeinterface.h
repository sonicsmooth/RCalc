#ifndef UIBRIDGEINT_H
#define UIBRIDGEINT_H


#include <memory>

class RCCore;
class UIBridgeInterface;

#include "rccore.h"
#include "vals.h"


class UIBridgeInterface
{
public:
    virtual ~UIBridgeInterface() = 0;
    virtual void setCore(std::shared_ptr<RCCore>) = 0;
    virtual void setOutputStates(Vals /* vartype , constype*/) = 0;
};

#endif // UIBRIDGEINT_H
