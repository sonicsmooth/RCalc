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
    struct {double a, x0, r;} r1exp, r2exp, currexp;

    MainWindow *w;
    std::shared_ptr<RCCore> pcore;
    //double scale;
    void forceSlider(QSlider *, int) const;
    void forceButton(QPushButton *, bool) const;
    double sliderToDouble(RCCore::vartype, int);
    int doubleToSlider(RCCore::vartype, double);

public:
    //UIBridge();
    void setCore(std::shared_ptr<RCCore>) override;
    void setOutputStates(Vals, Vals::constype) override;
    void setWindow(MainWindow *w);
    //double getScale() const;
    //void setScale(double);
    void setRange(RCCore::vartype, int, int, double, double);
    void setCoreValue(RCCore::vartype, int);
    void setCoreValue(RCCore::vartype, double);
    void setCoreValue(RCCore::vartype, std::string);
    void setCoreRange(RCCore::vartype, double, double);
    void setCoreDir(RCCore::vartype, Vals::dir);


};


#endif // UIBRIDGE_H
