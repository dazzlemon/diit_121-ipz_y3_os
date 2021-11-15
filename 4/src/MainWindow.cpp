#include "MainWindow.h"
#include "ui_MainWindow.h"// auto generated from MainWindow.ui

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent)
        , ui(new Ui::MainWindow) {
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
}