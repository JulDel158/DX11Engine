#pragma once

// these functions will handle all game/app inputs

#define NOMINMAX
#include <Windows.h>

namespace dxe::input {
		
		// updates the key map
	void Update(); 

		// 
	void Listen(UINT message, LPARAM lParam, HWND hwnd); 

		// returns true when a key is pressed once
	bool KeyPressed(int i);

		// returns true while a key is being held down
	bool KeyDown(int i);

		// returns true once a pressed key is released
	bool KeyUp(int i);

		// returns true when a mouse button is pressed 0 left, 1 right, 2 middle
	bool MouseButtonPressed(int i);

		// returns true while a mouse button is being held down 0 left, 1 right, 2 middle
	bool MouseButtonDown(int i);

		// returns true once a held down mouse button is released 0 left, 1 right, 2 middle
	bool MouseButtonUp(int i);

	POINT GetMouseWCoord();

	POINT GetMouseCcoord();

	void SetCursonPosition(int x, int y, bool ignore = false);

	void SetWindowHandle(HWND hWnd);

	void SetMouseDelta(LONG x, LONG y);

	POINT GetMouseDelta();

	//if (GetKeyState('A') & 0x8000) {} // key is held
	//SHORT keyState = GetKeyState(VK_CAPITAL/*(caps lock)*/);
	//bool isToggled = keyState & 1; // key has been toggled
	//bool isDown = keyState & 0x8000;

} // namespace dxe::input