#pragma once
#include "pipeline.hpp"
#include "object_data.hpp"

// Object information should be passed into the renderer which we will then pass into our pipeline to draw
// this class is also intended to handle pipeline settings and determine which data to send in which order (transparent objects get drawn last for example)
namespace dxe {

	class renderer {
	public:
		renderer(HWND windowHandle, Frame_cb &frame_cbuffer, Window_cb &window_cbuffer);
		~renderer();

		void update();
		void draw(GameObject* objects, GameObject* skyBoxes, const uint32_t size);

		void bindWindowBuffer();
	
	private:
		pipeline implementation;
		
		Frame_cb& frameCbuffer;

		Window_cb& windowCbuffer;

		Objectdata dCube;

		PIPELINE_PROPERTY_DESC objDesc;
		PIPELINE_PROPERTY_DESC skyboxDesc;
	};

} // dxe