#pragma once

#include "object_data.hpp"
#include "input.hpp"

// lib
#include <glm/glm.hpp>

// std
#include <vector>

namespace dxe {

	struct sphere {
		glm::vec3 pos{ 0.f };
		float radius{ 0.f };
	};

	struct enemy {
		GameObject object;
		sphere collider;
		bool enabled{ false };
	};

	const bool RayToSphereCollision(const glm::vec3 pos, glm::vec3 direction, const sphere target);

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

		void inputUpdate(const float dt);

		View_t camera;

		std::vector<GameObject> gameObjects;

		enemy testSubject;

		GameObject skyBox;

		input input;

		Scene_cb scb;

		static constexpr uint8_t MAX_ENEMIES = 10;
	};


} // namespace dxe