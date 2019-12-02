#ifndef RDIVIDER_H
#define RDIVIDER_H

#include <QWidget>
#include "rctypes.h"



class RDivider : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double VMax    READ VMax    WRITE setVMax    NOTIFY vmaxChanged)
    Q_PROPERTY(double VMin    READ VMin    WRITE setVMin    NOTIFY vminChanged)
    Q_PROPERTY(double VTop    READ VTop    WRITE setVTop    NOTIFY vtopChanged)
    Q_PROPERTY(double VBot    READ VBot    WRITE setVBot    NOTIFY vbotChanged)
    Q_PROPERTY(double VMid    READ VMid    WRITE setVMid    NOTIFY vmidChanged)
    Q_PROPERTY(double R1      READ R1      WRITE setR1      NOTIFY r1Changed)
    Q_PROPERTY(double R2      READ R1      WRITE setR2      NOTIFY r2Changed)
    Q_PROPERTY(double CurrMax READ CurrMax WRITE setCurrMax NOTIFY currMaxChanged)
    Q_PROPERTY(double CurrMin READ CurrMin WRITE setCurrMin NOTIFY currMinChanged)
    Q_PROPERTY(double Curr    READ Curr    WRITE setCurr    NOTIFY currChanged)

    typedef enum {NO_DRAG, VTOP_DRAG, VBOT_DRAG, VMID_DRAG} drag;
    drag state;
    double m_vmax, m_vmin, m_vtop, m_vbot, m_vmid, m_r1, m_r2;
    double m_currMax, m_currMin, m_curr;
    double m_slope, m_offsetVtop, m_offsetVbot, m_offsetVmid; // used for voltage mapping
    double m_acoeff, m_bcoeff; // used for current mapping
    int m_hMargin, m_vMargin;
    int m_clickMargin;
    int m_barHThick;
    int m_barVThick;
    int m_barVLong;
    int m_resThick;
    int m_resMinWidth;
    vartype m_disabled;

    int maxWidth() const;
    void updateVFunc(); // y=mx+b for voltages
    void updateCFunc(); // y=log(curr) for currents
    int voltToPixel(double, vartype) const;
    double pixelToVolt(int, vartype) const;
    int currToPixel(double) const;
    void drawResistor(QPainter *, int x, int y, int width, int height, double mintail, double zags) const;

public:
    explicit RDivider(QWidget *parent = nullptr);
    double VMax() const;
    double VMin() const;
    double VTop() const;
    double VBot() const;
    double VMid() const;
    double R1() const;
    double R2() const;
    double CurrMax() const;
    double CurrMin() const;
    double Curr() const;
    void setVMax(double);
    void setVMin(double);
    void setVTop(double);
    void setVBot(double);
    void setVMid(double);
    void setR1(double);
    void setR2(double);
    void setCurrMax(double);
    void setCurrMin(double);
    void setCurr(double);
    void setDisabled(vartype);
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
    void r1Changed(double);
    void r2Changed(double);
    void currMaxChanged(double);
    void currMinChanged(double);
    void currChanged(double);

public slots:
};

#endif // RDIVIDER_H
