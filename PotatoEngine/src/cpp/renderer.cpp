#include "../hpp/renderer.hpp"

#include "../hpp/debug_lines.hpp"

namespace dxe {

	renderer::renderer(HWND windowHandle, Frame_cb& fcb, Window_cb& wcb) : implementation(windowHandle), frameCbuffer{ fcb }, windowCbuffer{ wcb }, objDesc{ 0 }, skyboxDesc{ 0 } {
		dCube.dMakeCube(1.f);

		objDesc.inputLayout = INPUT_LAYOUT::OBJECT;
		objDesc.vertexBuffer = VERTEX_BUFFER::OBJ_40000;
		objDesc.indexBuffer = INDEX_BUFFER::OBJ_40000;
		objDesc.constantBuffer = CONSTANT_BUFFER::OBJECT_CB;
		objDesc.vertexShader = VERTEX_SHADER::OBJECT;
		objDesc.pixelShader = PIXEL_SHADER::OBJ_TEXTURE;
		objDesc.samplerState = SAMPLER_STATE::DEFAULT;

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

	void renderer::draw(GameObject* objects, GameObject* skyBoxes, const uint32_t size) {
		implementation.setRenderTargetView();

		implementation.bindFrameBuffer(frameCbuffer, true);
		
		implementation.drawSkybox(skyBoxes);

		implementation.drawGameObjects(objects, size);

		implementation.drawDebugLines();

#ifdef _DEBUG
		implementation.drawObject(dCube);
#endif

		implementation.present(0);
	}

	void renderer::bindWindowBuffer() {
		implementation.bindWindowBuffer(windowCbuffer);
	}

} // dxe