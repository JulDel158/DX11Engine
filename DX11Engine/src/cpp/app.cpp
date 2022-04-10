#include "../hpp/app.hpp"

#include "../hpp/dx_window.hpp"
#include "../hpp/debug_lines.hpp"
#include "../hpp/input.hpp"
#include "../hpp/game_scene.hpp"
#include "../hpp/nk_scene.hpp"

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
		dxRenderer(dxWindow.mainHWND, windowBuffer) { 
		float aspect = static_cast<float>(dxWindow.Width) / static_cast<float>(dxWindow.Height);

		// creating camera
		windowBuffer.setPerspectiveProjection(glm::pi<float>() / 4.f, aspect, 0.01f, 1000.f);
		dxRenderer.bindWindowBuffer();

		// audio engine
		DirectX::AUDIO_ENGINE_FLAGS eflags = DirectX::AudioEngine_Default;

#ifdef _DEBUG
		eflags = eflags | DirectX::AudioEngine_Debug;
#endif // _DEBUG

		audioEngine = std::make_shared<DirectX::AudioEngine>(eflags);

		input::SetWindowHandle(dxWindow.mainHWND);
		//input::ToggleCursor(false);

		gameScene = new nk_scene(audioEngine);
		//gameScene = new GameScene(audioEngine);
	}

	app::~app() {
		input::ToggleCursor(true);
		delete gameScene;
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
			else { // do updates and render down here
				timer.Signal();
				input::Update(); // updating global input
				if (!audioEngine->Update()) {
					if (audioEngine->IsCriticalError()) { // No audio device is active
						// x-x
					}
				}
				const float dt = static_cast<float>(timer.Delta());
				gameScene->update(dt);

#ifdef _DEBUG
				/*std::cout << "---------------------TIME DATA----------------------\n";
				std::cout << "Timer total: " << timer.TotalTime() << "\n";
				std::cout << "Timer total exact: " << timer.TotalTimeExact() << "\n";
				std::cout << "Timer delta: " << timer.Delta() << "\n";
				std::cout << "Timer smooth delta: " << timer.SmoothDelta() << "\n";
				std::cout << "Timer FPS: " << timer.SamplesPerSecond() << "\n";
				std::cout << "------------------------------------------------------------\n\n\n";*/

				// RAINBOW DEBUG LINES!!!!!!
				// debug_lines::rainbowUpdate(dt);
#endif // _DEBUG

				dxRenderer.draw(gameScene, dt);
			}
		}

		return msg;
	}

} // namespace dxe