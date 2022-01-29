#pragma once
#include "pipeline.hpp"
#include "object_data.hpp"

// Obect information should be passed into the renderer which we will then pass into our pipeline to draw
namespace dxe {

	class renderer {
	public:
		renderer(HWND windowHandle, Frame_cb &frame_cbuffer, Window_cb &window_cbuffer);
		~renderer();

		void update();
		void draw(Objectdata& obj);

		void bindWindowBuffer();
	
	private:
		pipeline implementation;
		
		Frame_cb& frameCbuffer;

		Window_cb& windowCbuffer;

		Objectdata dCube;
	};

} // dxe