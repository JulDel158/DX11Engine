#include "../hpp/input.hpp"

#include <windowsx.h>

namespace {
	unsigned char keyboardState[256];
	unsigned char prevKeyboardState[256];

	bool mouseState[5];  
	bool prevMouseState[5];

	POINT mousePointW;
	POINT mousePointC;
	/* left button 0 
	*  right button 1
	*  middle button 2
	*  4 - 5 reserved for additional input support
	* */
}

namespace dxe {

	void input::Update() {
		auto b = GetKeyboardState(keyboardState);
		if (!b) {
			//error
		}
	}

	void input::Listen(UINT message, LPARAM lParam, HWND hwnd) {

		switch (message)
		{
		case WM_MOUSEMOVE:
			mousePointW.x = mousePointC.x = GET_X_LPARAM(lParam);
			mousePointW.y = mousePointC.y = GET_Y_LPARAM(lParam);
			ScreenToClient(hwnd, &mousePointC);
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

	bool input::KeyPressed(int i) {
		const bool result = (keyboardState[i] & 0x80) && (keyboardState[i] != prevKeyboardState[i]);
		if (result) { prevKeyboardState[i] = keyboardState[i]; }
		return result;
	}

	bool input::KeyDown(int i) {
		return static_cast<bool>(keyboardState[i] & 0x80);
	}

	bool input::KeyUp(int i) {
		const bool result = !(keyboardState[i] & 0x80) && keyboardState[i] != prevKeyboardState[i];
		if (result) { prevKeyboardState[i] = keyboardState[i]; }
		return result;
	}

	bool input::MouseButtonPressed(int i) {
		const bool result = (mouseState[i] && mouseState[i] != prevMouseState[i]);
		if (result) { prevMouseState[i] = mouseState[i]; }
		return result;
	}

	bool input::MouseButtonDown(int i) {
		return mouseState[i];
	}

	bool input::MouseButtonUp(int i) {
		const bool result = !mouseState[i] && mouseState[i] != prevMouseState[i];
		if (result) { prevKeyboardState[i] = mouseState[i]; }
		return result;
	}

	POINT input::GetMouseWCoord() { return mousePointW; }

	POINT input::GetMouseCcoord() { return mousePointC; }

	void input::SetCursonPosition(int x, int y) {
		// TODO: ONLY SET THE CURSOR IF THIS WINDOW IS ACTIVE
		SetCursorPos(x, y);
	}

} // namespace dxe