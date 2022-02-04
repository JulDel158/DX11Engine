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
	app::app(HINSTANCE& hInstance, WNDPROC winProc, int nCmdShow, MSG& msg) : 
		dxWindow(hInstance, winProc, nCmdShow, msg), 
		dxRenderer(dxWindow.mainHWND, windowBuffer, scb) { 
		float aspect = static_cast<float>(dxWindow.Width) / static_cast<float>(dxWindow.Height);

		// creating camera
		windowBuffer.setPerspectiveProjection(glm::pi<float>() / 4.f, aspect, 0.01f, 1000.f);
		dxRenderer.bindWindowBuffer();

		/*camera.position = glm::vec3(0.f, 3.f, -15.f);
		camera.getView();*/

		// updating constant buffers
		// frameBuffer.view = camera.view;

		


		scb.dirLight.color = { 0.4f, 0.4f, 0.4f, 1.0f };
		scb.dirLight.direction = { 0.f, -1.f, 0.f };
		scb.ambient = { 1.f, 1.f, 1.f, 0.02f };

		scb.pointLight.color = { 0.f, 1.f, 1.f, 1.f };
		scb.pointLight.pos = { 0.f, 1.f, 0.f };
		scb.pointLight.radius = 5.f;

		scb.spotLight.color = { 0.f, 0.f, 0.f, 1.f };

		scb.spotLight.outerRatio = 1.f;
		scb.spotLight.innerRatio = 1.f;
		scb.spotLight.falloff = 1.f;

		scb.spotLight.range = 0.1f;
		scb.spotLight.focus = 10.f;
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
				const float dt = static_cast<float>(timer.Delta());
				scene1.update(dt);

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

				//glm::vec4 translate{ 0.f };
				//if (input.KeyDown('W')) { translate.z += camera.translationSpeed * dt; } // FOWARD
				//if (input.KeyDown('S')) { translate.z -= camera.translationSpeed * dt; } // BACKWARDS
				//if (input.KeyDown('D')) { translate.x += camera.translationSpeed * dt; } // RIGHT
				//if (input.KeyDown('A')) { translate.x -= camera.translationSpeed * dt; } // LEFT
				//if (input.KeyDown('Q')) { translate.y += camera.translationSpeed * dt; } // UP
				//if (input.KeyDown('E')) { translate.y -= camera.translationSpeed * dt; } // DOWN

				//if (input.KeyDown(VK_LEFT)) {  camera.rotation.y -= camera.rotationSpeed * dt; } // look left
				//if (input.KeyDown(VK_RIGHT)) { camera.rotation.y += camera.rotationSpeed * dt; } // look right
				//if (input.KeyDown(VK_UP)) {    camera.rotation.x -= camera.rotationSpeed * dt; } // look up
				//if (input.KeyDown(VK_DOWN)) {  camera.rotation.x += camera.rotationSpeed * dt; } // look down

				//camera.getView(translate);
				 //frameBuffer.view = camera.view;

				//skyBoxes[0].transform[3] = camera.view[3];
				/*skyBoxes[0].transform[3][1] = camera.view[3][1];
				skyBoxes[0].transform[3][2] = camera.view[3][2];*/

				{ // temp light stuff
					//glm::vec3 campos = camera.view[3];
					//glm::vec3 camforw = camera.view[2];
					//// att1 outer cone
					//if (input.KeyDown('Z')) { scb.spotLight.outerRatio += 0.05f * dt; }
					//if (input.KeyDown('X')) { scb.spotLight.outerRatio -= 0.05f * dt; scb.spotLight.outerRatio = glm::clamp(scb.spotLight.outerRatio, 0.f, 1000.f); }
					//// att2 inner cone
					//if (input.KeyDown('V')) { scb.spotLight.innerRatio += 0.05f * dt; }
					//if (input.KeyDown('B')) { scb.spotLight.innerRatio -= 0.05f * dt; scb.spotLight.innerRatio = glm::clamp(scb.spotLight.innerRatio, 0.f, 1000.f); }
					//// att3 falloff
					//if (input.KeyDown('K')) { scb.spotLight.falloff += 0.05f * dt; }
					//if (input.KeyDown('L')) { scb.spotLight.falloff -= 0.05f * dt; scb.spotLight.falloff = glm::clamp(scb.spotLight.falloff, 0.f, 1000.f); }
					//// focus
					//if (input.KeyDown('O')) { scb.spotLight.focus+= 20.0f * dt; }
					//if (input.KeyDown('P')) { scb.spotLight.focus-= 20.0f * dt; scb.spotLight.focus = glm::clamp(scb.spotLight.focus, 0.f, 1000.f); }

					//scb.spotLight.direction = camforw;
					//scb.spotLight.pos = campos;
				}

				//dxRenderer.update();

				// RAINBOW DEBUG LINES!!!!!!
				debug_lines::rainbowUpdate(dt);

				dxRenderer.draw(scene1);
			}
		}

		return msg;
	}

} // namespace dxe