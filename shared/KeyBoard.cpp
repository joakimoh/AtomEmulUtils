#include <allegro5/allegro5.h>

bool KeyBoard::checkForKey(ALLEGRO_EVENT event)
{
    switch (event.type) {

        /* ALLEGRO_EVENT_KEY_DOWN - a keyboard key was pressed.
         */
    case ALLEGRO_EVENT_KEY_DOWN:
        pressedKeyMask |= event.keyboard.keycode;
        pressedModifierMask |= event.keyboard.modifiers;
        keyAction();
        return true;

        /* ALLEGRO_EVENT_KEY_UP - a keyboard key was released.
         */
    case ALLEGRO_EVENT_KEY_UP:
        pressedKeyMask &= ~event.keyboard.keycode;
        pressedModifierMask &= ~event.keyboard.modifiers;
        keyAction();
        return true;

    default:
        return false;
    }
}