#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent)
        , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    connect(ui->speedSlider, SIGNAL(valueChanged(int)), 
            this,            SLOT(speedSliderChanged(int)));

    tickrate = ui->speedSlider->value();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::speedSliderChanged(int value) {
    QString text = "tickrate = ";
    text += QString::number(value);
    ui->label->setText(text);
    tickrate = value;
}