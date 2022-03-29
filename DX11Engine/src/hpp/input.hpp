#pragma once

// this class will handle all game inputs
#define NOMINMAX
#include <Windows.h>

namespace dxe {

	class input {
	public:

		input() = delete;
		~input() = delete;
		input(const input&) = delete;
		input& operator=(const input&) = delete;

		// updates the key map
		static void Update();

		// 
		static void Listen(UINT message, LPARAM lParam, HWND hwnd);

		// returns true when a key is pressed once
		static bool KeyPressed(int i);

		// returns true while a key is being held down
		static bool KeyDown(int i);

		// returns true once a pressed key is released
		static bool KeyUp(int i);

		// returns true when a mouse button is pressed 0 left, 1 right, 2 middle
		static bool MouseButtonPressed(int i);

		// returns true while a mouse button is being held down 0 left, 1 right, 2 middle
		static bool MouseButtonDown(int i);

		// returns true once a held down mouse button is released 0 left, 1 right, 2 middle
		static bool MouseButtonUp(int i);

		static POINT GetMouseWCoord();

		static POINT GetMouseCcoord();

		static void SetCursonPosition(int x, int y);

		static void SetWindowHandle(HWND hWnd);

	private:

		
	};

	//if (GetKeyState('A') & 0x8000) {} // key is held

	//SHORT keyState = GetKeyState(VK_CAPITAL/*(caps lock)*/);
	//bool isToggled = keyState & 1; // key has been toggled
	//bool isDown = keyState & 0x8000;

} // namespace dxe