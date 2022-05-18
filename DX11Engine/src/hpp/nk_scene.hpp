#pragma once

#include "scene.hpp"
#include "player.hpp"

namespace dxe {

	enum class DIFFICULTY {
		DEBUG = -1, NORMAL = 0, DIFFICULT = 1
	};

	enum class GAME_STATE { // TODO: CREATE A FINITE STATE MACHINE TO HANDLE GAME STATES
		DEBUG = -1, MAIN_MENU = 0, OPTIONS = 1, CREDITS = 2, GAME = 3, PAUSE = 4, LOADING = 5, EXIT = 6
	};

	class nk_scene : public scene {
	public:
		nk_scene(std::shared_ptr<DirectX::AudioEngine> audioEngine);
		~nk_scene();

		void update(const float dt);

	private:
		void inputUpdate(const float dt);

		std::shared_ptr<DirectX::AudioEngine> audioEngine;
		Player player;
		float gravity = 60.f;
		uint32_t level = 0;
		GAME_STATE currentState = GAME_STATE::DEBUG;
		DIFFICULTY difficulty = DIFFICULTY::DEBUG;
	};

} // namespace dxe