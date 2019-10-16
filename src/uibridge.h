#ifndef UIBRIDGE_H
#define UIBRIDGE_H

#include <QSlider>
#include <QPushButton>
#include "uibridgeinterface.h"
#include "rccore.h"
#include "vals.h"
#include "mainwindow.h"


class UIBridge : public UIBridgeInterface
{

private:
    MainWindow *w;
    void forceSlider(QSlider *, int) const;
    void forceButton(QPushButton *, bool) const;

public:
    UIBridge();
    void setWindow(MainWindow *w);
    void setState(Vals, Vals::constype);
};


#endif // UIBRIDGE_H
