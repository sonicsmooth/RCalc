#ifndef UIBRIDGE_H
#define UIBRIDGE_H

#include <QObject>
#include <QSlider>
#include <QPushButton>
#include <memory>
#include "uibridgeinterface.h"
#include "rccore.h"
#include "vals.h"
#include "mainwindow.h"


class UIBridge : /*public QObject, */ public UIBridgeInterface
{
//Q_OBJECT

private:
//    std::shared_ptr<RCCore> pcore;
    MainWindow *w;
    void forceSlider(QSlider *, int) const;
    void forceButton(QPushButton *, bool) const;

public:
    UIBridge();
//    void setCore(std::shared_ptr<RCCore> &);
    void setWindow(MainWindow *w);
    void setOutputs(Vals);
};


#endif // UIBRIDGE_H
