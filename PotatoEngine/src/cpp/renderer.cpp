#include "../hpp/renderer.hpp"

#include "../hpp/debug_lines.hpp"

namespace dxe {

	renderer::renderer(HWND windowHandle, Window_cb& wcb) : 
		implementation(windowHandle), 
		windowCbuffer{ wcb } {

		/*objDesc.inputLayout = INPUT_LAYOUT::OBJECT;
		objDesc.vertexBuffer = VERTEX_BUFFER::OBJ_40000;
		objDesc.indexBuffer = INDEX_BUFFER::OBJ_40000;
		objDesc.constantBuffer = CONSTANT_BUFFER::OBJECT_CB;
		objDesc.vertexShader = VERTEX_SHADER::OBJECT;
		objDesc.pixelShader = PIXEL_SHADER::OBJ_TEXTURE;
		objDesc.samplerState = SAMPLER_STATE::DEFAULT;*/
	}

	renderer::~renderer() { }

	void renderer::update(const View_t& camera) {
#ifdef _DEBUG
		debug_lines::addGrid();

		debug_lines::addDebugCube(glm::vec3{ 0.f }, 1.f, glm::vec4{1.f, 1.f, 1.f, 1.f});

		debug_lines::addDebugCube(glm::vec3{ 5.f, 0.f, 0.f }, 0.5f, glm::vec4{1.f, 0.f, 0.f, 1.f });

		debug_lines::addDebugCube(glm::vec3{ 0.f, 5.f, 0.f }, 0.5f, glm::vec4{ 0.f, 1.f, 0.f, 1.f });

		debug_lines::addDebugCube(glm::vec3{ 0.f, 0.f, 5.f }, 0.5f, glm::vec4{ 0.f, 0.f, 1.f, 1.f });

#endif // DEBUG
		frameCbuffer.view = camera.view;
	}

	void renderer::draw(GameScene const& scene) {
		update(scene.GetView());

		implementation.setRenderTargetView();

		implementation.bindFrameBuffer(frameCbuffer, scene.GetSceneBuffer(), scene.GetView().invertView);

		if (scene.GetSkyBox()) { // Must be drawn first
			implementation.drawSkybox(scene.GetSkyBox());
		}

		/*if (scene.GetObjectTotal() > 0) {
			implementation.drawGameObjects(scene.GetSceneObjects(), scene.GetObjectTotal());
		}*/

#ifdef _DEBUG
		implementation.drawDebugLines();
#endif
		// particles should be drawn last, but before the UI
		// implementation.drawParticle();

		implementation.drawCsParticles();

		if (scene.GetTextUITotal() > 0) { // Must be drawn last
			implementation.drawText(scene.GetTextUI(), scene.GetTextUITotal());
		}

		implementation.present(0);
	}

	void renderer::bindWindowBuffer() {
		implementation.bindWindowBuffer(windowCbuffer);
	}

} // dxe