#include "GUI.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include "Tokeniser.h"
#include "DeviceManager.h"
#include "Debugger.h"
#include <thread>
#include "Device.h"
#include <allegro5/allegro_image.h>
#include "allegro5/allegro_native_dialog.h"
#include "VideoSettings.h"
#include "Display.h"
#include "Engine.h"

using namespace std;

GUI::GUI(Engine *engine, ALLEGRO_EVENT_QUEUE* queue, ALLEGRO_DISPLAY* disp, DeviceManager * devices, double *speed, DebugManager* dm, string outDir):
    mDevices(devices), mDisplay(disp), mEmulationSpeed(speed), mDM(dm), mEngine(engine), mOutDir(outDir)
{
    // 
    // Create menu
    mMenu = al_build_menu(mMainMenu);
    if (mMenu == NULL) {
        cout << "Failed to build menu!\n";
        throw runtime_error("Failed to build menu!");
    }
    if (!al_set_display_menu(mDisplay, mMenu)) {
        cout << "Failed to set menu display!\n";
        ALLEGRO_MENU* pmenu = al_clone_menu_for_popup(mMenu);
        mPopupMenuOnly = true;
        al_destroy_menu(mMenu);
        mMenu = pmenu;
        if (!pmenu) {
            cout << "Failed to clone to popup menu!\n";
            throw runtime_error("Failed to clone to popup menu!");
        }
    }
    al_register_event_source(queue, al_get_default_menu_event_source());
    //al_register_event_source(queue, al_get_mouse_event_source());

    // Check for a Tape Recorder
    Device* dev;
    if (!mDevices->getDevice(DeviceId::TAPE_RECORDER_DEV, dev)) {
        //cout << "Failed to get access to Tape Recorder\n";
        //throw runtime_error("Failed to get access to Tape Recorder");
        al_set_menu_item_flags(mMenu, TAPE_RECORDER_ID, ALLEGRO_MENU_ITEM_DISABLED);
    }
    else {
        mTapeRec = (TapeRecorder*)dev;
    }

    // Check for an SD Card
    if (!mDevices->getDevice(DeviceId::SD_CARD, dev)) {
        //cout << "Failed to get access to MMC\n";
        //throw runtime_error("Failed to get access to MMC");
        al_set_menu_item_flags(mMenu, MMC_ID, ALLEGRO_MENU_ITEM_DISABLED);
    }
    else {
        mMMC = (SDCard*)dev;
        if (mMMC->cardInserted()) {
            al_set_menu_item_flags(mMenu, MMC_INSERT_ID, ALLEGRO_MENU_ITEM_DISABLED);
            al_set_menu_item_flags(mMenu, MMC_EJECT_ID, 0);
        }
        else {
            al_set_menu_item_flags(mMenu, MMC_INSERT_ID, 0);
            al_set_menu_item_flags(mMenu, MMC_EJECT_ID, ALLEGRO_MENU_ITEM_DISABLED);
        }
    }

}

GUI::~GUI()
{
    al_destroy_menu(mMenu);
}

bool GUI::itemSelected(ALLEGRO_EVENT* event)
{
    if (event == NULL || event->type != ALLEGRO_EVENT_MENU_CLICK)
        return false;

    switch (event->user.data1) {

    case ENTER_DBG_ID:
    {
        al_set_menu_item_flags(mMenu, ENTER_DBG_ID, ALLEGRO_MENU_ITEM_DISABLED);
        al_set_menu_item_flags(mMenu, EXIT_DBG_ID, 0);
        mDebugger = new Debugger(mEngine, mDevices, mDM, mOutDir);
        mDebugThread = thread(&Debugger::run, mDebugger);
        break;
    }

    case EXIT_DBG_ID:
    {
        al_set_menu_item_flags(mMenu, ENTER_DBG_ID, 0);
        al_set_menu_item_flags(mMenu, EXIT_DBG_ID, ALLEGRO_MENU_ITEM_DISABLED);
        mDebugThread.join();
        delete mDebugger;
        mDebugger = NULL;
        break;
    }

    case MMC_EJECT_ID:
    {
        al_set_menu_item_flags(mMenu, MMC_INSERT_ID, 0);
        al_set_menu_item_flags(mMenu, MMC_EJECT_ID, ALLEGRO_MENU_ITEM_DISABLED);

        if (mMMC != NULL && !mMMC->ejectCard()) {
            return false;
        }
        break;
    }
    case MMC_INSERT_ID:
    {
        if (mMMC != NULL && !mMMC->cardInserted()) {

            ALLEGRO_FILECHOOSER* filechooser;
            filechooser = al_create_native_file_dialog("", "Select an existing MMC image", "*.*;", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
            al_show_native_file_dialog(mDisplay, filechooser);
            int n = al_get_native_file_dialog_count(filechooser);
            if (n != 1)
                return false;
            string file = al_get_native_file_dialog_path(filechooser, 0);
            al_destroy_native_file_dialog(filechooser);

            if (!mMMC->insertCard(file)) {
                return false;
            }

            al_set_menu_item_flags(mMenu, MMC_INSERT_ID, ALLEGRO_MENU_ITEM_DISABLED);
            al_set_menu_item_flags(mMenu, MMC_EJECT_ID, 0);

        }
        break;
    }
    case SAVE_FROM_RAM:
    {
        // Select file to dump memory data into
        ALLEGRO_FILECHOOSER* filechooser;
        filechooser = al_create_native_file_dialog("", "Select  file to dump memory data to - name must be <any prefix excluding '_'>_<start adr in hex>_<end adr in hex><any suffix>", "*.*;", ALLEGRO_FILECHOOSER_SAVE);
        al_show_native_file_dialog(mDisplay, filechooser);
        int n = al_get_native_file_dialog_count(filechooser);
        if (n != 1)
            return false;
        string memory_dump_file_path_name = al_get_native_file_dialog_path(filechooser, 0);
        al_destroy_native_file_dialog(filechooser);

        // Extract data name and address info from file name
        filesystem::path file_path = memory_dump_file_path_name;
        filesystem::path file_stem = file_path.stem();
        string memory_dump_file_name = file_stem.string();
        string start_adr_s, end_adr_s;
        uint32_t start_adr, end_adr;
        string data_name;
        Tokeniser tok(memory_dump_file_name, '_');
        if (!tok.nextToken(data_name) || !tok.nextToken(start_adr_s) || !tok.nextToken(end_adr_s)) {
            cout << "Illegal format of file name '" << memory_dump_file_name << "'!\n";
            return false;
        }
        try {
            start_adr = stoi(start_adr_s, 0, 16);
            end_adr = stoi(end_adr_s, 0, 16);
        }
        catch (const invalid_argument& ia) {
            cout << "Illegal format of file name '" << memory_dump_file_name << "'!\n";
            return false;
        }
        //cout << "Start Address = 0x" << hex << start_adr << "\n";
        //cout << "End Address = 0x" << hex << end_adr << "\n";

        // Create file
        ofstream memory_dump_file(memory_dump_file_path_name, ios::out | ios::binary | ios::ate);
        if (!memory_dump_file) {
            cout << "couldn't create memory dump file '" << memory_dump_file_path_name << "!\n";
            return false;
        }

        // Write memory data to file
        uint8_t d;
        for (uint16_t a = (start_adr & 0xffff); a <= (end_adr & 0xffff); a++) {
            mDevices->dumpDeviceMemory(a, d);
            memory_dump_file << d;
        }
        memory_dump_file.close();


        break;
    }

    case LOAD_INTO_RAM:
    {
        ALLEGRO_FILECHOOSER* filechooser;
        filechooser = al_create_native_file_dialog("", "Select binary file with data to load into RAM", "*.*;", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
        al_show_native_file_dialog(mDisplay, filechooser);
        int n = al_get_native_file_dialog_count(filechooser);
        if (n != 1)
            return false;
        string program_file_name = al_get_native_file_dialog_path(filechooser, 0);
        al_destroy_native_file_dialog(filechooser);

        filesystem::path file_path = program_file_name;
        filesystem::path dir_path = file_path.parent_path();
        filesystem::path file_stem = file_path.stem();
        string file_ext = file_path.extension().string();
        filesystem::path info_file = file_stem.string() + ".inf";
        filesystem::path info_file_path = dir_path / info_file;
        string info_file_name = info_file_path.string();

        if (!filesystem::exists(info_file_path)) {
            // Try using stem + extension as the stem instead (if there are more than one '.' in a file name, the part before the first
            // '.' might be mistaken as the stem when it is actually the longer part that preceeds the last '.' that should be the stem).
            file_stem = file_stem.string() + file_ext;
            info_file = file_stem.string() + ".inf";
            info_file_path = dir_path / info_file;
            info_file_name = info_file_path.string();
            if (!filesystem::exists(info_file_path)) {
                cout << "*ERROR*\nPlease create an info file '" << info_file_name <<
                    "' with the first line starting with '<program file name without file extension> <load address in hex>' to load data into RAM!\n";
                return false;
            }
        }
        ifstream info_f(info_file_path);
        if (!info_f) {
            cout << "couldn't open INF file " << info_file_path << "\n";
            return false;
        }
        info_f.seekg(0);
        string program;
        info_f >> program;
        uint32_t load_adr;
        info_f >> hex >> load_adr;
        //cout << "Program '" << program << "' with load address 0x" << hex << load_adr << "\n";
        Program data = { program_file_name, (int)load_adr };
        if (!mDevices->loadData(data))
            return false;
        
        break;
    }

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
        al_set_menu_item_flags(mMenu, PLAY_ID, ALLEGRO_MENU_ITEM_DISABLED);
        al_set_menu_item_flags(mMenu, REWIND_ID, ALLEGRO_MENU_ITEM_DISABLED);
        al_set_menu_item_flags(mMenu, RECORD_ID, ALLEGRO_MENU_ITEM_DISABLED);
        al_set_menu_item_flags(mMenu, PAUSE_ID, ALLEGRO_MENU_ITEM_DISABLED);
        al_set_menu_item_flags(mMenu, STOP_ID, ALLEGRO_MENU_ITEM_CHECKED);
        al_set_menu_item_flags(mMenu, LOAD_TAPE_ID, 0);
        al_set_menu_item_flags(mMenu, SAVE_TAPE_ID, 0);
        if (mTapeRec->playing() || mTapeRec->recording()) {
            
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

    case SPEED_1_ID:
        if (mEmulationSpeed != NULL) {
            *mEmulationSpeed = 0.1;
            al_set_menu_item_flags(mMenu, SPEED_1_ID, ALLEGRO_MENU_ITEM_CHECKED);
            al_set_menu_item_flags(mMenu, SPEED_10_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_25_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_50_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_100_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_200_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_300_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_500_ID, 0);
        }
    break; 
    case SPEED_10_ID:
        if (mEmulationSpeed != NULL) {
            *mEmulationSpeed = 0.1;
            al_set_menu_item_flags(mMenu, SPEED_1_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_10_ID, ALLEGRO_MENU_ITEM_CHECKED);
            al_set_menu_item_flags(mMenu, SPEED_25_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_50_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_100_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_200_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_300_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_500_ID, 0);
        }
        break;

    case SPEED_25_ID:
        if (mEmulationSpeed != NULL) {
            *mEmulationSpeed = 0.25;
            al_set_menu_item_flags(mMenu, SPEED_1_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_10_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_25_ID, ALLEGRO_MENU_ITEM_CHECKED);
            al_set_menu_item_flags(mMenu, SPEED_50_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_100_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_200_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_300_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_500_ID, 0);
        }
        break;

    case SPEED_50_ID:
        if (mEmulationSpeed != NULL) {
            *mEmulationSpeed = 0.5;
            al_set_menu_item_flags(mMenu, SPEED_1_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_10_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_25_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_50_ID, ALLEGRO_MENU_ITEM_CHECKED);
            al_set_menu_item_flags(mMenu, SPEED_100_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_200_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_300_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_500_ID, 0);
        }
        break;

    case SPEED_100_ID:
        if (mEmulationSpeed != NULL) {
            *mEmulationSpeed = 1;
            al_set_menu_item_flags(mMenu, SPEED_1_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_10_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_25_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_50_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_100_ID, ALLEGRO_MENU_ITEM_CHECKED);
            al_set_menu_item_flags(mMenu, SPEED_200_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_300_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_500_ID, 0);
        }
        break;

    case SPEED_200_ID:
        if (mEmulationSpeed != NULL) {
            *mEmulationSpeed = 2;
            al_set_menu_item_flags(mMenu, SPEED_1_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_10_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_25_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_50_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_100_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_200_ID, ALLEGRO_MENU_ITEM_CHECKED);
            al_set_menu_item_flags(mMenu, SPEED_300_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_500_ID, 0);
        }
        break;

    case SPEED_300_ID:
        if (mEmulationSpeed != NULL) {
            *mEmulationSpeed = 3;
            al_set_menu_item_flags(mMenu, SPEED_1_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_10_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_25_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_50_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_100_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_200_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_300_ID, ALLEGRO_MENU_ITEM_CHECKED);
            al_set_menu_item_flags(mMenu, SPEED_500_ID, 0);
        }
        break;

    case SPEED_500_ID:
        if (mEmulationSpeed != NULL) {
            *mEmulationSpeed = 5;
            al_set_menu_item_flags(mMenu, SPEED_1_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_10_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_25_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_50_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_100_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_200_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_300_ID, 0);
            al_set_menu_item_flags(mMenu, SPEED_500_ID, ALLEGRO_MENU_ITEM_CHECKED);
        }
        break;


    default:
    {
        return false;
    }

    }

    return true;
}

bool GUI::popupMenu()
{
    if (mPopupMenuOnly) {
        if (!al_popup_menu(mMenu, mDisplay))
            cout << "Failed to launch popup menu!\n";
    }

    return true;
}