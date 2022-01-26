#include "../hpp/app.hpp"

#include "../hpp/dx_window.hpp"

namespace dxe {
	app::app(HINSTANCE& hInstance, WNDPROC winProc, int nCmdShow, MSG& msg) : dxWindow(hInstance, winProc, nCmdShow, msg), dxRenderer(dxWindow.mainHWND) {

	}

	app::~app() {

	}

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
				dxRenderer.update();
				dxRenderer.draw();
				// here you will do your update and render function for your engine
			}
		}

		return msg;
	}

} // namespace dxe