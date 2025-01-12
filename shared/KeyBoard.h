#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <allegro5/allegro5.h>

class Keyboard {

protected:

	ALLEGRO_KEYBOARD_STATE mKeyboardState;

	Keyboard();

	bool keyDown(int keycode);

};

#endif