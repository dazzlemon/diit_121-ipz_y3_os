// #include <windows.h>
// #include <string>
// #include <iostream>
// #include "dll22.hpp"

// int main() {
// 	HINSTANCE module = LoadLibrary("dll22.dll");
// 	if (!module) {
// 		std::cout << "Error, cant load module!\n";
// 	}

// 	int (__stdcall *MyFunc)(int, int) = (int(__stdcall *)(int, int))GetProcAddress(module, "f1");
// 	if (!MyFunc) {
// 		std::cout << "Error, cant load func!\n";
// 	} else {
// 		std::cout << "explicit: " << MyFunc(2, NULL) << '\n';
// 	}
// 	FreeLibrary(module);
	
// 	std::cout << "implicit: " << f1(2) << '\n';
// 	return 0;
// }

// qt main
#include "mainwindow.hpp"
#include <QApplication>

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}