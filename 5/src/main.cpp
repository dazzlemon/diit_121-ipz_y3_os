#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

#define MB_MODALERROR (MB_OK | MB_ICONERROR | MB_APPLMODAL)

const wchar_t szWindowClass[] = L"OS5";
const wchar_t szTitle[] = L"OS5";
HINSTANCE hInst;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wcex;
	wcex.cbSize        = sizeof(WNDCLASSEX);
	wcex.style         = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc   = WndProc;
	wcex.cbClsExtra    = 0;
	wcex.cbWndExtra    = 0;
	wcex.hInstance     = hInstance;
	wcex.hIcon         = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName  = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm       = LoadIcon(wcex.hInstance, IDI_APPLICATION);
	if (!RegisterClassEx(&wcex)) {
		MessageBox(NULL, L"Call to RegisterClassEx failed!", L"Message", MB_MODALERROR);
		return 1;
	}
	hInst = hInstance;
	HWND hWnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		500, 150, NULL, NULL,
		hInstance,
		NULL
	);
	HWND hWnd_edit1 = CreateWindow(
		L"Edit",
		L"Введите текст",
		WS_CHILD | WS_VISIBLE | WS_BORDER,
		5, 5, 100, 30,
		hWnd,
		(HMENU)3,
		hInstance,
		NULL
	);
	HWND hWnd_button1 = CreateWindow(
		L"button",
		L"Очистить",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		5, 50, 150, 30,
		hWnd,
		(HMENU)1,
		hInstance,
		NULL
	);
	HWND hWnd_button2 = CreateWindow(
		L"button",
		L"Передать текст",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		160, 50, 300, 30,
		hWnd,
		(HMENU)2,
		hInstance,
		NULL
	);
	if (!hWnd) {
		MessageBox(NULL, L"Call to CreateWindow failed!", L"Message", MB_MODALERROR);
		return 1;
	}
	MoveWindow(hWnd, 700, 100, 500, 120, TRUE);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hdc;
	switch (message) {
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_PAINT:
			break;
		case WM_COPYDATA: {
			PCOPYDATASTRUCT s = (PCOPYDATASTRUCT)lParam;
			wchar_t* str = new wchar_t[50];
			wcscpy(str, (LPCWSTR)s->lpData);
			hdc = BeginPaint(hWnd, &ps);
			SetDlgItemText(hWnd, 3, str);
			EndPaint(hWnd, &ps);
			break;
		}
		case WM_COMMAND: {
			if(LOWORD(wParam) == 1) {
				hdc = BeginPaint(hWnd, &ps);
				SetDlgItemText(hWnd, 3, L"");
				EndPaint(hWnd, &ps);
			}
			if(LOWORD(wParam) == 2) {
				wchar_t* str = new wchar_t[50];
				GetDlgItemText(hWnd, 3, str, 50);
				COPYDATASTRUCT cd;
				cd.dwData = 0;
				cd.cbData = wcslen(str) + 1;
				cd.lpData = str;
				HWND hRecieverWnd = FindWindow(NULL, L"OS5");
				if (hRecieverWnd != 0) {
					SendMessage(hRecieverWnd, WM_COPYDATA, 0, (LPARAM)&cd);
				} else {
					MessageBox(NULL, L"Окно приемника не создано", L"Message", MB_MODALERROR);
				}
			}
			break;
		}
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
			break;
	}
	return 0;
}