#ifndef UIBRIDGEINT_H
#define UIBRIDGEINT_H

#include <memory>
#include "vals.h"


class UIBridgeInterface
{
public:
    virtual ~UIBridgeInterface() = 0;
    virtual void setState(Vals vals) = 0;
};

#endif // UIBRIDGEINT_H
