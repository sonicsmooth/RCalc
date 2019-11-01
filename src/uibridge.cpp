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
double UIBridge::getScale() const {return scale;}
void UIBridge::setScale(double x) {scale = x;}

void UIBridge::setState(Vals vals, Vals::constype ct) {
    forceSlider(w->VTopSlider(),  vals.vtop * scale);
    forceSlider(w->VBotSlider(),  vals.vbot * scale);
    forceSlider(w->VMidSlider(),  vals.vmid * scale);
    forceSlider(w->R1Slider(),    vals.r1 * scale);
    forceSlider(w->R2Slider(),    vals.r2 * scale);
    forceSlider(w->CurrSlider(),  vals.curr * scale);
    forceSlider(w->RatioSlider(), vals.ratio * scale);

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

    w->VTopValEdit()->setText(QString::asprintf("%6.4f", vals.vtop));
    w->VBotValEdit()->setText(QString::asprintf("%6.4f", vals.vbot));
    w->VMidValEdit()->setText(QString::asprintf("%6.4f", vals.vmid));
    w->R1ValEdit()->setText(QString::asprintf("%6.4f", vals.r1));
    w->R2ValEdit()->setText(QString::asprintf("%6.4f", vals.r2));
    w->RatioValEdit()->setText(QString::asprintf("%6.4f", vals.ratio));
    w->CurrValEdit()->setText(QString::asprintf("%6.4f", vals.curr));



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

    palette.setColor(QPalette::Base, QColor(vals.vtopd ? Qt::black : Qt::blue));
    std::string inputstyle = "background:#f00;";
    std::string outputstyle = "background:#00f;";
    //w->VTopValEdit()->setPalette(palette);
    w->VTopValEdit()->setStyleSheet(vals.vtopd ? inputstyle.c_str() : outputstyle.c_str());

    palette.setColor(QPalette::Base, QColor(vals.vbotd ? Qt::black : Qt::blue));
    palette.setColor(QPalette::Text, QColor(vals.vbotd ? Qt::yellow : Qt::red));
    w->VBotValEdit()->setPalette(palette);

    palette.setColor(QPalette::Base, QColor(vals.vmidd ? Qt::black : Qt::blue));
    palette.setColor(QPalette::Text, QColor(vals.vbotd ? Qt::yellow : Qt::red));
    w->VMidValEdit()->setPalette(palette);

    palette.setColor(QPalette::Base, QColor(vals.r1d ? Qt::black : Qt::blue));
    palette.setColor(QPalette::Text, QColor(vals.vbotd ? Qt::yellow : Qt::red));
    w->R1ValEdit()->setPalette(palette);

    palette.setColor(QPalette::Base, QColor(vals.r2d ? Qt::black : Qt::blue));
    palette.setColor(QPalette::Text, QColor(vals.vbotd ? Qt::yellow : Qt::red));
    w->R2ValEdit()->setPalette(palette);

    palette.setColor(QPalette::Base, QColor(vals.currd ? Qt::black : Qt::blue));
    palette.setColor(QPalette::Text, QColor(vals.vbotd ? Qt::yellow : Qt::red));
    w->CurrValEdit()->setPalette(palette);



    std::string sstr(vals.str());
    w->TextEdit()->appendPlainText(QString::fromStdString(sstr));

}
