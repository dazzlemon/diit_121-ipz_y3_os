#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <string>

#define MB_MODALERROR (MB_OK | MB_ICONERROR | MB_APPLMODAL)
#define ErrorBox(msg) MessageBox(NULL, msg, L"Message", MB_MODALERROR)

wchar_t* otherWindowClassname;
const wchar_t szTitle[] = L"OS5";
HINSTANCE hInst;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
	// arg[0] - ClassName for this window
	// arg[1] - ClassName for other window
	int argc;
	LPWSTR* argv = CommandLineToArgvW(lpCmdLine, &argc);
	if (argc != 2) {
		ErrorBox((L"Expected two args got: \"" + std::wstring(lpCmdLine) + L"\"").c_str());
		return 1;
	}
	const wchar_t* szWindowClass = argv[0];
	otherWindowClassname = argv[1];

	WNDCLASSEX wcex {
		.cbSize        = sizeof(WNDCLASSEX),
		.style         = CS_HREDRAW | CS_VREDRAW,
		.lpfnWndProc   = WndProc,
		.cbClsExtra    = 0,
		.cbWndExtra    = 0,
		.hInstance     = hInstance,
		.hIcon         = LoadIcon(hInstance, IDI_APPLICATION),
		.hCursor       = LoadCursor(NULL, IDC_ARROW),
		.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
		.lpszMenuName  = NULL,
		.lpszClassName = szWindowClass,
		.hIconSm       = LoadIcon(wcex.hInstance, IDI_APPLICATION),
	};
	if (!RegisterClassEx(&wcex)) {
		ErrorBox(L"Call to RegisterClassEx failed!");
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
		L"Enter text",
		WS_CHILD | WS_VISIBLE | WS_BORDER,
		5, 5, 100, 30,
		hWnd,
		(HMENU)3,
		hInstance,
		NULL
	);
	HWND hWnd_button1 = CreateWindow(
		L"button",
		L"Clear",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		5, 50, 150, 30,
		hWnd,
		(HMENU)1,
		hInstance,
		NULL
	);
	HWND hWnd_button2 = CreateWindow(
		L"button",
		L"Send text",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		160, 50, 300, 30,
		hWnd,
		(HMENU)2,
		hInstance,
		NULL
	);
	if (!hWnd) {
		ErrorBox(L"Call to CreateWindow failed!");
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
				COPYDATASTRUCT cd {
					.dwData = 0,
					.cbData = 2 * wcslen(str) + 1,
					.lpData = str,
				};
				HWND hRecieverWnd = FindWindow(otherWindowClassname, NULL);
				if (hRecieverWnd) {
					SendMessage(hRecieverWnd, WM_COPYDATA, 0, (LPARAM)&cd);
				} else {
					ErrorBox((L"Can't find receiver window with classname \"" + std::wstring(otherWindowClassname) + L"\"").c_str());
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