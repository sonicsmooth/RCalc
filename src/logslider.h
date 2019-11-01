#ifndef LOGSLIDER_H
#define LOGSLIDER_H

#include <QSlider>

class LogSlider : public QSlider
{
    Q_OBJECT
    Q_PROPERTY(double extValue READ getExtValue WRITE setExtValue NOTIFY extValueChanged)
public:
    explicit LogSlider(QWidget *parent = nullptr);
    void sliderChange(QAbstractSlider::SliderChange);
    double getExtValue() const;
    void setExtValue(double);

signals:
    void extValueChanged();
public slots:
};

#endif // LOGSLIDER_H
