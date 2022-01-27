#include "../hpp/renderer.hpp"

#include "../hpp/debug_lines.hpp"

namespace dxe {

	renderer::renderer(HWND windowHandle) : implementation(windowHandle) { }

	renderer::~renderer() { }

	void renderer::update() {
		debug_lines::addGrid();
	}

	void renderer::draw() {
		implementation.setRenderTargetView();

		implementation.drawDebugLines();

		implementation.present(1);
	}

} // dxe