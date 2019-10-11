#include <memory>
#include <string>
#include <iostream>
#include <QString>
#include <QFont>
#include <QPalette>
#include <QSlider>
#include <QPushButton>
#include "uibridge.h"
#include "rccore.h"

UIBridge::UIBridge()
{

}

// void UIBridge::setCore(std::shared_ptr<RCCore> & pc) {
//     pcore = pc;
// }

void UIBridge::forceSlider(QSlider *slider, int val) const {
    slider->blockSignals(true);
    slider->setSliderPosition(val);
    slider->blockSignals(false);
}
void UIBridge::forceButton(QPushButton *button, bool val) const {
    button->blockSignals(true);
    button->setChecked(val);
    button->blockSignals(false);
}

void UIBridge::setWindow(MainWindow * mw) {
    w = mw;
    w->TextEdit()->setFont(QFont("Consolas"));
    setOutputs(Vals());
}

void UIBridge::setOutputs(Vals vals) {
    if (vals.vtopd == Vals::OUTPUT)  forceSlider(w->VTopSlider(), vals.vtop * 100.0);
    if (vals.vbotd == Vals::OUTPUT)  forceSlider(w->VBotSlider(), vals.vbot * 100.0);
    if (vals.vmidd == Vals::OUTPUT)  forceSlider(w->VMidSlider(), vals.vmid * 100.0);
    if (vals.r1d   == Vals::OUTPUT)  forceSlider(w->R1Slider(), vals.r1 * 100.0);
    if (vals.r2d   == Vals::OUTPUT)  forceSlider(w->R2Slider(), vals.r2 * 100.0);
    if (vals.ratiod == Vals::OUTPUT) forceSlider(w->RatioSlider(), vals.ratio * 100.0);
    if (vals.currd == Vals::OUTPUT)  forceSlider(w->CurrSlider(), vals.curr * 100.0);

    forceButton(w->VTopButton(), isin(vals.vtopd));
    forceButton(w->VBotButton(), isin(vals.vbotd));
    forceButton(w->VMidButton(), isin(vals.vmidd));
    forceButton(w->R1Button(), isin(vals.r1d));
    forceButton(w->R2Button(), isin(vals.r2d));
    forceButton(w->CurrButton(), isin(vals.currd));

    std::string sstr(vals.str());
    QString qstr(QString::fromStdString(sstr));
    w->TextEdit()->appendPlainText(qstr);
    std::cout << sstr;

    w->VTopValLabel()->setText(QString::asprintf("%6.4f", vals.vtop));
    w->VBotValLabel()->setText(QString::asprintf("%6.4f", vals.vbot));
    w->VMidValLabel()->setText(QString::asprintf("%6.4f", vals.vmid));
    w->R1ValLabel()->setText(QString::asprintf("%6.4f", vals.r1));
    w->R2ValLabel()->setText(QString::asprintf("%6.4f", vals.r2));
    w->RatioValLabel()->setText(QString::asprintf("%6.4f", vals.ratio));
    w->CurrValLabel()->setText(QString::asprintf("%6.4f", vals.curr));
//    w->VTopKLabel()->setText(vals.vtopk ? "valid" : "invalid");
//    w->VBotKLabel()->setText(vals.vbotk ? "valid" : "invalid");
//    w->VMidKLabel()->setText(vals.vmidk ? "valid" : "invalid");
//    w->R1KLabel()->setText(vals.r1k ? "valid" : "invalid");
//    w->R2KLabel()->setText(vals.r2k ? "valid" : "invalid");
//    w->RatioKLabel()->setText(vals.ratiok ? "valid" : "invalid");
//    w->CurrKLabel()->setText(vals.currk ? "valid" : "invalid");

    

    QPalette palette;
    palette.setColor(QPalette::WindowText, QColor(vals.constrainedType()==Vals::PROPER ? Qt::black : Qt::red));
    w->VTopValLabel()->setPalette(palette);
    w->VBotValLabel()->setPalette(palette);
    w->VMidValLabel()->setPalette(palette);
    w->R1ValLabel()->setPalette(palette);
    w->R2ValLabel()->setPalette(palette);
    w->RatioValLabel()->setPalette(palette);
    w->CurrValLabel()->setPalette(palette);
    w->VTopKLabel()->setPalette(palette);
    w->VBotKLabel()->setPalette(palette);
    w->VMidKLabel()->setPalette(palette);
    w->R1KLabel()->setPalette(palette);
    w->R2KLabel()->setPalette(palette);
    w->RatioKLabel()->setPalette(palette);
    w->CurrKLabel()->setPalette(palette);

}
