#pragma once

#include "object_data.hpp"

namespace dxe {

	class scene {
	public:

		scene(const scene&) = delete;
		scene& operator=(const scene&) = delete;

		virtual ~scene();

		virtual void update(const float dt) = 0;

		inline const GameObject* getSkyBox() const { return skyBox; }

		inline const GameObject* getTerrain() const { return terrain; }

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

	};






} // namespace dxe