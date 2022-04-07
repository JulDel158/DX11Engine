#include "../hpp/input.hpp"

#include <windowsx.h>

namespace {
	unsigned char keyboardState[256]{ 0 };
	unsigned char prevKeyboardState[256]{ 0 };
	POINT mouseDelta{ 0 };
	POINT mouseRawPos{ 0 };
	HWND windowHandle{ 0 };
}

namespace dxe::input {

	void Update() {
		memcpy(&prevKeyboardState, &keyboardState, 256 * sizeof(unsigned char));
		auto b = GetKeyboardState(keyboardState);
		if (!b) {
			//error
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

	POINT GetMouseWCoord() { 
		POINT mousePointW{ 0 };
		GetCursorPos(&mousePointW);
		return mousePointW; 
	}

	POINT GetMouseCcoord() { 
		POINT mousePointC;
		GetCursorPos(&mousePointC);
		ScreenToClient(windowHandle, &mousePointC);
		return mousePointC; 
	}

	POINT GetMouseDelta() {
		POINT temp = mouseDelta;
		mouseDelta.x = mouseDelta.y = 0;
		return temp;
	}

	POINT GetMouseRawPos() {
		return mouseRawPos;
	}

	void SetCursonPosition(int x, int y, bool ignore) {
		if (windowHandle == GetActiveWindow() || ignore)
			SetCursorPos(x, y);
	}

	void SetWindowHandle(HWND hWnd) {
		windowHandle = hWnd;
	}

	void SetMouseDelta(LONG x, LONG y) {
		mouseDelta.x = x;
		mouseDelta.y = y;

		mouseRawPos.x += x;
		mouseRawPos.y += y;
	}

	void SetMouseRawPos(LONG x, LONG y) {
		mouseRawPos.x = x;
		mouseRawPos.y = y;
	}

	void ToggleCursor(bool visible) {
		ShowCursor(visible);
	}

} // namespace dxe::input