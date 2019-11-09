#include <QApplication>
#include <QObject>
#include <QStyle>
#include <QSlider>
#include <QPushButton>

#include <iostream>
#include <iomanip>

#include "vals.h"
#include "rccore.h"
#include "engstr.h"
#include "uibridge.h"
#include "mainwindow.h"



int main(int argc, char *argv[])
{
    QApplication::setStyle("Fusion"); // Windows, windowsvista
    QApplication app(argc, argv);
    std::shared_ptr<RCCore> pcore = std::make_shared<RCCore>();
    std::shared_ptr<UIBridge> pbridge = std::make_shared<UIBridge>();
    MainWindow w;

    // Each slider has this many steps
    int steps = 1000;
    w.VTopSlider()->setMinimum(0);
    w.VBotSlider()->setMinimum(0);
    w.VMidSlider()->setMinimum(0);
    w.R1Slider()->setMinimum(0);
    w.R1Slider()->setMinimum(0);
    w.CurrSlider()->setMinimum(0);
    w.VTopSlider()->setMaximum(steps);
    w.VBotSlider()->setMaximum(steps);
    w.VMidSlider()->setMaximum(steps);
    w.R2Slider()->setMaximum(steps);
    w.R1Slider()->setMaximum(steps);
    w.CurrSlider()->setMaximum(steps);

    // Set core limits
    pcore->setVul(20.0);
    pcore->setVbl(-5.0);
    pcore->setRmax(1e6);
    pcore->setCurrmax(3.0);

    // Update bridge ranges
    pbridge->setRange(RCCore::VTOP, 0, steps, pcore->getVbl(), pcore->getVul());
    pbridge->setRange(RCCore::VBOT, 0, steps, pcore->getVbl(), pcore->getVul());
    pbridge->setRange(RCCore::VMID, 0, steps, pcore->getVbl(), pcore->getVul());
    pbridge->setRange(RCCore::R1, 5, steps, 1e-3, pcore->getRmax());
    pbridge->setRange(RCCore::R2, 5, steps, 1e-3, pcore->getRmax());
    pbridge->setRange(RCCore::CURR, 5, steps, 1e-6, pcore->getCurrmax());
 
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

    // crashes when R1 gets to about 250
    pbridge->setCore(pcore);
    pbridge->setCoreValue(RCCore::VTOP, 20.0);
    pbridge->setCoreValue(RCCore::VBOT, -5.0);
    pbridge->setCoreValue(RCCore::R1,   230.0);
    pbridge->setCoreValue(RCCore::CURR, 0.1);

    pbridge->setWindow(&w); // change to shared ptr
    pcore->setBridge(pbridge);
    pcore->update();

    int a,c;
    double b;
    
    a = 0;
    b = pbridge->sliderToDouble(RCCore::VTOP, a);
    c = pbridge->doubleToSlider(RCCore::VTOP, b);
    std::cout << a << " -> " << b << " -> " << c << std::endl;

    a = 50;
    b = pbridge->sliderToDouble(RCCore::VTOP, a);
    c = pbridge->doubleToSlider(RCCore::VTOP, b);
    std::cout << a << " -> " << b << " -> " << c << std::endl;

    a = 100;
    b = pbridge->sliderToDouble(RCCore::VTOP, a);
    c = pbridge->doubleToSlider(RCCore::VTOP, b);
    std::cout << a << " -> " << b << " -> " << c << std::endl;

////////

    a = 0;
    b = pbridge->sliderToDouble(RCCore::R1, a);
    c = pbridge->doubleToSlider(RCCore::R1, b);
    std::cout << a << " -> " << b << " -> " << c << std::endl;

    a = 4;
    b = pbridge->sliderToDouble(RCCore::R1, a);
    c = pbridge->doubleToSlider(RCCore::R1, b);
    std::cout << a << " -> " << b << " -> " << c << std::endl;

    a = 5;
    b = pbridge->sliderToDouble(RCCore::R1, a);
    c = pbridge->doubleToSlider(RCCore::R1, b);
    std::cout << a << " -> " << b << " -> " << c << std::endl;

    a = 50;
    b = pbridge->sliderToDouble(RCCore::R1, a);
    c = pbridge->doubleToSlider(RCCore::R1, b);
    std::cout << a << " -> " << b << " -> " << c << std::endl;

    a = 100;
    b = pbridge->sliderToDouble(RCCore::R1, a);
    c = pbridge->doubleToSlider(RCCore::R1, b);
    std::cout << a << " -> " << b << " -> " << c << std::endl;

    // Legit strings
    std::string testStrs[] = {
        "0","-0","0e4","-0e4","-0e-4","0.0","-0.0","0.0e4","-0.0e4","-0.0e-4",
        "123", ".5", "1.","1.e10","1.e+10","1.e-10","1.5","1.2345","1.2345e",
        "1.2345e1","1.2345e+1","1.2345e-1","1.2345e+12","1.2345e+13","1.2345e-12",
        "1.2345e-13","123 p","123    p","123p","123P","123n","123u","123m",
        "123k","123M","123g","123t","123z","123Z","123m","123milli","123M",
        "123Meg",".5u","1.u","1.e10u","1.e-10u","1.2345e1u","1.2345e-1u","-123 p",
        "-123    p","-123p","-123P","-123n","-123u","-123m","-123k","-123M",
        "-123g","-123t","-123z","-123Z","-123m","-123milli","-123M","-123Meg",
        "-.5u","-1.u","-1.e10u","-1.e-10u","-1.2345e1u","-1.2345e-1u"
    };

    std::cout << "STRING TEST" << std::endl;
    for (auto s1: testStrs) {
        double d = EngStr::strToDouble(s1);
        std::string s2 = EngStr::doubleToStr(d,4);
        std::cout << s1 << " -> " << d << " -> " << s2 << std::endl;
    }

    


   w.show();
    return app.exec();
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
