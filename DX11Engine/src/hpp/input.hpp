#pragma once

// these functions will handle all game/app inputs

#define NOMINMAX
#include <Windows.h>

namespace dxe::input {
		
	// updates the key map
	void Update(); 

	// returns true when a key is pressed once
	bool KeyPressed(int i);

	// returns true while a key is being held down
	bool KeyDown(int i);

	// returns true once a pressed key is released
	bool KeyUp(int i);

	POINT GetMouseWCoord();

	POINT GetMouseCcoord();

	POINT GetMouseDelta();

	POINT GetMouseRawPos();

	void SetCursonPosition(int x, int y, bool ignore = false); // warning will send WM_MOUSEMOVE MESSAGE to message pump (can lag your program)

	void SetWindowHandle(HWND hWnd);

	void SetMouseDelta(LONG x, LONG y);

	void SetMouseRawPos(LONG x, LONG y);

	void ToggleCursor(bool visible);
	//if (GetKeyState('A') & 0x8000) {} // key is held
	//SHORT keyState = GetKeyState(VK_CAPITAL/*(caps lock)*/);
	//bool isToggled = keyState & 1; // key has been toggled
	//bool isDown = keyState & 0x8000;

} // namespace dxe::input