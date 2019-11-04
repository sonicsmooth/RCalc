#include <QApplication>
#include <QObject>
#include <QStyle>
#include <QSlider>
#include <QPushButton>

#include <iostream>
#include <iomanip>

#include "vals.h"
#include "rccore.h"
#include "uibridge.h"
#include "mainwindow.h"
//#include "ui_mainwindow.h"



int main(int argc, char *argv[])
{
    QApplication::setStyle("Fusion"); // Windows, windowsvista
    QApplication a(argc, argv);
    std::shared_ptr<RCCore> pcore = std::make_shared<RCCore>();
    std::shared_ptr<UIBridge> pbridge = std::make_shared<UIBridge>();
    MainWindow w;

    // Slider int is divided by scale to get double
    double scale = 100.0;

    // Set core limits
    pcore->setVul(20);
    pcore->setVbl(0);
    pcore->setRmax(10000);
    pcore->setCurrmax(10);

    // Update sliders based on core's limits
    w.VTopSlider()->setMaximum(int(scale * pcore->getVul()));
    w.VTopSlider()->setMinimum(int(scale * pcore->getVbl()));
    w.VBotSlider()->setMaximum(int(scale * pcore->getVul()));
    w.VBotSlider()->setMinimum(int(scale * pcore->getVbl()));
    w.VMidSlider()->setMaximum(int(scale * pcore->getVul()));
    w.VMidSlider()->setMinimum(int(scale * pcore->getVbl()));
    w.R1Slider()->setMaximum(int(scale * pcore->getRmax()));
    w.R2Slider()->setMaximum(int(scale * pcore->getRmax()));
    w.CurrSlider()->setMaximum(int(scale * pcore->getCurrmax()));


    // signals affect core directly using lambda; slid values become inputs, which goes to update, which sets the buttons
    QObject::connect(w.VTopSlider(), &QSlider::valueChanged, [=](int x) {pbridge->setCoreValue(RCCore::VTOP, x);});
    QObject::connect(w.VBotSlider(), &QSlider::valueChanged, [=](int x) {pbridge->setCoreValue(RCCore::VBOT, x);});
    QObject::connect(w.VMidSlider(), &QSlider::valueChanged, [=](int x) {pbridge->setCoreValue(RCCore::VMID, x);});
    QObject::connect(w.R1Slider(),   &QSlider::valueChanged, [=](int x) {pbridge->setCoreValue(RCCore::R1,   x);});
    QObject::connect(w.R2Slider(),   &QSlider::valueChanged, [=](int x) {pbridge->setCoreValue(RCCore::R2,   x);});
    QObject::connect(w.CurrSlider(), &QSlider::valueChanged, [=](int x) {pbridge->setCoreValue(RCCore::CURR, x);});

    QObject::connect(w.VTopSlider(), &QSlider::sliderPressed, [=, &w]() {pbridge->setCoreValue(RCCore::VTOP, w.VTopSlider()->value());});
    QObject::connect(w.VBotSlider(), &QSlider::sliderPressed, [=, &w]() {pbridge->setCoreValue(RCCore::VBOT, w.VBotSlider()->value());});
    QObject::connect(w.VMidSlider(), &QSlider::sliderPressed, [=, &w]() {pbridge->setCoreValue(RCCore::VMID, w.VMidSlider()->value());});
    QObject::connect(w.R1Slider(),   &QSlider::sliderPressed, [=, &w]() {pbridge->setCoreValue(RCCore::R1,   w.R1Slider()->value());});
    QObject::connect(w.R2Slider(),   &QSlider::sliderPressed, [=, &w]() {pbridge->setCoreValue(RCCore::R2,   w.R2Slider()->value());});
    QObject::connect(w.CurrSlider(), &QSlider::sliderPressed, [=, &w]() {pbridge->setCoreValue(RCCore::CURR, w.CurrSlider()->value());});

    pbridge->setCoreValue(RCCore::VMID, 1.2);
    pbridge->setCoreValue(RCCore::R1,   20.0);
    pbridge->setCoreValue(RCCore::R2,   20.0);
    pbridge->setCoreValue(RCCore::CURR, 0.1);

    pbridge->setCore(pcore);
    pbridge->setWindow(&w); // change to shared ptr
    pbridge->setScale(scale);
    pcore->setBridge(pbridge);
    pcore->update();

    w.show();
    return a.exec();
}



void test(RCCore *pcore) {
#if(0)
    if (0) // 0x0f //ok
    {//        vtop vbot vmid  r1  r2 curr
    // 0x0f      0    0    1   1   1   1
    //pcore->update(RCCore::VTOP, 3.3);
    //pcore->update(RCCore::VBOT, 0.5);
    pcore->update(RCCore::VMID, 1.2);
    pcore->update(RCCore::R1,   20.0);
    pcore->update(RCCore::R2,   20.0);
    pcore->update(RCCore::CURR, 0.1);}
    if (0) // 0x17 //ok
    {//        vtop vbot vmid  r1  r2 curr
    // 0x17      0    1    0   1   1   1
    //pcore->update(RCCore::VTOP, 3.3);
    pcore->update(RCCore::VBOT, 0.5);
    //pcore->update(RCCore::VMID, 1.2);
    pcore->update(RCCore::R1,   20.0);
    pcore->update(RCCore::R2,   20.0);
    pcore->update(RCCore::CURR, 0.1);}
    if (0) // 0x1d //ok
    {//        vtop vbot vmid  r1  r2 curr
    // 0x1d      0    1    1   1   0   1
    //pcore->update(RCCore::VTOP, 3.3);
    pcore->update(RCCore::VBOT, 0.5);
    pcore->update(RCCore::VMID, 1.2);
    pcore->update(RCCore::R1,   20.0);
    //pcore->update(RCCore::R2,   20.0);
    pcore->update(RCCore::CURR, 0.1);}
    if (0) // 0x1e //ok
    {//        vtop vbot vmid  r1  r2 curr
    // 0x1e      0    1    1   1   1   0
    //pcore->update(RCCore::VTOP, 3.3);
    pcore->update(RCCore::VBOT, 0.5);
    pcore->update(RCCore::VMID, 1.2);
    pcore->update(RCCore::R1,   20.0);
    pcore->update(RCCore::R2,   20.0);
    //pcore->update(RCCore::CURR, 0.1);
    }
    if (0) // 0x27 //ok
    {//        vtop vbot vmid  r1  r2 curr
    // 0x27      1    0    0   1   1   1
    pcore->update(RCCore::VTOP, 3.3);
    //pcore->update(RCCore::VBOT, 0.5);
    //pcore->update(RCCore::VMID, 1.2);
    pcore->update(RCCore::R1,   20.0);
    pcore->update(RCCore::R2,   20.0);
    pcore->update(RCCore::CURR, 0.1);}
    if (0) // 0x2b //ok
    {//        vtop vbot vmid  r1  r2 curr
    // 0x2b      1    0    1   0   1   1
    pcore->update(RCCore::VTOP, 3.3);
    //pcore->update(RCCore::VBOT, 0.5);
    pcore->update(RCCore::VMID, 1.2);
    //pcore->update(RCCore::R1,   20.0);
    pcore->update(RCCore::R2,   20.0);
    pcore->update(RCCore::CURR, 0.1);}
    if (1) // 0x2e //ok
    {//        vtop vbot vmid  r1  r2 curr
    // 0x2e      1    0    1   1   1   0
    pcore->update(RCCore::VTOP, 3.3);
    //pcore->update(RCCore::VBOT, 0.5);
    pcore->update(RCCore::VMID, 1.2);
    pcore->update(RCCore::R1,   20.0);
    pcore->update(RCCore::R2,   20.0);
    //pcore->update(RCCore::CURR, 0.1);
    }
    if (0) // 0x33 //ok
    {//        vtop vbot vmid  r1  r2 curr
    // 0x33      1    1    0   0   1   1
    pcore->update(RCCore::VTOP, 3.3);
    pcore->update(RCCore::VBOT, 0.5);
    //pcore->update(RCCore::VMID, 1.2);
    //pcore->update(RCCore::R1,   20.0);
    pcore->update(RCCore::R2,   20.0);
    pcore->update(RCCore::CURR, 0.1);}
    if (0) // 0x35 //ok
    {//        vtop vbot vmid  r1  r2 curr
    // 0x35      1    1    0   1   0   1
    pcore->update(RCCore::VTOP, 3.3);
    pcore->update(RCCore::VBOT, 0.5);
    //pcore->update(RCCore::VMID, 1.2);
    pcore->update(RCCore::R1,   20.0);
    //pcore->update(RCCore::R2,   20.0);
    pcore->update(RCCore::CURR, 0.1);}
    if (0) // 0x36 //ok
    {//        vtop vbot vmid  r1  r2 curr
    // 0x36      1    1    0   1   1   0
    pcore->update(RCCore::VTOP, 5.0);
    pcore->update(RCCore::VBOT, 5.0);
    //pcore->update(RCCore::VMID, 1.2);
    pcore->update(RCCore::R1,   20.0);
    pcore->update(RCCore::R2,   20.0);
    pcore->update(RCCore::VBOT, 9.06);
    //pcore->update(RCCore::CURR, 0.1);
    }
    if (0) // 0x39 //ok
    {//        vtop vbot vmid  r1  r2 curr
    // 0x39      1    1    1   0   0   1
    pcore->update(RCCore::VTOP, 3.3);
    pcore->update(RCCore::VBOT, 0.5);
    pcore->update(RCCore::VMID, 1.2);
    //pcore->update(RCCore::R1,   20.0);
    //pcore->update(RCCore::R2,   20.0);
    pcore->update(RCCore::CURR, 0.1);}
    if (0) // 0x3a //ok
    {//        vtop vbot vmid  r1  r2 curr
    // 0x3a      1    1    1   0   1   0
    pcore->update(RCCore::VTOP, 5.0);
    pcore->update(RCCore::VBOT, 0.75);
    pcore->update(RCCore::VMID, 2.0);
    //pcore->update(RCCore::R1, 20.0);
    pcore->update(RCCore::R2,   20);
    //pcore->update(RCCore::CURR, 0.1);
    }
    if (0) // 0x3c //ok
    {//        vtop vbot vmid  r1  r2 curr
    // 0x3c      1    1    1   1   0   0
    pcore->update(RCCore::VTOP, 5);
    pcore->update(RCCore::VBOT, 0.0);
    pcore->update(RCCore::VMID, 2.0);
    pcore->update(RCCore::R1,   20.0);
    //pcore->update(RCCore::R2,   20.0);
    //pcore->update(RCCore::CURR, 0.1);
    }
#endif

}
