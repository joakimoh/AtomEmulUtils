#include "Engine.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <filesystem>
#include <cstdint>
#include "DebugTracing.h"
#include "ConnectionManager.h"
#include "DeviceManager.h"
#include "VideoDisplayUnit.h"
#include "SoundDevice.h"
#include "P6502.h"
#include "Display.h"
#include <chrono>
#include "KeyboardDevice.h"
#include "Debugger.h"
#include "Utility.h"
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



Engine::Engine(string mapFileName, VideoFormat videoFormat, bool enableHWAcc,
    DebugTracing *debugTracing, string outDir, RunState initialState) : mDT(debugTracing), mOutDir(outDir)
{
    if (debugTracing == nullptr)
        throw runtime_error("debug manager undefined");

    if (!allegroInit()) {
        cout << "Failed to initialise allegro5!\n";
        throw runtime_error("Failed to initialise allegro!\n");
    }

    // Set emulation speed
    mSpeedFactor = 1;

    mQueue = al_create_event_queue();

    // Create (uninitialised) display
    mDisplay = new Display(mQueue, videoFormat, enableHWAcc, mSpeedFactor, mDT);

    // Set up devices
    mConnectionManager = new ConnectionManager(mDT);
    mDeviceManager = new DeviceManager(
        mapFileName,
        mTickRate,            // CPU Clock frequency in MHz
        mDisplay,
        mDT, mConnectionManager, mMicroprocessor, mVDU, mSoundDevice, mDevices,
        mEmulationPeriodScheduledDevices, mHighRateScheduledDevices, mInstrScheduledDevices,
        mSpeedFactor, mLowEmulationRate, mHighEmulationRate
    );

    mDisplay->setTickRate(mTickRate);
    mDisplay->setCPUClockRate(mMicroprocessor->getDeviceClockRate());

    if (mMicroprocessor == NULL) {
        cout << "No microprocessor defined!\n";
        throw runtime_error("No microprocessor defined!");
    }

    // If headless emulation (i.e. no VDU), then as a default have the microprocessor halted
    if (mVDU == NULL) {
        mState = ENG_HALT;
        mDisplay->init(NO_FMT);
    }
    // Else have it running
    else
        mState = ENG_RUN;
    // Unless the user has explictly specified a preferred initial mode
    if (initialState != ENG_TBD)
        mState = initialState;

    // Create GUI with menu and callbacks
    mGUI = new GUI(this, mQueue, mDisplay, mDeviceManager, mVDU , mMicroprocessor , &mSpeedFactor, mDT, mConnectionManager, mOutDir);

    // Get debugger instance
    mDebugger = mGUI->getDebugger();

    // If headless emulation (i.e. no VDU), then start with the debugger being active
    if (mVDU == NULL)
        mGUI->startDebugger();

    // Setup emulation timer
    mEmulationTimer = al_create_timer(1.0 / mLowEmulationRate / mSpeedFactor);
    al_register_event_source(mQueue, al_get_timer_event_source(mEmulationTimer));
    al_start_timer(mEmulationTimer);
  

    // Catch keyboard events
    al_register_event_source(mQueue, al_get_keyboard_event_source());

    // Create mutex for debug purpose
    mutex mExecMutex;

    // Update GUI debugger options based on the initial debugger state
    mGUI->updateDebuggerOptions();

}

Engine::~Engine()
{
    al_unregister_event_source(mQueue, al_get_keyboard_event_source());
    if (mEmulationTimer != NULL) {
        al_stop_timer(mEmulationTimer);
        al_unregister_event_source(mQueue, al_get_timer_event_source(mEmulationTimer));
        al_destroy_timer(mEmulationTimer);
    }
    al_uninstall_audio();
 
    
    if (mGUI != nullptr)
        delete mGUI; // deletes Debugger
    
    if (mDeviceManager != nullptr)
        delete mDeviceManager; // deletes devices and device lookup tables
    
    if (mConnectionManager != nullptr)
        delete mConnectionManager;

    if (mDisplay != nullptr)
        delete mDisplay;  // deletes video settings
 
}

bool Engine::run()
{

    ALLEGRO_EVENT event;

    // Duration of one base (low rate) emulation period in CPU cycles.
    int CPU_cycles_per_low_rate_cycle = (int)round(mTickRate * 1e6 / mLowEmulationRate);

    // Duration of one sub emulation period in CPU cycles.
    int CPU_cycles_per_high_rate_cycle = (int)round(mTickRate * 1e6 / mHighEmulationRate);

    /*
    cout << "Timer: " << (1000 / mLowEmulationRate / mSpeedFactor) << " ms\n";
    cout << "CPU Rate:  " << mTickRate << " MHz\n";
    cout << "Low (Base) Emulation Rate: " << mLowEmulationRate << " Hz <=> " << CPU_cycles_per_low_rate_cycle << " CPU Cycles\n";
    cout << "High Emulation Rate: " << mHighEmulationRate << " Hz <=> " << CPU_cycles_per_high_rate_cycle << " CPU Cycles\n";

    for (int i = 0; i < mEmulationPeriodScheduledDevices.size(); i++)
        cout << "Low Rate Scheduled: " << mEmulationPeriodScheduledDevices[i]->name << "\n";

    for (int i = 0; i < mHighRateScheduledDevices.size(); i++)
        cout << "High Rate Scheduled: " << mHighRateScheduledDevices[i]->name << "\n";

    for (int i = 0; i < mInstrScheduledDevices.size(); i++)
        cout << "Intruction Scheduled: " << mInstrScheduledDevices[i]->name << "\n";    
    */
    int c = 0;
    uint64_t cycles_per_second = 0;
    int r = (int)mLowEmulationRate;
    auto start  = high_resolution_clock::now();
    uint64_t pCycleCount = 0;

    while (!mQuit)
    {

        auto start_slow_cycle = high_resolution_clock::now();

        if (_CPU_RUNNING(mState)) {

            checkForSpeedChange();

            // Advance time for each device that is scheduled on low rate (base emulation period) basis
            for (int i = 0; i < mEmulationPeriodScheduledDevices.size(); i++)
                mEmulationPeriodScheduledDevices[i]->advanceUntil(mTicks + CPU_cycles_per_low_rate_cycle);

            // update devices scheduled on instruction and high emulation rate basis for the low rate emulation period
            uint64_t end_cycle = mTicks + CPU_cycles_per_low_rate_cycle;
            while (mTicks < end_cycle) {

                // Acquire execution mutex
                mExecMutex.lock();

                if (_CPU_RUNNING(mState)) {

                    // Remember the return address for a potential JSR instruction
                    mCPURetAdr = mMicroprocessor->getPC() + 3;

                    // Execute one microprocessor instruction and advance time accordingly (mTicks updated)
                    uint64_t p_cycle_count = mTicks;
                    if (!mMicroprocessor->advanceInstr(mTicks)) {
                        // Execution stopped - exit
                        mExecMutex.unlock();
                        return false;
                    }

                    // Stop any ongoing extensive tracing
                    if (mTracingCountMax > 0) {
                        mTracingCount++;
                        if (mTracingCount >= mTracingCountMax) {
                            mDebugger->stopTracing();
                            mTracingCount = 0;
                            mTracingCountMax = -1;
                        }
                    }

                    // Advance time for each device scheduled on instruction basis so that it matches the time
                    // of the microprocessor.
                    for (int d = 0; d < mInstrScheduledDevices.size(); d++)
                        mInstrScheduledDevices[d]->advanceUntil(mTicks);

                    // Update devices scheduled on high emulation rate basis if one high rate period has elapsed.
                    // If there (after the execution of one instruction) is a wraparound of mTicks % CPU_cycles_per_high_rate_cycle it means that the
                    // zero value has been reached => time to run the devices scheduled on the high rate
                    if (mTicks % CPU_cycles_per_high_rate_cycle < p_cycle_count % CPU_cycles_per_high_rate_cycle) {
                        for (int i = 0; i < mHighRateScheduledDevices.size(); i++)
                            mHighRateScheduledDevices[i]->advanceUntil(mTicks);
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

        // measure actual speed
        cycles_per_second += mTicks - pCycleCount;
        pCycleCount = mTicks;
        if (c++ % mTimerRateInt == 0) {
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            //cout << duration << ", " << cycles_per_second << " cycles\n";
            mGUI->setActualEmulationSpeed(1e6 / duration.count() * mSpeedFactor);
            start = high_resolution_clock::now();
            cycles_per_second = 0;
        }
        
        mQuit = !mGUI->running(); 

        bool cont = true && !mQuit;
        // There could be more than one event in the queue - make sure to empty it before waiting for the next timer event
        while (cont) {

            if (event.type == ALLEGRO_EVENT_MENU_CLICK) {
                mGUI->itemSelected(&event);
            }
            else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && event.mouse.display == mDisplay->getDisplay() && event.mouse.button == ALLEGRO_MOUSE_BUTTON_RIGHT) {
                mGUI->popupMenu(); // popup menu but only if ribbon menu isn't supported
            }
            else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                mQuit = true;
                mDebugger->exit();
                mGUI->quit();
                al_flush_event_queue(mQueue);
            }
            else if (event.type == ALLEGRO_EVENT_TIMER) {
                // The timer event comes from the emulation speed timer (usually set to 50 or 60 Hz)
                // This will synchronise the execution on 50/60 Hz basis (via the wait event above)

            }

            cont = al_get_next_event(mQueue, &event);

        }

    }

    // Stop any potentially still running debugger thread
    mDebugger->exit();

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
    bool opcode_adr_triggered = mMicroprocessor->getOpcodePC() == mBreakAdr;
    bool was_JSR = mMicroprocessor->getOpcode() == P6502_JSR_OPCODE;

    if (_BRK_ANY_WAIT(mState) && mBreakWindowEnabled)
        updateLogWindow();

    if (mState == ENG_X_BRK_WAIT && opcode_adr_triggered) {
        mState = ENG_BRK_DET;
        mReadData = 0x0;
        mWrittenData = 0x0;
        mOperandAddress = 0x0;
    }
    else if (
        read_adr_triggered      && (_BRK_R_WAIT(mState) || _BRK_R_VAL_WAIT(mState) && read_data == mReadData        ) ||
        written_adr_triggered   && (_BRK_W_WAIT(mState) || _BRK_W_VAL_WAIT(mState) && written_data == mWrittenData  )
    ) {
        mState = ENG_BRK_DET;
        mReadData = read_data;
        mWrittenData = written_data;
        mOperandAddress = mMicroprocessor->operandAddress();
    }

    if (mState == ENG_STEP) {
        logInstr();
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
        mTimerRate = mLowEmulationRate * mSpeedFactor;
        mTimerRateInt = (int)round(mTimerRate);
        al_set_timer_speed(mEmulationTimer, 1.0 / mTimerRate);
        al_start_timer(mEmulationTimer);
        for (int i = 0; i < mDevices.size(); i++)
            mDevices[i]->setEmulationSpeed(mSpeedFactor);
        if (mVDU != NULL)
            mGUI->setScreenRefreshRate(mVDU->getRefreshRate());
       
    }
    pSpeedFactor = mSpeedFactor;
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

bool Engine::cont(ostream& sout)
{
    // If a breakpoint still is defined, wait for it to be triggered
    if (mBreakPoint)
        return setBreakPointAndWait(sout);

    // No breakpoint defined => just change to 'run' state
    mState = ENG_RUN;

    // Otherwise just continue execution (and wait for the user either pressing <CTRL-H> or selecting 'halt on the menu to stop it)
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
        wait = (mState != ENG_HALT) && mDebugger->waitingEnabled();
        mExecMutex.unlock();
    }

    if (!mDebugger->waitingEnabled())
        mState = ENG_RUN;

    return true;
}

bool Engine::setBreakPointAndWait(ostream& sout, RunState mode, uint16_t adr, uint8_t& readData, uint8_t& writtenData, uint16_t& operandAddress, bool repetition, bool enableTrace)
{
    mBreakWindowEnabled = enableTrace;
    mBreakPoint = true;
    mBreakPointMode = mode;
    mBreakAdr = (int)adr;
    mWrittenData = writtenData;
    mReadData = readData;
    mRecurringTracing = repetition;
    if (setBreakPointAndWait(sout)) {
        readData = mReadData;
        writtenData = mWrittenData;
        operandAddress = mOperandAddress;
        return true;
    }
    return false;
}

bool Engine::setBreakPointAndWait(ostream& sout)
{   
 
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

    bool triggered = false;
    bool stopped_exec = false;
    bool cont_waiting = true;
    while (cont_waiting) {
        mExecMutex.lock();
        stopped_exec = (mState == ENG_HALT || mState == ENG_BRK_DET);
        cont_waiting = !stopped_exec && mDebugger->waitingEnabled();
        triggered = (mState == ENG_BRK_DET);
        if (stopped_exec) {
            if (mRecurringTracing) {
                mState = p_state;
                cont_waiting = true;
                triggered = false;
                printInstrWindow(sout);
                sout << "-----------------------------------------------------------------------------------------------------------------------------------\n";
            }
            else
                mState = ENG_HALT;
        }
        mExecMutex.unlock();
    }

    if (triggered)
        mBreakPoint = false;

    if (!mDebugger->waitingEnabled())
        mState = ENG_RUN;

    return true;
}

bool Engine::clrBreakPoint()
{
    mBreakPoint = false;
    return true;
}

bool Engine::enableLogWindow(int sz)
{
    if (sz > 0 && sz < ENGINE_BUF_WINDOW_SZ) {
        clrLogWindow();
        mInstrBufferWindow.resize(sz);
        mPortBufferWindow.resize(sz);
        mSerialisedLogBuffer.resize(sz);
        mBufWinSz = sz;
        mBreakWindowEnabled = true;
        return true;
    }
    return false;
}


bool Engine::setLoggedPorts(vector<DevicePort*> loggedPorts)
{
    // Clear port buffer used in stepping
    mPortLogBuffer.clear();

    mLoggedPorts = loggedPorts;

    return true;
}

bool Engine::setLoggedDevices(vector<Device*> loggedDevices)
{
    // Clear device buffer used in stepping
    mSerialisedLogBuffer.clear();

    mLoggedDevices = loggedDevices;

    return true;
}

void Engine::disableLogWindow()
{
    clrLogWindow();
    mBreakWindowEnabled = false;
}

void Engine::clrLogWindow()
{
    mBufWindowReadIndex = mBufWindowWriteIndex = mBufferInstrSize = 0;
}

void Engine::updateLogWindow()
{
    // Get instruction
    InstrLogData instr_log_data;
    mMicroprocessor->getInstrLogData(instr_log_data);

    // Update circular instruction buffer
    mInstrBufferWindow[mBufWindowWriteIndex] = instr_log_data;

    // Update circular port value buffer
    if (mLoggedPorts.size() > 0) {
        for (int i = 0; i < mLoggedPorts.size(); i++)
            mTmpLoggedPortValues[i] = Device::getPortVal(mLoggedPorts[i]);
        mPortBufferWindow[mBufWindowWriteIndex] = mTmpLoggedPortValues;
    }

    // Update circular device state buffer
    if (mLoggedDevices.size() > 0) {
        for (int i = 0; i < mLoggedDevices.size(); i++) {
            SerialisedState mTmpSerialisedState = { 0 };
            mLoggedDevices[i]->serialiseState(mTmpSerialisedStates[i]);
        }
        mPSerialisedBufferWindow[mBufWindowWriteIndex] = mTmpSerialisedStates;
    }

    if (/*mBufWindowWriteIndex == mBufWindowReadIndex &&*/ mBufferInstrSize == mBufWinSz)
        mBufWindowReadIndex = (mBufWindowReadIndex + 1) % mBufWinSz;
    mBufWindowWriteIndex = (mBufWindowWriteIndex + 1) % mBufWinSz;
    if (mBufferInstrSize < mBufWinSz)
        mBufferInstrSize++;
}

void Engine::logInstr()
{
    InstrLogData instr_log_data;
    mMicroprocessor->getInstrLogData(instr_log_data);
    mInstrLogBuffer.push_back(instr_log_data);

    // Update port value buffer
    if (mLoggedPorts.size() > 0) {
        for (int i = 0; i < mLoggedPorts.size(); i++)
            mTmpLoggedPortValues[i] = Device::getPortVal(mLoggedPorts[i]);
        mPortLogBuffer.push_back(mTmpLoggedPortValues);
    }

    // Update device state buffer
    if (mLoggedDevices.size() > 0) {     
        for (int i = 0; i < mLoggedDevices.size(); i++) {
           SerialisedState mTmpSerialisedState = { 0 };  
           mLoggedDevices[i]->serialiseState(mTmpSerialisedStates[i]);
        }
        mSerialisedLogBuffer.push_back(mTmpSerialisedStates);     
    }
}

void Engine::printInstrWindow(ostream& sout)
{
    if (mBufferInstrSize > 0) {
        int read_pos = mBufWindowReadIndex;
        for (int i = 0; i < mBufferInstrSize; i++) {
            mMicroprocessor->printInstrLogData(sout, mInstrBufferWindow[read_pos]);
            sout << " ";
            if (mLoggedPorts.size() > 0) {
                for (int j = 0; j < mLoggedPorts.size(); j++) {
                    DevicePort* port = mLoggedPorts[j];
                    sout << port->dev->name << ":" << port->name << "=" << hex << (int)mPortBufferWindow[read_pos][j] << " ";
                }
            }
            sout << "\n";
            if (mLoggedDevices.size() > 0) {
                for (int j = 0; j < mLoggedDevices.size(); j++) {
                    Device* dev = mLoggedDevices[j];
                    sout << dev->name << ": ";
                    dev->outputSerialisedState(mPSerialisedBufferWindow[i][j], sout);
                    sout << "\n";
                }
            }
            read_pos = (read_pos + 1) % mBufWinSz;
        }
        clrLogWindow();
    }
    else {
        // Always print the last executed instruction even if no window was defined
        InstrLogData instr_log_data;
        mMicroprocessor->getInstrLogData(instr_log_data);
        mMicroprocessor->printInstrLogData(sout, instr_log_data);
        sout << "\n";
    }
}

void Engine::printInstrLog(ostream& sout)
{
    for (int i = 0; i < mInstrLogBuffer.size(); i++) {
        mMicroprocessor->printInstrLogData(sout, mInstrLogBuffer[i]);
        if (mLoggedPorts.size() > 0) {
            sout << " ";
            for (int j = 0; j < mLoggedPorts.size(); j++) {
                DevicePort* port = mLoggedPorts[j];
                sout << port->dev->name << ":" << port->name << "=" << hex << (int)mPortLogBuffer[i][j] << " ";
            }
        }       
        sout << "\n";
        if (mLoggedDevices.size() > 0) {
            for (int j = 0; j < mLoggedDevices.size(); j++) {
                Device* dev = mLoggedDevices[j];
                sout << dev->name << ": ";
                dev->outputSerialisedState(mSerialisedLogBuffer[i][j], sout);
                sout << "\n";
            }
        }
    }
    sout << "\n";
    mInstrLogBuffer.clear();
    mPortLogBuffer.clear();
}

bool Engine::reset()
{
    return mMicroprocessor->reset();
}

string Engine::getBreakPointInfo() {

    stringstream sout;

    if (!mBreakPoint)
        return "";

    switch (mBreakPointMode) {
        case ENG_X_BRK_WAIT:
            sout << "Break on execution at address 0x" << hex << mBreakAdr;
            break;
        case ENG_R_BRK_WAIT:
            sout << "Break on reading of address 0x" << hex << mBreakAdr;
            break;
        case ENG_R_V_BRK_WAIT:
            sout << "Break on reading of value 0x" << hex << (int)mReadData << " from address 0x" << mBreakAdr;
            break;
        case ENG_W_BRK_WAIT:
            sout << "Break on writing to address 0x" << hex << mBreakAdr;
            break;
        case ENG_W_V_BRK_WAIT:
            sout << "Break on writing of value 0x" << hex << (int)mWrittenData << " to address 0x" << mBreakAdr;
            break;
        case ENG_RW_BRK_WAIT:
            sout << "Break on reading or writing to/from address 0x" << mBreakAdr;
            break;
        case ENG_RW_V_BRK_WAIT:
            sout << "Break on reading or writing the value 0x" << hex << (int)mWrittenData << " to/from address 0x" << mBreakAdr;
            break;
        default:
            break;
    }
 
    return sout.str();
}
