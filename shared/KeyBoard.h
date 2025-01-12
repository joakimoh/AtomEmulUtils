#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <allegro5/allegro5.h>
#include "DebugInfo.h"

class Keyboard {

protected:

	ALLEGRO_KEYBOARD_STATE mKeyboardState;

	DebugInfo mDebugInfo;

	Keyboard(DebugInfo debugInfo);

	bool keyDown(int keycode);

};

#endif