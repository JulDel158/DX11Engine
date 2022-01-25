#include <conio.h>
#include <iostream>
#include <tchar.h>
#include <windows.h>


// main window class name
static TCHAR szWindowClass[] = _T("Potato Engine");

// title bar name
static TCHAR szTitle[] = _T("First Game");

HINSTANCE hInst;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void create_console();
void destroy_console(int exit_code);
MSG begin_main_loop();

namespace {
	HWND main_hwnd = NULL;
}

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR	   lpCmdLine,
	_In_ int	   nCmdShow
) {
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);

	// TODO: Conside using throw
	if (!RegisterClassEx(&wcex)) {
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Potato Engine"),
			NULL);

		return 1;
	}

	// storing instance handle in our global variable
	hInst = hInstance;

	main_hwnd = CreateWindow(
		szWindowClass,	// name of application
		szTitle,		// name displayed on title bar
		WS_POPUPWINDOW | WS_CAPTION, // the type of window to create
		CW_USEDEFAULT,	// initial postion (x)
		CW_USEDEFAULT,	// initial position (y)
		1280,			// initial window size
		720,			// initial window size
		NULL,			// parent of this window
		NULL,			// menu bar
		hInstance,		// first parameter from WinMain
		NULL			// unused in this app
	);

	// TODO: Conside using throw
	if (!main_hwnd) {
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Potato Engine"),
			NULL);

		return 1;
	}

	create_console();

	// parameters to ShowWindow explained:
	// hWnd: the value returned from CreateWindow
	// nCmdShow: the fourth parameter from WinMain
	ShowWindow(main_hwnd, nCmdShow);
	UpdateWindow(main_hwnd);

	MSG msg = begin_main_loop();

	destroy_console(static_cast<int>(msg.wParam));

	return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}

void create_console() {
#ifndef NDEBUG
	AllocConsole();
	FILE* new_std_in_out;
	freopen_s(&new_std_in_out, "CONOUT$", "w", stdout);
	freopen_s(&new_std_in_out, "CONIN$", "r", stdin);
	std::cout << "Debug Console Opened.\n"; 
#endif
}

void destroy_console(int exit_code) {
#ifndef NDEBUG

	if (exit_code) {
		std::cout << "Exit Code: " << exit_code << "\n";
		std::cout << "Press any key to continue...";
		_getch(); // read a character from the console
	}

	FreeConsole();

#endif
}

MSG begin_main_loop() {
	MSG msg;
	// TODO: add timer class

	while (true) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {

			if (msg.message == WM_QUIT) {
				break;
			}

			// Translates message and sends them to WndProc
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		} else {
			// here you will do your update and render function for your engine
		}
	}

	return msg;
}