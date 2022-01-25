#pragma once
#include <string>
#include <windows.h>


// BUG: only works in debug mode :/

namespace pe {

	class dx_window
	{
	public:
		dx_window(HINSTANCE& hInstance, WNDPROC winProc, int nCmdShow, MSG& msg);
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

		HWND mainHWND{ 0 };
	};

} // namespace pe