#include <allegro5/allegro.h>
#include "Keyboard.h"
#include <iostream>
#include "DebugInfo.h"

using namespace std;

Keyboard::Keyboard(DebugInfo debugInfo) : mDebugInfo(debugInfo)
{
    al_get_keyboard_state(&mKeyboardState);
}

bool Keyboard::keyDown(int keycode)
{
    al_get_keyboard_state(&mKeyboardState);
    return al_key_down(&mKeyboardState, keycode);
}