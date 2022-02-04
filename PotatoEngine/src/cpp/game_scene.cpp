#include "../hpp/game_scene.hpp"

// std
#include <algorithm>
#include <Windows.h>
#include <iostream>

namespace dxe {
	GameScene::GameScene() {
		camera.position = glm::vec3(0.f, 1.f, 0.f);
		camera.updateView();

		gameObjects.reserve(MAX_ENEMIES + 2);

		//GameObject tempGameObj;
		

		// GameObject skyBox;

		//tempGameObj.model.loadObject("assets/models/piramid.obj", false);
		//// tempGameObj.model.dMakePlane();
		//tempGameObj.transform[0][0] = tempGameObj.transform[1][1] = tempGameObj.transform[2][2] = 10.f;
		//tempGameObj.resourceId = 1;

		plane.model.dMakePlane();
		plane.transform[0][0] = plane.transform[1][1] = plane.transform[2][2] = 40.f;
		plane.transform[3] = { -20.f, 0.f, -20.f, 1.f };
		plane.resourceId = 0;

		enemies[0].enabled = true;
		enemies[0].collider.radius = 1.2f;
		enemies[0].object.model.dMakeCube(1.f);
		enemies[0].object.resourceId = 3;
		glm::vec4 temppos = glm::vec4(0.f, 1.f, 10.f, 1.f);
		enemies[0].object.transform[3] = temppos;
		enemies[0].collider.pos = glm::vec3(temppos.x, temppos.y, temppos.z);

		

		// gameObjects.push_back(std::move(tempGameObj));
		gameObjects.push_back(plane);
		// need to keep track of the index
		gameObjects.push_back(enemies[0].object);
		enemies[0].objectIndex = gameObjects.size();

		skyBox.model.loadObject("assets/models/CUBE.obj");
		skyBox.resourceId = 2;

		// lights
		scb.dirLight.color = { 0.4f, 0.4f, 0.4f, 1.0f };
		scb.dirLight.direction = { 0.f, -1.f, 0.f };
		scb.ambient = { 1.f, 1.f, 1.f, 0.02f };

		scb.pointLight.color = { 1.f, 1.f, 0.f, 1.f };
		scb.pointLight.pos = { 4.f, 3.f, 0.f };
		scb.pointLight.radius = 10.f;

		scb.spotLight.color = { 0.f, 0.f, 0.f, 1.f };

		// THE HIGHER THESE VALUES ARE, THE MORE DIM THE LIGHT WILL BE
		scb.spotLight.outerRatio = 1.f;
		scb.spotLight.innerRatio = 1.f;
		scb.spotLight.falloff    = 1.f;

		scb.spotLight.range = 0.1f;
		scb.spotLight.focus = 10.f;
	}

	GameScene::~GameScene() {}

	void GameScene::update(const float dt) {
		inputUpdate(dt);

		// updating enemies
		for (int i = 0; i < MAX_ENEMIES; ++i) {

			// auto it = std::find(gameObjects.begin(), gameObjects.end(), enemies[i].object);

			if (enemies[i].enabled && enemies[i].objectIndex == -1) { // the enemy is active but game object is not in draw list
				gameObjects.push_back(enemies[i].object);
				enemies[i].objectIndex = gameObjects.size();

			} else if (!enemies[i].enabled && enemies[i].objectIndex >= 0) { // the enemy is dissabled and game object is in draw list
				gameObjects.erase(gameObjects.begin() + (enemies[i].objectIndex - 1));
				enemies[i].objectIndex = -1;
			}
		}

		// copying the position of the camera into the skybox
		skyBox.transform[3] = camera.view[3];

		// temp light debug stuff for spot light
		{
			//glm::vec3 campos = camera.view[3];
			//glm::vec3 camforw = camera.view[2];
			//// att1 outer cone
			//if (input.KeyDown('Z')) { scb.spotLight.outerRatio += 0.05f * dt; }
			//if (input.KeyDown('X')) { scb.spotLight.outerRatio -= 0.05f * dt; scb.spotLight.outerRatio = glm::clamp(scb.spotLight.outerRatio, 0.f, 1000.f); }
			//// att2 inner cone
			//if (input.KeyDown('V')) { scb.spotLight.innerRatio += 0.05f * dt; }
			//if (input.KeyDown('B')) { scb.spotLight.innerRatio -= 0.05f * dt; scb.spotLight.innerRatio = glm::clamp(scb.spotLight.innerRatio, 0.f, 1000.f); }
			//// att3 falloff
			//if (input.KeyDown('K')) { scb.spotLight.falloff += 0.05f * dt; }
			//if (input.KeyDown('L')) { scb.spotLight.falloff -= 0.05f * dt; scb.spotLight.falloff = glm::clamp(scb.spotLight.falloff, 0.f, 1000.f); }
			//// focus
			//if (input.KeyDown('O')) { scb.spotLight.focus+= 20.0f * dt; }
			//if (input.KeyDown('P')) { scb.spotLight.focus-= 20.0f * dt; scb.spotLight.focus = glm::clamp(scb.spotLight.focus, 0.f, 1000.f); }

			//scb.spotLight.direction = camforw;
			//scb.spotLight.pos = campos;
		}
	}

	void GameScene::inputUpdate(const float dt) {
		input.Update();
		// in our demo the player will be unable to move and will be fixated at one point on the screen
		// he will however be able to rotate the camera
		
		glm::vec4 translate{ 0.f };
#ifdef _DEBUG
		if (input.KeyDown('W')) { translate.z += camera.translationSpeed * dt; } // FOWARD
		if (input.KeyDown('S')) { translate.z -= camera.translationSpeed * dt; } // BACKWARDS
		if (input.KeyDown('D')) { translate.x += camera.translationSpeed * dt; } // RIGHT
		if (input.KeyDown('A')) { translate.x -= camera.translationSpeed * dt; } // LEFT
		if (input.KeyDown('Q')) { translate.y += camera.translationSpeed * dt; } // UP
		if (input.KeyDown('E')) { translate.y -= camera.translationSpeed * dt; } // DOWN
#endif // _DEBUG


		if (input.KeyDown(VK_LEFT)) { camera.rotation.y -= camera.rotationSpeed * dt; } // look left
		if (input.KeyDown(VK_RIGHT)) { camera.rotation.y += camera.rotationSpeed * dt; } // look right
		if (input.KeyDown(VK_UP)) { camera.rotation.x -= camera.rotationSpeed * dt; } // look up
		if (input.KeyDown(VK_DOWN)) { camera.rotation.x += camera.rotationSpeed * dt; } // look down
		camera.rotation.x = glm::clamp(camera.rotation.x, -90.f, 0.f); // camera can only look up

		camera.updateView(translate);

		if (input.KeyDown(VK_SPACE)) {
			// here we should check collision with all enemies
			for (int i = 0; i < MAX_ENEMIES; ++i) {
				if (RayToSphereCollision(camera.position, camera.getFoward(), enemies[i].collider)) {
					enemies[i].enabled = false;
				} else {
					enemies[i].enabled = true;
				}
			}
		} 
	}

	const GameObject* GameScene::GetSceneObjects() const { return gameObjects.data(); }

	const GameObject* GameScene::GetSkyBox() const { return &skyBox; }

	const uint32_t GameScene::GetObjectTotal() const { return static_cast<uint32_t>(gameObjects.size()); }

	const Scene_cb& GameScene::GetSceneBuffer() const { return scb; }

	const View_t& GameScene::GetView() const { return camera; }

} // namespace dxe