#include "mainwindow.hpp"
#include "ui_mainwindow.h"// auto generated from MainWindow.ui
#include "postfix_evaluator.hpp"

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
, ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	QPushButton* charButtons[15];
	for (int i = 0; i < 10; i++) {
		QString buttonName = "Button" + QString::number(i);
		charButtons[i] = MainWindow::findChild<QPushButton*>(buttonName);
		connect(charButtons[i], SIGNAL(released()), this, SLOT(CharPressed()));
	}

	QString otherCharButtonNames[5] = {
		"ButtonMinus",
		"ButtonPlus",
		"ButtonSlash",
		"ButtonAsterisk",
		"ButtonSpace"
	};
	for (int i = 0; i < 5; i++) {
		charButtons[10 + i] = MainWindow::findChild<QPushButton*>(otherCharButtonNames[i]);
		connect(charButtons[10 + i], SIGNAL(released()), this, SLOT(CharPressed()));
	}

	QPushButton* eraseButton = MainWindow::findChild<QPushButton*>("ButtonErase");
	connect(eraseButton, SIGNAL(released()), this, SLOT(ErasePressed()));

	QPushButton* clearButton = MainWindow::findChild<QPushButton*>("ButtonClear");
	connect(clearButton, SIGNAL(released()), this, SLOT(ClearPressed()));

	QPushButton* evaluateButton = MainWindow::findChild<QPushButton*>("ButtonEvaluate");
	connect(evaluateButton, SIGNAL(released()), this, SLOT(EvaluatePressed()));
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::CharPressed() {
	QPushButton *button = (QPushButton*)sender();
	QString buttonVal = button->text();
	QString oldDisplayVal = ui->Display->text();
	if (buttonVal == "Space") {
		buttonVal = " ";
	}
	ui->Display->setText(oldDisplayVal + buttonVal);
}

void MainWindow::ErasePressed() {
	QString oldDisplayVal = ui->Display->text();
	ui->Display->setText(oldDisplayVal.chopped(1));
}

void MainWindow::ClearPressed() {
	ui->Display->setText("");
}

void MainWindow::EvaluatePressed() {
	std::string expr = ui->Display->text().toStdString();
	PostfixEvaluator pe;
	std::string res = pe.eval(expr);
	ui->Display->setText(QString::fromStdString(res));
}
