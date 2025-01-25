#ifndef GUI_H
#define GUI_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include "allegro5/allegro_native_dialog.h"

using namespace std;



class GUI {

private:

    
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
        TAPE_ID,

        HELP_ABOUT_ID
    };

    ALLEGRO_MENU_INFO menuInfo[17] = {

          ALLEGRO_START_OF_MENU("&File", FILE_ID),
             { "&Open",        FILE_OPEN_ID,           0,                                                      NULL },
             {NULL, (uint16_t) -1, 0, NULL}, //ALLEGRO_MENU_SEPARATOR,
             { "E&xit",        FILE_EXIT_ID,           0,                                                      NULL },
             ALLEGRO_END_OF_MENU,

          ALLEGRO_START_OF_MENU("&Tape Recorder", TAPE_RECORDER_ID),
             { "&Play",        PLAY_ID,                ALLEGRO_MENU_ITEM_DISABLED,                             NULL },
             { "&Record",      RECORD_ID,              ALLEGRO_MENU_ITEM_DISABLED,                             NULL },
             { "Pause",        PAUSE_ID,               ALLEGRO_MENU_ITEM_DISABLED,                             NULL },
             { "Rewind",       REWIND_ID,              ALLEGRO_MENU_ITEM_DISABLED,                             NULL },
             { "Stop",         STOP_ID,                ALLEGRO_MENU_ITEM_DISABLED | ALLEGRO_MENU_ITEM_CHECKED, NULL },
             { "&Select Tape", TAPE_ID,                0,                                                      NULL },
             ALLEGRO_END_OF_MENU,

          ALLEGRO_START_OF_MENU("&Help", 0),
             { "&About",       HELP_ABOUT_ID,          0,                                                      NULL },
             ALLEGRO_END_OF_MENU,

          ALLEGRO_END_OF_MENU
    };
  

    ALLEGRO_MENU* mMenu = NULL;
    ALLEGRO_DISPLAY* mDisplay = NULL;

public:

    GUI(ALLEGRO_DISPLAY * disp);
    ~GUI();

	bool itemSelected(int id);
};


#endif