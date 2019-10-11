//#include <QtWidgets/QSlider>
//#include <QtWidgets/QPlainTextEdit>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QSlider * MainWindow::VTopSlider() {return ui->VTopSlider;}
QSlider * MainWindow::VBotSlider() {return ui->VBotSlider;}
QSlider * MainWindow::VMidSlider() {return ui->VMidSlider;}
QSlider * MainWindow::R1Slider() {return ui->R1Slider;}
QSlider * MainWindow::R2Slider() {return ui->R2Slider;}
QSlider * MainWindow::RatioSlider() {return ui->RatioSlider;}
QSlider * MainWindow::CurrSlider() {return ui->CurrSlider;}
QLabel * MainWindow::VTopValLabel() {return ui->VTopValLabel;}
QLabel * MainWindow::VBotValLabel() {return ui->VBotValLabel;}
QLabel * MainWindow::VMidValLabel() {return ui->VMidValLabel;}
QLabel * MainWindow::R1ValLabel() {return ui->R1ValLabel;}
QLabel * MainWindow::R2ValLabel() {return ui->R2ValLabel;}
QLabel * MainWindow::RatioValLabel() {return ui->RatioValLabel;}
QLabel * MainWindow::CurrValLabel() {return ui->CurrValLabel;}
QLabel * MainWindow::VTopKLabel() {return ui->VTopKLabel;}
QLabel * MainWindow::VBotKLabel() {return ui->VBotKLabel;}
QLabel * MainWindow::VMidKLabel() {return ui->VMidKLabel;}
QLabel * MainWindow::R1KLabel() {return ui->R1KLabel;}
QLabel * MainWindow::R2KLabel() {return ui->R2KLabel;}
QLabel * MainWindow::RatioKLabel() {return ui->RatioKLabel;}
QLabel * MainWindow::CurrKLabel() {return ui->CurrKLabel;}
QPushButton * MainWindow::VTopButton() {return ui->VTopButton;}
QPushButton * MainWindow::VBotButton() {return ui->VBotButton;}
QPushButton * MainWindow::VMidButton() {return ui->VMidButton;}
QPushButton * MainWindow::R1Button() {return ui->R1Button;}
QPushButton * MainWindow::R2Button() {return ui->R2Button;}
QPushButton * MainWindow::CurrButton() {return ui->CurrButton;}
QPlainTextEdit * MainWindow::TextEdit() {return ui->plainTextEdit;}

