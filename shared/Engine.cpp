#include "Engine.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <filesystem>
#include <cstdint>
#include "DebugManager.h"
#include "ConnectionManager.h"
#include "DeviceManager.h"
#include "VideoDisplayUnit.h"
#include "SoundDevice.h"
#include "P6502.h"
#include "Display.h"
#include <chrono>
using namespace std::chrono;



using namespace std;



bool Engine::allegroInit()
{
    if (!al_init()) {
        cout << "Failed to initialise allegro5\n";
        return false;
    }

    if (!al_init_native_dialog_addon()) {
        cout << "Failed to initialise allegro5 native dialog addon\n";
        return false;
    }

    if (!al_init_image_addon()) {
        cout << "Failed to initialise allegro5 image addon\n";
        return false;
    }

    if (!al_install_keyboard()) {
        cout << "Failed to initialise allegro5 keyboard\n";
        return false;
    }

    if (!al_install_mouse()) {
        cout << "Failed to initialise allegro5 mouse\n";
        return false;
    }

    if (!al_init_primitives_addon()) {
        cout << "Failed to initialise allegro5 primitives addon\n";
        return false;
    }

    if (!al_install_audio()) {
        cout << "Failed to initialise allegro5 audio addon\n";
        return false;
    }

    

    return true;
}



Engine::Engine(string mapFileName, Program& program, Program& data, double emulationSpeed, VideoFormat videoFormat, bool enableHWAcc,
    DebugManager *debugManager, string outDir, RunState initialState) : mDM(debugManager), mOutDir(outDir)
{
    if (!allegroInit()) {
        cout << "Failed to initialise allegro5!\n";
        throw runtime_error("Failed to initialise allegro!\n");
    }

    // Set emulation speed
    mSpeedFactor = emulationSpeed / 100;

    mQueue = al_create_event_queue();

    // Create display with menus
    mDisplay = new Display(mQueue, videoFormat, enableHWAcc, mSpeedFactor, mDM);
    mAllegroDisplay = mDisplay->getDisplay();
    ALLEGRO_BITMAP *allegro_display_bitmap = mDisplay->getDisplayBitmap();

    // Set the audio sampling rate to rate to the rate of 1/2 a scan line (usually 1/32us = 31.25 kHz)
    VideoSettings *video_settings = mDisplay->getVideoSettings();
    Resolution res = video_settings->getTotalResolution();
    int sample_rate = (int)round(res.height * video_settings->getFieldRate());

    // Set up devices
    ConnectionManager connection_manager(mDM);
    mDevices = new DeviceManager(
        *video_settings,
        mapFileName,
        mCPUClock,            // CPU Clock frequency in MHz
        sample_rate,          // audio sample rate corresponding to a rate of at least twice per scan line
        mAllegroDisplay, allegro_display_bitmap, video_settings->getVideoResolution(),
        mDM, program, data, connection_manager, mMicroprocessor, mVDU, mSoundDevice,
        mEmulationPeriodScheduledDevices, mHighRateScheduledDevices, mInstrScheduledDevices,
        mSpeedFactor, mLowEmulationRate, mHighEmulationRate
    );
    if (!mDM->setDevices(mDevices)) {
        cout << "Failed to initialise debug manager with device info!\n";
        throw runtime_error("Failed to initialise debug manager with device info!\n");
    }
    if (mMicroprocessor == NULL) {
        cout << "No microprocessor defined!\n";
        throw runtime_error("No microprocessor defined!");
    }
    if (!mDM->setMicrocontroller(mMicroprocessor)) {
        cout << "Failed to initialise debug manager with microconroller info!\n";
        throw runtime_error("Failed to initialise debug manager with microconroller info!\n");
    }
    // If headless emulation (i.e. no VDU), then as a default have the microprocessor halted
    if (mVDU == NULL)
        mState = ENG_HALT;
    // Else have it running
    else
        mState = ENG_RUN;
    // Unless the user has explictly specified a preferred initial mode
    if (initialState != ENG_TBD)
        mState = initialState;

    // Create GUI with menu and callbacks
    mGUI = new GUI(this, mQueue, mAllegroDisplay, mDevices, &mSpeedFactor, mDM, mOutDir);

    // Setup emulation timer
    mEmulationTimer = al_create_timer(1.0 / mLowEmulationRate / mSpeedFactor);
    al_register_event_source(mQueue, al_get_timer_event_source(mEmulationTimer));
    al_start_timer(mEmulationTimer);

    // Create mutex for debug purpose
    mutex mExecMutex;

}

Engine::~Engine()
{

    if (mEmulationTimer != NULL) {
        al_stop_timer(mEmulationTimer);
        al_unregister_event_source(mQueue, al_get_timer_event_source(mEmulationTimer));
        al_destroy_timer(mEmulationTimer);
    }
    al_uninstall_audio();

    if (mVDU != NULL)
        delete mVDU;
    if (mSoundDevice != NULL)
        delete mSoundDevice;
    if (mMicroprocessor != NULL)
        delete mMicroprocessor;
    if (mDisplay != NULL)
        delete mDisplay;
    if (mGUI != NULL)
        delete mGUI;
    if (mDevices != NULL)
        delete mDevices;
}

bool Engine::run()
{

    ALLEGRO_EVENT event;

    // Duration of one base (low rate) emulation period in CPU cycles.
    int CPU_cycles_per_low_rate_cycle = (int)round(mCPUClock * 1e6 / mLowEmulationRate);

    // Duration of one sub emulation period in CPU cycles.
    int CPU_cycles_per_high_rate_cycle = (int)round(mCPUClock * 1e6 / mHighEmulationRate);

    /*
    cout << "Timer: " << (1000 / mLowEmulationRate / mSpeedFactor) << " ms\n";
    cout << "CPU Rate:  " << mCPUClock << " MHz\n";
    cout << "Low (Base) Emulation Rate: " << mLowEmulationRate << " Hz <=> " << CPU_cycles_per_low_rate_cycle << " CPU Cycles\n";
    cout << "High Emulation Rate: " << mHighEmulationRate << " Hz <=> " << CPU_cycles_per_high_rate_cycle << " CPU Cycles\n";

    for (int i = 0; i < mEmulationPeriodScheduledDevices.size(); i++)
        cout << "Low Rate Scheduled: " << mEmulationPeriodScheduledDevices[i]->name << "\n";

    for (int i = 0; i < mHighRateScheduledDevices.size(); i++)
        cout << "High Rate Scheduled: " << mHighRateScheduledDevices[i]->name << "\n";

    for (int i = 0; i < mInstrScheduledDevices.size(); i++)
        cout << "Intruction Scheduled: " << mInstrScheduledDevices[i]->name << "\n";    
    
    int c = 0;
    uint64_t cycles_per_second = 0;
    int r = (int)mLowEmulationRate;
    auto start  = high_resolution_clock::now();
    uint64_t pCycleCount = 0;
    */

    while (!mQuit)
    {
        if (mState != ENG_HALT && mState  != ENG_BRK_DET) {

            checkForSpeedChange();

            // Advance time for each device that is scheduled on low rate (base emulation period) basis
            for (int i = 0; i < mEmulationPeriodScheduledDevices.size(); i++)
                mEmulationPeriodScheduledDevices[i]->advanceUntil(mCycleCount + CPU_cycles_per_low_rate_cycle);

            // update devices scheduled on instruction and high emulation rate basis for the low rate emulation period
            uint64_t end_cycle = mCycleCount + CPU_cycles_per_low_rate_cycle;
            while (mCycleCount < end_cycle) {

                // Acquire execution mutex
                mExecMutex.lock();

                if (mState != ENG_HALT) {

                    // Remember the return address for a potential JSR instruction
                    mCPURetAdr = mMicroprocessor->getPC() + 3;

                    // Execute one microprocessor instruction and advance time accordingly (mCycleCount updated)
                    uint64_t p_cycle_count = mCycleCount;
                    if (!mMicroprocessor->advanceInstr(mCycleCount)) {
                        // Execution stopped - exit
                        mExecMutex.unlock();
                        return false;
                    }

                    // Advance time for each device scheduled on instruction basis so that it matches the time
                    // of the microprocessor.
                    for (int d = 0; d < mInstrScheduledDevices.size(); d++)
                        mInstrScheduledDevices[d]->advanceUntil(mCycleCount);

                    // Update devices scheduled on high emulation rate basis if one high rate period has elapsed.
                    // If there (after the execution of one instruction) is a wraparound of mCycleCount % CPU_cycles_per_high_rate_cycle it means that the
                    // zero value has been reached => time to run the devices scheduled on the high rate
                    if (mCycleCount % CPU_cycles_per_high_rate_cycle < p_cycle_count % CPU_cycles_per_high_rate_cycle) {
                        for (int i = 0; i < mHighRateScheduledDevices.size(); i++)
                            mHighRateScheduledDevices[i]->advanceUntil(mCycleCount);
                    }

                    // Check whether a breakpoint has been reached or not and take action if it has been reached
                    checkForBreakPoint();
                }

                // Release execution mutex
                mExecMutex.unlock();

            }

        }

        // wait for event
        al_wait_for_event(mQueue, &event);

        /*
        cycles_per_second += mCycleCount - pCycleCount;
        pCycleCount = mCycleCount;
        if (c++ % r == 0) {
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            cout << duration << ", " << cycles_per_second << " cycles\n";
            start = high_resolution_clock::now();
            cycles_per_second = 0;
        }
        */

        bool cont = true;
        // There could be more than one event in the queue - make sure to empty it before waiting for the next timer event
        while (cont) {

            // act on event
            if (event.type == ALLEGRO_EVENT_MENU_CLICK) {
                mGUI->itemSelected(&event);
            }
            else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && event.mouse.display == mAllegroDisplay && event.mouse.button == ALLEGRO_MOUSE_BUTTON_RIGHT) {
                mGUI->popupMenu(); // popup menu but only if ribbon menu isn't supported
            }
            else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                mQuit = true;
                al_flush_event_queue(mQueue);
            }
            else if (event.type == ALLEGRO_EVENT_TIMER) {
                // The timer event comes from the emulation speed timer (defaults to 60 Hz)
                // This will synchronise the execution on 60 Hz basis (via the wait event above)

            }

            cont = al_get_next_event(mQueue, &event);

        }

        // Check for
        checkForUserInput();

    }

    return true;
}

// If a break point has been set and is triggered, then save instruction data and change the break point state
// to triggered (ENG_BRK_DET). Another thread (a Debugger thread calling setBreakPointAndWait()) will
// then stop its wait and use the saved instruction data.
void Engine::checkForBreakPoint()
{
    uint8_t read_data, written_data;
    bool read_adr_triggered = mMicroprocessor->readAdr(read_data) == mBreakAdr;
    bool written_adr_triggered = mMicroprocessor->writtenAdr(written_data) == mBreakAdr;
    uint16_t opcode_adr_triggered = mMicroprocessor->getPC() == mBreakAdr;
    bool was_JSR = mMicroprocessor->getOpcode() == P6502_JSR_OPCODE;
    if (
        mState == ENG_X_BRK_WAIT && opcode_adr_triggered
        ) {
        mState = ENG_BRK_DET;
        mReadData = 0x0;
        mWrittenData = 0x0;
        mOperandAddress = 0x0;
    }
    else if (
        read_adr_triggered && (
            mState == ENG_R_BRK_WAIT || mState == ENG_RW_BRK_WAIT ||
            (mState == ENG_R_V_BRK_WAIT || mState == ENG_RW_V_BRK_WAIT) && read_data == mReadData
            ) ||
        written_adr_triggered && (
            mState == ENG_W_BRK_WAIT || mState == ENG_RW_BRK_WAIT ||
            (mState == ENG_W_V_BRK_WAIT || mState == ENG_RW_V_BRK_WAIT) && written_data == mWrittenData
            )
        ) {
        mState = ENG_BRK_DET;
        mReadData = read_data;
        mWrittenData = written_data;
        mOperandAddress = mMicroprocessor->operandAddress();
    }

    if (mState == ENG_STEP) {
        if (mSteps == 1)
            mState = ENG_HALT;
        else
            mSteps--;
    }
    else if (mState == ENG_STEP_OVER) {
        if (was_JSR) {
            mState = ENG_WAIT_ON_RET;
            mRetAdr = mCPURetAdr;
        }
        else
            mState = ENG_HALT;
    }
    else if (mState == ENG_WAIT_ON_RET && mMicroprocessor->getPC() == mRetAdr) {
        mState = ENG_HALT;
    }

}

//
// Update emulation timer and sound timing when the emulation speed is updated
//
void Engine::checkForSpeedChange()
{
    if (mSpeedFactor != pSpeedFactor) {
        al_stop_timer(mEmulationTimer);
        al_set_timer_speed(mEmulationTimer, 1.0 / mLowEmulationRate / mSpeedFactor);
        al_start_timer(mEmulationTimer);
        if (mSoundDevice != NULL)
            mSoundDevice->setEmulationSpeed(mSpeedFactor);
    }
    pSpeedFactor = mSpeedFactor;
}

// Check for user key input
void Engine::checkForUserInput()
{
    ALLEGRO_KEYBOARD_STATE keyboard_state;

    // Get keyboard state
    al_get_keyboard_state(&keyboard_state);

    if (!mKeyPressed) {

        // Start microprocessor debugging (tracing) if user presses <CTRL-D>
        if (al_key_down(&keyboard_state, ALLEGRO_KEY_LCTRL) && al_key_down(&keyboard_state, ALLEGRO_KEY_D)) {
            toggleTracing(DBG_6502);
            mKeyPressed = true;

        }

        // Arm the delayed triggering-based debugging (tracing) if user presses <CTRL-T>
        // Debugging starts after the user has pressed <CTRl-T> and the specified triggering condition is met
        else if (al_key_down(&keyboard_state, ALLEGRO_KEY_LCTRL) && al_key_down(&keyboard_state, ALLEGRO_KEY_T)) {
            mDM->armTriggering();
            mKeyPressed = true;
        }

        // Start/stop video display unit tracing if user presses <CTRL-V>
        else if (al_key_down(&keyboard_state, ALLEGRO_KEY_LCTRL) && al_key_down(&keyboard_state, ALLEGRO_KEY_V)) {
            toggleTracing(DBG_VDU);
            mKeyPressed = true;
        }

        // Stop any ongoing continuous debugger tracing if user presses <CTRL-B>
        else if (al_key_down(&keyboard_state, ALLEGRO_KEY_LCTRL) && al_key_down(&keyboard_state, ALLEGRO_KEY_B)) {
            mDM->disableBuffering();
            mRecurringTracing = false;
            mKeyPressed = true;
        }

        // Halt executionif user presses <CTRL-H>
        else if (al_key_down(&keyboard_state, ALLEGRO_KEY_LCTRL) && al_key_down(&keyboard_state, ALLEGRO_KEY_H)) {
            mState = ENG_HALT;
            mKeyPressed = true;
        }

    }
    else {
        if (!al_key_down(&keyboard_state, ALLEGRO_KEY_LCTRL) &&
            !al_key_down(&keyboard_state, ALLEGRO_KEY_D) &&
            !al_key_down(&keyboard_state, ALLEGRO_KEY_T) &&
            !al_key_down(&keyboard_state, ALLEGRO_KEY_V) &&
            !al_key_down(&keyboard_state, ALLEGRO_KEY_B)
            ) {
            mKeyPressed = false;
        }
    }
}

bool Engine::toggleTracing(DebugLevel level)
{
    if (mDM->tracingActive()) {
        mDM->clearDebugLevel(level);
        mDM->disableTracing();
    }
    else {
        mDM->setDebugLevel(level);
        mDM->enableTracing();
    }

    return true;
}


bool Engine::halt()
{
    mExecMutex.lock();
    mState = ENG_HALT;
    mExecMutex.unlock();
    stringstream sout;
    mMicroprocessor->outputState(sout);
    cout << sout.str();
    return true;
}

bool Engine::cont()
{
    // If a breakpoint still is defined, wait for it to be triggered
    if (mBreakPoint)
        return setBreakPointAndWait();

    // Otherwise just continue execution (and wait for the user pressing <CTRL-H> to stop it

    mExecMutex.lock();
    mState = ENG_RUN;
    mExecMutex.unlock();
    bool wait = true;
    while (wait) {
        mExecMutex.lock();
        wait = (mState != ENG_HALT);
        mExecMutex.unlock();
    }

    stringstream sout;
    mMicroprocessor->outputState(sout);
    cout << sout.str();
    return true;
}

bool Engine::step(int n)
{
    return step(n, false);
}

bool Engine::step(int n, bool step_over)
{
    bool wait = true;
    mExecMutex.lock();
    mSteps = n;
    if (step_over && n == 1)
        mState = ENG_STEP_OVER;
    else if (n > 0)
        mState = ENG_STEP;
    else {
        mExecMutex.unlock();
        return false;
    }

    mExecMutex.unlock();
    while (wait) {
        mExecMutex.lock();
        wait = (mState != ENG_HALT);
        mExecMutex.unlock();
    }

    stringstream sout;
    mMicroprocessor->outputState(sout);
    cout << sout.str();

    return true;
}

bool Engine::setBreakPointAndWait(RunState mode, uint16_t adr, uint8_t& readData, uint8_t& writtenData, uint16_t& operandAddress, bool repetition)
{
    mBreakPoint = true;
    mBreakPointMode = mode;
    mBreakAdr = (int)adr;
    mRecurringTracing = repetition;
    if (setBreakPointAndWait()) {
        readData = mReadData;
        writtenData = mWrittenData;
        operandAddress = mOperandAddress;
        return true;
    }
    return false;
}

bool Engine::setBreakPointAndWait()
{   
    bool triggered = false;
    bool stopped = false;
    mExecMutex.lock();
    switch (mBreakPointMode) {
    case ENG_X_BRK_WAIT:
        mState = ENG_X_BRK_WAIT;
        break;
    case ENG_R_BRK_WAIT:
        mState = ENG_R_BRK_WAIT;
        break;
    case ENG_W_BRK_WAIT:
        mState = ENG_W_BRK_WAIT;
        break;
    case ENG_RW_BRK_WAIT:
        mState = ENG_RW_BRK_WAIT;
        break;
    case ENG_R_V_BRK_WAIT:
        mState = ENG_R_V_BRK_WAIT;
        break;
    case ENG_W_V_BRK_WAIT:
        mState = ENG_W_V_BRK_WAIT;
        break;
    case ENG_RW_V_BRK_WAIT:
        mState = ENG_RW_V_BRK_WAIT;
        break;
    default:
        mExecMutex.unlock();
        return false;
    }
    
    RunState p_state = mState;
    mExecMutex.unlock();
    while (!stopped) {
        mExecMutex.lock();
        stopped = (mState == ENG_HALT || mState == ENG_BRK_DET);
        triggered = (mState == ENG_BRK_DET);
        if (stopped) {
            if (mRecurringTracing) {
                mState = p_state;
                triggered = false;
                cout << "-----------------------------------------------------------------------------------------------------------------------------------\n";
            }
            else
                mState = ENG_HALT;
            mDM->outputBufferWindow(cout);
        }
        mExecMutex.unlock();
    } 

    stringstream sout;
    mMicroprocessor->outputState(sout);
    cout << sout.str();

    if (triggered)
        mBreakPoint = false;

    return true;
}
