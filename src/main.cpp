#include <QApplication>
#include <QObject>
#include <QStyle>
#include <QSlider>
#include <QPushButton>
#include <QRegExp>
#include <QRegExpValidator>
#include <QString>

#include <iostream>
#include <iomanip>

#include "rctypes.h"
#include "vals.h"
#include "rccore.h"
#include "engstr.h"
#include "uibridge.h"
#include "mainwindow.h"
#include "ui_form.h"



int main(int argc, char *argv[])
{
    QApplication::setStyle("Fusion"); // Windows, windowsvista
    QApplication app(argc, argv);
    std::shared_ptr<RCCore> pcore = std::make_shared<RCCore>();
    std::shared_ptr<UIBridge> pbridge = std::make_shared<UIBridge>();
    MainWindow w;

    // Each slider has this many steps + 1
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
    w.RatioSlider()->setMaximum(steps);

    // Set core limits
    pcore->setVMax(20.0);
    pcore->setVMin(-5.0);
    pcore->setRMax(1e6);
    pcore->setCurrMax(3.0);
    pcore->setRatioMax(1e6);

    // Update bridge ranges so it knows how to interpret slider values
    pbridge->setRange(VTOP, 0, steps, pcore->VMin(), pcore->VMax());
    pbridge->setRange(VBOT, 0, steps, pcore->VMin(), pcore->VMax());
    pbridge->setRange(VMID, 0, steps, pcore->VMin(), pcore->VMax());
    pbridge->setRange(R1, 5, steps, 1e-3, pcore->RMax());
    pbridge->setRange(R2, 5, steps, 1e-3, pcore->RMax());
    pbridge->setRange(CURR, 5, steps, 1e-6, pcore->CurrMax());
    pbridge->setRange(RATIO, 5, steps, 1e-6, pcore->RatioMax());

    // For now set Rdivider manually, later maybe in bridge
    w.RDivider()->setVMax(pcore->VMax());
    w.RDivider()->setVMin(pcore->VMin());


    // signals affect core directly using lambda; slid values become inputs, which goes to update, which sets the buttons
    QObject::connect(w.VTopSlider(), &QSlider::valueChanged, [=](int x) {pbridge->setCoreValue(VTOP, x);});
    QObject::connect(w.VBotSlider(), &QSlider::valueChanged, [=](int x) {pbridge->setCoreValue(VBOT, x);});
    QObject::connect(w.VMidSlider(), &QSlider::valueChanged, [=](int x) {pbridge->setCoreValue(VMID, x);});
    QObject::connect(w.R1Slider(),   &QSlider::valueChanged, [=](int x) {pbridge->setCoreValue(R1,   x);});
    QObject::connect(w.R2Slider(),   &QSlider::valueChanged, [=](int x) {pbridge->setCoreValue(R2,   x);});
    QObject::connect(w.CurrSlider(), &QSlider::valueChanged, [=](int x) {pbridge->setCoreValue(CURR, x);});

    QObject::connect(w.VTopSlider(), &QSlider::sliderPressed, [=, &w]() {pbridge->setCoreValue(VTOP, w.VTopSlider()->value());});
    QObject::connect(w.VBotSlider(), &QSlider::sliderPressed, [=, &w]() {pbridge->setCoreValue(VBOT, w.VBotSlider()->value());});
    QObject::connect(w.VMidSlider(), &QSlider::sliderPressed, [=, &w]() {pbridge->setCoreValue(VMID, w.VMidSlider()->value());});
    QObject::connect(w.R1Slider(),   &QSlider::sliderPressed, [=, &w]() {pbridge->setCoreValue(R1,   w.R1Slider()->value());});
    QObject::connect(w.R2Slider(),   &QSlider::sliderPressed, [=, &w]() {pbridge->setCoreValue(R2,   w.R2Slider()->value());});
    QObject::connect(w.CurrSlider(), &QSlider::sliderPressed, [=, &w]() {pbridge->setCoreValue(CURR, w.CurrSlider()->value());});

    QObject::connect(w.RDivider(), &RDivider::vtopChanged, [=](double x) {pbridge->setCoreValue(VTOP, x);});
    QObject::connect(w.RDivider(), &RDivider::vbotChanged, [=](double x) {pbridge->setCoreValue(VBOT, x);});
    QObject::connect(w.RDivider(), &RDivider::vmidChanged, [=](double x) {pbridge->setCoreValue(VMID, x);});

    // Set up validate-and-send on each line edit
    auto vas = [=](QLineEdit * qle, vartype vt) {
        double x = EngStr::strToDouble(qle->text().toStdString());
        if (isnan(x)) qle->setStyleSheet("background:red");
        else          pbridge->setCoreValue(vt, x);
    };
    QObject::connect(w.VTopValEdit(), &QLineEdit::editingFinished, [=, &w]() {vas(w.VTopValEdit(), VTOP);});
    QObject::connect(w.VBotValEdit(), &QLineEdit::editingFinished, [=, &w]() {vas(w.VBotValEdit(), VBOT);});
    QObject::connect(w.VMidValEdit(), &QLineEdit::editingFinished, [=, &w]() {vas(w.VMidValEdit(), VMID);});
    QObject::connect(w.R1ValEdit(),   &QLineEdit::editingFinished, [=, &w]() {vas(w.R1ValEdit(),   R1);});
    QObject::connect(w.R2ValEdit(),   &QLineEdit::editingFinished, [=, &w]() {vas(w.R2ValEdit(),   R2);});
    QObject::connect(w.CurrValEdit(), &QLineEdit::editingFinished, [=, &w]() {vas(w.CurrValEdit(), CURR);});

    // Set some initial conditions
    pbridge->setCore(pcore);
    pbridge->setCoreValue(VTOP, 5.0);
    pbridge->setCoreValue(VBOT, 1.0);
    pbridge->setCoreValue(VMID, 3.3);
    pbridge->setCoreValue(R1, 1000);

    pbridge->setWindow(&w); // change to shared ptr
    pcore->setBridge(pbridge);
    pcore->update();


    w.show();

//    QWidget *junkwidget = new QWidget;
//    Ui::Form junkform;
//    junkform.setupUi(junkwidget);
//    junkwidget->show();

    return app.exec();
}



void test(RCCore *pcore) {
#if(0)
    if (0) // 0x0f //ok
    {//        vtop vbot vmid  r1  r2 curr
    // 0x0f      0    0    1   1   1   1
    //pcore->update(VTOP, 3.3);
    //pcore->update(VBOT, 0.5);
    pcore->update(VMID, 1.2);
    pcore->update(R1,   20.0);
    pcore->update(R2,   20.0);
    pcore->update(CURR, 0.1);}
    if (0) // 0x17 //ok
    {//        vtop vbot vmid  r1  r2 curr
    // 0x17      0    1    0   1   1   1
    //pcore->update(VTOP, 3.3);
    pcore->update(VBOT, 0.5);
    //pcore->update(VMID, 1.2);
    pcore->update(R1,   20.0);
    pcore->update(R2,   20.0);
    pcore->update(CURR, 0.1);}
    if (0) // 0x1d //ok
    {//        vtop vbot vmid  r1  r2 curr
    // 0x1d      0    1    1   1   0   1
    //pcore->update(VTOP, 3.3);
    pcore->update(VBOT, 0.5);
    pcore->update(VMID, 1.2);
    pcore->update(R1,   20.0);
    //pcore->update(R2,   20.0);
    pcore->update(CURR, 0.1);}
    if (0) // 0x1e //ok
    {//        vtop vbot vmid  r1  r2 curr
    // 0x1e      0    1    1   1   1   0
    //pcore->update(VTOP, 3.3);
    pcore->update(VBOT, 0.5);
    pcore->update(VMID, 1.2);
    pcore->update(R1,   20.0);
    pcore->update(R2,   20.0);
    //pcore->update(CURR, 0.1);
    }
    if (0) // 0x27 //ok
    {//        vtop vbot vmid  r1  r2 curr
    // 0x27      1    0    0   1   1   1
    pcore->update(VTOP, 3.3);
    //pcore->update(VBOT, 0.5);
    //pcore->update(VMID, 1.2);
    pcore->update(R1,   20.0);
    pcore->update(R2,   20.0);
    pcore->update(CURR, 0.1);}
    if (0) // 0x2b //ok
    {//        vtop vbot vmid  r1  r2 curr
    // 0x2b      1    0    1   0   1   1
    pcore->update(VTOP, 3.3);
    //pcore->update(VBOT, 0.5);
    pcore->update(VMID, 1.2);
    //pcore->update(R1,   20.0);
    pcore->update(R2,   20.0);
    pcore->update(CURR, 0.1);}
    if (1) // 0x2e //ok
    {//        vtop vbot vmid  r1  r2 curr
    // 0x2e      1    0    1   1   1   0
    pcore->update(VTOP, 3.3);
    //pcore->update(VBOT, 0.5);
    pcore->update(VMID, 1.2);
    pcore->update(R1,   20.0);
    pcore->update(R2,   20.0);
    //pcore->update(CURR, 0.1);
    }
    if (0) // 0x33 //ok
    {//        vtop vbot vmid  r1  r2 curr
    // 0x33      1    1    0   0   1   1
    pcore->update(VTOP, 3.3);
    pcore->update(VBOT, 0.5);
    //pcore->update(VMID, 1.2);
    //pcore->update(R1,   20.0);
    pcore->update(R2,   20.0);
    pcore->update(CURR, 0.1);}
    if (0) // 0x35 //ok
    {//        vtop vbot vmid  r1  r2 curr
    // 0x35      1    1    0   1   0   1
    pcore->update(VTOP, 3.3);
    pcore->update(VBOT, 0.5);
    //pcore->update(VMID, 1.2);
    pcore->update(R1,   20.0);
    //pcore->update(R2,   20.0);
    pcore->update(CURR, 0.1);}
    if (0) // 0x36 //ok
    {//        vtop vbot vmid  r1  r2 curr
    // 0x36      1    1    0   1   1   0
    pcore->update(VTOP, 5.0);
    pcore->update(VBOT, 5.0);
    //pcore->update(VMID, 1.2);
    pcore->update(R1,   20.0);
    pcore->update(R2,   20.0);
    pcore->update(VBOT, 9.06);
    //pcore->update(CURR, 0.1);
    }
    if (0) // 0x39 //ok
    {//        vtop vbot vmid  r1  r2 curr
    // 0x39      1    1    1   0   0   1
    pcore->update(VTOP, 3.3);
    pcore->update(VBOT, 0.5);
    pcore->update(VMID, 1.2);
    //pcore->update(R1,   20.0);
    //pcore->update(R2,   20.0);
    pcore->update(CURR, 0.1);}
    if (0) // 0x3a //ok
    {//        vtop vbot vmid  r1  r2 curr
    // 0x3a      1    1    1   0   1   0
    pcore->update(VTOP, 5.0);
    pcore->update(VBOT, 0.75);
    pcore->update(VMID, 2.0);
    //pcore->update(R1, 20.0);
    pcore->update(R2,   20);
    //pcore->update(CURR, 0.1);
    }
    if (0) // 0x3c //ok
    {//        vtop vbot vmid  r1  r2 curr
    // 0x3c      1    1    1   1   0   0
    pcore->update(VTOP, 5);
    pcore->update(VBOT, 0.0);
    pcore->update(VMID, 2.0);
    pcore->update(R1,   20.0);
    //pcore->update(R2,   20.0);
    //pcore->update(CURR, 0.1);
    }
#endif

}

void testConversions(UIBridge *pbridge) {
    int a,c;
    double b;
    
    a = 0;
    b = pbridge->sliderToDouble(VTOP, a);
    c = pbridge->doubleToSlider(VTOP, b);
    std::cout << a << " -> " << b << " -> " << c << std::endl;

    a = 50;
    b = pbridge->sliderToDouble(VTOP, a);
    c = pbridge->doubleToSlider(VTOP, b);
    std::cout << a << " -> " << b << " -> " << c << std::endl;

    a = 100;
    b = pbridge->sliderToDouble(VTOP, a);
    c = pbridge->doubleToSlider(VTOP, b);
    std::cout << a << " -> " << b << " -> " << c << std::endl;

////////

    a = 0;
    b = pbridge->sliderToDouble(R1, a);
    c = pbridge->doubleToSlider(R1, b);
    std::cout << a << " -> " << b << " -> " << c << std::endl;

    a = 4;
    b = pbridge->sliderToDouble(R1, a);
    c = pbridge->doubleToSlider(R1, b);
    std::cout << a << " -> " << b << " -> " << c << std::endl;

    a = 5;
    b = pbridge->sliderToDouble(R1, a);
    c = pbridge->doubleToSlider(R1, b);
    std::cout << a << " -> " << b << " -> " << c << std::endl;

    a = 50;
    b = pbridge->sliderToDouble(R1, a);
    c = pbridge->doubleToSlider(R1, b);
    std::cout << a << " -> " << b << " -> " << c << std::endl;

    a = 100;
    b = pbridge->sliderToDouble(R1, a);
    c = pbridge->doubleToSlider(R1, b);
    std::cout << a << " -> " << b << " -> " << c << std::endl;

}

void testStrings() {
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


}
