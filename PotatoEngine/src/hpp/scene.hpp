#pragma once

#include "object_data.hpp"

// should probably be done in another place
#pragma comment(lib,"dsound.lib")
#pragma comment(lib,"dxguid.lib")

// lib
#define NOMINMAX
#include <dsound.h>
#include <Audio.h>

namespace dxe {

	class scene {
	public:

		scene(const scene&) = delete;
		scene& operator=(const scene&) = delete;

		virtual ~scene();

		virtual void update(const float dt) = 0;

		// getters
		inline const GameObject* getSceneObjects() const { return gameObjects; }

		inline const GameObject* getSceneObjAt(uint32_t inx) { if (inx < gObjCount) { return &gameObjects[inx]; } return nullptr; }

		inline const uint32_t getObjCount() const { return gObjCount; }

		inline const GameObject* getTerrain() const { return terrain; }

		inline const GameObject* getSkyBox() const { return skyBox; }

		inline const Scene_cb* getSceneBuffer() const { return scb; }

		inline const Emitter* getEmitters() const { return particleEmitters; }

		inline const Emitter* getEmitterAt(uint32_t inx) { if (inx < emitterCount) { return &particleEmitters[inx]; } return nullptr; }

		inline const uint32_t getEmitterCount() const { return emitterCount; }

	private:

		View_t camera;

		// all the game objects in this scene, to be dynamically allocated
		GameObject* gameObjects{ nullptr };

		uint32_t gObjCount{ 0 };

		GameObject* terrain{ nullptr };

		GameObject* skyBox{ nullptr };

		Scene_cb* scb{ nullptr };

		Emitter* particleEmitters{ nullptr };

		uint32_t emitterCount{ 0 };

		// may change
		std::unique_ptr<DirectX::SoundEffectInstance[]> soundInstances;

		uint32_t sInstancesCount{ 0 };

	};






} // namespace dxe