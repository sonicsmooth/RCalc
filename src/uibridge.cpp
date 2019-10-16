#include <memory>
#include <string>
#include <iostream>
#include <QString>
#include <QFont>
#include <QPalette>
#include <QSlider>
#include <QPushButton>
#include "uibridge.h"

UIBridge::UIBridge(){}

void UIBridge::forceSlider(QSlider *slider, int val) const {
    slider->blockSignals(true);
    slider->setSliderPosition(val);
    slider->blockSignals(false);
}
void UIBridge::forceButton(QPushButton *button, bool val) const {
    button->blockSignals(true);
    button->setChecked(val);
    button->blockSignals(false);
    button->setText(val ? "Input" : "Output");
}

void UIBridge::setWindow(MainWindow * mw) {
    w = mw;
}

void UIBridge::setState(Vals vals, Vals::constype ct) {
    forceSlider(w->VTopSlider(),  vals.vtop * 100.0);
    forceSlider(w->VBotSlider(),  vals.vbot * 100.0);
    forceSlider(w->VMidSlider(),  vals.vmid * 100.0);
    forceSlider(w->R1Slider(),    vals.r1 * 100.0);
    forceSlider(w->R2Slider(),    vals.r2 * 100.0);
    forceSlider(w->CurrSlider(),  vals.curr * 100.0);
    forceSlider(w->RatioSlider(), vals.ratio * 100.0);

    forceButton(w->VTopButton(), isin(vals.vtopd));
    forceButton(w->VBotButton(), isin(vals.vbotd));
    forceButton(w->VMidButton(), isin(vals.vmidd));
    forceButton(w->R1Button(),   isin(vals.r1d));
    forceButton(w->R2Button(),   isin(vals.r2d));
    forceButton(w->CurrButton(), isin(vals.currd));

    w->VTopValLabel()->setText(QString::asprintf("%6.4f", vals.vtop));
    w->VBotValLabel()->setText(QString::asprintf("%6.4f", vals.vbot));
    w->VMidValLabel()->setText(QString::asprintf("%6.4f", vals.vmid));
    w->R1ValLabel()->setText(QString::asprintf("%6.4f", vals.r1));
    w->R2ValLabel()->setText(QString::asprintf("%6.4f", vals.r2));
    w->RatioValLabel()->setText(QString::asprintf("%6.4f", vals.ratio));
    w->CurrValLabel()->setText(QString::asprintf("%6.4f", vals.curr));

    // Sets text color
    QPalette palette;
    palette.setColor(QPalette::WindowText, QColor(ct==Vals::PROPER ? Qt::black : Qt::red));
    w->VTopValLabel()->setPalette(palette);
    w->VBotValLabel()->setPalette(palette);
    w->VMidValLabel()->setPalette(palette);
    w->R1ValLabel()->setPalette(palette);
    w->R2ValLabel()->setPalette(palette);
    w->RatioValLabel()->setPalette(palette);
    w->CurrValLabel()->setPalette(palette);

    std::string sstr(vals.str());
    std::cout << sstr;
    w->TextEdit()->appendPlainText(QString::fromStdString(sstr));

}
