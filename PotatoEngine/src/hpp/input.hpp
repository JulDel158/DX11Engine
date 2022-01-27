#pragma once

// this class will handle all game inputs

namespace dxe {

	class input {
	public:

		input();
		~input();

		void Update();

		bool KeyPressed(int i);
		bool KeyDown(int i);
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