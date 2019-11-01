#include <QSlider>
#include <iostream>
#include "logslider.h"

LogSlider::LogSlider(QWidget *parent) : QSlider(parent)
{

}

double LogSlider::getExtValue() const {

}
void LogSlider::setExtValue(double) {
    
}


void LogSlider::sliderChange(QAbstractSlider::SliderChange change) {
    std::cout << this->sliderPosition() << std::endl;
    QSlider::sliderChange(change);
}
