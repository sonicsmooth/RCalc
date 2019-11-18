#ifndef RDIVIDER_H
#define RDIVIDER_H

#include <QWidget>




class RDivider : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double VMax    READ VMax    WRITE setVMax    NOTIFY vmaxChanged)
    Q_PROPERTY(double VMin    READ VMin    WRITE setVMin    NOTIFY vminChanged)
    Q_PROPERTY(double VTop    READ VTop    WRITE setVTop    NOTIFY vtopChanged)
    Q_PROPERTY(double VBot    READ VBot    WRITE setVBot    NOTIFY vbotChanged)
    Q_PROPERTY(double VMid    READ VMid    WRITE setVMid    NOTIFY vmidChanged)
    Q_PROPERTY(double CurrMax READ CurrMax WRITE setCurrMax NOTIFY currMaxChanged)
    Q_PROPERTY(double CurrMin READ CurrMin WRITE setCurrMin NOTIFY currMinChanged)
    Q_PROPERTY(double Curr    READ Curr    WRITE setCurr    NOTIFY currChanged)

    typedef enum {NO_DRAG, VTOP_DRAG, VBOT_DRAG, VMID_DRAG} drag;
    drag state;
    double m_vmax, m_vmin, m_vtop, m_vbot, m_vmid;
    double m_currMax, m_currMin, m_curr;
    double m_slope, m_offset; // used for voltage mapping
    double m_lcoeff, m_ldiv; // used for current mapping
    int m_margin;
    int m_clickMargin;
    int m_barHThick;
    int m_barVThick;
    int m_barVLong;
    int m_resThick;
    int m_resMinWidth;

    void updateVFunc(); // y=mx+b for voltages
    void updateCFunc(); // y=log(curr) for currents
    int voltToPixel(double) const;
    double pixelToVolt(int) const;
    int currToPixel(double) const;

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
    double CurrMax() const;
    void setCurrMax(double);
    double CurrMin() const;
    void setCurrMin(double);
    double Curr() const;
    void setCurr(double);
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
    void currMaxChanged(double);
    void currMinChanged(double);
    void currChanged(double);

public slots:
};

#endif // RDIVIDER_H
