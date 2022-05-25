#include "../hpp/renderer.hpp"

#include "../hpp/debug_lines.hpp"

#include <imgui.h>
#include <imgui_impl_dx11.h>

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

	//void renderer::draw(GameScene& scene, const float dt) {
//		update(scene.GetView());
//
//		implementation.setRenderTargetView();
//
//		implementation.bindFrameBuffer(frameCbuffer, scene.GetSceneBuffer(), scene.GetView().invertView);
//
//		if (scene.GetSkyBox()) { // Must be drawn first
//			implementation.drawSkybox(scene.GetSkyBox());
//		}
//
//		/*if (scene.GetObjectTotal() > 0) {
//			implementation.drawGameObjects(scene.GetSceneObjects(), scene.GetObjectTotal());
//		}*/
//
//#ifdef _DEBUG
//		implementation.drawDebugLines();
//#endif
//		// particles should be drawn last, but before the UI
//		if (scene.GetParticles()) {
//			if (scene.GetParticles()->updated) {
//				implementation.drawParticle(scene.GetParticles()); // will not update particles
//			} else {
//				implementation.updateAndDrawParticles(scene.GetParticles(), dt); // will update particles in the gpu
//			}
//		}
//
//		//implementation.drawCsParticles(dt);
//
//		if (scene.GetTextUITotal() > 0) { // Must be drawn last
//			implementation.drawText(scene.GetTextUI(), scene.GetTextUITotal());
//		}
//
//		implementation.present(0);
	//}

	void renderer::draw(scene* const scene, const float dt) {
		update(*scene->getView()); // updating view buffer

		implementation.setRenderTargetView();

		implementation.bindFrameBuffer(frameCbuffer, *scene->getSceneBuffer(), scene->getView()->invertView);

		if (scene->getSkyBox()) {
			implementation.drawSkybox(scene->getSkyBox());
		}

		if (scene->getTerrain()) {
			implementation.drawGameObjects(&scene->getTerrain()->object, 1);

		#ifdef  _DEBUG
			for (const auto& n : scene->getTerrain()->tree) {
				debug_lines::addAabb(n.aabb(), {0.f, 1.f, 1.f, 1.f});
			}
			debug_lines::addAabb(scene->getTerrain()->tree[0].aabb(), { 0.f, 1.f, 1.f, 1.f });
		#endif //  _DEBUG

		}

		if (scene->getObjCount() > 0) {
			implementation.drawGameObjects(scene->getSceneObjects(), scene->getObjCount());
		}

	#ifdef _DEBUG
		implementation.drawDebugLines();
	#endif

		if (scene->getEmitters()) {
			if (scene->getEmitters()->updated) {
				implementation.drawParticle(scene->getEmitters());
			} else {
				implementation.updateAndDrawParticles(scene->getEmitters(), dt);
			}
		}


		if (scene->GetTextUITotal() > 0) { // Must be drawn last
			implementation.drawText(scene->GetTextUI(), scene->GetTextUITotal());
		}

		// ImGui Draw calls
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		implementation.present(0);
	}

	void renderer::bindWindowBuffer() {
		implementation.bindWindowBuffer(windowCbuffer);
	}

} // dxe