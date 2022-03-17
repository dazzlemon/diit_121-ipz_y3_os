#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <string>
#include <array>
#include <optional>

#define MB_MODALERROR (MB_OK | MB_ICONERROR | MB_APPLMODAL)
#define ErrorBox(msg) MessageBox(NULL, msg, L"Message", MB_MODALERROR)

#define CELLSIZE 150

wchar_t* otherWindowClassname;
const wchar_t szTitle[] = L"OS5";
HINSTANCE hInst;
bool isSecondPlayer = false;

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
	isSecondPlayer = FindWindow(otherWindowClassname, NULL);// if this window was created last it is secondPlayer

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
		CELLSIZE * 3, CELLSIZE * 3, NULL, NULL,
		hInstance,
		NULL
	);
	std::array<std::array<HWND, 3>, 3> tictactoeButtons;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			tictactoeButtons[i][j] = CreateWindow(
				L"button",
				L"_",
				WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | (isSecondPlayer ? WS_DISABLED : 0),
				0 + i * CELLSIZE, 0 + j * CELLSIZE, CELLSIZE, CELLSIZE,
				hWnd,
				HMENU(3 * i + j),
				hInstance,
				NULL
			);
		}
	}
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

enum class Player { X, O };
using Grid = std::array<std::array<
	std::optional<Player>
, 3>, 3>;

#define Empty std::optional<Player>{}

Grid grid;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hdc;
	switch (message) {
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_COPYDATA: {
			PCOPYDATASTRUCT s = (PCOPYDATASTRUCT)lParam;
			int id = LOWORD(LPARAM(s->lpData));
			int i = id / 3;
			int j = id % 3;
			grid[i][j] = std::optional(isSecondPlayer ? Player::X : Player::O);

			// set button text
			hdc = BeginPaint(hWnd, &ps);
			SetDlgItemText(hWnd, id, isSecondPlayer ? L"X" : L"O");
			EndPaint(hWnd, &ps);

			// enable empty buttons(
			for (int i = 0; i < 9; i++) {
				if (!grid[i][j]) {
					EnableWindow(GetDlgItem(hWnd, i), true);
				}
			}
			break;
		}
		case WM_COMMAND: {
			HWND hRecieverWnd = FindWindow(otherWindowClassname, NULL);
			if (hRecieverWnd) {
				int id = LOWORD(lParam);
				int i = id / 3;
				int j = id % 3;
				grid[i][j] = std::optional(isSecondPlayer ? Player::O : Player::X);

				// set button text
				hdc = BeginPaint(hWnd, &ps);
				SetDlgItemText(hWnd, LOWORD(lParam), isSecondPlayer ? L"O" : L"X");
				EndPaint(hWnd, &ps);

				// disable own buttons
				for (int i = 0; i < 9; i++) {
					EnableWindow(GetDlgItem(hWnd, i), false);
				}

				// ask other window to enable it's buttons
				COPYDATASTRUCT cd {
					.dwData = 0,
					.cbData = sizeof(wParam),
					.lpData = PVOID(wParam),
				};
				SendMessage(hRecieverWnd, WM_COPYDATA, 0, (LPARAM)&cd);
			} else {
				ErrorBox((L"Can't find receiver window with classname \"" + std::wstring(otherWindowClassname) + L"\"").c_str());
			}
			break;
		}
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
			break;
	}
	return 0;
}