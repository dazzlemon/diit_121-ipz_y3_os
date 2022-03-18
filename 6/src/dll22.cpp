#include <windows.h>
#include <math.h>
#pragma hdrstop
#pragma argsused

int WINAPI DllMain(HINSTANCE hInst, DWORD fdwReason, PVOID pvReserved) {
	return TRUE;
}

extern "C" float __declspec(dllexport) __stdcall f1(int a) {
	return a * a;
}