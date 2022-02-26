#include "../hpp/input.hpp"
#define NOMINMAX
#include <Windows.h>

namespace {
	unsigned char keyboardState[256];
	unsigned char prevKeyboardState[256];
}

namespace dxe {

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