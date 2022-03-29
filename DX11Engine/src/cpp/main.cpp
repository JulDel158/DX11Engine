
#pragma comment(lib, "DirectXTK.lib")

#include "../hpp/app.hpp"

// std
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include "../hpp/input.hpp"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR	   lpCmdLine,
	_In_ int	   nCmdShow
) {
	MSG msg;

	dxe::app app(hInstance, WndProc, nCmdShow, msg);

	msg = app.run();

	return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
	{
		if (message >= WM_MOUSEFIRST && message <= WM_MOUSELAST) { // mouse message
			dxe::input::Listen(message, lParam, hWnd);
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	}

	return 0;
}