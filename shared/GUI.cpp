#include "GUI.h"
#include <iostream>
#include "CUTSInterface.h"

using namespace std;


GUI::GUI(ALLEGRO_DISPLAY* disp, ALLEGRO_MENU* menu, Devices * devices): mMenu(menu), mDisplay(disp), mDevices(devices)
{
    Device* dev;
    if (!mDevices->getDevice("CUTS", dev)) {
        cout << "Failed to get access to cassette interface\n";
        throw runtime_error("Failed to get access to cassette interface");
    }
    mCUTS = (CUTSInterface*)dev;
}

GUI::~GUI()
{
    
}

bool GUI::itemSelected(ALLEGRO_EVENT* event)
{
    if (event == NULL || event->type != ALLEGRO_EVENT_MENU_CLICK)
        return false;

    switch (event->user.data1) {

    case PLAY_ID:
    {    
        if (mCUTS->playing()) {
            cout << "PLAY\n";
            al_set_menu_item_flags(mMenu, PLAY_ID, ALLEGRO_MENU_ITEM_CHECKED);
            al_set_menu_item_flags(mMenu, PAUSE_ID, 0);
            al_set_menu_item_flags(mMenu, REWIND_ID, 0);
            al_set_menu_item_flags(mMenu, STOP_ID, 0);
            mCUTS->play();
        }
        break;
    }

    case RECORD_ID:
    {  
        if (mCUTS->recording()) {
            cout << "RECORD\n";
            al_set_menu_item_flags(mMenu, RECORD_ID, ALLEGRO_MENU_ITEM_CHECKED);
            al_set_menu_item_flags(mMenu, PAUSE_ID, 0);
            al_set_menu_item_flags(mMenu, STOP_ID, 0);
            mCUTS->record();
        }
        break;
    }

    case PAUSE_ID:
    {
        if (mCUTS->playing() || mCUTS->recording()) {
            cout << "PAUSE\n";
            if (mCUTS->playing())
                al_set_menu_item_flags(mMenu, PLAY_ID, 0);
            else
                al_set_menu_item_flags(mMenu, RECORD_ID, 0);

            al_set_menu_item_flags(mMenu, PAUSE_ID, ALLEGRO_MENU_ITEM_CHECKED);
            al_set_menu_item_flags(mMenu, REWIND_ID, 0);
            al_set_menu_item_flags(mMenu, STOP_ID, 0);
            mCUTS->pause();
        }
        break;
    }
    case REWIND_ID:
    {
        if (mCUTS->playing()) {
            cout << "REWIND\n";
            al_set_menu_item_flags(mMenu, PLAY_ID, 0);
            al_set_menu_item_flags(mMenu, PAUSE_ID, 0);
            al_set_menu_item_flags(mMenu, REWIND_ID, ALLEGRO_MENU_ITEM_CHECKED);
            al_set_menu_item_flags(mMenu, STOP_ID, 0);
            mCUTS->rewind();
        }
        break;
    }

    case STOP_ID:
    {
        if (mCUTS->playing() || mCUTS->recording()) {
            cout << "STOP\n";
            al_set_menu_item_flags(mMenu, PLAY_ID, ALLEGRO_MENU_ITEM_DISABLED);
            al_set_menu_item_flags(mMenu, REWIND_ID, ALLEGRO_MENU_ITEM_DISABLED);
            al_set_menu_item_flags(mMenu, RECORD_ID, ALLEGRO_MENU_ITEM_DISABLED);
            al_set_menu_item_flags(mMenu, PAUSE_ID, ALLEGRO_MENU_ITEM_DISABLED);
            al_set_menu_item_flags(mMenu, STOP_ID, ALLEGRO_MENU_ITEM_DISABLED);
            al_set_menu_item_flags(mMenu, LOAD_TAPE_ID, 0);
            al_set_menu_item_flags(mMenu, SAVE_TAPE_ID, 0);
            mCUTS->stop();
        }
        break;
    }

    case LOAD_TAPE_ID:
    {
        if (!mCUTS->playing() && !mCUTS->recording()) {
            cout << "Load from Tape\n";
            ALLEGRO_FILECHOOSER* filechooser;
            filechooser = al_create_native_file_dialog("", "Select an existing CSW tape file", "*.csw;", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
            al_show_native_file_dialog(mDisplay, filechooser);
            int n = al_get_native_file_dialog_count(filechooser);
            if (n != 1)
                return false;
            cout << "#" << dec << n << " files selected!\n";
            string file = al_get_native_file_dialog_path(filechooser, 0);
            cout << "File '" << file << " selected!\n";
            if (!mCUTS->startLoadFile(file)) {
                return false;
            }
            al_destroy_native_file_dialog(filechooser);

            al_set_menu_item_flags(mMenu, LOAD_TAPE_ID, ALLEGRO_MENU_ITEM_DISABLED);
            al_set_menu_item_flags(mMenu, SAVE_TAPE_ID, ALLEGRO_MENU_ITEM_DISABLED);
            al_set_menu_item_flags(mMenu, RECORD_ID, ALLEGRO_MENU_ITEM_DISABLED);
            al_set_menu_item_flags(mMenu, PLAY_ID, 0);
            al_set_menu_item_flags(mMenu, PAUSE_ID, 0);
            al_set_menu_item_flags(mMenu, REWIND_ID, 0);
            al_set_menu_item_flags(mMenu, STOP_ID, 0);
        
        }
        break;
    }

    case SAVE_TAPE_ID:
    { 
        if (!mCUTS->playing() && !mCUTS->recording()) {
            cout << "Save to Tape\n";
            ALLEGRO_FILECHOOSER* filechooser;
            filechooser = al_create_native_file_dialog("", "Name the CSW tape file to be created", "*.csw;", ALLEGRO_FILECHOOSER_SAVE);
            al_show_native_file_dialog(mDisplay, filechooser);
            int n = al_get_native_file_dialog_count(filechooser);
            if (n != 1)
                return false;
            string file = al_get_native_file_dialog_path(filechooser, 0);

            cout << "File '" << file << " selected!\n";
            if (!mCUTS->startSaveFile(file)) {
                return false;
            }
            al_destroy_native_file_dialog(filechooser);

            al_set_menu_item_flags(mMenu, LOAD_TAPE_ID, ALLEGRO_MENU_ITEM_DISABLED);
            al_set_menu_item_flags(mMenu, SAVE_TAPE_ID, ALLEGRO_MENU_ITEM_DISABLED);
            al_set_menu_item_flags(mMenu, PLAY_ID, ALLEGRO_MENU_ITEM_DISABLED);
            al_set_menu_item_flags(mMenu, REWIND_ID, ALLEGRO_MENU_ITEM_DISABLED);

            al_set_menu_item_flags(mMenu, RECORD_ID, 0);
            al_set_menu_item_flags(mMenu, PAUSE_ID, 0);
            al_set_menu_item_flags(mMenu, STOP_ID, 0);

            
        }
        break;
    }

    default:
    {
        cout << "???\n";
        return false;
    }

    }

    return true;
}