#include "rdivider.h"
#include <iostream>
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

double RDivider::VMax() const {
    return m_vmax;
}
void RDivider::setVMax(double x) {
    m_vmax = x;
    emit vmaxChanged(x);
    update();
}
double RDivider::VMin() const {
    return m_vmin;
}
void RDivider::setVMin(double x) {
    m_vmin = x;
    emit vminChanged(x);
    update();
}
double RDivider::VTop() const {
    return m_vtop;
}
void RDivider::setVTop(double x) {
    m_vtop = x;
    emit vtopChanged(x);
    update();
}
double RDivider::VBot() const {
    return m_vbot;
}
void RDivider::setVBot(double x) {
    m_vbot = x;
    emit vbotChanged(x);
    update();
}
double RDivider::VMid() const {
    return m_vmid;
}
void RDivider::setVMid(double x) {
    m_vmid = x;
    emit vmidChanged(x);
    update();
}

double RDivider::Curr() const {
    return m_curr;
}
void RDivider::setCurr(double x) {
    m_curr = x;
    emit currChanged(x);
    update();
}
void RDivider::setDisabled(vartype vt) {
    m_disabled = vt;
}


double RDivider::CurrMax() const {
    return m_currMax;
}
void RDivider::setCurrMax(double x) {
    m_currMax = x;
    emit currMaxChanged(x);
    update();
}

double RDivider::CurrMin() const {
    return m_currMin;
}
void RDivider::setCurrMin(double x) {
    m_currMin = x;
    emit currMinChanged(x);
    update();
}


void RDivider::paintEvent(QPaintEvent *event) {
    (void) event;
    QPainter painter(this);
    QPen HNormal(Qt::red, m_barHThick, Qt::SolidLine, Qt::FlatCap);
    QPen VNormal(Qt::red, m_barVThick, Qt::SolidLine, Qt::FlatCap);
    QPen RNormal(Qt::gray, 1);
    //painter.setPen(QPen(Qt::blue));
    //painter.drawRect(0,0, width()-1, height()-1);

    // Set pixel levels based on voltage
    int vtopp = voltToPixel(m_vtop, VTOP);
    int vbotp = voltToPixel(m_vbot, VBOT);
    int vmidp = voltToPixel(m_vmid, VMID);
    int midpt = (width() - 1) / 2;

    // Horiz bars
    painter.setPen(HNormal);
    painter.drawLine(m_hMargin, vtopp, width() - 1 - m_hMargin, vtopp);
    painter.drawLine(m_hMargin, vbotp, width() - 1 - m_hMargin, vbotp);
    painter.drawLine(m_hMargin, vmidp, width() - 1 - m_hMargin, vmidp);

    // Vert bars
    painter.setPen(VNormal);
    painter.drawLine(midpt, vtopp,              midpt, vtopp + m_barVLong);
    painter.drawLine(midpt, vbotp - m_barVLong, midpt, vbotp);
    painter.drawLine(midpt, vmidp - m_barVLong, midpt, vmidp + m_barVLong);

    // Resistors
    int reswidth = currToPixel(m_curr);
    int resstart = (width() - 1 - reswidth) / 2;
    painter.setPen(RNormal);
    painter.drawRect(resstart, vtopp + m_barVLong, currToPixel(m_curr), vmidp - m_barVLong - vtopp - m_barVLong);
    painter.drawRect(resstart, vmidp + m_barVLong, currToPixel(m_curr), vbotp - m_barVLong - vmidp - m_barVLong);

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
    auto within = [=](int rc, int voffset) {
        return abs(event->y() - (rc+voffset)) < (m_clickMargin + m_barHThick/2);
        };
    if (within(voltToPixel(m_vtop, VTOP), 0) && m_disabled != VTOP)
        state = VTOP_DRAG;
    else if (within(voltToPixel(m_vbot, VBOT), 0) && m_disabled != VBOT)
        state = VBOT_DRAG;
    else if (within(voltToPixel(m_vmid, VMID), 0) && m_disabled != VMID)
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
