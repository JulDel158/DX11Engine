#include "../hpp/input.hpp"

#include <windowsx.h>

namespace {
	unsigned char keyboardState[256];
	unsigned char prevKeyboardState[256];

	bool mouseState[5];
	bool prevMouseState[5];
	bool updateMouse{ true }; // this prevents a mouse update on the same cycle that the message/s are sent
	POINT mousePointW;
	POINT mousePointC;
	POINT mouseDelta;
	/* left button 0
	*  right button 1
	*  middle button 2
	*  4 - 5 reserved for additional input support
	* */

	HWND windowHandle{ 0 };
}

namespace dxe::input {

	void Update() {
		memcpy(&prevKeyboardState, &keyboardState, 256 * sizeof(unsigned char));
		if (updateMouse) { memcpy(&prevMouseState, &mouseState, 5 * sizeof(bool)); }
		auto b = GetKeyboardState(keyboardState);
		if (!b) {
			//error
		}
		updateMouse = true;

		/*GetCursorPos(&mousePointW);
		mousePointC = mousePointW;
		ScreenToClient(windowHandle, &mousePointC);*/
	}

	void Listen(UINT message, LPARAM lParam, HWND hwnd) {
		updateMouse = false;
		switch (message)
		{
		case WM_MOUSEMOVE:
			
			break;
		case WM_LBUTTONDOWN:
			mouseState[0] = true;
			break;
		case WM_LBUTTONUP:
			mouseState[0] = false;
			break;
		case WM_LBUTTONDBLCLK:
			break;
		case WM_RBUTTONDOWN:
			mouseState[1] = true;
			break;
		case WM_RBUTTONUP:
			mouseState[1] = false;
			break;
		case WM_RBUTTONDBLCLK:
			break;
		case WM_MBUTTONDOWN:
			mouseState[2] = true;
			break;
		case WM_MBUTTONUP:
			mouseState[2] = false;
			break;
		case WM_MBUTTONDBLCLK:
			break;
		default:
			break;
		}
	}

	bool KeyPressed(int i) {
		return (keyboardState[i] & 0x80) && (keyboardState[i] != prevKeyboardState[i]);
	}

	bool KeyDown(int i) {
		return static_cast<bool>(keyboardState[i] & 0x80);
	}

	bool KeyUp(int i) {
		return !(keyboardState[i] & 0x80) && keyboardState[i] != prevKeyboardState[i];
	}

	bool MouseButtonPressed(int i) {
		return mouseState[i] && mouseState[i] != prevMouseState[i];
	}

	bool MouseButtonDown(int i) {
		return mouseState[i];
	}

	bool MouseButtonUp(int i) {
		return !mouseState[i] && mouseState[i] != prevMouseState[i];
	}

	POINT GetMouseWCoord() { 
		GetCursorPos(&mousePointW);
		return mousePointW; 
	}

	POINT GetMouseCcoord() { 
		GetCursorPos(&mousePointC);
		ScreenToClient(windowHandle, &mousePointC);
		return mousePointC; }

	void SetCursonPosition(int x, int y, bool ignore) {
		// TODO: ONLY SET THE CURSOR IF THIS WINDOW IS ACTIVE
		if (windowHandle == GetActiveWindow() || ignore)
			SetCursorPos(x, y);
		//mousePointW.x = mousePointC.x = GET_X_LPARAM(lParam);
		//mousePointW.y = mousePointC.y = GET_Y_LPARAM(lParam);
		/*GetCursorPos(&mousePointW);
		mousePointC = mousePointW;
		ScreenToClient(windowHandle, &mousePointC);*/
	}

	void SetWindowHandle(HWND hWnd) {
		windowHandle = hWnd;
	}

	void SetMouseDelta(LONG x, LONG y) {
		mouseDelta.x = x;
		mouseDelta.y = y;
	}

	POINT GetMouseDelta()
	{
		POINT temp = mouseDelta;
		mouseDelta.x = mouseDelta.y = 0;
		return temp;
	}

} // namespace dxe::input