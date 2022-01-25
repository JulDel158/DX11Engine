#include "../hpp/dx_window.hpp"

// std
#include <cassert>
#include <conio.h>
#include <iostream>
#include <stdexcept>

namespace dxe {
	dx_window::dx_window(HINSTANCE& hInst, WNDPROC winProc, int nCmdShow, MSG& ms) : 
		windowClass{ L"Potato Engine" }, 
		title{ L"First Game" }, 
		msg{ ms },
		hInstance{ hInst }, 
		mainHWND{ NULL },
		wcex{ 0 }
		{
		
        initWindowClass(hInst, winProc);
		createWindow();

		createConsole();

		// parameters to ShowWindow explained:
		// hWnd: the value returned from CreateWindow
		// nCmdShow: the fourth parameter from WinMain
		if (mainHWND) {
		ShowWindow(mainHWND, nCmdShow);
		UpdateWindow(mainHWND);
		}
    }

    dx_window::~dx_window() {
		destroyConsole(static_cast<int>(msg.wParam));
    }

	void dx_window::initWindowClass(HINSTANCE& hInst, WNDPROC winProc) {
		
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = winProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInst;
		wcex.hIcon = LoadIcon(hInst, IDI_APPLICATION);
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = NULL;
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = windowClass.c_str();
		wcex.hIconSm = LoadIcon(hInst, IDI_APPLICATION);


		if (!RegisterClassEx(&wcex)) {
			throw std::runtime_error("Call to RegisterClassEx in initWindowClass failed!");
		}
	}

	void dx_window::createWindow() {

		mainHWND = CreateWindow(
			windowClass.c_str(),			// name of application
			title.c_str(),					// name displayed on title bar
			WS_POPUPWINDOW | WS_CAPTION,	// the type of window to create
			CW_USEDEFAULT,					// initial postion (x)
			CW_USEDEFAULT,					// initial position (y)
			1280,							// initial window size
			720,							// initial window size
			NULL,							// parent of this window
			NULL,							// menu bar
			hInstance,						// first parameter from WinMain
			NULL							// unused in this app
		);

		if (!mainHWND) {
			throw std::runtime_error("Call to CreateWindow failed!");
		}
	}

    void dx_window::createConsole() {
	#ifndef NDEBUG
		AllocConsole();
		FILE* new_std_in_out;
		freopen_s(&new_std_in_out, "CONOUT$", "w", stdout);
		freopen_s(&new_std_in_out, "CONIN$", "r", stdin);
		std::cout << "Debug Console Opened.\n";
	#endif
    }


    void dx_window::destroyConsole(int exitCode) {
	#ifndef NDEBUG
		if (exitCode) {
			std::cout << "Exit Code: " << exitCode << "\n";
			std::cout << "Press any key to continue...";
			_getch(); // read a character from the console
		}

		FreeConsole();
	#endif
    }

} // namespace dxe