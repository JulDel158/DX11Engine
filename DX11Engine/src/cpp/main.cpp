
#pragma comment(lib, "DirectXTK.lib")

#include "../hpp/app.hpp"

// std
#include <iostream>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include "../hpp/input.hpp"

// lib
#include <imgui.h>
#include <imgui_impl_win32.h>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR	   lpCmdLine,
	_In_ int	   nCmdShow
) {
	MSG msg;

	dxe::app app(hInstance, WndProc, nCmdShow, msg);
	RAWINPUTDEVICE rid;
	rid.usUsagePage = 0x01; // default page usage
	rid.usUsage = 0x02; // mouse usage
	rid.dwFlags = 0;
	rid.hwndTarget = nullptr; // no specific window target
	
	// remember assert is your enemy
	BOOL br = RegisterRawInputDevices(&rid, 1, sizeof(rid));
	assert(br == TRUE &&
		"failed to register mouse raw input device!\n");
	
	msg = app.run();

	return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);
	
	switch (message)
	{
	case WM_INPUT: {// raw input
		UINT size{ 0 };
		// first we get the size of the data
		if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam),
			RID_INPUT,
			nullptr,
			&size,
			sizeof(RAWINPUTHEADER)) == -1) {
			// error
			break;
		}
		char* buffer = new char[size];
		// reading input data
		if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam),
			RID_INPUT,
			buffer,
			&size,
			sizeof(RAWINPUTHEADER)) != size) {
			// potential error
			delete[] buffer;
			break;
		}

		auto& ri = reinterpret_cast<const RAWINPUT&>(*buffer);
		if (ri.header.dwType == RIM_TYPEMOUSE) {
			dxe::input::SetMouseDelta(ri.data.mouse.lLastX, ri.data.mouse.lLastY);
		}
		
		delete[] buffer;
		break;
	}
	case WM_DESTROY: {
		PostQuitMessage(0);
		break;
	}
	default:
	{
		std::cout << "default\n";
		//if (message >= WM_MOUSEFIRST && message <= WM_MOUSELAST) { // mouse message
		//	dxe::input::Listen(message, lParam, hWnd);
		//}
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	}

	return 0;
}