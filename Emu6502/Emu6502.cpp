
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <filesystem>
#include <cstdint>
#include <thread>
#define ALLEGRO_STATICLINK
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include "allegro5/allegro_native_dialog.h"
#include "ArgParser.h"
#include "../shared/P6502.h"
#include "../shared/Codec6502.h"
#include "../shared/VDU6847.h"
#include "../shared/GUI.h"

#include <chrono>


using namespace std;

int main(int argc, const char* argv[])
{
    al_init();
    al_install_keyboard();
    al_init_primitives_addon();
    al_init_image_addon();
    al_init_native_dialog_addon();

    ALLEGRO_TIMER* emu_speed_timer = al_create_timer(1.0 / 60); // 60 Hz frequency as default emulation speed
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    
    ALLEGRO_EVENT event;
    ALLEGRO_FONT* font = al_create_builtin_font();

    // Create display
    ALLEGRO_DISPLAY* disp = al_create_display(648, 486);
    al_set_window_title(disp, "6502 System Emulator");

    //al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(emu_speed_timer));
    al_register_event_source(queue, al_get_default_menu_event_source());

    // Create menu
    GUI gui(disp);

    ArgParser arg_parser = ArgParser(argc, argv);

    if (arg_parser.failed())
        return -1;

    int n_cycles_per_60_hz = (int)round(arg_parser.cMHz * 1000000 / 60);
    ALLEGRO_BITMAP* display = al_get_target_bitmap();

    ConnectionManager connection_manager(arg_parser.debugInfo);

    Device* vdu_device = NULL;
    vector<Device*> non_vdu_devices;
    Devices devices(
        arg_parser.mapFileName, n_cycles_per_60_hz, 1.0, display,
        arg_parser.debugInfo, arg_parser.program, arg_parser.data, connection_manager, vdu_device, non_vdu_devices

    );
    VDU6847* vdu = (VDU6847*)vdu_device;

    if (vdu == NULL) {
        cout << "No video data unit defined!\n";
        return -1;
    }
   

    al_start_timer(emu_speed_timer);

    
    int cycle_step = 2;
    uint64_t cycle_count = 0;

    // RESET all devices
    for (int d = 0; d < non_vdu_devices.size(); d++)
        non_vdu_devices[d]->reset();
    vdu->reset();
    if (arg_parser.debugInfo.dbgLevel & DBG_VERBOSE)
        cout << "All devices now reset...\n";

    al_clear_to_color(al_map_rgb(0, 0, 0));

    bool quit = false;
    while (!quit)
    {   


        // Advance time one 60 Hz NTSC field scan line at a time until a complete field (262 lines) has been scanned
        for (int scan_line = 0; scan_line < 262; scan_line++) {


            // Scan one field scan_line and save time passed in target cycle count to be used as reference
            // target time for the 6502 and the other devices (PIA, VIA, RAM & ROM). This
            // is required to keep execution synchronised with the field updating.
            uint64_t target_cycle_count;
            vdu->advanceLine(target_cycle_count);

            // Advance each device in steps (cycle_step) until it has reached a time corresponding to one scan scan_line
            while (cycle_count < target_cycle_count) {

                cycle_count += cycle_step;

                // advance time for each device until cycle_count has been reached  (or slightly passed)
                for (int d = 0; d < non_vdu_devices.size(); d++)
                    non_vdu_devices[d]->advance(cycle_count);

            }

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
            gui.itemSelected(event.user.data1);
        }

    }

    al_stop_timer(emu_speed_timer);
    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(emu_speed_timer);
    al_destroy_event_queue(queue);

    
 
    return 0;

}