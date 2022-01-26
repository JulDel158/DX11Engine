#pragma once
#include "pipeline.hpp"

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