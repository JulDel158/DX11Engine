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

		if (camera) {
			delete camera;
		}

		if (textureResourcesData) {
			for (uint64_t i = 0; i < textureRDCount; ++i) {
				if (textureResourcesData[i])
					textureResourcesData[i]->Release();
			}

			delete[] textureResourcesData;
		}
	}

	void scene::allocateGameObjs(uint64_t size) {
		// BEWARE: constant use of this function can lead to fragmentation
		// It is recomended to allocate all your memory at once
		if (!gameObjects) {
			gameObjects = new GameObject[size];
			gObjSize = size;
		} else {
			GameObject* ptr = new GameObject[size + gObjSize];
			memcpy(ptr, gameObjects, gObjSize * sizeof(GameObject));
			gObjSize += size;
			delete[] gameObjects;
			gameObjects = ptr;
			ptr = nullptr;
		}
	}

	void scene::allocateEmitters(uint64_t size) {
		if (!particleEmitters) {
			particleEmitters = new Emitter[size];
			emitterCount = size;
		} else {
			Emitter* ptr = new Emitter[size + emitterCount];
			memcpy(ptr, particleEmitters, emitterCount * sizeof(particleEmitters));
			emitterCount += size;
			delete[] particleEmitters;
			particleEmitters = ptr;
			ptr = nullptr;
		}
	}

	void scene::allocateTextwraps(uint64_t size) {
		if (!textui) {
			textui = new Textwrap[size];
			textUiCount = size;
		} else {
			Textwrap* ptr = new Textwrap[size + textUiCount];
			memcpy(ptr, textui, textUiCount * sizeof(Textwrap));
			textUiCount += size;
			delete[] textui;
			textui = ptr;
			ptr = nullptr;
		}
	}

} // namespace dxe

