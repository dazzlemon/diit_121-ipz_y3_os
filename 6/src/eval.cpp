#include <windows.h>
#include <string>
#include <iostream>
#include "postfix_evaluator.hpp"
#pragma hdrstop
#pragma argsused

PostfixEvaluator pe;

int WINAPI DllMain(HINSTANCE hInst, DWORD fdwReason, PVOID pvReserved) {
	std::cout << "DllMain\n";
	return TRUE;
}

extern "C" char* __declspec(dllexport) __stdcall eval(const char* expr) {
	return strdup(pe.eval(expr).c_str());
}