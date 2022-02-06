#pragma once

#include "object_data.hpp"
#include "input.hpp"
#include "collision.hpp"

// lib
#include <glm/glm.hpp>

// std
#include <vector>

namespace dxe {

	struct enemy {
		sphere collider;
		GameObject* object{ nullptr };
		glm::vec3 velocity{ 0.f };
		float speed{ 0.f };
		float respawnTime{ 0.f };


		enemy() = default;
		~enemy() { object = nullptr; }
	};

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
		const Textwrap* GetTextUI() const;
		const uint32_t GetTextUITotal() const;

	private:

		void inputUpdate(const float dt);

		View_t camera;

		std::vector<GameObject> gameObjects;

		GameObject plane;

		GameObject skyBox;

		input input;

		Scene_cb scb;

		static constexpr uint8_t MAX_ENEMIES = 10;

		enemy enemies[MAX_ENEMIES];

		std::vector<Textwrap> textui;

		uint32_t score{ 0 };
	};


} // namespace dxe