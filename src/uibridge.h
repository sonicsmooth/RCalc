#ifndef UIBRIDGE_H
#define UIBRIDGE_H

#include <string>
#include <memory>
#include <QSlider>
#include <QPushButton>
#include "uibridgeinterface.h"
#include "rccore.h"
#include "vals.h"
#include "mainwindow.h"


class UIBridge : public UIBridgeInterface
{

private:
    // y=mx+b, for linear, where x is slider, y is output
    struct {double m, b;} vtopmxb, vbotmxb, vmidmxb;
    // y = a * 10^((x-x0)*r) where x is slider, y is output
    struct {double a, x0, r;} r1exp, r2exp, currexp, ratioexp;
    MainWindow *w;
    std::shared_ptr<RCCore> pcore;

    void forceSlider(QSlider *, int) const;
    void forceButton(QPushButton *, bool) const;

public:
    //UIBridge();
    double sliderToDouble(vartype, int);
    int doubleToSlider(vartype, double);
    void setCore(std::shared_ptr<RCCore>) override;
    void setOutputStates(Vals /*, vartype, constype*/) override;
    void setWindow(MainWindow *w);
    void setRange(vartype, int, int, double, double);
    void setCoreValue(vartype, int);
    void setCoreValue(vartype, double);
    void setCoreValue(vartype, std::string);
    void setCoreDir(vartype, dir);


};


#endif // UIBRIDGE_H
