#include "GUI.h"
#include <iostream>

using namespace std;


GUI::GUI(ALLEGRO_DISPLAY * disp)
{
    mDisplay = disp;
    mMenu = al_build_menu(menuInfo);
    if (!mMenu || !al_set_display_menu(mDisplay, mMenu)) {
        cout << "Failed to create menu!\n";
        throw runtime_error("Failed to create menu");
    }
}

GUI::~GUI()
{
    al_destroy_menu(mMenu);
}

bool GUI::itemSelected(int id)
{
    switch (id) {
    case PLAY_ID:
        cout << "PLAY\n";
        break;
    case RECORD_ID:
        cout << "PLAY\n";
        break;
    case PAUSE_ID:
        cout << "PAUSE\n";
        break;
    case REWIND_ID:
        cout << "REWIND\n";
        break;
    case STOP_ID:
        cout << "STOP\n";
        break;
    case TAPE_ID:
    {
        cout << "Select Tape\n";
        ALLEGRO_FILECHOOSER* filechooser;
        filechooser = al_create_native_file_dialog("", "Select a CSW tape file", "*.csw;", 1);
        al_show_native_file_dialog(mDisplay, filechooser);
        string file = al_get_native_file_dialog_path(filechooser, 0);
        cout << "File '" << file << " selected!\n";
        break;
    }
    default:
        cout << "???\n";
        return false;
    }

    return true;
}