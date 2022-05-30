#include "../hpp/nk_scene.hpp"

#include "../hpp/debug_lines.hpp"
#include "../hpp/file_reader.hpp"
#include "../hpp/input.hpp"

#include "../hpp/mesh_data.hpp"

namespace dxe {
	nk_scene::nk_scene(std::shared_ptr<DirectX::AudioEngine> audioEngine) : audioEngine(audioEngine),
	map(game_map(10, 10, 3, 5, glm::vec2(100.0f, 100.f))){
		std::srand(static_cast<unsigned int>(std::time(0)));
		//game_map map = game_map(20, 20, 3, 15, glm::vec2(1.f, 1.f));
		map.printMapData();
		map.clearSmallestFloor();
		map.generateNextFloor();
		map.printMapData();
		map.clearFloor(3);
		map.generateNextFloor();
		map.printMapData();
		
		
		int debug = 0;

		// camera initialization
		camera = new View_t();
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
		terrain->generateWalkPlane();
		//terrain->object.model.loadObject("assets/models/debug_terrain.obj", false);
		//tools::file_reader::loadBVH("assets/models/debug_terrain.bvh", *terrain);
		////terrain->loadTerrain("assets/models/quad.obj", false, false);
		//terrain->object.isActive = true;
		//terrain->object.resourceId = 7;
		//terrain->object.transform[0][0] = terrain->object.transform[1][1] = terrain->object.transform[2][2] = 5.f; // scale
		//terrain->object.transform[3][1] = -20.f;
		//terrain->resizeBVH(terrain->object.transform); // resizing bvh and model
		////terrain->expandBVHRootSize(glm::vec3(2.f, 10.f, 2.f));
		//terrain->object.transform = glm::mat4{ 1.f };
		//=============================================================

		//========================== skybox ===========================
		skybox = new GameObject;
		skybox->model.loadObject("assets/models/CUBE.obj", true);
		skybox->resourceId = 2;
		//=============================================================

		// TODO: CHANGE HOW WE SEND LIGHT DATA INTO RENDERER TO SUPPORT MULTIPLE LIGHTS
		//========================= lights ============================
		scb = new Scene_cb;
		scb->dirLight.color = { 0.1f, 0.1f, 0.f, 0.0f };
		scb->dirLight.direction = { 1.f, -1.f, -1.f };
		scb->ambient = { 1.f, 1.f, 1.f, 0.2f };

		player = Player(100, 0, 15.f, 60.f, 10.f, 5.f, 5.5f);
		player.resizeCollider(glm::vec3(2.5f, 10.0f, 2.5f));

		allocateGameObjs(map.getRequiredMeshCount());
		map.generateRoomMeshes(gameObjects, 0, gObjSize);
		/*MakePlane(gameObjects[0].model, 10.f, 10.f);
		gameObjects[0].isActive = true;
		gameObjects[0].resourceId = 1;
		gameObjects[0].setPosition(glm::vec3(0.f, 15.f, 0.f));*/
	}

	nk_scene::~nk_scene(){
	}

	// main game loop
	void nk_scene::update(const float dt){
		inputUpdate(dt);

		static const auto groundClamp = [&](glm::mat3 triangle)->bool {
			glm::vec3 intersection{ 0.f };
			//glm::vec3 tempPos = player.pos;
			if (RayToTriangleCollision(player.getPosition(), glm::vec3{ 0.f, 1.f, 0.f }, triangle, intersection)) {
				if (player.getPosition().y >= player.currentJump) {
					player.setPosition(intersection);
					player.yVelocity = 0.f;
				}
				player.isGrounded = true;
//#ifdef _DEBUG
				//std::cout << "Player Collided with terrai aabb\n";
//#endif // _DEBUG

				//player.yAcceleration = 0.f;
				return true;
			}
			player.isGrounded = false;
			return false;
		};

		// applying gravity force to player
		//player.setPosition(player.getPosition() - glm::vec3{ 0.f, gravity * dt, 0.f });
		//player.addGravityConstantForce(-gravity, dt);
		player.update(dt);
		terrain->traverseTree(player.getBox(), groundClamp);
		camera->setPosition(player.getPosition() + glm::vec3(0.f, player.currentOffset, 0.f));

		skybox->transform[3] = glm::vec4(camera->position, 1.f); // positioning skybox at camera location

#ifdef _DEBUG
		debug_lines::addAabb(player.getBox(), { 1.f, 1.f, 0.f, 1.f });
#endif // _DEBUG
	}

	void nk_scene::inputUpdate(const float dt){
		glm::vec4 translate{ 0.f };
		const auto mousePos = input::GetMouseDelta();
		const bool canRun = player.isRunning() && !player.isCrouched();

		if (input::KeyDown(VK_LSHIFT)) { player.setRun(true); } else { player.setRun(false); }
		if (input::KeyPressed(VK_LCONTROL)) { player.toggleCrouch(); }
		if (input::KeyDown(VK_SPACE)) { player.jump(); }

		// movement
		if (input::KeyDown('W')) { translate.z += canRun ? player.runSpeed * dt : player.speed * dt; } // FOWARD
		if (input::KeyDown('S')) { translate.z -= canRun ? player.runSpeed * dt : player.speed * dt; } // BACKWARDS
		if (input::KeyDown('D')) { translate.x += canRun ? player.runSpeed * dt : player.speed * dt; } // RIGHT
		if (input::KeyDown('A')) { translate.x -= canRun ? player.runSpeed * dt : player.speed * dt; } // LEFT
		if (input::KeyDown('Q')) { translate.y += canRun ? player.runSpeed * dt : player.speed * dt; } // UP
		if (input::KeyDown('E')) { translate.y -= canRun ? player.runSpeed * dt : player.speed * dt; } // DOWN

		// camera rotation
		player.FPControls(translate, static_cast<float>(mousePos.y), static_cast<float>(mousePos.x));
		camera->rotation = player.getRotation();
		/*camera->position.x = player.getPosition().x;
		camera->position.y = player.getPosition().y;
		camera->position.z = player.getPosition().z;*/
		camera->updateView(glm::vec4(0.f));
	}

} // namespace dxe