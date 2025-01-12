#include <allegro5/allegro.h>
#include "Keyboard.h"
#include <iostream>

using namespace std;

Keyboard::Keyboard()
{
    al_get_keyboard_state(&mKeyboardState);
}

bool Keyboard::keyDown(int keycode)
{
    al_get_keyboard_state(&mKeyboardState);
    return al_key_down(&mKeyboardState, keycode);
}