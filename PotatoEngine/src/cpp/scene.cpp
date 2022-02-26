#include "../hpp/scene.hpp"

namespace dxe {

	// safety measure to prevent any leaks
	// each derived scene should allocate and release its memory
	scene::~scene() { 

		if (gameObjects) {
			delete[] gameObjects;
			gameObjects = nullptr;
		}

		if (skyBox) {
			delete skyBox;
			skyBox = nullptr;
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
	}

} // namespace dxe

