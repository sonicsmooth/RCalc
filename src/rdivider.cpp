#include "rdivider.h"
#include <iostream>
#include <QPainter>
#include <QPen>


/* assume there is margin from each of top and bottom
 * topmost is zero, bottom-most is positive
then:
 0        + margin is vMax
 height() - margin is vMin
 we map voltage values to pixels based on widget height


*/

double RDivider::calculateSlope(double x0, double x1) {
    // x0 and x1 represent the min and max voltages represented
    return (height() + 1) / (x1 - x0);
}
double RDivider::calculateOffset(double slope, double x0, double y0) {
    // x0 is the minimum voltage
    // y0 is the minimum pixel (top of box)
    return y0 - slope * x0;
}


RDivider::RDivider(QWidget *parent) : QWidget(parent)
{
    m_vmax = 10;
    m_vmin = 0;
    m_margin = 10;
    m_slope = calculateSlope(0, 10);
    m_offset = calculateOffset(m_slope, 0, m_margin);
}

double RDivider::VMax() const {
    return m_vmax;
}
void RDivider::setVMax(double x) {
    m_vmax = x;
    emit vmaxChanged(x);
}
double RDivider::VMin() const {
    return m_vmin;
}
void RDivider::setVMin(double x) {
    m_vmin = x;
    emit vminChanged(x);
}
double RDivider::VTop() const {
    return m_vtop;
}
void RDivider::setVTop(double x) {
    m_vtop = x;
    emit vtopChanged(x);
}
double RDivider::VBot() const {
    return m_vbot;
}
void RDivider::setVBot(double x) {
    m_vbot = x;
    emit vbotChanged(x);
}
double RDivider::VMid() const {
    return m_vmid;
}
void RDivider::setVMid(double x) {
    m_vmid = x;
    emit vmidChanged(x);
}

void RDivider::paintEvent(QPaintEvent *event) {
    //std::cout << width() << " " << height() << std::endl;

    QPainter painter(this);
    painter.setPen(QPen(Qt::blue));
    painter.drawRect(0,0, width()-1, height()-1);
}

void RDivider::mouseMoveEvent(QMouseEvent *event) {
    std::cout << "Move" << std::endl;
}

void RDivider::mousePressEvent(QMouseEvent *event) {
    std::cout << "press" << std::endl;
}

void RDivider::resizeEvent(QResizeEvent *event) {
    m_slope = calculateSlope(m_vmin, m_vmax);
    m_offset = calculateOffset(m_slope, m_vmin, m_margin);
//    std::cout << "resize" << std::endl;
//    std::cout << width() << " " << height() << std::endl;
    std::cout << m_slope << " " << m_offset << std::endl;
}
