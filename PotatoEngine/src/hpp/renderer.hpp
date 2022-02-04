#pragma once
#include "pipeline.hpp"
#include "object_data.hpp"
#include "game_scene.hpp"

// Object information should be passed into the renderer which we will then pass into our pipeline to draw
// this class is also intended to handle pipeline settings and determine which data to send in which order (transparent objects get drawn last for example)
namespace dxe {

	class renderer {
	public:
		renderer(HWND windowHandle, Window_cb &window_cbuffer, Scene_cb& scene_buffer);
		~renderer();

		void draw(GameScene const& scene);

		void bindWindowBuffer();
	
	private:

		void update(const View_t& camera);

		pipeline implementation;
		
		Frame_cb frameCbuffer;

		Window_cb& windowCbuffer;

		Scene_cb& sceneCbuffer;

		Objectdata dCube;

		/*PIPELINE_PROPERTY_DESC objDesc;
		PIPELINE_PROPERTY_DESC skyboxDesc;*/
	};

} // dxe