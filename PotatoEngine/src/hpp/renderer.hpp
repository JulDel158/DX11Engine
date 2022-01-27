#pragma once
#include "pipeline.hpp"
#include "object_data.hpp"

// Obect information should be passed into the renderer which we will then pass into our pipeline to draw
namespace dxe {

	class renderer {
	public:
		renderer(HWND windowHandle, View_t &viewProj);
		~renderer();

		void update();
		void draw();
	
	private:
		pipeline implementation;
		
		View_t& viewProj;
	};

} // dxe