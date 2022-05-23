#include "../hpp/game_scene.hpp"

#include "../hpp/file_reader.hpp"
#include "../hpp/debug_lines.hpp"
#include "../hpp/game_map.hpp"

// std
#include <Windows.h>
#include <iostream>
#include <time.h>
#include <limits>
#include <string>

// lib
#include <glm/gtc/random.hpp>

namespace dxe {
	// REMINDED, WE ARE NOT CURRENTLY WORKING INSIDE THIS FILE
	GameScene::GameScene(std::shared_ptr<DirectX::AudioEngine> _audioEngine) {
		

		// camera initialization
		camera = new View_t();
		camera->position = glm::vec3(0.f, 0.f, 0.f);
		camera->updateView();

		/*textui = new Textwrap[2];
		textUiCount = 2;*/
		allocateTextwraps(2);
		/*textui.clear();
		textui.resize(2);*/
		
		// score ui
		textui[0].color = glm::vec4(1.f, 0.f, 1.f, 1.f);
		textui[0].layer = 0.f;
		textui[0].origin = glm::vec2(0.f, 0.f);
		textui[0].position = glm::vec2(0.f, 0.f);
		textui[0].rotation = 0.f;
		textui[0].scale = glm::vec2(1.f, 1.f);
		textui[0].active = true;
		textui[0].text = std::wstring(L"MOUSE DELTA: " + std::to_wstring(score));

		// temp crosshair ui
		textui[1].color = glm::vec4(1.f, 0.f, 0.f, 1.f);
		textui[1].layer = 0.f;
		textui[1].origin = glm::vec2(0.f, 0.f);
		textui[1].position = glm::vec2(610, 300);
		textui[1].rotation = 0.f;
		textui[1].scale = glm::vec2(2.5f, 2.5f);
		textui[1].active = true;
		textui[1].text = std::wstring(L"+");


		//gameObjects.clear();
		//gameObjects.resize(MAX_ENEMIES + 1);
		/*gameObjects = new GameObject[MAX_ENEMIES + 1];
		gobjSize = MAX_ENEMIES + 1;*/
		allocateGameObjs(MAX_ENEMIES);
		
		// IMPORTANT, SINCE WE ARE ALLOCATING THE ENEMY UNITS FIRST THIS IS FINE, IF WE INSERT OTHER GAME OBJECTS PRIOR, YOU WILL HAVE TO PROPERLY MATCH THE GAME OBJECT FROM THE ENEMY ARRAY
		for (int i = 0; i < MAX_ENEMIES; ++i) {
			enemies[i].object = &gameObjects[i];

			enemies[i].object->model.loadObject("assets/models/cube1.obj", false);//dMakeCube(1.f); //loadObject("assets/models/piramid.obj", false);
			enemies[i].object->resourceId = 0;

			const float scale = glm::linearRand(1.0f, 10.f);
			enemies[i].collider.radius =  enemies[i].object->transform[0][0] = enemies[i].object->transform[1][1] = enemies[i].object->transform[2][2] = scale;
			enemies[i].collider.radius += 0.5f;
			glm::vec4 randompos = glm::linearRand(glm::vec4{ -80.f, 60.f, -80.f, 1.f }, glm::vec4{ 80.f, 100.f, 80.f, 1.f });
			enemies[i].object->transform[3] = randompos;
			enemies[i].collider.pos = glm::vec3(randompos.x, randompos.y, randompos.z);
			enemies[i].object->isActive = true;
			enemies[i].velocity = -glm::normalize(randompos);
			enemies[i].speed = glm::linearRand(3.0f, 10.f);
		}
		
		// terrain plane game object
		terrain = new Terrain;
		//terrain->loadTerrain("assets/models/terrain_1.obj", false, false); // generates bvh tree
		terrain->object.model.loadObject("assets/models/debug_terrain.obj", false);
		terrain->object.isActive = true;
		terrain->object.resourceId = 7;

		// testing terrain stuff
		//tools::exportBVH("assets/models/terrain_1.bvh", *terrain);

		tools::file_reader::loadBVH("assets/models/debug_terrain.bvh", *terrain);
		
		terrain->object.transform[0][0] = terrain->object.transform[1][1] = terrain->object.transform[2][2] = 10.f; // scale
		terrain->object.transform[3][1] = -10.f;

		// testing bvh resizing
		terrain->resizeBVH(terrain->object.transform);

		terrain->object.transform = glm::mat4{ 1.f };

		//plane.model.dMakePlane();
		//terrain->transform[0][0] = terrain->transform[1][1] = terrain->transform[2][2] = 40.f;
		//terrain->transform[3] = { -20.f, 0.f, -20.f, 1.f };
		//terrain->resourceId = 0;
		
		// gameObjects[gobjSize - 1] = plane;

		// skybox
		skybox = new GameObject;
		skybox->model.loadObject("assets/models/CUBE.obj", true);
		skybox->resourceId = 2;

		// lights
		scb = new Scene_cb;
		scb->dirLight.color = { 0.1f, 0.1f, 0.f, 0.0f };
		scb->dirLight.direction = { 1.f, -1.f, -1.f };
		scb->ambient = { 1.f, 1.f, 1.f, 0.2f };

		scb->pointLight[0].color = { 1.f, 0.40f, 0.f, 1.f };
		scb->pointLight[0].pos = { 0.f, 10.f, 0.f };
		scb->pointLight[0].radius = 100.f;

		scb->spotLight.color = { 0.f, 0.f, 0.f, 1.f };

		// THE HIGHER THESE VALUES ARE, THE MORE DIM THE LIGHT WILL BE
		scb->spotLight.outerRatio = 0.2f;
		scb->spotLight.innerRatio = 0.05f;
		scb->spotLight.falloff    = 0.f;

		scb->spotLight.range = 200.f;
		scb->spotLight.focus = 100.f;

		// sound stuff
		audioEngine = _audioEngine;

		soundData = std::make_unique<DirectX::SoundEffect>(audioEngine.get(), L"assets\\audio\\8bit_gunloop_explosion.wav");
		songData = std::make_unique<DirectX::SoundEffect>(audioEngine.get(), L"assets\\audio\\mc_theme.wav");
		songData2 = std::make_unique<DirectX::SoundEffect>(audioEngine.get(), L"assets\\audio\\sd_theme_v1.wav");

		soundInstance = songData->CreateInstance();
		soundInstance2 = songData2->CreateInstance();

		// particle emitter
		allocateEmitters(1);
		particleEmitters->pos = glm::vec3(0.f, 0.f, 10.f);
		particleEmitters->flyweigth.minTime = 2.f;
		particleEmitters->flyweigth.maxTime = 5.5f;
		particleEmitters->flyweigth.scaleStart = 0.5f;
		particleEmitters->flyweigth.scaleRate = 1.5f;
		particleEmitters->flyweigth.velMinVals = glm::vec3(-20.f, 1.f, -20.f);
		particleEmitters->flyweigth.velMaxVals = glm::vec3(20.f, 10.f, 20.f);
		particleEmitters->particles.resize(50);

		// player stuff
		player_collider.center = camera->position;
		player_collider.extent = { 2.5f, 10.0f, 2.5f };

		
	}

	GameScene::~GameScene() {}

	void GameScene::update(const float dt) {
		inputUpdate(dt);

		// updating the text ui
		

		// updating enemies
		for (int i = 0; i < MAX_ENEMIES; ++i) {

			if (!enemies[i].object) { continue; }
			enemies[i].object->isActive = false;
			//if (!enemies[i].object->isActive) {
			//	if (enemies[i].respawnTime <= 0.f) {
			//		// we must respawn the enemy
			//		glm::vec4 randompos = glm::linearRand(glm::vec4{ -80.f, 60.f, -80.f, 1.f }, glm::vec4{ 80.f, 100.f, 80.f, 1.f });
			//		enemies[i].object->transform[3] = randompos;
			//		enemies[i].collider.pos = glm::vec3(randompos.x, randompos.y, randompos.z);
			//		enemies[i].object->isActive = true;
			//		enemies[i].velocity = -glm::normalize(randompos);
			//		enemies[i].speed = glm::linearRand(3.0f, 10.f);
			//	}
			//	enemies[i].respawnTime -= dt;
			//	continue;
			//}
			//glm::vec4 pos = enemies[i].object->transform[3];

			//// updating entity's movement
			//pos += glm::vec4(enemies[i].velocity.x, enemies[i].velocity.y, enemies[i].velocity.z, 0.f) * enemies[i].speed * dt;

			//enemies[i].object->transform[3] = pos;
			//enemies[i].collider.pos = glm::vec3{ pos.x, pos.y, pos.z };
		}

		// copying the position of the camera into the skybox


		auto f = [&](glm::mat3 triangle)->bool {
			glm::vec3 intersection{ 0.f };
			glm::vec3 tempPos = camera->position;
			const float groundOffset = 8.f;
			tempPos.y -= groundOffset;
			if (RayToTriangleCollision(tempPos, glm::vec3{ 0.f, 1.f, 0.f }, triangle, intersection)) {

				intersection.x = camera->position.x;
				intersection.z = camera->position.z;
				intersection.y += groundOffset;
				camera->setPosition(intersection);
				return true;
			}
			return false;
		};

		const float grav = 60.f;
		camera->position.y -= grav * dt;
		camera->setPosition(camera->position);
		terrain->traverseTree(player_collider, f);

		player_collider.center = camera->position;
		skybox->transform[3] = glm::vec4(camera->position, 1.f);

		// temp light debug stuff for spot light
		{
			glm::vec3 campos = camera->view[3];
			glm::vec3 camforw = camera->view[2];
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

			scb->spotLight.direction = camforw;
			scb->spotLight.pos = campos;
		}

		// small particle update for debug purposes
		if (particleEmitters->updated) {
			particleEmitters->pos.z = -10.f;
			for (auto& p : particleEmitters->particles) {
				if (p.lifeSpan <= 0.0f) {
					p.lifeSpan = glm::linearRand(particleEmitters->flyweigth.minTime, particleEmitters->flyweigth.maxTime);
					p.pos = particleEmitters->pos;
					p.scale = particleEmitters->flyweigth.scaleStart;
					p.velocity = glm::linearRand(particleEmitters->flyweigth.velMinVals, particleEmitters->flyweigth.velMaxVals);
				} else {
					p.pos += p.velocity * dt;
					p.scale += particleEmitters->flyweigth.scaleRate * dt;
					p.lifeSpan -= dt;
				}
			}
		}
		else { // gonna update in the gpu, switching positions to differentiate update functions
			particleEmitters->pos.z = 10.f;
		}

#ifdef _DEBUG
		debug_lines::addAabb(player_collider, { 1.f, 1.f, 0.f, 1.f });
#endif // _DEBUG
	}

	void GameScene::inputUpdate(const float dt) {
		
		glm::vec4 translate{ 0.f };
		if (input::KeyDown('W')) { translate.z += camera->translationSpeed * dt; } // FOWARD
		if (input::KeyDown('S')) { translate.z -= camera->translationSpeed * dt; } // BACKWARDS
		if (input::KeyDown('D')) { translate.x += camera->translationSpeed * dt; } // RIGHT
		if (input::KeyDown('A')) { translate.x -= camera->translationSpeed * dt; } // LEFT
		if (input::KeyDown('Q')) { translate.y += camera->translationSpeed * dt * 5.f; } // UP
		if (input::KeyDown('E')) { translate.y -= camera->translationSpeed * dt; } // DOWN
		
		
		//input::SetCursonPosition(100, 100); //keep cursor in a constant place for certain first person mouse stuff
		const auto mousePos = input::GetMouseDelta();
		glm::vec2 mouseDx = {  static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)};
		camera->rotation.x += mouseDx.y * camera->rotationSpeed;
		camera->rotation.y += mouseDx.x * camera->rotationSpeed;
	
		//debug mouse delta from raw input
		//printf("Raw Input mouse delta: x:%f y%f\n", mouseDx.x, mouseDx.y);
		textui[0].text = std::wstring(L"MOUSE DELTA: x:" + std::to_wstring(mousePos.x) + L" y:" + std::to_wstring(mousePos.y));

		//if (input::KeyDown(VK_LEFT)) { camera->rotation.y += -1.f * camera->rotationSpeed * dt; } // look left
		//if (input::KeyDown(VK_RIGHT)) { camera->rotation.y += 1.f * camera->rotationSpeed * dt; } // look right
		//if (input::KeyDown(VK_UP)) { camera->rotation.x -= camera->rotationSpeed * dt; } // look up
		//if (input::KeyDown(VK_DOWN)) { camera->rotation.x += camera->rotationSpeed * dt; } // look down

		camera->rotation.x = glm::clamp(camera->rotation.x, -89.9f, 89.9f); // camera can only look up

		camera->updateView(translate);

		if (input::KeyPressed(VK_LBUTTON)) { // here we check collision with active objects
			enemy* finalTarget = nullptr;
			float pTime = std::numeric_limits<float>::infinity();//{ -1.f };
			float castTime{ 0.f };
			for (int i = 0; i < MAX_ENEMIES; ++i) {
				if (!enemies[i].object || !enemies[i].object->isActive) { 
					continue; 
				}
				
				// TODO: for bullets we may want to make sure we only hit one targe by checking which target was hit first and leaving any other collisions intact
				if (RayToSphereCollision(camera->position, camera->getFoward(), enemies[i].collider, castTime)) {
					if (castTime < pTime) {
						pTime = castTime;
						finalTarget = &enemies[i];
					}
				}
			} // end for

			if (finalTarget) {
				finalTarget->object->isActive = false;
				finalTarget->respawnTime = glm::linearRand(3.f, 5.f);
				score += 10;
			}

			//PLAY SOUND EFFECT
			soundData->Play();
		} 

		// debug tests
		if (input::KeyPressed('8')) { soundInstance2->Stop(); }
		if (input::KeyPressed('7')) { soundInstance2->Play(true); }
		if (input::KeyPressed('5')) { soundInstance->Play(false); }

		if (input::KeyPressed('1')) { skybox->resourceId = 0; } // WARNING: THIS WILL THROW AN ERROR FROM THE RENDERER AS THIS SUB RESOURCE IS NOT A CUBEMAP BUT A TEXTURE2D
		if (input::KeyPressed('2')) { skybox->resourceId = 2; }

		if (input::KeyPressed('0')) {
			particleEmitters->updated = !particleEmitters->updated;
		}
	}

} // namespace dxe