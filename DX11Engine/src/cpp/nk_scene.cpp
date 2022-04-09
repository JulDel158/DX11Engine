#include "../hpp/nk_scene.hpp"

#include "../hpp/file_reader.hpp"

namespace dxe {
	nk_scene::nk_scene(std::shared_ptr<DirectX::AudioEngine> audioEngine) : audioEngine(audioEngine) {
		// camera initialization
		camera = std::make_unique<View_t>();
		camera->position = glm::vec3(0.f, 0.f, 0.f);
		camera->updateView();

		//========================== UI Text ==========================
		allocateTextwraps(3);
		// health
		textui[0].color = glm::vec4(1.f, 0.f, 1.f, 1.f);
		textui[0].layer = 0.f;
		textui[0].origin = glm::vec2(0.f, 0.f);
		textui[0].position = glm::vec2(0.f, 0.f);
		textui[0].rotation = 0.f;
		textui[0].scale = glm::vec2(1.f, 1.f);
		textui[0].text = std::wstring(L"Health: 0");
		// temp crosshair
		textui[1].color = glm::vec4(0.f, 1.f, 1.f, 1.f);
		textui[1].layer = 0.f;
		textui[1].origin = glm::vec2(0.f, 0.f);
		textui[1].position = glm::vec2(610, 300);
		textui[1].rotation = 0.f;
		textui[1].scale = glm::vec2(2.f, 2.f);
		textui[1].text = std::wstring(L"+");

		// game over
		textui[1].color = glm::vec4(1.f, 0.f, 0.f, 1.f);
		textui[1].layer = 0.f;
		textui[1].origin = glm::vec2(0.f, 0.f);
		textui[1].position = glm::vec2(610, 300);
		textui[1].rotation = 0.f;
		textui[1].scale = glm::vec2(5.f, 5.f);
		textui[1].text = std::wstring(L"Game Over");
		//=============================================================

		//====================== Terrain Loading ======================
		terrain = new Terrain;
		terrain->object.model.loadObject("assets/models/debug_terrain.obj", false);
		tools::file_reader::loadBVH("assets/models/debug_terrain.bvh", *terrain);
		terrain->object.isActive = true;
		terrain->object.resourceId = 7;
		terrain->object.transform[0][0] = terrain->object.transform[1][1] = terrain->object.transform[2][2] = 10.f; // scale
		terrain->object.transform[3][1] = -10.f;
		terrain->resizeBVH(terrain->object.transform); // resizing bvh and model
		terrain->object.transform = glm::mat4{ 1.f };
		//=============================================================

		//========================== skybox ===========================
		skybox = new GameObject;
		skybox->model.loadObject("assets/models/CUBE.obj", true);
		skybox->resourceId = 2;
		//=============================================================

		// TODO: CHANGE HOW WE SEND LIGHT DATA INTO RENDERER TO SUPPORT MULTIPLE LIGHTS


	}

	nk_scene::~nk_scene(){

	}

	// main game loop
	void nk_scene::update(const float dt){

	}

	void nk_scene::inputUpdate(const float dt){

	}

} // namespace dxe