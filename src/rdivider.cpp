#include "rdivider.h"
#include "vals.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <QPainter>
#include <QColor>
#include <QPolygonF>
#include <QPointF>
#include <QPen>
#include <QMouseEvent>


/* assume there is margin from each of top and bottom
 * topmost is zero, bottom-most is positive
then:
 0        + margin is vMax
 height() - margin is vMin
 we map voltage values to pixels based on widget height


*/

int RDivider::maxWidth() const {
    return width() - 1 - 2 * m_hMargin;
}

void RDivider::updateVFunc() {
    // These are the coefficients used in y=mx+b, where x is volts,
    // and y and b are in pixels.
    // The three offsets are to accomodate the vbar length
    m_slope = -(height() - 2 * (1 + m_vMargin)) / (m_vmax - m_vmin);
    m_offsetVtop = (height() - 1 - m_vMargin) - m_slope * m_vmin - 2 * m_barVLong;
    m_offsetVbot = (height() - 1 - m_vMargin) - m_slope * m_vmin + 2 * m_barVLong;
    m_offsetVmid = (height() - 1 - m_vMargin) - m_slope * m_vmin;
    update();
}
void RDivider::updateCFunc() {
    // Calculates m_acoeff and m_bcoeff such that
    // When curr == m_currMin, then return is m_resMinWidth
    // when curr == m_currMax, then return is width() - 1 - 2 * m_margin;
    m_acoeff = m_currMin;
    m_bcoeff = log10(m_currMax / m_acoeff) / (maxWidth() - m_resMinWidth);
}
int RDivider::voltToPixel(double volt, vartype vt) const {
    // Returns vertical pixel location given voltage
    // aka y = mx + b
    if (vt == VTOP) return int(m_slope * volt + m_offsetVtop);
    if (vt == VBOT) return int(m_slope * volt + m_offsetVbot);
    if (vt == VMID) return int(m_slope * volt + m_offsetVmid);
    throw std::logic_error("Error in voltToPixel");
     
}
double RDivider::pixelToVolt(int pixel, vartype vt) const {
    // Return voltage given vertical pixel location
    if (vt == VTOP) return (pixel - m_offsetVtop ) / m_slope;
    if (vt == VBOT) return (pixel - m_offsetVbot ) / m_slope;
    if (vt == VMID) return (pixel - m_offsetVmid ) / m_slope;
    throw std::logic_error("Error in pixelToVolt");
}
int RDivider::currToPixel(double curr) const {
    // returns pixel width of resistor given current.
    // When curr < m_currMin, then return is m_resMinWidth
    // when curr > m_currMax, then return is width() - 1 - 2 * m_margin;
    // otherwise return the log of the current such that 
    // the minimum current maps to the minimum resistor width,
    // and the max of the currnt maps to the maximum window width
    if (curr < m_currMin) return m_resMinWidth;
    if (curr > m_currMax) return maxWidth();
    return int(round(log10(curr / m_acoeff) / m_bcoeff));
}

RDivider::RDivider(QWidget *parent) : 
    QWidget(parent),
    m_vmax (10),
    m_vmin (0),
    m_vtop (10),
    m_vbot (0),
    m_vmid (5),
    m_currMax(3),
    m_currMin(0.001),
    m_curr(0.01),
    m_hMargin (20),
    m_vMargin (40),
    m_clickMargin(2),
    m_barHThick(7),
    m_barVThick(3),
    m_barVLong(15),
    m_resThick(3),
    m_resMinWidth(3),
    m_inCode(0),
    m_disabled(NONE)
{
    updateVFunc();
    updateCFunc();
}

double RDivider::VMax() const {return m_vmax;}
double RDivider::VMin() const {return m_vmin;}
double RDivider::VTop() const {return m_vtop;}
double RDivider::VBot() const {return m_vbot;}
double RDivider::VMid() const {return m_vmid;}
double RDivider::R1() const {return m_r1;}
double RDivider::R2() const {return m_r2;}
double RDivider::Curr() const {return m_curr;}
double RDivider::CurrMax() const {return m_currMax;}
double RDivider::CurrMin() const {return m_currMin;}

void RDivider::setVMax(double x) {
    m_vmax = x;
    emit vmaxChanged(x);
    update();
}
void RDivider::setVMin(double x) {
    m_vmin = x;
    emit vminChanged(x);
    update();
}
void RDivider::setVTop(double x) {
    m_vtop = x;
    emit vtopChanged(x);
    update();
}
void RDivider::setVBot(double x) {
    m_vbot = x;
    emit vbotChanged(x);
    update();
}
void RDivider::setVMid(double x) {
    m_vmid = x;
    emit vmidChanged(x);
    update();
}
void RDivider::setR1(double x) {
    m_r1 = x;
    emit r1Changed(x);
    update();
}
void RDivider::setR2(double x) {
    m_r2 = x;
    emit r2Changed(x);
    update();
}
void RDivider::setCurr(double x) {
    m_curr = x;
    emit currChanged(x);
    update();
}
void RDivider::setDisabled(vartype vt) {
    m_disabled = vt;
}
void RDivider::setIncode(int ic) {
    m_inCode = ic;
}

void RDivider::setCurrMax(double x) {
    m_currMax = x;
    emit currMaxChanged(x);
    update();
}
void RDivider::setCurrMin(double x) {
    m_currMin = x;
    emit currMinChanged(x);
    update();
}

void RDivider::drawResistor(QPainter *p, int x, int y, int width, int height, double mintail, double zags) const {
    // Draws the resistor zig zags and vertical tails
    // x,y,width,height are as though for a rectangle
    // mintail is the shortest the tails will be
    // zags is how many full cycles of zig-zag there are
    int numpts = int(std::floor(double(zags) * 4.0) + 1.0);
    int usableheight = height - int(round(2.0 * mintail));
    double nomperiod = 2.0 * width / sqrt(3);
    double idealuse = nomperiod * zags;
    double actualuse = std::min(idealuse, double(usableheight));
    double actualperiod = actualuse / zags;
    double qperiod = actualperiod / 4.0;
    double actualtail = (height - actualuse) / 2.0;

    double corners[] = {0.0, width/2.0, 0.0, -width/2.0};
    double xos = double(x) + width/2.0;
    double lastx = xos;
    double lasty = y;
    double thisx = xos;
    double thisy = y + actualtail;
    double slope = 0.0;

    QPolygonF pts;
    pts << QPointF(xos, y);
    if (qperiod < 1.0) {
        pts << QPointF(xos + corners[3], thisy);
        pts << QPointF(xos + corners[1], thisy);
    } else {
    for (int i = 0; i < numpts; i++) {
        int idx = i & 0x3;
        thisx = corners[idx] + double(xos);
        slope = (thisx - lastx) / (thisy - lasty); // weird
        pts << QPointF(thisx, thisy);
        lastx = thisx;
        lasty = thisy;
        thisy += qperiod;
        }
    }
    slope = lastx == xos ? slope : -slope;
    thisy = y + height - actualtail;
    thisx = lastx + slope * (thisy - lasty);
    pts << QPointF(thisx, thisy);
    pts << QPointF(xos, thisy);
    pts << QPointF(xos, y + height);
    p->drawPolyline(pts);
}

void RDivider::paintEvent(QPaintEvent *event) {

    (void) event;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawRect(0,0,width()-1,height()-1);

    double rthick = (log10(m_curr)-1 + 6.0) * 3;
    double borderp = 3.0; // adds to rthick


    QColor cinput("#bffcc6");
    QColor coutput("#b28dff");
    
    // Horizontal bar pens
    QPen HInput   (cinput,    rthick, Qt::SolidLine, Qt::FlatCap);
    QPen HOutput  (coutput,   rthick, Qt::SolidLine, Qt::FlatCap);
    QPen HBg      (Qt::black, rthick, Qt::SolidLine, Qt::FlatCap);
    QPen HDisabled(Qt::gray,  rthick, Qt::SolidLine, Qt::FlatCap);
    HInput.setWidthF(rthick);
    HOutput.setWidthF(rthick);
    HBg.setWidthF(rthick + 2*borderp);
    HDisabled.setWidthF(rthick);

    // Vertical bar pens
    QPen VInput   (cinput,    rthick, Qt::SolidLine, Qt::FlatCap);
    QPen VOutput  (coutput,   rthick, Qt::SolidLine, Qt::FlatCap);
    QPen VBg      (Qt::black, rthick, Qt::SolidLine, Qt::FlatCap);
    QPen VDisabled(Qt::gray,  rthick, Qt::SolidLine, Qt::FlatCap);
    VInput.setWidthF(rthick);
    VOutput.setWidthF(rthick);
    VBg.setWidthF(rthick + borderp);
    VDisabled.setWidthF(rthick);

    // Resistor pens
    QPen RInput   (cinput,    rthick, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
    QPen ROutput  (coutput,   rthick, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
    QPen RBg      (Qt::black, rthick, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
    QPen RDisabled(Qt::gray,  rthick, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
    RInput.setWidthF(rthick);
    ROutput.setWidthF(rthick);
    RBg.setWidthF(rthick + borderp);
    RDisabled.setWidthF(rthick);

    // Set pixel levels based on voltage, includes offset for vbar
    int vtopp = voltToPixel(m_vtop, VTOP);
    int vbotp = voltToPixel(m_vbot, VBOT);
    int vmidp = voltToPixel(m_vmid, VMID);
    int midpt = (width() - 1) / 2;

    // VTOP
    // background/border
    painter.setPen(HBg);
    painter.drawLine(m_hMargin - borderp, vtopp, width() - 1 - m_hMargin + borderp, vtopp);
    painter.setPen(VBg);
    painter.drawLine(midpt, vtopp - borderp, midpt, vtopp + m_barVLong + borderp);
    // foreground/main
    painter.setPen(m_disabled == VTOP ? HDisabled : isin(m_inCode, VTOP) ? HInput : HOutput);
    painter.drawLine(m_hMargin, vtopp, width() - 1 - m_hMargin, vtopp);
    painter.setPen(m_disabled == VTOP ? VDisabled : isin(m_inCode, VTOP) ? VInput : VOutput);
    painter.drawLine(midpt, vtopp, midpt, vtopp + m_barVLong);

    // VBOT
    // background
    painter.setPen(HBg);
    painter.drawLine(m_hMargin - borderp, vbotp, width() - 1 - m_hMargin + borderp, vbotp);
    painter.setPen(VBg);
    painter.drawLine(midpt, vbotp - m_barVLong, midpt, vbotp);
    // foreground
    painter.setPen(m_disabled == VBOT ? HDisabled : isin(m_inCode, VBOT) ? HInput : HOutput);
    painter.drawLine(m_hMargin, vbotp, width() - 1 - m_hMargin, vbotp);
    painter.setPen(m_disabled == VBOT ? VDisabled : isin(m_inCode, VBOT) ? VInput : VOutput);
    painter.drawLine(midpt, vbotp - m_barVLong, midpt, vbotp);

    // VMID
    painter.setPen(HBg);
    painter.drawLine(m_hMargin - borderp, vmidp, width() - 1 - m_hMargin + borderp, vmidp);
    painter.setPen(VBg);
    painter.drawLine(midpt, vmidp - m_barVLong, midpt, vmidp + m_barVLong);

    painter.setPen(m_disabled == VMID ? HDisabled : isin(m_inCode, VMID) ? HInput : HOutput);
    painter.drawLine(m_hMargin, vmidp, width() - 1 - m_hMargin, vmidp);
    painter.setPen(m_disabled == VMID ? VDisabled : isin(m_inCode, VMID) ? VInput : VOutput);
    painter.drawLine(midpt, vmidp - m_barVLong, midpt, vmidp + m_barVLong);

    // Resistors
    int w = int(width() * 0.25);
    int x = (width() - 1 - w) / 2;
    double minzags = 2;
    double maxzags = 12;
    double zags1 = std::min(std::max(log10(m_r1) + 1.0, minzags), maxzags);
    double zags2 = std::min(std::max(log10(m_r2) + 1.0, minzags), maxzags);
    double mintail = 2;
    int y1 = vtopp + m_barVLong;
    int y2 = vmidp + m_barVLong;
    int h1 = vmidp - vtopp - 2 * m_barVLong;
    int h2 = vbotp - vmidp - 2 * m_barVLong;
    painter.setPen(RBg);
    drawResistor(&painter, x, y1, w, h1, mintail, zags1);
    painter.setPen(m_disabled == vartype::R1 ? RDisabled : isin(m_inCode, vartype::R1) ? RInput : ROutput);
    drawResistor(&painter, x, y1, w, h1, mintail, zags1);

    painter.setPen(RBg);
    drawResistor(&painter, x, y2, w, h2, mintail, zags2);
    painter.setPen(m_disabled == vartype::R2 ? RDisabled : isin(m_inCode, vartype::R2) ? RInput : ROutput);
    drawResistor(&painter, x, y2, w, h2, mintail, zags2);

    painter.end();

}

void RDivider::mouseMoveEvent(QMouseEvent *event) {
    if (state == NO_DRAG)
        return;
    int mousey = event->y();
    if (state == VTOP_DRAG)
        setVTop(pixelToVolt(mousey, VTOP));
    else if (state == VBOT_DRAG)
        setVBot(pixelToVolt(mousey, VBOT));
    else if (state == VMID_DRAG)
        setVMid(pixelToVolt(mousey, VMID));
}

void RDivider::mousePressEvent(QMouseEvent *event) {
    if (!(event->buttons() & Qt::LeftButton)) {
        state = RDivider::NO_DRAG;
        return;
    }
    auto within = [=](int rc) {
        return abs(event->y() - rc) < (m_clickMargin + m_barHThick/2);
        };
    if (within(voltToPixel(m_vtop, VTOP)) && m_disabled != VTOP)
        state = VTOP_DRAG;
    else if (within(voltToPixel(m_vbot, VBOT)) && m_disabled != VBOT)
        state = VBOT_DRAG;
    else if (within(voltToPixel(m_vmid, VMID)) && m_disabled != VMID)
        state = VMID_DRAG;
}
void RDivider::mouseReleaseEvent(QMouseEvent * event) {
    if (!(event->buttons() & Qt::LeftButton))
        state = RDivider::NO_DRAG;
}

void RDivider::resizeEvent(QResizeEvent *event) {
    updateVFunc();
    updateCFunc();
}
