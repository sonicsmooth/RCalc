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
QSlider * MainWindow::VTopSlider() {return m_ui->VTopSlider;}
QSlider * MainWindow::VBotSlider() {return m_ui->VBotSlider;}
QSlider * MainWindow::VMidSlider() {return m_ui->VMidSlider;}
QSlider * MainWindow::R1Slider() {return m_ui->R1Slider;}
QSlider * MainWindow::R2Slider() {return m_ui->R2Slider;}
QSlider * MainWindow::RatioSlider() {return m_ui->RatioSlider;}
QSlider * MainWindow::CurrSlider() {return m_ui->CurrSlider;}
QLineEdit * MainWindow::VTopValEdit() {return m_ui->VTopValEdit;}
QLineEdit * MainWindow::VBotValEdit() {return m_ui->VBotValEdit;}
QLineEdit * MainWindow::VMidValEdit() {return m_ui->VMidValEdit;}
QLineEdit * MainWindow::R1ValEdit() {return m_ui->R1ValEdit;}
QLineEdit * MainWindow::R2ValEdit() {return m_ui->R2ValEdit;}
QLineEdit * MainWindow::RatioValEdit() {return m_ui->RatioValEdit;}
QLineEdit * MainWindow::CurrValEdit() {return m_ui->CurrValEdit;}
QPlainTextEdit * MainWindow::TextEdit() {return m_ui->plainTextEdit;}
RDivider * MainWindow::RDivider() {return m_ui->rDivider;}
QPushButton * MainWindow::EngagedButton() {
    return m_ui->EngagedButton;}
