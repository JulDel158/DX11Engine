#include "../hpp/game_scene.hpp"

#include <Windows.h>

namespace dxe {
	GameScene::GameScene() {
		camera.position = glm::vec3(0.f, 3.f, -15.f);
		camera.getView();

		GameObject tempGameObj;
		GameObject plane;

		// GameObject skyBox;

		tempGameObj.model.loadObject("assets/models/piramid.obj", false);
		// tempGameObj.model.dMakePlane();
		tempGameObj.transform[0][0] = tempGameObj.transform[1][1] = tempGameObj.transform[2][2] = 10.f;
		tempGameObj.resourceId = 1;

		plane.model.dMakePlane();
		plane.transform[0][0] = plane.transform[1][1] = plane.transform[2][2] = 40.f;
		plane.transform[3] = { -20.f, 0.f, -20.f, 1.f };
		plane.resourceId = 0;

		gameObjects.push_back(std::move(tempGameObj));
		gameObjects.push_back(std::move(plane));

		skyBox.model.loadObject("assets/models/CUBE.obj");
		skyBox.resourceId = 2;
	}

	GameScene::~GameScene() {}

	void GameScene::update(const float dt) {
		input.Update();

		glm::vec4 translate{ 0.f };
		if (input.KeyDown('W')) { translate.z += camera.translationSpeed * dt; } // FOWARD
		if (input.KeyDown('S')) { translate.z -= camera.translationSpeed * dt; } // BACKWARDS
		if (input.KeyDown('D')) { translate.x += camera.translationSpeed * dt; } // RIGHT
		if (input.KeyDown('A')) { translate.x -= camera.translationSpeed * dt; } // LEFT
		if (input.KeyDown('Q')) { translate.y += camera.translationSpeed * dt; } // UP
		if (input.KeyDown('E')) { translate.y -= camera.translationSpeed * dt; } // DOWN

		if (input.KeyDown(VK_LEFT)) {  camera.rotation.y -= camera.rotationSpeed * dt; } // look left
		if (input.KeyDown(VK_RIGHT)) { camera.rotation.y += camera.rotationSpeed * dt; } // look right
		if (input.KeyDown(VK_UP)) {    camera.rotation.x -= camera.rotationSpeed * dt; } // look up
		if (input.KeyDown(VK_DOWN)) {  camera.rotation.x += camera.rotationSpeed * dt; } // look down

		camera.getView(translate);

		// copying the position of the camera into the skybox
		skyBox.transform[3] = camera.view[3];
	}

	const GameObject* GameScene::GetSceneObjects() const { return gameObjects.data(); }

	const GameObject* GameScene::GetSkyBox() const { return &skyBox; }

	const uint32_t GameScene::GetObjectTotal() const { return static_cast<uint32_t>(gameObjects.size()); }

	const View_t& GameScene::GetView() const { return camera; }

} // namespace dxe