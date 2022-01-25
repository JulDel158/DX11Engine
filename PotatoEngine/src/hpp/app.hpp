#pragma once

#include "dx_window.hpp"

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
	};

} // namespace dxe