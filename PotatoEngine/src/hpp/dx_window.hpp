#pragma once
#include <string>
#define NOMINMAX
#include <windows.h>

// dx_window handles window creation

namespace dxe {

	class dx_window
	{
	public:
		dx_window(HINSTANCE& hInstance, WNDPROC winProc, int nCmdShow, MSG& msg, unsigned int w = 1280, unsigned int h = 720);
		~dx_window();

		dx_window(const dx_window&) = delete;
		dx_window& operator=(const dx_window&) = delete;
		
	private:

		void initWindowClass(HINSTANCE& hInstance, WNDPROC winProc);
		void createWindow();

		void createConsole();
		void destroyConsole(int exitCode);

		std::wstring windowClass;
		std::wstring title;

		MSG& msg;
		
		WNDCLASSEX wcex{ 0 };

		HINSTANCE hInstance{ 0 };

	public:
		// window handle
		HWND mainHWND{ 0 };

		unsigned int Width, Height;
	};

} // namespace dxe