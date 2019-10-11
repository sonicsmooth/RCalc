#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSlider>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QSlider * VTopSlider();
    QSlider * VBotSlider();
    QSlider * VMidSlider();
    QSlider * R1Slider();
    QSlider * R2Slider();
    QSlider * RatioSlider();
    QSlider * CurrSlider();
    QLabel * VTopValLabel();
    QLabel * VBotValLabel();
    QLabel * VMidValLabel();
    QLabel * R1ValLabel();
    QLabel * R2ValLabel();
    QLabel * RatioValLabel();
    QLabel * CurrValLabel();
    QLabel * VTopKLabel();
    QLabel * VBotKLabel();
    QLabel * VMidKLabel();
    QLabel * R1KLabel();
    QLabel * R2KLabel();
    QLabel * RatioKLabel();
    QLabel * CurrKLabel();
    QPushButton * VTopButton();
    QPushButton * VBotButton();
    QPushButton * VMidButton();
    QPushButton * R1Button();
    QPushButton * R2Button();
    QPushButton * CurrButton();
    QPlainTextEdit * TextEdit();
    


private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
