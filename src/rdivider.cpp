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

void RDivider::updateSlope() {
    // arguments are the min and max voltages represented
    m_slope = -(height() - 2 * (1 + m_margin)) / (m_vmax - m_vmin);
    update();
}
void RDivider::updateOffset() {
    // vmin is the minimum voltage
    // top is the minimum pixel (top of box), typically margin
    m_offset = (height() - 1 - m_margin) - m_slope * m_vmin;
    update();
}
int RDivider::voltToPixel(double volt) const {
    // Returns vertical pixel location given voltage
    // Uses member variables for slope and offset
    return int(m_slope * volt + m_offset); 
}
double RDivider::pixelToVolt(int pixel) const {
    // Return voltage given vertical pixel location
    // uses member variables for slope and offset
    return (pixel - m_offset ) / m_slope;
}

RDivider::RDivider(QWidget *parent) : QWidget(parent)
{
    m_vmax = 10;
    m_vmin = 0;
    m_margin = 5;
    m_vtop = 10;
    m_vbot = 0;
    m_vmid = 5;
    updateSlope();
    updateOffset();
    m_clickMargin = 3;
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

void RDivider::paintEvent(QPaintEvent *event) {
    //std::cout << width() << " " << height() << std::endl;

    QPainter painter(this);
    painter.setPen(QPen(Qt::blue));
    painter.drawRect(0,0, width()-1, height()-1);

    int vtopp = voltToPixel(m_vtop);
    int vbotp = voltToPixel(m_vbot);
    int vmidp = voltToPixel(m_vmid);
    painter.setPen(QPen(Qt::red));
    painter.drawLine(m_margin, vtopp, width()-1-m_margin, vtopp);
    painter.drawLine(m_margin, vbotp, width()-1-m_margin, vbotp);
    painter.drawLine(m_margin, vmidp, width()-1-m_margin, vmidp);

    painter.end();

}

void RDivider::mouseMoveEvent(QMouseEvent *event) {
    if (state == NO_DRAG)
        return;
    int mousey = event->y();
    if (state == VTOP_DRAG)
        setVTop(pixelToVolt(mousey));
    else if (state == VBOT_DRAG)
        setVBot(pixelToVolt(mousey));
    else if (state == VMID_DRAG)
        setVMid(pixelToVolt(mousey));
}

void RDivider::mousePressEvent(QMouseEvent *event) {
    if (!(event->buttons() & Qt::LeftButton)) {
        state = RDivider::NO_DRAG;
        std::cout << "no drag" << std::endl;
        return;
    }

    auto within = [=](int rc) {return abs(event->y() - rc) < m_clickMargin;};
    if (within(voltToPixel(m_vtop))) {
        state = VTOP_DRAG;
        std::cout << "vtop drag" << std::endl;
    }
    else if (within(voltToPixel(m_vbot))) {
        state = VBOT_DRAG;
        std::cout << "vbot drag" << std::endl;
    }
    else if (within(voltToPixel(m_vmid))) {
        state = VMID_DRAG;
        std::cout << "vmid drag" << std::endl;
    }
}
void RDivider::mouseReleaseEvent(QMouseEvent * event) {
    if (!(event->buttons() & Qt::LeftButton)) {
        state = RDivider::NO_DRAG;
        std::cout << "no drag" << std::endl;
        return;
    }
}

void RDivider::resizeEvent(QResizeEvent *event) {
    updateSlope();
    updateOffset();
    std::cout << width() << "x" << height() << "-> ";
    std::cout << "slope|offset = " << m_slope << "|" << m_offset << "\t";
    std::cout << "vmin|vmax = " << m_vmin << "|" << m_vmax << "\t";
    std::cout << "pxmin|pxmax = " << voltToPixel(m_vmin) << "|" << voltToPixel(m_vmax) << std::endl;
}