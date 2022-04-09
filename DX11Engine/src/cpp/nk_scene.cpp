#include "../hpp/nk_scene.hpp"

namespace dxe {
	nk_scene::nk_scene(std::shared_ptr<DirectX::AudioEngine> audioEngine) : audioEngine(audioEngine) {
		// camera initialization
		camera = std::make_unique<View_t>();
		camera->position = glm::vec3(0.f, 0.f, 0.f);
		camera->updateView();

		// UI Text
		allocateTextwraps(3);
		
	}

	nk_scene::~nk_scene(){

	}

	// main game loop
	void nk_scene::update(const float dt){

	}

	void nk_scene::inputUpdate(const float dt){

	}

} // namespace dxe