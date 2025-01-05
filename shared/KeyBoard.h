#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <allegro5/allegro5.h>

class KeyBoard {

public:
	bool checkForKey(ALLEGRO_EVENT event);	

protected:
	virtual void keyAction(int pressedKey) = 0;
	int pressedKeyMask;
	unsigned pressedModifierMask;
};

#endif