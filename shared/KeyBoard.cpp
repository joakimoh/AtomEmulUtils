#include <allegro5/allegro5.h>
#include "Keyboard.h"
#include <iostream>

using namespace std;

bool Keyboard::checkForKey(ALLEGRO_EVENT event)
{

    switch (event.type) {

        /* ALLEGRO_EVENT_KEY_DOWN - a keyboard key was pressed.
         */
    case ALLEGRO_EVENT_KEY_DOWN:
        keyDown(event.keyboard.keycode, event.keyboard.modifiers);
        return true;

        /* ALLEGRO_EVENT_KEY_UP - a keyboard key was released.
         */
    case ALLEGRO_EVENT_KEY_UP:
        keyUp(event.keyboard.keycode);
        return true;

    default:
        return false;
    }
}

Keyboard::Keyboard()
{
}