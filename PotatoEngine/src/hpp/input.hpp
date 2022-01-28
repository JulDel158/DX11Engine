#pragma once

// this class will handle all game inputs

namespace dxe {

	class input {
	public:

		input();
		~input();

		// updates the key map
		void Update();

		// returns true when a key is pressed once
		bool KeyPressed(int i);

		// returns true while a key is being held down
		bool KeyDown(int i);

		// returns true once a pressed key is released
		bool KeyUp(int i);

	private:

		unsigned char keyboardState[256];
		unsigned char prevKeyboardState[256];
	};

	//if (GetKeyState('A') & 0x8000) {} // key is held

	//SHORT keyState = GetKeyState(VK_CAPITAL/*(caps lock)*/);
	//bool isToggled = keyState & 1; // key has been toggled
	//bool isDown = keyState & 0x8000;

} // namespace dxe