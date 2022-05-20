#pragma once

#include "dx_window.hpp"
#include "game_scene.hpp"
#include "input.hpp"
#include "object_data.hpp"
#include "renderer.hpp"
#include "XTime.hpp"

// should probably be done in another place
//#pragma comment(lib,"dsound.lib")
//#pragma comment(lib,"dxguid.lib")

// std
#include <vector>

// lib
#include <imgui.h>

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

		renderer dxRenderer;

		Window_cb windowBuffer;

		XTime timer;

		std::unique_ptr<DirectX::AudioEngine> audioEngine;

		GameScene scene1;
	};

} // namespace dxe