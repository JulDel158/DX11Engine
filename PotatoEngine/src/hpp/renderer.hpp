#pragma once
#include "pipeline.hpp"

// Obect information should be passed into the renderer which we will then pass into our pipeline to draw
namespace dxe {

	class renderer {
	public:
		renderer(HWND windowHandle);
		~renderer();

		void update();
		void draw();
	
	private:
		pipeline implementation;
		
	};

} // dxe