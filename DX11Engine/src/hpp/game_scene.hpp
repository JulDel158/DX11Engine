#pragma once

#include "input.hpp"
#include "collision.hpp"
#include "scene.hpp"

// lib
#include <glm/glm.hpp>
#define NOMINMAX
#include <dsound.h>
#include <Audio.h>

// std
#include <vector>
#include <memory>

namespace dxe {

	struct enemy {
		sphere_t collider;
		GameObject* object{ nullptr };
		glm::vec3 velocity{ 0.f };
		float speed{ 0.f };
		float respawnTime{ 0.f };


		enemy() = default;
		~enemy() { object = nullptr; }
	};

	class GameScene : public scene {

	public:
		GameScene();
		~GameScene();

		void update(const float dt);

	private:

		void inputUpdate(const float dt);

		static constexpr uint8_t MAX_ENEMIES = 10;

		enemy enemies[MAX_ENEMIES];

		uint32_t score{ 0 };
		
		// we will put the sound engine here for now
		std::unique_ptr<DirectX::AudioEngine> audioEngine;
		std::unique_ptr<DirectX::SoundEffect> soundData;
		std::unique_ptr<DirectX::SoundEffect> songData;
		std::unique_ptr<DirectX::SoundEffect> songData2;
		std::unique_ptr<DirectX::SoundEffectInstance> soundInstance;
		std::unique_ptr<DirectX::SoundEffectInstance> soundInstance2;

		// particle stuff
		// Emitter pEmitter;
	};


} // namespace dxe