#include "../hpp/game_scene.hpp"

#include "../hpp/file_reader.hpp"

// std
#include <Windows.h>
#include <iostream>
#include <time.h>
#include <limits>
#include <string>

// lib
#include <glm/gtc/random.hpp>

namespace dxe {

	GameScene::GameScene() {
		std::srand(static_cast<unsigned int>(std::time(0)));

		// camera initialization
		camera = std::make_unique<View_t>();
		camera->position = glm::vec3(0.f, 1.f, 0.f);
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
		textui[0].text = std::wstring(L"SCORE: " + std::to_wstring(score));

		// temp crosshair ui
		textui[1].color = glm::vec4(1.f, 0.f, 0.f, 1.f);
		textui[1].layer = 0.f;
		textui[1].origin = glm::vec2(0.f, 0.f);
		textui[1].position = glm::vec2(610, 300);
		textui[1].rotation = 0.f;
		textui[1].scale = glm::vec2(2.5f, 2.5f);
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
		// terrain->loadTerrain("assets/models/terrain_1.obj", false, true); // generates bvh tree
		terrain->object.model.loadObject("assets/models/terrain_1.obj", false);
		terrain->object.isActive = true;
		terrain->object.resourceId = 7;

		// testing terrain stuff
		// tools::exportBVH("assets/models/terrain_1.bvh", *terrain);

		tools::file_reader::loadBVH("assets/models/terrain_1.bvh", *terrain);
		
		terrain->object.transform[0][0] = terrain->object.transform[1][1] = terrain->object.transform[2][2] = 20.f; // scale
		terrain->object.transform[3][1] = -10.f;

		// testing bvh resizing
		terrain->resizeBVH(terrain->object.transform);

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

		scb->pointLight.color = { 1.f, 0.40f, 0.f, 1.f };
		scb->pointLight.pos = { 0.f, 0.f, 12.f };
		scb->pointLight.radius = 40.f;

		scb->spotLight.color = { 0.f, 0.f, 0.f, 1.f };

		// THE HIGHER THESE VALUES ARE, THE MORE DIM THE LIGHT WILL BE
		scb->spotLight.outerRatio = 0.2f;
		scb->spotLight.innerRatio = 0.05f;
		scb->spotLight.falloff    = 0.f;

		scb->spotLight.range = 200.f;
		scb->spotLight.focus = 100.f;

		// sound stuff
		DirectX::AUDIO_ENGINE_FLAGS eflags = DirectX::AudioEngine_Default;

	#ifdef _DEBUG
		eflags = eflags | DirectX::AudioEngine_Debug;
	#endif // _DEBUG

		audioEngine = std::make_unique<DirectX::AudioEngine>(eflags);

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
	}

	GameScene::~GameScene() {}

	void GameScene::update(const float dt) {
		inputUpdate(dt);

		if (!audioEngine->Update())
		{
			// No audio device is active
			if (audioEngine->IsCriticalError())
			{
				// x-x
			}
		}

		// updating the text ui
		textui[0].text = std::wstring(L"SCORE: " + std::to_wstring(score));

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
		skybox->transform[3] = camera->view[3];

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
	}

	void GameScene::inputUpdate(const float dt) {
		// in our demo the player will be unable to move and will be fixated at one point on the screen
		// he will however be able to rotate the camera
		
		glm::vec4 translate{ 0.f };
//#ifdef _DEBUG
		if (input::KeyDown('W')) { translate.z += camera->translationSpeed * dt; } // FOWARD
		if (input::KeyDown('S')) { translate.z -= camera->translationSpeed * dt; } // BACKWARDS
		if (input::KeyDown('D')) { translate.x += camera->translationSpeed * dt; } // RIGHT
		if (input::KeyDown('A')) { translate.x -= camera->translationSpeed * dt; } // LEFT
		if (input::KeyDown('Q')) { translate.y += camera->translationSpeed * dt; } // UP
		if (input::KeyDown('E')) { translate.y -= camera->translationSpeed * dt; } // DOWN
//#endif // _DEBUG
		static bool released0 = true;
		if (input::KeyPressed('0') && released0) { 
			particleEmitters->updated = !particleEmitters->updated;
			released0 = false; 
		} else {
			released0 = true;
		}
		if (input::KeyPressed('8')) { soundInstance2->Stop(); }
		if (input::KeyPressed('7')) { soundInstance2->Play(true); }
		if (input::KeyPressed('5')) { soundInstance->Play(false); }
		if (input::KeyPressed('1')) { skybox->resourceId = 0; } // WARNING: THIS WILL THROW AN ERROR FROM THE RENDERER AS THIS SUB RESOURCE IS NOT A CUBEMAP BUT A TEXTURE2D
		if (input::KeyPressed('2')) { skybox->resourceId = 2; }
		if (input::KeyDown(VK_LEFT)) { camera->rotation.y -= camera->rotationSpeed * dt; } // look left
		if (input::KeyDown(VK_RIGHT)) { camera->rotation.y += camera->rotationSpeed * dt; } // look right
		if (input::KeyDown(VK_UP)) { camera->rotation.x -= camera->rotationSpeed * dt; } // look up
		if (input::KeyDown(VK_DOWN)) { camera->rotation.x += camera->rotationSpeed * dt; } // look down

//#ifndef _DEBUG
//		camera.rotation.x = glm::clamp(camera.rotation.x, -90.f, 0.f); // camera can only look up
//#endif // !_DEBUG


		camera->updateView(translate);

		if (input::KeyPressed(VK_SPACE)) { // here we check collision with active objects
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
	}

	/*const GameObject* GameScene::GetSceneObjects() const { return gameObjects.data(); }

	const GameObject* GameScene::GetSkyBox() const { return &skyBox; }

	const uint32_t GameScene::GetObjectTotal() const { return static_cast<uint32_t>(gameObjects.size()); }

	const Scene_cb& GameScene::GetSceneBuffer() const { return scb; }

	const View_t& GameScene::GetView() const { return camera; }

	const Textwrap* GameScene::GetTextUI() const { return textui.data(); }

	const uint32_t GameScene::GetTextUITotal() const { return static_cast<uint32_t>(textui.size()); }

	Emitter* GameScene::GetParticles() { return &pEmitter; }*/

} // namespace dxe