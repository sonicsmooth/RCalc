#include <QSlider>
#include <cmath>
#include <iostream>
#include "logslider.h"

LogSlider::LogSlider(QWidget *parent) : QSlider(parent)
{

}

double LogSlider::getExtValue() const {
    return m_extValue;
}
void LogSlider::setExtValue(double x) {
    // called from outside
    // sets slider position and built-in value to log10 of x
    m_extValue = x;
    setValue(int(1000.0*log10(x)));
    std::cout << "m_extValue: " << m_extValue << std::endl;
}


void LogSlider::sliderChange(QAbstractSlider::SliderChange change) {
    int pos = sliderPosition();
    m_extValue = pow(10.0, (double(pos) / 1000.0));
    std::cout << "Position: " << pos << ", m_extValue: " << m_extValue << std::endl;
    QSlider::sliderChange(change);
}
