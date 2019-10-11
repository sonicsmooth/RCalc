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
    // Button text, color, etc., happes locally here

    // signals affect core directly using lambda
    QObject::connect(w.VTopSlider(), &QSlider::valueChanged, [=](int x) {pcore->set_vtop(double(x)/100);}); // also updates button status via nested update()
    QObject::connect(w.VBotSlider(), &QSlider::valueChanged, [=](int x) {pcore->set_vbot(double(x)/100);});
    QObject::connect(w.VMidSlider(), &QSlider::valueChanged, [=](int x) {pcore->set_vmid(double(x)/100);});
    QObject::connect(w.R1Slider(),   &QSlider::valueChanged, [=](int x) {pcore->set_r1(double(x)/100);});
    QObject::connect(w.R2Slider(),   &QSlider::valueChanged, [=](int x) {pcore->set_r2(double(x)/100);});
    QObject::connect(w.CurrSlider(), &QSlider::valueChanged, [=](int x) {pcore->set_curr(double(x)/100);});

    // // Touching the slider causes the button to become an input
    // QObject::connect(w.VTopSlider(), &QSlider::sliderPressed, [&w]() {w.VTopButton()->setChecked(true);});
    // QObject::connect(w.VBotSlider(), &QSlider::sliderPressed, [&w]() {w.VBotButton()->setChecked(true);});
    // QObject::connect(w.VMidSlider(), &QSlider::sliderPressed, [&w]() {w.VMidButton()->setChecked(true);});
    // QObject::connect(w.R1Slider(),   &QSlider::sliderPressed, [&w]() {w.R1Button()->setChecked(true);});
    // QObject::connect(w.R2Slider(),   &QSlider::sliderPressed, [&w]() {w.R2Button()->setChecked(true);});
    // QObject::connect(w.CurrSlider(), &QSlider::sliderPressed, [&w]() {w.CurrButton()->setChecked(true);});

    // The button text changes based on its being an input or output
    QObject::connect(w.VTopButton(), &QPushButton::toggled, [&w](bool checked) {w.VTopButton()->setText(checked ? "Input" : "Output");});
    QObject::connect(w.VBotButton(), &QPushButton::toggled, [&w](bool checked) {w.VBotButton()->setText(checked ? "Input" : "Output");});
    QObject::connect(w.VMidButton(), &QPushButton::toggled, [&w](bool checked) {w.VMidButton()->setText(checked ? "Input" : "Output");});
    QObject::connect(w.R1Button(),   &QPushButton::toggled, [&w](bool checked) {w.R1Button()->setText(checked ? "Input" : "Output");});
    QObject::connect(w.R2Button(),   &QPushButton::toggled, [&w](bool checked) {w.R2Button()->setText(checked ? "Input" : "Output");});
    QObject::connect(w.CurrButton(), &QPushButton::toggled, [&w](bool checked) {w.CurrButton()->setText(checked ? "Input" : "Output");});

    // The input direction changes based on button input or output -- value goes straight to core input
    QObject::connect(w.VTopButton(), &QPushButton::toggled, [&pcore](bool checked) {pcore->set_vtop(checked ? Vals::INPUT : Vals::OUTPUT);});
    QObject::connect(w.VBotButton(), &QPushButton::toggled, [&pcore](bool checked) {pcore->set_vbot(checked ? Vals::INPUT : Vals::OUTPUT);});
    QObject::connect(w.VMidButton(), &QPushButton::toggled, [&pcore](bool checked) {pcore->set_vmid(checked ? Vals::INPUT : Vals::OUTPUT);});
    QObject::connect(w.R1Button(),   &QPushButton::toggled, [&pcore](bool checked) {pcore->set_r1(checked ? Vals::INPUT : Vals::OUTPUT);});
    QObject::connect(w.R2Button(),   &QPushButton::toggled, [&pcore](bool checked) {pcore->set_r2(checked ? Vals::INPUT : Vals::OUTPUT);});
    QObject::connect(w.CurrButton(), &QPushButton::toggled, [&pcore](bool checked) {pcore->set_curr(checked ? Vals::INPUT : Vals::OUTPUT);});


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
