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

class Display;
class Debugger;
class ConnectionManager;
class Display;
class Engine;
class VideoDisplayUnit;

using namespace std;

enum {

    FILE_ID = 1,
    LOAD_INTO_RAM,
    SAVE_FROM_RAM,
	FILE_RESET_ID,
    FILE_EXIT_ID,

    DBG_ID,
    START_DBG_ID,
	STOP_WAIT_DBG_ID,
	STOP_TRACING_DBG_ID,
	RESET_DBG_ID,

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
    SPEED_1_ID,
	SPEED_10_ID,
    SPEED_25_ID,
    SPEED_50_ID,
    SPEED_REALTIME_ID,
    SPEED_2X_ID,
    SPEED_3X_ID,
    SPEED_5X_ID,
	SPEED_10X_ID,
	SPEED_25X_ID,
	SPEED_50X_ID,
	SPEED_100X_ID,

	RATE_ID,
	RATE_5_ID,
	RATE_10_ID,
	RATE_15_ID,
	RATE_25_ID,
	RATE_30_ID,
	RATE_50_ID,
	RATE_60_ID,

    HELP_ABOUT_ID
};



class GUI {

private:

	bool mQuit = false;

	ALLEGRO_EVENT_QUEUE* mQueue = NULL;
#define MY_ALLEGRO_MENU_SEPARATOR {NULL, (uint16_t)-1,0, NULL}

	ALLEGRO_MENU_INFO mMainMenu[54] = {

		ALLEGRO_START_OF_MENU("&File", FILE_ID),
			{ "&Load data into RAM",   LOAD_INTO_RAM,  0,  NULL },
		MY_ALLEGRO_MENU_SEPARATOR,
			{ "&Save memory data to file",   SAVE_FROM_RAM,  0,  NULL },
		MY_ALLEGRO_MENU_SEPARATOR,
			{ "Reset",                 FILE_RESET_ID,   0,  NULL },
		MY_ALLEGRO_MENU_SEPARATOR,
			{ "E&xit",                 FILE_EXIT_ID,   0,  NULL },
		ALLEGRO_END_OF_MENU,

		ALLEGRO_START_OF_MENU("&Memory Card", MMC_ID),
			{ "Eject",             MMC_EJECT_ID,           ALLEGRO_MENU_ITEM_DISABLED, NULL },
		 { "Insert",            MMC_INSERT_ID,          0,                          NULL },

		ALLEGRO_END_OF_MENU,

		ALLEGRO_START_OF_MENU("&Speed", SPEED_ID),
			{ "Real-time",			SPEED_REALTIME_ID,		ALLEGRO_MENU_ITEM_CHECKED, NULL },
			{ "1%",					SPEED_1_ID,				ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
			{ "10%",				SPEED_10_ID,            ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
			{ "25%",				SPEED_25_ID,            ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
			{ "50%",				SPEED_50_ID,            ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
			{ "x2",					SPEED_2X_ID,			ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
			{ "x3",					SPEED_3X_ID,			ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
			{ "x5",					SPEED_5X_ID,			ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
			{ "x10",				SPEED_10X_ID,           ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
			{ "x25",				SPEED_25X_ID,           ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
			{ "x50",				SPEED_50X_ID,           ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
			{ "x100",				SPEED_100X_ID,				ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
			ALLEGRO_END_OF_MENU,

		ALLEGRO_START_OF_MENU("&Screen refresh rate", RATE_ID),
			{ "5",					RATE_5_ID,			ALLEGRO_MENU_ITEM_CHECKBOX, NULL },

			{ "10",					RATE_10_ID,            ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
			{ "15",					RATE_15_ID,            ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
			{ "25",					RATE_25_ID,            ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
			{ "30",					RATE_30_ID,            ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
			{ "50",					RATE_50_ID,            ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
			{ "60",					RATE_60_ID,            ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
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
			{ "Start",              START_DBG_ID,           ALLEGRO_MENU_ITEM_DISABLED,                             NULL },
			{ "Stop waiting",       STOP_WAIT_DBG_ID,       ALLEGRO_MENU_ITEM_DISABLED,                             NULL },
			{ "Stop tracing",       STOP_TRACING_DBG_ID,       0,                             NULL },
			ALLEGRO_END_OF_MENU,

		  ALLEGRO_END_OF_MENU
	};

	ALLEGRO_MENU* mMenu = nullptr;
	bool mPopupMenuOnly = false;
    Display* mDisplay = nullptr;
	ALLEGRO_DISPLAY* mAllegroDisplay = nullptr;
    DeviceManager* mDevices = nullptr;
    TapeRecorder* mTapeRec = nullptr;
    double *mEmulationSpeed = nullptr;

    SDCard *mMMC = nullptr;
    DebugTracing* mDT = nullptr;
	ConnectionManager *mCM = nullptr;
    Debugger *mDebugger = nullptr;

    thread mDebugThread;

	Engine *mEngine = nullptr;
	string mOutDir;
	P6502* mCPU = nullptr;

	VideoSettings mVideoSettings;
	VideoDisplayUnit* mVDU = nullptr;

public:

    GUI(Engine *engine, ALLEGRO_EVENT_QUEUE *queue, Display* display, DeviceManager * devices, VideoDisplayUnit* vdu, P6502 *cpu, double *emulationSpeed, 
		DebugTracing *dm, ConnectionManager* cm, string outDir);
    ~GUI();

	bool itemSelected(ALLEGRO_EVENT  *event);

	bool popupMenu();

	void setActualEmulationSpeed(double speed);

	void setScreenRefreshRate(double rate);

	bool running() { return mQuit; }

	void quit() { mQuit = true; }

	Debugger* getDebugger() { return mDebugger; }

	bool startDebugger();

	void updateDebuggerOptions();

};


#endif