#pragma once

#include "dx_window.hpp"
#include "game_scene.hpp"
#include "input.hpp"
#include "object_data.hpp"
#include "renderer.hpp"
#include "XTime.hpp"

// std
#include <vector>

namespace dxe {

	class app {

	public:
		app(HINSTANCE& hInstance, WNDPROC winProc, int nCmdShow, MSG& msg);
		~app();

		app(const app&) = delete;
		app& operator=(const app&) = delete;
		app(const app&&) = delete;
		app& operator=(const app&&) = delete;

		MSG run();

	private:
		dx_window dxWindow;

		input input;

		renderer dxRenderer;

		// View_t camera;

		// Frame_cb frameBuffer;

		Window_cb windowBuffer;

		Scene_cb scb;

		XTime timer;

		// std::vector<GameObject> gameObjects;

		// std::vector<GameObject> skyBoxes;

		GameScene scene1;
	};

} // namespace dxe