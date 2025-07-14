#ifndef ENGINE_H
#define ENGINE_H


#include <iostream>
#include "../shared/DebugManager.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include "allegro5/allegro_audio.h"
#include <allegro5/allegro_image.h>
#include "allegro5/allegro_native_dialog.h"
#include "VideoSettings.h"
#include "DebugManager.h"
#include "Display.h"
#include <mutex>
#include <semaphore>
#include <thread>

class SoundDevice;

using namespace std;

class Engine {



private:

	Display* mDisplay = NULL;

	double mSpeedFactor = 1;

	uint64_t mCycleCount = 0;
	bool mQuit = false;
	int mFieldRate = 50;

	Devices* mDevices = NULL;

	VideoDisplayUnit* mVDU = NULL;
	SoundDevice* mSoundDevice = NULL;
	vector<Device*> mFieldScheduledDevices, mHalfLineScheduledDevices, mInstrScheduledDevices;
	P6502* mMicroprocessor = NULL;
	double mCPUClock = 1.0;

	ALLEGRO_DISPLAY* mAllegroDisplay = NULL;
	ALLEGRO_EVENT_QUEUE* mQueue = NULL;
	ALLEGRO_TIMER* mfieldTimer = NULL;

	DebugManager *mDM = NULL;
	bool mRecurringTracing = false;


	GUI *mGUI = NULL;

	// Debugger state
	enum RunState {ENG_RUN, ENG_HALT, ENG_STEP, ENG_X_BRK_WAIT, ENG_R_BRK_WAIT, ENG_W_BRK_WAIT, ENG_RW_BRK_WAIT, ENG_BRK_DET};
	RunState mState = ENG_RUN;
	int mSteps = 0;
	int mBreakAdr = -1;
	uint8_t mReadData = 0xff;
	uint8_t mWrittenData = 0xff;
	uint16_t mOperandAddress = 0xffff;

	// Create mutex for debug purpose
	mutex mExecMutex;

	bool allegroInit();

	bool toggleTracing(DebugLevel level);

public:

	Engine(string mapFileName, Program &program, Program &data, double emulationSpeed, VideoFormat videoFormat, bool enableHWAcc, DebugManager *debugManager);
	~Engine();

	bool run();

	bool halt();

	bool cont();

	bool step(int n);

	bool setBreakPointAndWait(int mode, uint16_t adr, uint8_t &readData, uint8_t &writtenData, uint16_t &operandAdr, bool repetition);
};


#endif