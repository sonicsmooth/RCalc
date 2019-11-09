#include <QSlider>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QPlainTextEdit>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete m_ui;
}
//Ui::MainWindow * MainWindow::ui() {return m_ui;}
QSlider * MainWindow::VTopSlider() {return m_ui->VTopSlider;}
QSlider * MainWindow::VBotSlider() {return m_ui->VBotSlider;}
QSlider * MainWindow::VMidSlider() {return m_ui->VMidSlider;}
QSlider * MainWindow::R1Slider() {return m_ui->R1Slider;}
QSlider * MainWindow::R2Slider() {return m_ui->R2Slider;}
QSlider * MainWindow::RatioSlider() {return m_ui->RatioSlider;}
QSlider * MainWindow::CurrSlider() {return m_ui->CurrSlider;}
//QLabel * MainWindow::VTopValLabel() {return m_ui->VTopValLabel;}
//QLabel * MainWindow::VBotValLabel() {return m_ui->VBotValLabel;}
//QLabel * MainWindow::VMidValLabel() {return m_ui->VMidValLabel;}
//QLabel * MainWindow::R1ValLabel() {return m_ui->R1ValLabel;}
//QLabel * MainWindow::R2ValLabel() {return m_ui->R2ValLabel;}
//QLabel * MainWindow::RatioValLabel() {return m_ui->RatioValLabel;}
//QLabel * MainWindow::CurrValLabel() {return m_ui->CurrValLabel;}
QLineEdit * MainWindow::VTopValEdit() {return m_ui->VTopValEdit;}
QLineEdit * MainWindow::VBotValEdit() {return m_ui->VBotValEdit;}
QLineEdit * MainWindow::VMidValEdit() {return m_ui->VMidValEdit;}
QLineEdit * MainWindow::R1ValEdit() {return m_ui->R1ValEdit;}
QLineEdit * MainWindow::R2ValEdit() {return m_ui->R2ValEdit;}
QLineEdit * MainWindow::RatioValEdit() {return m_ui->RatioValEdit;}
QLineEdit * MainWindow::CurrValEdit() {return m_ui->CurrValEdit;}
//QPushButton * MainWindow::VTopButton() {return m_ui->VTopButton;}
//QPushButton * MainWindow::VBotButton() {return m_ui->VBotButton;}
//QPushButton * MainWindow::VMidButton() {return m_ui->VMidButton;}
//QPushButton * MainWindow::R1Button() {return m_ui->R1Button;}
//QPushButton * MainWindow::R2Button() {return m_ui->R2Button;}
//QPushButton * MainWindow::CurrButton() {return m_ui->CurrButton;}
QPlainTextEdit * MainWindow::TextEdit() {return m_ui->plainTextEdit;}

