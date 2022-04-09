#pragma once

#include "object_data.hpp"

// lib
#define NOMINMAX
#include <dsound.h>
#include <Audio.h>

namespace dxe {

	class scene {
	public:

		scene(const scene&) = delete;
		scene& operator=(const scene&) = delete;
		scene();
		virtual ~scene();

		virtual void update(const float dt) = 0;

		// getters
		inline const View_t* getView() const { return camera.get(); }
		inline const GameObject* getSceneObjects() const { return gameObjects; }
		inline const GameObject* getSceneObjAt(uint64_t inx) { if (inx < gObjSize) { return &gameObjects[inx]; } return nullptr; }
		inline const uint64_t getObjCount() const { return gObjSize; }
		inline const Terrain* getTerrain() const { return terrain; }
		inline const GameObject* getSkyBox() const { return skybox; }
		inline const Scene_cb* getSceneBuffer() const { return scb; }
		inline Emitter* getEmitters() const { return particleEmitters; }
		inline Emitter* getEmitterAt(uint64_t inx) { if (inx < emitterCount) { return &particleEmitters[inx]; } return nullptr; }
		inline const uint64_t getEmitterCount() const { return emitterCount; }
		inline const Textwrap* GetTextUI() const { return textui; }
		inline const Textwrap* GetTextUIAt(uint64_t inx) const { if (inx < textUiCount) { return &textui[inx]; } return nullptr; }
		inline const uint64_t GetTextUITotal() const { return textUiCount; }

	protected:

		std::unique_ptr<View_t> camera;

		// all the game objects in this scene, to be dynamically allocated
		GameObject* gameObjects{ nullptr };

		uint64_t gObjSize{ 0 };

		Terrain* terrain{ nullptr };

		GameObject* skybox{ nullptr };

		Scene_cb* scb{ nullptr };

		Emitter* particleEmitters{ nullptr }; // TODO: CHANGE RENDERER PARTICLE FUNCTIONS TO PROCESS ALL EMITTERS IN A SCENE

		uint64_t emitterCount{ 0 };

		// may change
		std::unique_ptr<DirectX::SoundEffectInstance[]> soundInstances;

		uint64_t sInstancesCount{ 0 };

		Textwrap* textui{ nullptr };

		uint64_t textUiCount{ 0 };

	protected:
		void allocateGameObjs(uint64_t size);
		void allocateEmitters(uint64_t size);
		void allocateTextwraps(uint64_t size);
	};






} // namespace dxe