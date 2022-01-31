#include "../hpp/renderer.hpp"

#include "../hpp/debug_lines.hpp"

namespace dxe {

	renderer::renderer(HWND windowHandle, Frame_cb& fcb, Window_cb& wcb) : implementation(windowHandle), frameCbuffer{ fcb }, windowCbuffer{ wcb } {
		dCube.dMakeCube(1.f);
	}

	renderer::~renderer() { }

	void renderer::update() {
		debug_lines::addGrid();
#ifdef _DEBUG

		debug_lines::addDebugCube(glm::vec3{ 0.f }, 1.f, glm::vec4{1.f, 1.f, 1.f, 1.f});

		debug_lines::addDebugCube(glm::vec3{ 5.f, 0.f, 0.f }, 0.5f, glm::vec4{1.f, 0.f, 0.f, 1.f });

		debug_lines::addDebugCube(glm::vec3{ 0.f, 5.f, 0.f }, 0.5f, glm::vec4{ 0.f, 1.f, 0.f, 1.f });

		debug_lines::addDebugCube(glm::vec3{ 0.f, 0.f, 5.f }, 0.5f, glm::vec4{ 0.f, 0.f, 1.f, 1.f });

#endif // DEBUG

	}

	void renderer::draw(Objectdata& obj) {
		implementation.setRenderTargetView();

		implementation.bindFrameBuffer(frameCbuffer, true);
		
		implementation.drawObject(obj);

		implementation.drawDebugLines();

		implementation.present(0);
	}

	void renderer::bindWindowBuffer() {
		implementation.bindWindowBuffer(windowCbuffer);
	}

} // dxe