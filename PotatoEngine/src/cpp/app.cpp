#include "../hpp/app.hpp"

#include "../hpp/dx_window.hpp"
#include "../hpp/debug_lines.hpp"

#define GLM_FORCE_RADIANS
#include <glm/gtc/constants.hpp>

// std
#ifdef _DEBUG
#include <iostream>
#endif
#include <utility>

namespace dxe {
	app::app(HINSTANCE& hInstance, WNDPROC winProc, int nCmdShow, MSG& msg) : dxWindow(hInstance, winProc, nCmdShow, msg), dxRenderer(dxWindow.mainHWND, frameBuffer, windowBuffer) { 
		float aspect = static_cast<float>(dxWindow.Width) / static_cast<float>(dxWindow.Height);

		// creating camera
		windowBuffer.setPerspectiveProjection(glm::pi<float>() / 4.f, aspect, 0.01f, 1000.f);
		camera.position = glm::vec3(0.f, 3.f, -15.f);
		camera.getView();

		// updating constant buffers
		frameBuffer.view = camera.view;

		dxRenderer.bindWindowBuffer();

		GameObject tempGameObj;

		GameObject skyBox;

		tempGameObj.model.loadObject("assets/models/piramid.obj", false);
		tempGameObj.transform[0][0] = tempGameObj.transform[1][1] = tempGameObj.transform[2][2] = 10.f;
		tempGameObj.resourceId = 0;

		skyBox.model.loadObject("assets/models/CUBE.obj");
		skyBox.resourceId = 2;

		gameObjects.push_back(std::move(tempGameObj));

		skyBoxes.push_back(std::move(skyBox));
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

#ifdef _DEBUG
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
#endif // _DEBUG

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
				skyBoxes[0].transform[3][0] = camera.view[3][0];
				skyBoxes[0].transform[3][1] = camera.view[3][1];
				skyBoxes[0].transform[3][2] = camera.view[3][2];

				dxRenderer.update();

				// RAINBOW DEBUG LINES!!!!!!
				debug_lines::rainbowUpdate(dt);

				dxRenderer.draw(gameObjects.data(), &skyBoxes[0], static_cast<uint32_t>(gameObjects.size()));
			}
		}

		return msg;
	}

} // namespace dxe