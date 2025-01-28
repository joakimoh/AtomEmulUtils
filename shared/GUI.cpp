#include "GUI.h"
#include <iostream>

using namespace std;


GUI::GUI(ALLEGRO_DISPLAY* disp, ALLEGRO_MENU* menu, Devices * devices): mMenu(menu), mDisplay(disp), mDevices(devices)
{
    Device* dev;
    if (!mDevices->getDevice(DeviceId::TAPE_RECORDER_DEV, dev)) {
        cout << "Failed to get access to Tape Recorder\n";
        throw runtime_error("Failed to get access to Tape Recorder");
    }
    mTapeRec = (TapeRecorder *)dev;
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
        if (mTapeRec->playing()) {
            al_set_menu_item_flags(mMenu, PLAY_ID, ALLEGRO_MENU_ITEM_CHECKED);
            al_set_menu_item_flags(mMenu, PAUSE_ID, 0);
            al_set_menu_item_flags(mMenu, REWIND_ID, 0);
            al_set_menu_item_flags(mMenu, STOP_ID, 0);
            mTapeRec->play();
        }
        break;
    }

    case RECORD_ID:
    {  
        if (mTapeRec->recording()) {
            al_set_menu_item_flags(mMenu, RECORD_ID, ALLEGRO_MENU_ITEM_CHECKED);
            al_set_menu_item_flags(mMenu, PAUSE_ID, 0);
            al_set_menu_item_flags(mMenu, REWIND_ID, ALLEGRO_MENU_ITEM_DISABLED);
            al_set_menu_item_flags(mMenu, STOP_ID, 0);
            mTapeRec->record();
        }
        break;
    }

    case PAUSE_ID:
    {
        if (mTapeRec->playing() || mTapeRec->recording()) {
            if (mTapeRec->playing()) {
                al_set_menu_item_flags(mMenu, PLAY_ID, 0);
                al_set_menu_item_flags(mMenu, PAUSE_ID, ALLEGRO_MENU_ITEM_CHECKED);
                al_set_menu_item_flags(mMenu, REWIND_ID, 0);
                al_set_menu_item_flags(mMenu, STOP_ID, 0);
            }
            else {
                al_set_menu_item_flags(mMenu, RECORD_ID, 0);
                al_set_menu_item_flags(mMenu, PAUSE_ID, ALLEGRO_MENU_ITEM_CHECKED);
                al_set_menu_item_flags(mMenu, REWIND_ID, ALLEGRO_MENU_ITEM_DISABLED);
                al_set_menu_item_flags(mMenu, STOP_ID, 0);

            }

            
            mTapeRec->pause();
        }
        break;
    }
    case REWIND_ID:
    {
        if (mTapeRec->playing()) {
            al_set_menu_item_flags(mMenu, PLAY_ID, 0);
            al_set_menu_item_flags(mMenu, PAUSE_ID, 0);
            al_set_menu_item_flags(mMenu, REWIND_ID, ALLEGRO_MENU_ITEM_CHECKED);
            al_set_menu_item_flags(mMenu, STOP_ID, 0);
            mTapeRec->rewind();
        }
        break;
    }

    case STOP_ID:
    {
        if (mTapeRec->playing() || mTapeRec->recording()) {
            al_set_menu_item_flags(mMenu, PLAY_ID, ALLEGRO_MENU_ITEM_DISABLED);
            al_set_menu_item_flags(mMenu, REWIND_ID, ALLEGRO_MENU_ITEM_DISABLED);
            al_set_menu_item_flags(mMenu, RECORD_ID, ALLEGRO_MENU_ITEM_DISABLED);
            al_set_menu_item_flags(mMenu, PAUSE_ID, ALLEGRO_MENU_ITEM_DISABLED);
            al_set_menu_item_flags(mMenu, STOP_ID, ALLEGRO_MENU_ITEM_CHECKED);
            al_set_menu_item_flags(mMenu, LOAD_TAPE_ID, 0);
            al_set_menu_item_flags(mMenu, SAVE_TAPE_ID, 0);
            mTapeRec->stop();
        }
        break;
    }

    case LOAD_TAPE_ID:
    {
        if (!mTapeRec->playing() && !mTapeRec->recording()) {
            ALLEGRO_FILECHOOSER* filechooser;
            filechooser = al_create_native_file_dialog("", "Select an existing CSW tape file", "*.csw;", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
            al_show_native_file_dialog(mDisplay, filechooser);
            int n = al_get_native_file_dialog_count(filechooser);
            if (n != 1)
                return false;
            string file = al_get_native_file_dialog_path(filechooser, 0);
            if (!mTapeRec->startLoadFile(file)) {
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
        if (!mTapeRec->playing() && !mTapeRec->recording()) {
            ALLEGRO_FILECHOOSER* filechooser;
            filechooser = al_create_native_file_dialog("", "Name the CSW tape file to be created", "*.csw;", ALLEGRO_FILECHOOSER_SAVE);
            al_show_native_file_dialog(mDisplay, filechooser);
            int n = al_get_native_file_dialog_count(filechooser);
            if (n != 1)
                return false;
            string file = al_get_native_file_dialog_path(filechooser, 0);

            if (!mTapeRec->startSaveFile(file)) {
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
        return false;
    }

    }

    return true;
}