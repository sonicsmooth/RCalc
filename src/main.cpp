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

    //auto setVTopGTBot = [=](int x) {pcore->setVTop(double(x)/100);};
    //auto setVBotLTTop = [=](int x) {pcore->setVBot(double(x)/100);};
    //auto setVMidBTTB  = [=](int x) {pcore->setVMid(double(x)/100);};

    // signals affect core directly using lambda; slid values become inputs, which goes to update, which sets the buttons
    QObject::connect(w.VTopSlider(), &QSlider::valueChanged, [=](int x) {pcore->update(RCCore::VTOP, double(x)/100.0);});//pcore->setVTop(double(x)/100);});
    QObject::connect(w.VBotSlider(), &QSlider::valueChanged, [=](int x) {pcore->update(RCCore::VBOT, double(x)/100.0);});//pcore->setVBot(double(x)/100);});
    QObject::connect(w.VMidSlider(), &QSlider::valueChanged, [=](int x) {pcore->update(RCCore::VMID, double(x)/100.0);});//pcore->setVBot(double(x)/100);}););
    QObject::connect(w.R1Slider(),   &QSlider::valueChanged, [=](int x) {pcore->update(RCCore::R1,   double(x)/100.0);});//pcore->setR1  (double(x)/100);});
    QObject::connect(w.R2Slider(),   &QSlider::valueChanged, [=](int x) {pcore->update(RCCore::R2,   double(x)/100.0);});//pcore->setR2  (double(x)/100);});
    QObject::connect(w.CurrSlider(), &QSlider::valueChanged, [=](int x) {pcore->update(RCCore::CURR, double(x)/100.0);});//pcore->setCurr(double(x)/100);});

    QObject::connect(w.VTopSlider(), &QSlider::sliderPressed, [=, &w]() {pcore->update(RCCore::VTOP, w.VTopSlider()->value()/100.0);});//[=]() {pcore->setVTop(Vals::INPUT);});
    QObject::connect(w.VBotSlider(), &QSlider::sliderPressed, [=, &w]() {pcore->update(RCCore::VBOT, w.VBotSlider()->value()/100.0);});//[=]() {pcore->setVBot(Vals::INPUT);});
    QObject::connect(w.VMidSlider(), &QSlider::sliderPressed, [=, &w]() {pcore->update(RCCore::VMID, w.VMidSlider()->value()/100.0);});//[=]() {pcore->setVMid(Vals::INPUT);});
    QObject::connect(w.R1Slider(),   &QSlider::sliderPressed, [=, &w]() {pcore->update(RCCore::R1, w.R1Slider()->value()/100.0);});//[=]() {pcore->setR1  (Vals::INPUT);});
    QObject::connect(w.R2Slider(),   &QSlider::sliderPressed, [=, &w]() {pcore->update(RCCore::R2, w.R2Slider()->value()/100.0);});//[=]() {pcore->setR2  (Vals::INPUT);});
    QObject::connect(w.CurrSlider(), &QSlider::sliderPressed, [=, &w]() {pcore->update(RCCore::CURR, w.CurrSlider()->value()/100.0);});//[=]() {pcore->setCurr(Vals::INPUT);});

    // The input direction changes based on button input or output -- value goes straight to core input
//    QObject::connect(w.VTopButton(), &QPushButton::toggled, [&pcore](bool checked) {pcore->setVTop(checked ? Vals::INPUT : Vals::OUTPUT);});
//    QObject::connect(w.VBotButton(), &QPushButton::toggled, [&pcore](bool checked) {pcore->setVBot(checked ? Vals::INPUT : Vals::OUTPUT);});
//    QObject::connect(w.VMidButton(), &QPushButton::toggled, [&pcore](bool checked) {pcore->setVMid(checked ? Vals::INPUT : Vals::OUTPUT);});
//    QObject::connect(w.R1Button(),   &QPushButton::toggled, [&pcore](bool checked) {pcore->setR1  (checked ? Vals::INPUT : Vals::OUTPUT);});
//    QObject::connect(w.R2Button(),   &QPushButton::toggled, [&pcore](bool checked) {pcore->setR2  (checked ? Vals::INPUT : Vals::OUTPUT);});
//    QObject::connect(w.CurrButton(), &QPushButton::toggled, [&pcore](bool checked) {pcore->setCurr(checked ? Vals::INPUT : Vals::OUTPUT);});

    //pcore->setVTop(3.3);
    //pcore->update(RCCore::VBOT, 0.5);//setVBot(0.5);
    pcore->update(RCCore::VMID, 3.5);//setVMid(1.5);
    pcore->update(RCCore::R1, 20.0);//setR1(20);
    pcore->update(RCCore::R2, 20.0);//setR1(20);
    pcore->update(RCCore::CURR, 0.1);//setCurr(0.1);

    pbridge->setWindow(&w);
    pcore->setBridge(pbridge);
    pcore->update();

    w.show();
    return a.exec();
}


