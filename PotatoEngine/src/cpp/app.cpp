#include "../hpp/app.hpp"

#include "../hpp/dx_window.hpp"

#define GLM_FORCE_RADIANS
#include "glm/gtc/constants.hpp"

#include <iostream>

namespace dxe {
	app::app(HINSTANCE& hInstance, WNDPROC winProc, int nCmdShow, MSG& msg) : dxWindow(hInstance, winProc, nCmdShow, msg), dxRenderer(dxWindow.mainHWND, frameBuffer, windowBuffer) { 
		float aspect = static_cast<float>(dxWindow.Width) / static_cast<float>(dxWindow.Height);

		// creating camera
		windowBuffer.setPerspectiveProjection(glm::pi<float>() / 4.f, aspect, 0.01f, 1000.f);
		camera.position = glm::vec3(0.f, 3.f, -15.f);
		camera.getView();
		// camera.UpdateView();
		// camera.FPSViewRH({ 0.f, 3.f, -15.f }, glm::radians(0.f), glm::radians(0.f));

		// updating constant buffers
		frameBuffer.view = camera.view;

		dxRenderer.bindWindowBuffer();

		temporaryObject.loadObject("assets/models/piramid.obj", false);
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
			else { // do updates and render down here
				timer.Signal();
				input.Update();

#ifndef NDEBUG
				if (input.KeyPressed((int)'A')) { std::cout << "A key was pressed!\n"; }
				if (input.KeyDown((int)'A')) { std::cout << "A key is down!\n"; }
				if (input.KeyUp((int)'A')) { std::cout << "A key was released!\n"; }
				std::cout << "---------------------TIME DATA----------------------\n";
				std::cout << "Timer total: " << timer.TotalTime() << "\n";
				std::cout << "Timer total exact: " << timer.TotalTimeExact() << "\n";
				std::cout << "Timer delta: " << timer.Delta() << "\n";
				std::cout << "Timer smooth delta: " << timer.SmoothDelta() << "\n";
				std::cout << "Timer FPS: " << timer.SamplesPerSecond() << "\n";
				std::cout << "------------------------------------------------------------\n\n\n";
#endif // !NDEBUG

				glm::vec4 translate{ 0.f };
				const float dt = static_cast<float>(timer.Delta());

				if (input.KeyDown('W')) { translate.z += camera.translationSpeed * dt; } // FOWARD
				if (input.KeyDown('S')) { translate.z -= camera.translationSpeed * dt; } // BACKWARDS
				if (input.KeyDown('D')) { translate.x += camera.translationSpeed * dt; } // RIGHT
				if (input.KeyDown('A')) { translate.x -= camera.translationSpeed * dt; } // LEFT
				if (input.KeyDown('Q')) { translate.y += camera.translationSpeed * dt; } // UP
				if (input.KeyDown('E')) { translate.y -= camera.translationSpeed * dt; } // DOWN

				if (input.KeyDown(VK_LEFT)) {  camera.rotation.y -= camera.rotationSpeed * dt; } // look left
				if (input.KeyDown(VK_RIGHT)) { camera.rotation.y += camera.rotationSpeed * dt; } // look right
				if (input.KeyDown(VK_UP)) {    camera.rotation.x -= camera.rotationSpeed * dt; } // look up
				if (input.KeyDown(VK_DOWN)) {  camera.rotation.x += camera.rotationSpeed * dt; } // look down

				camera.getView(translate);
				frameBuffer.view = camera.view;

				dxRenderer.update();
				dxRenderer.draw(temporaryObject);
			}
		}

		return msg;
	}

} // namespace dxe