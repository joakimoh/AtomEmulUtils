#include "Engine.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <filesystem>
#include <cstdint>




#include "ConnectionManager.h"
#include "Devices.h"
#include "VideoDisplayUnit.h"
#include "SoundDevice.h"
#include "P6502.h"
#include "Display.h"



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



Engine::Engine(string mapFileName, Program& program, Program& data, double emulationSpeed, VideoFormat videoFormat, bool enableHWAcc, DebugManager *debugManager) : mDM(debugManager)
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
    mDevices = new Devices(
        *video_settings,
        mapFileName,
        mCPUClock,            // CPU Clock frequency in MHz
        sample_rate,          // audio sample rate corresponding to a rate of at least twice per scan line
        mAllegroDisplay, allegro_display_bitmap, video_settings->getVideoResolution(),
        mDM, program, data, connection_manager, mMicroprocessor, mVDU, mSoundDevice,
        mFieldScheduledDevices, mHalfLineScheduledDevices, mInstrScheduledDevices,
        mSpeedFactor

    );
    if (!mDM->setDevices(mDevices)) {
        cout << "Failed to initialise debug manager with device info!\n";
        throw runtime_error("Failed to initialise debug manager with device info!\n");
    }
    if (mMicroprocessor == NULL) {
        cout << "No microprocessor defined!\n";
        throw runtime_error("No microprocessor defined!");
    }
    if (mVDU == NULL) {
        cout << "No video display unit defined!\n";
        throw runtime_error("No video display unit defined!");
    }

    // Create GUI with menu and callbacks
    mGUI = new GUI(this, mQueue, mAllegroDisplay, mDevices, &mSpeedFactor, mDM);

    // Setup emulation timer
    mfieldTimer = al_create_timer(1.0 / mFieldRate / mSpeedFactor);
    al_register_event_source(mQueue, al_get_timer_event_source(mfieldTimer));
    al_start_timer(mfieldTimer);

    // Create mutex for debug purpose
    mutex mExecMutex;

}

Engine::~Engine()
{

    if (mfieldTimer != NULL) {
        al_stop_timer(mfieldTimer);
        al_unregister_event_source(mQueue, al_get_timer_event_source(mfieldTimer));
        al_destroy_timer(mfieldTimer);
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
    ALLEGRO_KEYBOARD_STATE keyboard_state;

    int field_cnt = 0;
    bool key_pressed = false;
    int field_cycle_count = 0;
    int p_cycle_count = 0;
    double line_count = 0;
    double p_line_count = 0;
    double lines_per_field = 0;
    double p_speed_factor = mSpeedFactor;
    ALLEGRO_EVENT event;

    while (!mQuit)
    {

        if (mState != ENG_HALT) {

            int p_field_rate = mFieldRate;
            double field_rate_d = mVDU->getFieldRate();
            mFieldRate = (int)round(field_rate_d);

            //
            // Update field timer and sound timing when either the field rate or the emulation speed is updated
            //
            if (p_speed_factor != mSpeedFactor || (mFieldRate != p_field_rate && mFieldRate > 10)) {
                p_field_rate = mFieldRate;
                p_speed_factor = mSpeedFactor;
                if (mfieldTimer != NULL) {
                    al_stop_timer(mfieldTimer);
                    al_unregister_event_source(mQueue, al_get_timer_event_source(mfieldTimer));
                    al_destroy_timer(mfieldTimer);
                    mfieldTimer = NULL;
                }
                mfieldTimer = al_create_timer(1.0 / mFieldRate / mSpeedFactor);
                al_register_event_source(mQueue, al_get_timer_event_source(mfieldTimer));
                al_start_timer(mfieldTimer);

                if (mSoundDevice != NULL)
                    mSoundDevice->setFieldRate(mFieldRate, mSpeedFactor);
            }


            // Get field duration (in CPU cycles).
            // Required for the cases these parameters are not hard-coded but can be reconfigured by
            // the software.
            int cycles_per_field = (int)round(mCPUClock * 1e6 / field_rate_d);

            // Advance time for each devices that is scheduled on field basis
            for (int i = 0; i < mFieldScheduledDevices.size(); i++)
                mFieldScheduledDevices[i]->advance(mCycleCount + cycles_per_field);

            //
            // Advance time one field scan line at a time until a complete field has been scanned
            // 
            // For interlaced modes, only every second scan line will be processed...
            //
            bool end_of_field = false;
            bool add_half_line = false;
            //cout << "\n\n*** START OF FIELD\n";
            while (!end_of_field) {
                int screen_scan_line = mVDU->getScreenScanLine();
                int field = mVDU->fieldScanLineOffset();
                int active_lines = mVDU->getActiveLines();
                int adjusted_scanline = screen_scan_line - field;
                int n_screen_scan_lines = mVDU->getScreenScanLines();
                bool interlaced_mode = mVDU->interlaceOn();

                // Scan one field screen scan line and save time passed in target cycle count to be used as reference
                // target time for the 6502 and the other devices (PIA, VIA, Sound, Tape Recorder, RAM & ROM). This
                // is required to keep execution synchronised with the field updating.
                uint64_t target_cycle_count;
                uint64_t start_count = mCycleCount;

                // Advance time for the VDU corresponding to one scan line (1/2 scan line if it is the last line and interlace is enabled)
                if (interlaced_mode && add_half_line) {
                    mVDU->advanceHalfLine(target_cycle_count);
                    line_count += 0.5;
                    end_of_field = true;
                }
                else {
                    mVDU->advanceLine(target_cycle_count);
                    line_count++;
                }

                uint64_t half_line_step = (target_cycle_count - start_count) / 2;
                uint64_t start_target_cnt = start_count + half_line_step;
                for (int half_line = 0; half_line < 2; half_line++) {
                    uint64_t half_line_target = start_target_cnt + half_line_step * half_line;

                    // update devices scheduled on half line basis
                    for (int i = 0; i < mHalfLineScheduledDevices.size(); i++)
                        mHalfLineScheduledDevices[i]->advance(half_line_target);

                    // update devices scheduled on instruction basis in a tight loop
                    while (mCycleCount < half_line_target) {

                        // Acquire execution mutex
                        mExecMutex.lock();

                        if (mState != ENG_HALT && mState != ENG_BRK_DET) {

                            // Execute one microprocessor instruction and advance time accordingly (cycle_count updated)
                            if (!mMicroprocessor->advanceInstr(mCycleCount)) {
                                // Execution stopped - exit
                                return 0;
                            }

                            // Advance time for each device scheduled on instruction basis so that it matches the time
                            // of the microprocessor.
                            for (int d = 0; d < mInstrScheduledDevices.size(); d++)
                                mInstrScheduledDevices[d]->advance(mCycleCount);

                            if (mState == ENG_BRK_WAIT && mMicroprocessor->getPC() == mBreakAdr)
                                mState = ENG_BRK_DET;

                            if (mState == ENG_STEP) {
                                if (mSteps == 1)
                                    mState = ENG_HALT;
                                else
                                    mSteps--;
                            }
                        }
                        // Release execution mutex
                        mExecMutex.unlock();
                    }
                }

                // Check for end of field
                if (interlaced_mode && adjusted_scanline == n_screen_scan_lines - 2) {
                    add_half_line = true;
                }
                else if (!interlaced_mode && adjusted_scanline == n_screen_scan_lines - 2) {
                    end_of_field = true;
                }

            }


            field_cnt = (field_cnt + 1) % mFieldRate;

        }

        // wait for event
        al_wait_for_event(mQueue, &event);

        bool cont = true;
        // There could be more than one event in the queue - make sure to empty it before waiting for next timer event
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

        //
        // Check for user key input
        //

        // Get keyboard state
        al_get_keyboard_state(&keyboard_state);

        if (!key_pressed) {

            // Start microprocessor debugging (tracing) if user presses <CTRL-D>
            if (al_key_down(&keyboard_state, ALLEGRO_KEY_LCTRL) && al_key_down(&keyboard_state, ALLEGRO_KEY_D)) {
                mDM->toggleUCdebug();
                key_pressed = true;
            }

            // Enable microprocessor debugging (tracing) if user presses <CTRL-T>
            // Debugging starts after the user has pressed <CTRl-T> and the specified triggering condition is met
            else if (al_key_down(&keyboard_state, ALLEGRO_KEY_LCTRL) && al_key_down(&keyboard_state, ALLEGRO_KEY_T)) {
                mDM->toggleCondition();
                key_pressed = true;
            }

            // Start video display unit tracing if user presses <CTRL-V>
            else if (al_key_down(&keyboard_state, ALLEGRO_KEY_LCTRL) && al_key_down(&keyboard_state, ALLEGRO_KEY_V)) {
               mDM->addDebugLevel(DBG_VDU);
                key_pressed = true;
            }
        }
        else {
            if (!al_key_down(&keyboard_state, ALLEGRO_KEY_LCTRL) &&
                !al_key_down(&keyboard_state, ALLEGRO_KEY_D) &&
                !al_key_down(&keyboard_state, ALLEGRO_KEY_T) &&
                !al_key_down(&keyboard_state, ALLEGRO_KEY_V)
                ) {
                key_pressed = false;
            }
        }


        }

    return true;
}


bool Engine::halt()
{
    mExecMutex.lock();
    mState = ENG_HALT;
    mExecMutex.unlock();
    return true;
}

bool Engine::cont()
{
    mExecMutex.lock();
    mState = ENG_RUN;
    mExecMutex.unlock();
    return true;
}

bool Engine::step(int n)
{
    bool wait = true;
    mExecMutex.lock();
    mSteps = n;
    mState = ENG_STEP;
    mDM->setDebugLevel(DBG_6502);
    mExecMutex.unlock();
    while (wait) {
        mExecMutex.lock();
        wait = (mState == ENG_STEP);
        mExecMutex.unlock();
    }
    mDM->setDebugLevel(DBG_NONE);
    return true;
}

bool Engine::setBreakPointAndWait(uint16_t adr)
{
    mBreakAdr = (int)adr;
    bool triggered = false;
    mExecMutex.lock();
    mState = ENG_BRK_WAIT;
    mExecMutex.unlock();
    while (!triggered) {
        mExecMutex.lock();
        triggered = (mState == ENG_BRK_DET);
        if (triggered) {
            mState == ENG_HALT;
        }
        mExecMutex.unlock();
    } 
    return true;
}