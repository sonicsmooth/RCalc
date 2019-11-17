#ifndef RDIVIDER_H
#define RDIVIDER_H

#include <QWidget>




class RDivider : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double VMax READ VMax WRITE setVMax NOTIFY vmaxChanged)
    Q_PROPERTY(double VMin READ VMin WRITE setVMin NOTIFY vminChanged)
    Q_PROPERTY(double VTop READ VTop WRITE setVTop NOTIFY vtopChanged)
    Q_PROPERTY(double VBot READ VBot WRITE setVBot NOTIFY vbotChanged)
    Q_PROPERTY(double VMid READ VMid WRITE setVMid NOTIFY vmidChanged)

    typedef enum {NO_DRAG, VTOP_DRAG, VBOT_DRAG, VMID_DRAG} drag;
    drag state;
    double m_vmax, m_vmin, m_vtop, m_vbot, m_vmid;
    double m_slope, m_offset;
    int m_margin;
    int m_clickMargin;

    void updateSlope();
    void updateOffset();
    int voltToPixel(double) const;
    double pixelToVolt(int) const;

public:
    explicit RDivider(QWidget *parent = nullptr);
    double VMax() const;
    void setVMax(double);
    double VMin() const;
    void setVMin(double);
    double VTop() const;
    void setVTop(double);
    double VBot() const;
    void setVBot(double);
    double VMid() const;
    void setVMid(double);
protected:
    void paintEvent(QPaintEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void resizeEvent(QResizeEvent *) override;
    
signals:
    void vmaxChanged(double);
    void vminChanged(double);
    void vtopChanged(double);
    void vbotChanged(double);
    void vmidChanged(double);

public slots:
};

#endif // RDIVIDER_H
