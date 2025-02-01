
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <filesystem>
#include <cstdint>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include "allegro5/allegro_native_dialog.h"
#include "allegro5/allegro_audio.h"
#include "ArgParser.h"
#include "../shared/P6502.h"
#include "../shared/Codec6502.h"
#include "../shared/VideoDisplayUnit.h"
#include "../shared/GUI.h"

#include <chrono>
#include <cmath>


using namespace std;

ALLEGRO_MENU_INFO main_menu[] = {

      ALLEGRO_START_OF_MENU("&File", FILE_ID),
         { "&Open",        FILE_OPEN_ID,                0,                                                      NULL },
         ALLEGRO_MENU_SEPARATOR,
         { "E&xit",        FILE_EXIT_ID,                0,                                                      NULL },
         ALLEGRO_END_OF_MENU,

      ALLEGRO_START_OF_MENU("&Tape Recorder", TAPE_RECORDER_ID),
         { "&Play",             PLAY_ID,                ALLEGRO_MENU_ITEM_DISABLED | ALLEGRO_MENU_ITEM_CHECKED, NULL },
         { "&Record",           RECORD_ID,              ALLEGRO_MENU_ITEM_DISABLED | ALLEGRO_MENU_ITEM_CHECKED, NULL },
         { "Pause",             PAUSE_ID,               ALLEGRO_MENU_ITEM_DISABLED | ALLEGRO_MENU_ITEM_CHECKED, NULL },
         { "Rewind",            REWIND_ID,              ALLEGRO_MENU_ITEM_DISABLED | ALLEGRO_MENU_ITEM_CHECKED, NULL },
         { "Stop",              STOP_ID,                ALLEGRO_MENU_ITEM_DISABLED | ALLEGRO_MENU_ITEM_CHECKED, NULL },
         { "&Load from Tape",   LOAD_TAPE_ID,           0,                                                      NULL },
         { "&Save to Tape",     SAVE_TAPE_ID,           0,                                                      NULL },
         ALLEGRO_END_OF_MENU,

      ALLEGRO_START_OF_MENU("&Help", 0),
         { "&About",       HELP_ABOUT_ID,               0,                                                      NULL },
         ALLEGRO_END_OF_MENU,

      ALLEGRO_END_OF_MENU
};

double vdu_cnt = 0;
double uc_cnt = 0;
double sound_device_cnt = 0;
double keyboard_cnt = 0;
double other_dev_cnt[10] = { 0 };
int frame_cnt = 0;


int main(int argc, const char* argv[])
{
    if (!al_init())
		cout << "Failed to initialise allegro5\n";

    if (!al_init_native_dialog_addon())
		cout << "Failed to initialise allegro5 native dialog addon\n";

    if (!al_init_image_addon())
		cout << "Failed to initialise allegro5 image addon\n";

    if (!al_install_keyboard())
		cout << "Failed to initialise allegro5 keyboard\n";

	if (!al_install_mouse())
		cout << "Failed to initialise allegro5 mouse\n";

    if (!al_init_primitives_addon())
		cout << "Failed to initialise allegro5 primitives addon\n";

    if (!al_install_audio())
		cout << "Failed to initialise allegro5 audio addon\n";

    ALLEGRO_TIMER* emu_speed_timer = al_create_timer(1.0 / 60); // 60 Hz frequency as default emulation speed
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    
    ALLEGRO_EVENT event;
    ALLEGRO_FONT* font = al_create_builtin_font();
	ALLEGRO_MENU *pmenu = NULL;

#ifdef ALLEGRO_GTK_TOPLEVEL
   al_set_new_display_flags(ALLEGRO_RESIZABLE | ALLEGRO_GTK_TOPLEVEL);
#else
   al_set_new_display_flags(ALLEGRO_RESIZABLE);
#endif

    ALLEGRO_DISPLAY* disp = al_create_display(648, 486);
    al_set_window_title(disp, "6502 System Emulator");
    ALLEGRO_BITMAP* disp_bm = al_get_target_bitmap();

    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(emu_speed_timer));
    al_register_event_source(queue, al_get_default_menu_event_source());
	al_register_event_source(queue, al_get_mouse_event_source());


    ArgParser arg_parser = ArgParser(argc, argv);

    if (arg_parser.failed())
        return -1; 
    

    ConnectionManager connection_manager(arg_parser.debugInfo);

    VideoDisplayUnit* vdu = NULL;
    Device* sound_device = NULL;
    vector<Device*> other_devices;
    P6502 * microprocessor = NULL;
    Device* keyboard = NULL;
    Devices devices(
        arg_parser.mapFileName,
        arg_parser.cMHz,                        // CPU Clock frequency in MHz
        32000,                      // audio sample rate corresponding to a rate of at least twice per scan line
        disp_bm,
        arg_parser.debugInfo, arg_parser.program, arg_parser.data, connection_manager, microprocessor, vdu, sound_device, keyboard, other_devices

    );

    if (microprocessor == NULL) {
        cout << "No microprocessor defined!\n";
        return -1;
    }

    if (vdu == NULL) {
        cout << "No video display unit defined!\n";
        return -1;
    }
    int n_scan_lines = vdu->getScanLinesPerFrame();

    if (keyboard == NULL) {
        cout << "No keyboard defined!\n";
        return -1;
    }

    // Create menu
    ALLEGRO_MENU * menu = al_build_menu(main_menu);
    if (menu == NULL ) {
        cout << "Failed to build menu!\n";
        //return -1;
    }
    if (!al_set_display_menu(disp, menu)) {
        cout << "Failed to set menu display!\n";
      pmenu = al_clone_menu_for_popup(menu);
      al_destroy_menu(menu);
      menu = pmenu;
		if (!pmenu)
        	cout << "Failed to clone to popup menu!\n";
        //return -1;
    }
    GUI gui(disp, menu, &devices);
   

    al_start_timer(emu_speed_timer);

    
    int cycle_step = 2;
    uint64_t cycle_count = 0;

    // RESET all devices
    for (int d = 0; d < other_devices.size(); d++)
        other_devices[d]->reset();
    sound_device->reset();
    vdu->reset();
    keyboard->reset();
    microprocessor->reset();   
    if (arg_parser.debugInfo.dbgLevel & DBG_VERBOSE)
        cout << "All devices now reset...\n";

    al_clear_to_color(al_map_rgb(0, 0, 0));

    bool quit = false;
    while (!quit)
    {   

        // Advance time one field scan line at a time until a complete field has been scanned
        for (int scan_line = 0; scan_line < n_scan_lines; scan_line++) {

            // Scan one field scan_line and save time passed in target cycle count to be used as reference
            // target time for the 6502 and the other devices (PIA, VIA, Sound, Tape Recorder, RAM & ROM). This
            // is required to keep execution synchronised with the field updating.
            uint64_t target_cycle_count;
            uint64_t start_count = cycle_count;

            auto vdu_start = chrono::high_resolution_clock::now();

            vdu->advanceLine(target_cycle_count);
            //cout << vdu->name << " cycle count: " << dec << vdu->getCycleCount() << " (" << cycle_count << ")\n";

            auto vdu_stop = chrono::high_resolution_clock::now();
            auto vdu_dur = chrono::duration_cast<chrono::microseconds>(vdu_stop - vdu_start);
            vdu_cnt += vdu_dur.count();

            uint64_t half_line_step = (target_cycle_count - start_count)  / 2;
            uint64_t start_target_cnt = start_count + half_line_step;
            for (int half_line = 0; half_line < 2; half_line++) {

                uint64_t half_line_target = start_target_cnt + half_line_step * half_line;

                auto sound_device_start = chrono::high_resolution_clock::now();

                // update sound device twice per scan line <=> 31.5 kHz for NTSC
                sound_device->advance(half_line_target);
                //cout << sound_device->name << " cycle count: " << dec << sound_device->getCycleCount() << " (" << cycle_count << ")\n";

                auto sound_device_stop = chrono::high_resolution_clock::now();
                auto sound_device_dur = chrono::duration_cast<chrono::microseconds>(sound_device_stop - sound_device_start);
                sound_device_cnt += sound_device_dur.count();

                // Advance each device in steps (cycle_step) until it has reached a time corresponding to one scan scan_line
                while (cycle_count < half_line_target) {

                    cycle_count += cycle_step;
                 
                    auto uc_start = chrono::high_resolution_clock::now();

                    // advance time for the microprocessor
                    microprocessor->advance(cycle_count);
                    //cout << "uC cycle count: " << dec << microprocessor->getCycleCount() << " (" << cycle_count << ")\n";

                    auto uc_stop = chrono::high_resolution_clock::now();
                    auto uc_dur = chrono::duration_cast<chrono::microseconds>(uc_stop - uc_start);
                    uc_cnt += uc_dur.count();

                    // Advance time for the other devices until cycle_count has been reached  (or slightly passed)
                    for (int d = 0; d < other_devices.size(); d++) {

                        auto other_dev_start = chrono::high_resolution_clock::now();

                        // advance time for the device
                        other_devices[d]->advance(cycle_count);
                        //cout << other_devices[d]->name << " cycle count: " << dec << other_devices[d]->getCycleCount() << " (" << cycle_count << ")\n";

                        auto other_dev_stop = chrono::high_resolution_clock::now();
                        auto other_dev_dur = chrono::duration_cast<chrono::microseconds>(other_dev_stop - other_dev_start);
                        other_dev_cnt[d] += other_dev_dur.count();
                      
                    }
                    

                }
            }

        }

        frame_cnt = (frame_cnt + 1) % 60;
        if (false && frame_cnt == 0) {
            cout << "VDU ms per sec: " << vdu_cnt / 1000 << "\n";
            cout << "Sound ms per sec: " << sound_device_cnt / 1000<< "\n";
            cout << "microcontroller ms per sec: " << uc_cnt / 1000 << "\n";
            for (int d = 0; d < other_devices.size(); d++) {
                cout << other_devices[d]->name << " ms per per sec: " << other_dev_cnt[d] / 1000<< "\n";
                other_dev_cnt[d] = 0;
            }
            vdu_cnt = 0;
            sound_device_cnt = 0;
            uc_cnt = 0;
            cout << "\n\n";
        }


        // wait for event
        al_wait_for_event(queue, &event);

        // act on event
        if (event.type == ALLEGRO_EVENT_TIMER) {
            // The timer event comes from the emulation speed timer (defaults to 60 Hz)
            // This will synchronise the execution on 60 Hz basis (via the wait event above)

        } 
        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            quit = true;
            al_flush_event_queue(queue);
        }
        else if (event.type == ALLEGRO_EVENT_MENU_CLICK) {
            gui.itemSelected(&event);
        }
		else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && event.mouse.display == disp && event.mouse.button == ALLEGRO_MOUSE_BUTTON_RIGHT) {
            if (pmenu) {
                if (!al_popup_menu(menu, disp))
                    cout << "Failed to launch popup menu!\n";
			}
		}

    }

    al_stop_timer(emu_speed_timer);
    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(emu_speed_timer);
    al_destroy_event_queue(queue);

    al_destroy_menu(menu);

    al_uninstall_audio();

    
 
    return 0;

}
