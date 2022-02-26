#pragma once

// this class will handle all game inputs

namespace dxe {

	class input {
	public:

		input() = delete;
		~input() = delete;
		input(const input&) = delete;
		input& operator=(const input&) = delete;

		// updates the key map
		static void Update();

		// returns true when a key is pressed once
		static bool KeyPressed(int i);

		// returns true while a key is being held down
		static bool KeyDown(int i);

		// returns true once a pressed key is released
		static bool KeyUp(int i);

	private:

		
	};

	//if (GetKeyState('A') & 0x8000) {} // key is held

	//SHORT keyState = GetKeyState(VK_CAPITAL/*(caps lock)*/);
	//bool isToggled = keyState & 1; // key has been toggled
	//bool isDown = keyState & 0x8000;

} // namespace dxe