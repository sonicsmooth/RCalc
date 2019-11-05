#include <memory>
#include <string>
#include <iostream>
#include <QString>
#include <QFont>
#include <QPalette>
#include <QSlider>
#include <QPushButton>
#include "uibridge.h"

//UIBridge::UIBridge(){}

void UIBridge::setCore(std::shared_ptr<RCCore> pc) {
    pcore = pc;
}

void UIBridge::setOutputStates(Vals vals, Vals::constype ct) {
    forceSlider(w->VTopSlider(),  doubleToSlider(RCCore::VTOP, vals.vtop));
    forceSlider(w->VBotSlider(),  doubleToSlider(RCCore::VBOT, vals.vbot));
    forceSlider(w->VMidSlider(),  doubleToSlider(RCCore::VMID, vals.vmid));
    forceSlider(w->R1Slider(),    doubleToSlider(RCCore::R1,   vals.r1));
    forceSlider(w->R2Slider(),    doubleToSlider(RCCore::R2,   vals.r2));
    forceSlider(w->CurrSlider(),  doubleToSlider(RCCore::CURR, vals.curr));
    //forceSlider(w->RatioSlider(), doubleToSlider(RCCore::, vals.vtop));

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
// double UIBridge::getScale() const {return scale;}
// void UIBridge::setScale(double x) {scale = x;}

void UIBridge::setRange(RCCore::vartype vt, int x1, int x2, double y1, double y2) {
    // Tells the bridge the expected range of the slider, and the desired output
    // values; the mapping coefficients are calculated and stored in this function.
    // Whether the mapping from input to output is linear or log is based
    // on vt.  If voltage, then mapping is linear.  If current or resistance, then
    // mapping is log.  Slider should also be set to range (x1, x2), or the output
    // won't match.
    
    // linearly if vt=voltage, log if resistor or current
    // for linear: m = (y2 - y1) / (x2 - x1)5
    //             b = y1 - m * x1
    //             then y = mx+b
    //                  x = (y - b) / m
    // for log: a = y1
    //          x0 = x1
    //          r = log10(y2/y1) / (x2 - x1)
    //          then y = a*10^((x-x0)*r) when x < x0, else 0
    //               x = log10(y/a)/r + x0
    // just calculate all 5, then apply as needed
    double m = (y2 - y1) / (x2 - x1);
    double b = y1 - m * x1;
    double a = y1;
    double x0 = x1;
    double r = log10(y2 / y1) / (x2 - x1);
    switch(vt) {
        case RCCore::VTOP:
            vtopmxb.m = m;
            vtopmxb.b = b;
            break;
        case RCCore::VBOT:
            vbotmxb.m = m;
            vbotmxb.b = b;
            break;
        case RCCore::VMID:
            vmidmxb.m = m;
            vmidmxb.b = b;
            break;
        case RCCore::R1:
            r1exp.a = a;
            r1exp.x0 = x0;
            r1exp.r = r;
            break;
        case RCCore::R2:
            r2exp.a = a;
            r2exp.x0 = x0;
            r2exp.r = r;
            break;
        case RCCore::CURR:
            currexp.a = a;
            currexp.x0 = x0;
            currexp.r = r;
            break;
        default:
            throw std::logic_error("bad switch in UIBridge::setRange");
            
    }
}
double UIBridge::sliderToDouble(RCCore::vartype vt, int x) {
    // Return linear values mapped values for voltages
    switch (vt) {
        case RCCore::VTOP: return vtopmxb.m * x + vtopmxb.b;
        case RCCore::VBOT: return vbotmxb.m * x + vbotmxb.b;
        case RCCore::VMID: return vmidmxb.m * x + vmidmxb.b;
        case RCCore::R1: 
            return (x < r1exp.x0) ? 0.0 : r1exp.a * pow(10.0, (x - r1exp.x0) * r1exp.r); 
        case RCCore::R2: 
            return (x < r2exp.x0) ? 0.0 : r2exp.a * pow(10.0, (x - r2exp.x0) * r2exp.r);
        case RCCore::CURR: 
            return (x < currexp.x0) ? 0.0 : currexp.a * pow(10.0, (x - currexp.x0) * currexp.r);
        default:
            throw std::logic_error("bad switch in UIBridge::sliderToDouble");
    }
}
int UIBridge::doubleToSlider(RCCore::vartype vt, double y) {
    // Return linear value back to slider.
    // TODO: ensure stuff gets calculated again with new integer
    switch (vt) {
        case RCCore::VTOP: return int(round((y - vtopmxb.b) / vtopmxb.m));
        case RCCore::VBOT: return int(round((y - vbotmxb.b) / vbotmxb.m));
        case RCCore::VMID: return int(round((y - vmidmxb.b) / vmidmxb.m));
        case RCCore::R1: {
            int ret = int(round(log10(y / r1exp.a) / r1exp.r + r1exp.x0));
            if (ret < r1exp.x0) return 0;
            return ret;
        }
        case RCCore::R2: {
            int ret = int(round(log10(y / r2exp.a) / r2exp.r + r2exp.x0));
            if (ret < r2exp.x0) return 0;
            return ret;
        }
        case RCCore::CURR: {
            int ret = int(round(log10(y / currexp.a) / currexp.r + currexp.x0));
            if (ret < currexp.x0) return 0;
            return ret;
    }
        default:
            throw std::logic_error("bad switch in UIBridge::linvalToSlider");
    }
}

void UIBridge::setCoreValue(RCCore::vartype vt, int sliderVal) {
    // log expands based on parameters, then sets core value
    pcore->setInput(vt, sliderToDouble(vt, sliderVal));
}

void UIBridge::setCoreValue(RCCore::vartype vt, double directVal) {
    // sets core value directly
    pcore->setInput(vt, directVal);
}

void UIBridge::setCoreValue(RCCore::vartype vt, std::string stringVal) {
    // Converts from string to double, then sets core value
}

