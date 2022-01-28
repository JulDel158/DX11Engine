#include "../hpp/renderer.hpp"

#include "../hpp/debug_lines.hpp"

namespace dxe {

	renderer::renderer(HWND windowHandle, View_t& viewP) : implementation(windowHandle), viewProj{ viewP } { }

	renderer::~renderer() { }

	void renderer::update() {
		debug_lines::addGrid();
		debug_lines::addDebugCube(glm::vec3{ 0.f }, 1.f, glm::vec4{1.f, 1.f, 1.f, 1.f});

		debug_lines::addDebugCube(glm::vec3{ 5.f, 0.f, 0.f }, 0.5f, glm::vec4{1.f, 0.f, 0.f, 1.f });

		debug_lines::addDebugCube(glm::vec3{ 0.f, 5.f, 0.f }, 0.5f, glm::vec4{ 0.f, 1.f, 0.f, 1.f });

		debug_lines::addDebugCube(glm::vec3{ 0.f, 0.f, 5.f }, 0.5f, glm::vec4{ 0.f, 0.f, 1.f, 1.f });
	}

	void renderer::draw() {
		implementation.bindFrameBuffer(viewProj);

		implementation.setRenderTargetView();

		implementation.drawDebugLines();

		implementation.present(1);
	}

} // dxe