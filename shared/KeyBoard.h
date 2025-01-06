#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <allegro5/allegro5.h>

class Keyboard {

public:
	bool checkForKey(ALLEGRO_EVENT event);	

protected:
	virtual void keyDown(int key, unsigned modifiers) = 0;
	virtual void keyUp(int key) = 0;
};

#endif