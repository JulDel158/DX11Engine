#include "../hpp/app.hpp"

#include "../hpp/dx_window.hpp"

#define GLM_FORCE_RADIANS
#include "glm/gtc/constants.hpp"

#include <iostream>

namespace dxe {
	app::app(HINSTANCE& hInstance, WNDPROC winProc, int nCmdShow, MSG& msg) : dxWindow(hInstance, winProc, nCmdShow, msg), dxRenderer(dxWindow.mainHWND, camera) { 
		float aspect = static_cast<float>(dxWindow.Width) / static_cast<float>(dxWindow.Height);

		// creating camera
		camera.setPerspectiveProjection(glm::pi<float>() / 4.f, aspect, 0.01f, 100.f);
		camera.FPSViewRH({ 0.f, 3.f, -15.f }, glm::radians(0.f), glm::radians(0.f));
		// tempView.view = glm::inverse(tempView.view);
		// tempView.dPrintViewMat();
		/*camera.view = glm::mat4{ 1.f };
		camera.LookAtTarget({ 0.f, 5.f, -15.f }, { 0.f, 5.f, 0.f }, { 0.f, 1.f, 0.f });
		camera.view = glm::inverse(camera.view);
		camera.dPrintViewMat();*/

		
	}

	app::~app() {}

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

#ifndef NDEBUG
				if (input.KeyPressed((int)'A')) { std::cout << "A key was pressed!\n"; }
				if (input.KeyDown((int)'A')) { std::cout << "A key is down!\n"; }
				if (input.KeyUp((int)'A')) { std::cout << "A key was released!\n"; }
#endif // !NDEBUG

				dxRenderer.update();
				dxRenderer.draw();
			}
		}

		return msg;
	}

} // namespace dxe