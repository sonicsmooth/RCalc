#include <memory>
#include <string>
#include <iostream>
#include <QString>
#include <QFont>
#include <QPalette>
#include <QSlider>
#include <QPushButton>
#include "uibridge.h"
#include "engstr.h"

//UIBridge::UIBridge(){}

// TODO:
// Each box can hove one of three bits set for color display:
// Input/Output
// In/edge of range
// System normal/underconstrained

// TODO:
// Figure out how to disable sliders when system is about to be in mixed constraint
// aka one '1' bit move away from being in states 1b or 2d.
// only disable slider if it's the next in the list
// From 1b, when you set a 0 to 1, another 1 becomes 0
// set bit 5 to 1 and see 4 possible outcomes, then set bit 2 to 1 and 4 more outcomes
//            tb m12c

// bit 2 down (r1), bit 0 up
// From 1e    01 1110 // 1e -> r1 last, disable curr
// to   1b    01 1011

// bit 2 down (r1), bit 1 up
// From 1d    01 1101 // 1d -> r1 last, disable r2
// to   1b    01 1011

// bit 2 down (r1), bit 3 up
// From 17    01 0111 // 17 -> r1 last, disable vmid
// to   1b    01 1011

// bit 2 down (r1), bit 4 up
// From 0f    00 1111 // 0f -> r1 last, disable vbot
// to   1b    01 1011

// bit 5 (vtop) down, bit 0 up
// From 3a    11 1010 // 3a -> vtop last, disable curr
// to   1b,   01 1011,

// bit 5 down (vtop), bit 1 up
// From 39    11 1001 // 39 -> vtop last, disable r2
// to   1b,   01 1011,

// bit 5 down (vtop), bit 3 up
// From 33    11 0011 // 33 -> vtop last, disable vmid
// to   1b,   01 1011,

// bit 5 down (vtop), bit 4 up
// From 2b    10 1011 // 2b -> vtop last, disable vbot
// to   1b,   01 1011,



// bit 1 down (r2), bit 0 up (curr)
// from 2e    10 1110
// to   2d    10 1101

// bit 1 down (r2), bit 2 up (r1)
// from 2b    10 1011
// to   2d    10 1101

// bit 1 down (r2), bit 3 up (vmid)
// from 27    10 0111
// to   2d    10 1101

// bit 1 down (r2), bit 5 up (vtop)
// from 0f    00 1111
// to   2d    10 1101

// bit 4 down (vbot), bit 0 up (curr)
// from 3c    11 1100
// to   2d    10 1101

// bit 4 down (vbot), bit 2 up (r1)
// from 39    11 1001
// to   2d    10 1101

// bit 4 down (vbot), bit 3 up (vmid)
// from 35    11 0101
// to   2d    10 1101

// bit 4 down (vbot), bit 5 up (vtop)
// from 1d      01 1101
// to   2d    10 1101


//
/*      INPUTS
        vtop vbot vmid r1  r2 curr  comment
0x0f      0    0    1   1   1   1
0x17      0    1    0   1   1   1
0x1b      0    1    1   0   1   1	mixed
0x1d      0    1    1   1   0   1
0x1e      0    1    1   1   1   0
0x27      1    0    0   1   1   1
0x2b      1    0    1   0   1   1
0x2d      1    0    1   1   0   1	mixed
0x2e      1    0    1   1   1   0
0x33      1    1    0   0   1   1
0x35      1    1    0   1   0   1
0x36      1    1    0   1   1   0
0x39      1    1    1   0   0   1
0x3a      1    1    1   0   1   0
0x3c      1    1    1   1   0   0	*/

void UIBridge::setCore(std::shared_ptr<RCCore> pc)
{
    pcore = pc;
}

void UIBridge::setOutputStates(Vals vals /*, constype ct */)
{
    // Sets the sliders positions.
    forceSlider(w->VTopSlider(), doubleToSlider(VTOP, vals.vtop));
    forceSlider(w->VBotSlider(), doubleToSlider(VBOT, vals.vbot));
    forceSlider(w->VMidSlider(), doubleToSlider(VMID, vals.vmid));
    forceSlider(w->R1Slider(), doubleToSlider(R1, vals.r1));
    forceSlider(w->R2Slider(), doubleToSlider(R2, vals.r2));
    forceSlider(w->CurrSlider(), doubleToSlider(CURR, vals.curr));
    forceSlider(w->RatioSlider(), doubleToSlider(RATIO, vals.ratio));

    // Enable/disable sliders and text, if necessary.  They may all be enabled, or
    // at most one will be disabled.
    w->VTopSlider()->setDisabled(vals.disable == VTOP);
    w->VBotSlider()->setDisabled(vals.disable == VBOT);
    w->VMidSlider()->setDisabled(vals.disable == VMID);
    w->R1Slider()->setDisabled(vals.disable == R1);
    w->R2Slider()->setDisabled(vals.disable == R2);
    w->CurrSlider()->setDisabled(vals.disable == CURR);
    w->RatioSlider()->setDisabled(true);

    w->VTopValEdit()->setDisabled(vals.disable == VTOP);
    w->VBotValEdit()->setDisabled(vals.disable == VBOT);
    w->VMidValEdit()->setDisabled(vals.disable == VMID);
    w->R1ValEdit()->setDisabled(vals.disable == R1);
    w->R2ValEdit()->setDisabled(vals.disable == R2);
    w->CurrValEdit()->setDisabled(vals.disable == CURR);
    w->RatioValEdit()->setDisabled(true);

    // Fill in the text with proper numbers.
    w->VTopValEdit()->setText(QString::fromStdString(EngStr::doubleToStr(vals.vtop, 4)));
    w->VBotValEdit()->setText(QString::fromStdString(EngStr::doubleToStr(vals.vbot, 4)));
    w->VMidValEdit()->setText(QString::fromStdString(EngStr::doubleToStr(vals.vmid, 4)));
    w->R1ValEdit()->setText(QString::fromStdString(EngStr::doubleToStr(vals.r1, 4)));
    w->R2ValEdit()->setText(QString::fromStdString(EngStr::doubleToStr(vals.r2, 4)));
    w->CurrValEdit()->setText(QString::fromStdString(EngStr::doubleToStr(vals.curr, 4)));
    w->RatioValEdit()->setText(QString::fromStdString(EngStr::doubleToStr(vals.ratio, 4)));

    // Set labels normal color if constraints are proper, otherwise outputs are red
    // if (ct == PROPER) {
    auto setDirColors = [=](QLineEdit *le, dir d) {
        le->setStyleSheet(isin(d) ? "color: darkblue; background-color: #bffcc6" : "color: darkblue; background-color: #b28dff");
    };
    setDirColors(w->VTopValEdit(), vals.vtopd);
    setDirColors(w->VBotValEdit(), vals.vbotd);
    setDirColors(w->VMidValEdit(), vals.vmidd);
    setDirColors(w->R1ValEdit(), vals.r1d);
    setDirColors(w->R2ValEdit(), vals.r2d);
    setDirColors(w->CurrValEdit(), vals.currd);
    setDirColors(w->RatioValEdit(), vals.ratiod);
    // } else
    // {
    //     auto setErrorBackground = [=](QLineEdit * le) {
    //         le->setStyleSheet("background-color: #ffabab");
    //     };
    //     if (!isin(vals.vtopd)) setErrorBackground(w->VTopValEdit());
    //     if (!isin(vals.vbotd)) setErrorBackground(w->VBotValEdit());
    //     if (!isin(vals.vmidd)) setErrorBackground(w->VMidValEdit());
    //     if (!isin(vals.r1d)) setErrorBackground(w->R1ValEdit());
    //     if (!isin(vals.r2d)) setErrorBackground(w->R2ValEdit());
    //     if (!isin(vals.currd)) setErrorBackground(w->CurrValEdit());
    //     if (!isin(vals.ratiod)) setErrorBackground(w->RatioValEdit());
    // }

    std::string sstr(vals.str());
    w->TextEdit()->appendPlainText(QString::fromStdString(sstr));
}

void UIBridge::forceSlider(QSlider *slider, int val) const
{
    slider->blockSignals(true);
    slider->setSliderPosition(val);
    slider->blockSignals(false);
}
void UIBridge::forceButton(QPushButton *button, bool val) const
{
    button->blockSignals(true);
    button->setChecked(val);
    button->blockSignals(false);
    button->setText(val ? "Input" : "Output");
}

void UIBridge::setWindow(MainWindow *mw)
{
    w = mw;
}
// double UIBridge::getScale() const {return scale;}
// void UIBridge::setScale(double x) {scale = x;}

void UIBridge::setRange(vartype vt, int x1, int x2, double y1, double y2)
{
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
    switch (vt)
    {
    case VTOP:
        vtopmxb.m = m;
        vtopmxb.b = b;
        break;
    case VBOT:
        vbotmxb.m = m;
        vbotmxb.b = b;
        break;
    case VMID:
        vmidmxb.m = m;
        vmidmxb.b = b;
        break;
    case R1:
        r1exp.a = a;
        r1exp.x0 = x0;
        r1exp.r = r;
        break;
    case R2:
        r2exp.a = a;
        r2exp.x0 = x0;
        r2exp.r = r;
        break;
    case CURR:
        currexp.a = a;
        currexp.x0 = x0;
        currexp.r = r;
        break;
    case RATIO:
        ratioexp.a = a;
        ratioexp.x0 = x0;
        ratioexp.r = r;
        break;
    default:
        throw std::logic_error("bad switch in UIBridge::setRange");
    }
}
double UIBridge::sliderToDouble(vartype vt, int x)
{
    // Return linear values mapped values for voltages
    switch (vt)
    {
    case VTOP:
        return vtopmxb.m * x + vtopmxb.b;
    case VBOT:
        return vbotmxb.m * x + vbotmxb.b;
    case VMID:
        return vmidmxb.m * x + vmidmxb.b;
    case R1:
        return (x < r1exp.x0) ? 0.0 : r1exp.a * pow(10.0, (x - r1exp.x0) * r1exp.r);
    case R2:
        return (x < r2exp.x0) ? 0.0 : r2exp.a * pow(10.0, (x - r2exp.x0) * r2exp.r);
    case CURR:
        return (x < currexp.x0) ? 0.0 : currexp.a * pow(10.0, (x - currexp.x0) * currexp.r);
    case RATIO: // deliberately not defined, as we don't expect the ratio slider to be manipulated
    default:
        throw std::logic_error("bad switch in UIBridge::sliderToDouble");
    }
}
int UIBridge::doubleToSlider(vartype vt, double y)
{
    // Return linear value back to slider.
    // TODO: ensure stuff gets calculated again with new integer
    switch (vt)
    {
    case VTOP:
        return int(round((y - vtopmxb.b) / vtopmxb.m));
    case VBOT:
        return int(round((y - vbotmxb.b) / vbotmxb.m));
    case VMID:
        return int(round((y - vmidmxb.b) / vmidmxb.m));
    case R1:
    {
        int ret = int(round(log10(y / r1exp.a) / r1exp.r + r1exp.x0));
        if (ret < r1exp.x0)
            return 0;
        return ret;
    }
    case R2:
    {
        int ret = int(round(log10(y / r2exp.a) / r2exp.r + r2exp.x0));
        if (ret < r2exp.x0)
            return 0;
        return ret;
    }
    case CURR:
    {
        int ret = int(round(log10(y / currexp.a) / currexp.r + currexp.x0));
        if (ret < currexp.x0)
            return 0;
        return ret;
    }
    case RATIO:
    {
        int ret = int(round(log10(y / ratioexp.a) / ratioexp.r + ratioexp.x0));
        if (ret < ratioexp.x0)
            return 0;
        return ret;
    }
    default:
        throw std::logic_error("bad switch in UIBridge::linvalToSlider");
    }
}

void UIBridge::setCoreValue(vartype vt, int sliderVal)
{
    // log expands based on parameters, then sets core value
    pcore->setInput(vt, sliderToDouble(vt, sliderVal));
}

void UIBridge::setCoreValue(vartype vt, double directVal)
{
    // sets core value directly
    pcore->setInput(vt, directVal);
}

void UIBridge::setCoreValue(vartype vt, std::string stringVal)
{
    // Converts from string to double, then sets core value
    pcore->setInput(vt, EngStr::strToDouble(stringVal));
}
