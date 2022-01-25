#pragma once
#include <string>
#include <windows.h>

namespace pe {

	class dx_window
	{
	public:
		dx_window(HINSTANCE& hInstance, WNDPROC& winProc, int nCmdShow, MSG& msg);
		~dx_window();

		dx_window(const dx_window&) = delete;
		dx_window& operator=(const dx_window&) = delete;

	private:

		void initWindowClass(WNDPROC& winProc);
		void createWindow();

		void createConsole();
		void destroyConsole(int exitCode);

		std::wstring windowClass;
		std::wstring title;

		MSG& msg;
		
		HINSTANCE hInstance;

		HWND mainHWND;

		WNDCLASSEX wcex;
	};

} // namespace pe