#pragma once

#include "object_data.hpp"
#include "input.hpp"
#include "collision.hpp"

#pragma comment(lib,"dsound.lib")
#pragma comment(lib,"dxguid.lib")

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
		Emitter* GetParticles();

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
		
		// we will put the sound engine here for now
		std::unique_ptr<DirectX::AudioEngine> audioEngine;
		std::unique_ptr<DirectX::SoundEffect> soundData;
		std::unique_ptr<DirectX::SoundEffect> songData;
		std::unique_ptr<DirectX::SoundEffect> songData2;
		std::unique_ptr<DirectX::SoundEffectInstance> soundInstance;
		std::unique_ptr<DirectX::SoundEffectInstance> soundInstance2;

		// particle stuff
		Emitter pEmitter;
	};


} // namespace dxe