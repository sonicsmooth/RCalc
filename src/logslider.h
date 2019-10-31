#ifndef LOGSLIDER_H
#define LOGSLIDER_H

#include <QSlider>

class LogSlider : public QSlider
{
    Q_OBJECT
public:
    explicit LogSlider(QWidget *parent = nullptr);
public slots:
   void setValue(int);

signals:

public slots:
};

#endif // LOGSLIDER_H
