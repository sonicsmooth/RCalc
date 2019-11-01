#ifndef LOGSLIDER_H
#define LOGSLIDER_H

#include <QSlider>

class LogSlider : public QSlider
{
    Q_OBJECT
private:
    double m_extValue;
    Q_PROPERTY(double extValue  MEMBER m_extValue  READ getExtValue  WRITE setExtValue  NOTIFY extValueChanged)

public:
    explicit LogSlider(QWidget *parent = nullptr);
    void sliderChange(QAbstractSlider::SliderChange);
    double getExtValue() const;
    void setExtValue(double); // called from outside

signals:
    void extValueChanged(double);
public slots:
};

#endif // LOGSLIDER_H
