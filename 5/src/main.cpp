#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <string>
#include <array>

#define MB_MODALERROR (MB_OK | MB_ICONERROR | MB_APPLMODAL)
#define ErrorBox(msg) MessageBox(NULL, msg, L"Message", MB_MODALERROR)

#define CELLSIZE 150

wchar_t* otherWindowClassname;
const wchar_t szTitle[] = L"OS5";
HINSTANCE hInst;
bool isSecondPlayer = false;
std::array<HWND, 9> buttonHandles;

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
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			buttonHandles[i * 3 + j] = CreateWindow(
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

enum Player { PlayerX = 1, PlayerO = -1, PlayerEmpty = 0};
enum class GameState { WinnerX, WinnerO, Tie, Continue };
using Grid = std::array<Player, 9>;

struct Message {
	int id;
	GameState gameState;
};

#define Empty std::optional<Player>{}

Grid grid;

bool isWinner(Grid grid, size_t row, size_t col) {
	auto player = grid[row * 3 + col];
	// check row
	if (
		grid[row * 3    ] +
		grid[row * 3 + 1] +
		grid[row * 3 + 2] == player * 3
	) {
		return true;
	}
	// check column
	if (
		grid[        col] +
		grid[1 * 3 + col] +
		grid[2 * 3 + col] == player * 3
	) {
		return true;
	}

	// check main diag
	if (
		row == col &&
		grid[0] +
		grid[4] +
		grid[8] == player * 3
	) {
		return true;
	}
	// check anti diag
	if (
		row + col == 2 &&
		grid[2] +
		grid[4] +
		grid[6] == player * 3
	) {
		return true;
	}
	return false;
}

bool isFull(Grid grid) {
	return std::all_of(grid.begin(), grid.end(), [](int i) { return i; });
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hdc;
	switch (message) {
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_COPYDATA: {
			PCOPYDATASTRUCT s = (PCOPYDATASTRUCT)lParam;
			Message message = *reinterpret_cast<Message*>(s->lpData);
			grid[message.id] = isSecondPlayer ? PlayerX : PlayerO;

			// set button text
			hdc = BeginPaint(hWnd, &ps);
			SetDlgItemText(hWnd, message.id, isSecondPlayer ? L"X" : L"O");
			EndPaint(hWnd, &ps);
			
			if (message.gameState != GameState::Continue) {
				PostQuitMessage(0);
				break;
			}

			// enable empty buttons
			for (int i = 0; i < 9; i++) {
				if (!grid[i]) {
					EnableWindow(GetDlgItem(hWnd, i), true);
				}
			}
			break;
		}
		case WM_COMMAND: {
			HWND hRecieverWnd = FindWindow(otherWindowClassname, NULL);
			if (hRecieverWnd) {
				int id = LOWORD(wParam);
				grid[id] = isSecondPlayer ? PlayerO : PlayerX;

				// set button text
				hdc = BeginPaint(hWnd, &ps);
				!SetDlgItemText(hWnd, LOWORD(wParam), isSecondPlayer ? L"O" : L"X");
				EndPaint(hWnd, &ps);

				// disable own buttons
				for (int i = 0; i < 9; i++) {
					EnableWindow(GetDlgItem(hWnd, i), false);
				}

				Message message {
					.id = id,
					.gameState = isWinner(grid, id / 3, id % 3)
						? isSecondPlayer
							? GameState::WinnerO
							: GameState::WinnerX
						: isFull(grid)
							? GameState::Tie
							: GameState::Continue
				};
				// ask other window to enable it's buttons
				COPYDATASTRUCT cd {
					.dwData = 0,
					.cbData = sizeof(message),
					.lpData = PVOID(&message),
				};
				switch (message.gameState) {
					case GameState::WinnerO:
						ErrorBox(L"O's won!");
						PostQuitMessage(0);
						break;
					case GameState::WinnerX:
						ErrorBox(L"X's won!");
						PostQuitMessage(0);
						break;
					case GameState::Tie:
						ErrorBox(L"Tie!");
						PostQuitMessage(0);
						break;
					default:
						break;
				}
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