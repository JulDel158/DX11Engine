#include "../hpp/input.hpp"

#include <Windows.h>


namespace dxe {
	input::input() {}
	input::~input() {}

	void input::Update() {
		GetKeyboardState(keyboardState);
	}

	bool input::KeyPressed(int i) {
		const bool result = (keyboardState[i] & 0x80) && (keyboardState[i] != prevKeyboardState[i]);
		if (result) { prevKeyboardState[i] = keyboardState[i]; }
		return result;
	}

	bool input::KeyDown(int i) {
		return (bool)(keyboardState[i] & 0x80);
	}

	bool input::KeyUp(int i) {
		const bool result = !(keyboardState[i] & 0x80) && keyboardState[i] != prevKeyboardState[i];
		if (result) { prevKeyboardState[i] = keyboardState[i]; }
		return result;
	}

} // namespace dxe