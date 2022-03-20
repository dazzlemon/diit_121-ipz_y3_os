#include <windows.h>
#include <string>
#include "postfix_evaluator.hpp"
#pragma hdrstop
#pragma argsused

int WINAPI DllMain(HINSTANCE hInst, DWORD fdwReason, PVOID pvReserved) {
	return TRUE;
}

extern "C" char* __declspec(dllexport) __stdcall eval(const char* expr) {
	PostfixEvaluator pe;
	return strdup(pe.eval(expr).c_str());
}