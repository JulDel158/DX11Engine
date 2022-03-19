#include "../hpp/scene.hpp"

namespace dxe {
	scene::scene(){}

	// safety measure to prevent any leaks
	// each derived scene should allocate and release its memory
	scene::~scene() { 

		if (gameObjects) {
			delete[] gameObjects;
			gameObjects = nullptr;
		}

		if (skybox) {
			delete skybox;
			skybox = nullptr;
		}

		if (terrain) {
			delete terrain;
			terrain = nullptr;
		}

		if (scb) {
			delete scb;
			scb = nullptr;
		}

		if (particleEmitters) {
			delete[] particleEmitters;
			particleEmitters = nullptr;
		}

		if (textui) {
			delete[] textui;
			textui = nullptr;
		}

		camera.release();
	}

	void scene::allocateGameObjs(uint32_t size) {
		gameObjects = new GameObject[size];
		gobjSize = size;
	}

	void scene::allocateEmitters(uint32_t size) {
		particleEmitters = new Emitter[size];
		emitterCount = size;
	}

	void scene::allocateTextwraps(uint32_t size) {
		textui = new Textwrap[size];
		textUiCount = size;
	}

} // namespace dxe

