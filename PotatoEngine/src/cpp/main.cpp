
#include "../hpp/dx_window.hpp"

// std
#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

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
	MSG msg;

	pe::dx_window wnd(hInstance, WndProc, nCmdShow, msg);

	// TODO: move this function to app implementation
	msg = begin_main_loop();

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