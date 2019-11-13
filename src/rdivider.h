#ifndef RDIVIDER_H
#define RDIVIDER_H

#include <QWidget>

class RDivider : public QWidget
{
    Q_OBJECT
public:
    explicit RDivider(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *event) override;

signals:

public slots:
};

#endif // RDIVIDER_H
