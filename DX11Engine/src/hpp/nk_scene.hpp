#pragma once

#include "scene.hpp"

namespace dxe {

	class nk_scene : public scene {
	public:
		nk_scene(std::shared_ptr<DirectX::AudioEngine> audioEngine);
		~nk_scene();

		void update(const float dt);

	private:

		void inputUpdate(const float dt);

		std::shared_ptr<DirectX::AudioEngine> audioEngine;
	};

} // namespace dxe