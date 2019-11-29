#include "rdivider.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <QPainter>
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

void RDivider::drawResistor(QPainter *p, int x, int y, int width, int height, int tail, double zags) const {
    // The x, y, width, height define the bounding box. The resistor drops down
    // from the top by tail amount, then does zags full cycles back and forth,
    // starting and stopping at x + width / 2, then drops down another tail to
    // reach the bottom.  Length may be added to tails as needed to ensure the
    // exact integer number of zags, or perhaps the pixel period of each zag is
    // calculated as needed. There are two variables to represent -- resistance
    // and current. Potentially there is also power, but we aren't doing power
    // yet. These should map somehow in an intuitive way to things like the
    // number of zags, the thickness of the line, the color of the line, and
    // also the width of the bounding box.  For now the width of the bounding
    // box represents current, and line thickness and color are fixed. There may
    // also be the need to represent whether the resistor is disabled, meaning
    // you can't click on it to change it. In any case, these things are
    // controlled by the caller; this function is strictly to draw the top and
    // bottom tail, and the zags, using the provided bounding box and QPen.
    
    // Just the bounding box
    //p->drawRect(x, y, width, height);

    // For zags...
    int xleft   = x;
    int xmid    = x + width / 2;
    int xright  = x + width;
    int yspace = height - 2 * tail;
    double yperiod = double(yspace) / double(zags); // in pixels
    double p1by4 = yperiod / 4.0;
    double p2by4 = yperiod / 2.0;
    double ystart = y + tail;

    if (yperiod == 0.0)
        p->drawLine(xleft, int(round(ystart)), xright, int(round(ystart)));
    else {
         p->drawLine(xmid, y , xmid, int(ystart));
        //for (int i=0; i < int(zags)+1; i++)
        int lastx;
        while (true) {
            lastx = xmid;
            if (ystart + p1by4 > y + height) break;
            p->drawLine(xmid, int(round(ystart)), xright, int(round(ystart + p1by4)));
            ystart += p1by4;

            lastx = xright;
            if (ystart + p2by4 > y + height) break;
            p->drawLine(xright, int(round(ystart)), xleft, int(round(ystart + p2by4)));
            ystart += p2by4;

            lastx = xleft;
            if (ystart + p1by4 > y + height) break;
            p->drawLine(xleft, int(round(ystart)), xmid, int(round(ystart + p1by4)));
            ystart += p1by4;
        }
        p->drawLine(lastx, int(round(ystart)), xmid, y+height);
    }
}

void RDivider::paintEvent(QPaintEvent *event) {

    (void) event;
    QPainter painter(this);
    QPen HNormal(Qt::red, m_barHThick, Qt::SolidLine, Qt::FlatCap);
    QPen HDisabled(Qt::gray, m_barHThick, Qt::SolidLine, Qt::FlatCap);
    QPen VNormal(Qt::red, m_barVThick, Qt::SolidLine, Qt::FlatCap);
    QPen VDisabled(Qt::gray, m_barVThick, Qt::SolidLine, Qt::FlatCap);

    //double rthick = (log10(m_curr)-1 + 6.0) * 3;
    //QPen RNormal(Qt::gray, int(round(rthick)), Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin);
    QPen RNormal(Qt::gray, 10, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);

    // Set pixel levels based on voltage, includes offset for vbar
    int vtopp = voltToPixel(m_vtop, VTOP);
    int vbotp = voltToPixel(m_vbot, VBOT);
    int vmidp = voltToPixel(m_vmid, VMID);
    int midpt = (width() - 1) / 2;

    // VTOP
    painter.setPen(m_disabled == VTOP ? HDisabled : HNormal);
    painter.drawLine(m_hMargin, vtopp, width() - 1 - m_hMargin, vtopp);
    painter.setPen(m_disabled == VTOP ? VDisabled : VNormal);
    painter.drawLine(midpt, vtopp, midpt, vtopp + m_barVLong);

    // VBOT
    painter.setPen(m_disabled == VBOT ? HDisabled : HNormal);
    painter.drawLine(m_hMargin, vbotp, width() - 1 - m_hMargin, vbotp);
    painter.setPen(m_disabled == VBOT ? VDisabled : VNormal);
    painter.drawLine(midpt, vbotp - m_barVLong, midpt, vbotp);

    // VMID
    painter.setPen(m_disabled == VMID ? HDisabled : HNormal);
    painter.drawLine(m_hMargin, vmidp, width() - 1 - m_hMargin, vmidp);
    painter.setPen(m_disabled == VMID ? VDisabled : VNormal);
    painter.drawLine(midpt, vmidp - m_barVLong, midpt, vmidp + m_barVLong);
    // Resistors
    int w = int(width() * 0.35); //currToPixel(m_curr);
    int x = (width() - 1 - w) / 2;
    int y1 = vtopp + m_barVLong;
    int y2 = vmidp + m_barVLong;
    int h1 = vmidp - vtopp - 2 * m_barVLong;
    int h2 = vbotp - vmidp - 2 * m_barVLong;
    double zags1 = std::min(std::max(log10(m_r1) + 1.0, 1.0), 12.0);
    double zags2 = std::min(std::max(log10(m_r2) + 1.0, 1.0), 12.0);
    int tail1 = 0; //(h1 - resuse1)/2;
    int tail2 = 0; //(h2 - resuse2)/2;
    painter.setPen(RNormal);
    drawResistor(&painter, x, y1, w, h1, tail1, zags1);
    drawResistor(&painter, x, y2, w, h2, tail2, zags2);
    
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
