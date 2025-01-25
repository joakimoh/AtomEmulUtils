#ifndef GUI_H
#define GUI_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include "allegro5/allegro_native_dialog.h"
#include "Device.h"
#include "CUTSInterface.h"

using namespace std;

enum {

    FILE_ID = 1,
    FILE_OPEN_ID,
    FILE_EXIT_ID,

    TAPE_RECORDER_ID,
    PLAY_ID,
    RECORD_ID,
    PAUSE_ID,
    REWIND_ID,
    STOP_ID,
    LOAD_TAPE_ID,
    SAVE_TAPE_ID,

    HELP_ABOUT_ID
};

class GUI {

private:

    
   
  

    ALLEGRO_MENU* mMenu = NULL;
    ALLEGRO_DISPLAY* mDisplay = NULL;
    Devices* mDevices = NULL;
    CUTSInterface * mCUTS = NULL;

public:

    GUI(ALLEGRO_DISPLAY* disp, ALLEGRO_MENU * menu, Devices * devices);
    ~GUI();

	bool itemSelected(ALLEGRO_EVENT  *event);
};


#endif