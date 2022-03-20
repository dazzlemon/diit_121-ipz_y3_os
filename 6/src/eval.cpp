#include <windows.h>
#include <iostream>
#include "postfix_evaluator.hpp"
#pragma hdrstop
#pragma argsused

// in windows all global vars are constructed just before DLL_PROCESS_ATTACH
// and destructed right after DLL_PROCESS_DETACH
PostfixEvaluator pe;

int WINAPI DllMain(HINSTANCE hInst, DWORD fdwReason, PVOID pvReserved) {
	switch (fdwReason) { 
		case DLL_PROCESS_ATTACH:
			std::cout << "DllMain process attached\n";
			break;
		case DLL_PROCESS_DETACH:
			std::cout << "DllMain process detached\n";
			break;
	}
	return TRUE;
}

extern "C" char* __declspec(dllexport) __stdcall eval(const char* expr) {
	return strdup(pe.eval(expr).c_str());
}