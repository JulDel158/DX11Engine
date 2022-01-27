#include "../hpp/app.hpp"

#include "../hpp/dx_window.hpp"

#include <iostream>

namespace dxe {
	app::app(HINSTANCE& hInstance, WNDPROC winProc, int nCmdShow, MSG& msg) : dxWindow(hInstance, winProc, nCmdShow, msg), dxRenderer(dxWindow.mainHWND) { }

	app::~app() { }

	MSG app::run() {
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

			}
			else {
				input.Update();
				// DEBUG STUFF
				if (input.KeyPressed((int)'A')) { std::cout << "A key was pressed!\n"; }
				if (input.KeyDown((int)'B')) { std::cout << "B key is down!\n"; }
				if (input.KeyUp((int)'A')) { std::cout << "A key was released!\n"; }

				dxRenderer.update();
				dxRenderer.draw();
				// here you will do your update and render function for your engine
			}
		}

		return msg;
	}

} // namespace dxe