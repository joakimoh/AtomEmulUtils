#ifndef GUI_H
#define GUI_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include "allegro5/allegro_native_dialog.h"
#include "Device.h"
#include "TapeRecorder.h"
#include "RAM.h"
#include "SDCard.h"
#include <thread>
#include <Debugger.h>
#include <iostream>
#include <cstdint>


using namespace std;

enum {

    FILE_ID = 1,
    LOAD_INTO_RAM,
    SAVE_FROM_RAM,
    FILE_EXIT_ID,

    DBG_ID,
    ENTER_DBG_ID,
    EXIT_DBG_ID,

    MMC_ID,
    MMC_EJECT_ID,
    MMC_INSERT_ID,

    TAPE_RECORDER_ID,
    PLAY_ID,
    RECORD_ID,
    PAUSE_ID,
    REWIND_ID,
    STOP_ID,
    LOAD_TAPE_ID,
    SAVE_TAPE_ID,

    SPEED_ID,
    SPEED_10_ID,
    SPEED_25_ID,
    SPEED_50_ID,
    SPEED_100_ID,
    SPEED_200_ID,
    SPEED_300_ID,
    SPEED_500_ID,

    HELP_ABOUT_ID
};

class Display;
class Engine;

class GUI {

private:
#define MY_ALLEGRO_MENU_SEPARATOR {NULL, (uint16_t)-1,0, NULL}

	ALLEGRO_MENU_INFO mMainMenu[38] = {

		ALLEGRO_START_OF_MENU("&File", FILE_ID),
			{ "&Load data into RAM",   LOAD_INTO_RAM,  0,  NULL },
		MY_ALLEGRO_MENU_SEPARATOR,
			{ "&Save memory data to file",   SAVE_FROM_RAM,  0,  NULL },
		MY_ALLEGRO_MENU_SEPARATOR,
			{ "E&xit",                 FILE_EXIT_ID,   0,  NULL },
		ALLEGRO_END_OF_MENU,

		ALLEGRO_START_OF_MENU("&Memory Card", MMC_ID),
			{ "Eject",             MMC_EJECT_ID,           ALLEGRO_MENU_ITEM_DISABLED, NULL },
		 { "Insert",            MMC_INSERT_ID,          0,                          NULL },

		ALLEGRO_END_OF_MENU,

		ALLEGRO_START_OF_MENU("&Speed", SPEED_ID),
			{ "Real-time",         SPEED_100_ID,           ALLEGRO_MENU_ITEM_CHECKED, NULL },
			{ "10%",               SPEED_10_ID,            ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
			{ "25%",               SPEED_25_ID,            ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
			{ "50%",               SPEED_50_ID,            ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
			{ "200%",              SPEED_200_ID,           ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
			{ "300%",              SPEED_300_ID,           ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
			{ "500%",              SPEED_500_ID,           ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
			ALLEGRO_END_OF_MENU,

		ALLEGRO_START_OF_MENU("&Tape Recorder", TAPE_RECORDER_ID),
			{ "&Play",             PLAY_ID,                ALLEGRO_MENU_ITEM_DISABLED | ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
			{ "&Record",           RECORD_ID,              ALLEGRO_MENU_ITEM_DISABLED | ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
			{ "Pause",             PAUSE_ID,               ALLEGRO_MENU_ITEM_DISABLED | ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
			{ "Rewind",            REWIND_ID,              ALLEGRO_MENU_ITEM_DISABLED | ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
			{ "Stop",               STOP_ID,                ALLEGRO_MENU_ITEM_DISABLED | ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
			{ "&Load from Tape",    LOAD_TAPE_ID,           0,                                                      NULL },
			{ "&Save to Tape",      SAVE_TAPE_ID,           0,                                                      NULL },
			ALLEGRO_END_OF_MENU,

		ALLEGRO_START_OF_MENU("&Help", 0),
			{ "&About",             HELP_ABOUT_ID,               0,                                                 NULL },
			ALLEGRO_END_OF_MENU,

		ALLEGRO_START_OF_MENU("&Debugger", DBG_ID),
			{ "Enter",              ENTER_DBG_ID,           0,                                                      NULL },
			{ "Exit",               EXIT_DBG_ID,            ALLEGRO_MENU_ITEM_DISABLED,                             NULL },
			ALLEGRO_END_OF_MENU,

		  ALLEGRO_END_OF_MENU
	};

	ALLEGRO_MENU* mMenu = NULL;
	bool mPopupMenuOnly = false;
    ALLEGRO_DISPLAY* mDisplay = NULL;
    DeviceManager* mDevices = NULL;
    TapeRecorder* mTapeRec = NULL;
    double *mEmulationSpeed = NULL;

    SDCard *mMMC = NULL;
    DebugManager* mDM = NULL;
    Debugger *mDebugger = NULL;

    thread mDebugThread;

	Engine *mEngine = NULL;
	string mOutDir;

public:

    GUI(Engine *engine, ALLEGRO_EVENT_QUEUE *queue, ALLEGRO_DISPLAY* disp, DeviceManager * devices, double *emulationSpeed, DebugManager *dm, string outDir);
    ~GUI();

	bool itemSelected(ALLEGRO_EVENT  *event);

	bool popupMenu();
};


#endif