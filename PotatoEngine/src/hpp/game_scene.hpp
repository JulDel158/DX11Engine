#pragma once

#include "object_data.hpp"
#include "input.hpp"

// std
#include <vector>

namespace dxe {

	class GameScene {

	public:
		GameScene();
		~GameScene();

		void update(const float dt);

		const GameObject* GetSceneObjects() const;
		const GameObject* GetSkyBox() const;
		const uint32_t GetObjectTotal() const;
		const View_t& GetView() const;
		const Scene_cb& GetSceneBuffer() const;

	private:

		View_t camera;

		std::vector<GameObject> gameObjects;

		GameObject skyBox;

		input input;

		Scene_cb scb;
	};


} // namespace dxe