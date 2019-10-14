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
#include "ui_mainwindow.h"



int main(int argc, char *argv[])
{
    QApplication::setStyle("Fusion"); // Windows, windowsvista
    QApplication a(argc, argv);
    std::shared_ptr<RCCore> pcore = std::make_shared<RCCore>();
    std::shared_ptr<UIBridge> pbridge = std::make_shared<UIBridge>();
    MainWindow w;

    // Slider and button inputs go to core, which updates vars, then updates slider and button states.

    // signals affect core directly using lambda; slid values become inputs, which goes to update, which sets the buttons
    QObject::connect(w.VTopSlider(), &QSlider::valueChanged, [=](int x) {pcore->setVTop(double(x)/100);}); // also updates button status via nested update()
    QObject::connect(w.VBotSlider(), &QSlider::valueChanged, [=](int x) {pcore->setVBot(double(x)/100);});
    QObject::connect(w.VMidSlider(), &QSlider::valueChanged, [=](int x) {pcore->setVMid(double(x)/100);});
    QObject::connect(w.R1Slider(),   &QSlider::valueChanged, [=](int x) {pcore->setR1  (double(x)/100);});
    QObject::connect(w.R2Slider(),   &QSlider::valueChanged, [=](int x) {pcore->setR2  (double(x)/100);});
    QObject::connect(w.CurrSlider(), &QSlider::valueChanged, [=](int x) {pcore->setCurr(double(x)/100);});

    // The input direction changes based on button input or output -- value goes straight to core input
    QObject::connect(w.VTopButton(), &QPushButton::toggled, [&pcore](bool checked) {pcore->setVTop(checked ? Vals::INPUT : Vals::OUTPUT);});
    QObject::connect(w.VBotButton(), &QPushButton::toggled, [&pcore](bool checked) {pcore->setVBot(checked ? Vals::INPUT : Vals::OUTPUT);});
    QObject::connect(w.VMidButton(), &QPushButton::toggled, [&pcore](bool checked) {pcore->setVMid(checked ? Vals::INPUT : Vals::OUTPUT);});
    QObject::connect(w.R1Button(),   &QPushButton::toggled, [&pcore](bool checked) {pcore->setR1  (checked ? Vals::INPUT : Vals::OUTPUT);});
    QObject::connect(w.R2Button(),   &QPushButton::toggled, [&pcore](bool checked) {pcore->setR2  (checked ? Vals::INPUT : Vals::OUTPUT);});
    QObject::connect(w.CurrButton(), &QPushButton::toggled, [&pcore](bool checked) {pcore->setCurr(checked ? Vals::INPUT : Vals::OUTPUT);});

    pcore->setVTop(3.3);
    pcore->setVBot(-5.0);
    pcore->setR1(50);
    pcore->setR2(50);

    pbridge->setWindow(&w);
    pcore->setBridge(pbridge);
    pcore->update();




/*      INPUTS                             
        vtop vbot vmid r1  r2 curr  comment
0x0f      0    0    1   1   1   1	
0x17      0    1    0   1   1   1	
0x1b      0    1    1   0   1   1	under
0x1d      0    1    1   1   0   1	
0x1e      0    1    1   1   1   0	
0x27      1    0    0   1   1   1	
0x2b      1    0    1   0   1   1	
0x2d      1    0    1   1   0   1	under
0x2e      1    0    1   1   1   0  
0x33      1    1    0   0   1   1	
0x35      1    1    0   1   0   1	
0x36      1    1    0   1   1   0	
0x31      1    1    1   0   0   1	
0x32      1    1    1   0   1   0	
0x3c      1    1    1   1   0   0	*/


    w.show();
    return a.exec();
}
