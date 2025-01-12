
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

#include "ArgParser.h"
#include "../shared/P6502.h"
#include "../shared/Codec6502.h"
#include "../shared/AtomKeyboard.h"

#include "../shared/VDU6847.h"
#include "../shared/PIA8255.h"

#include <chrono>


using namespace std;

int main(int argc, const char* argv[])
{
    al_init();
    al_install_keyboard();
    al_init_primitives_addon();
    al_init_image_addon();

    ALLEGRO_TIMER* emu_speed_timer = al_create_timer(1.0 / 60); // 60 Hz frequency as default emulation speed
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_DISPLAY* disp = al_create_display(648, 486); 
    

    ALLEGRO_FONT* font = al_create_builtin_font();

    //al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(emu_speed_timer));

    ALLEGRO_EVENT event;

    al_set_window_title(disp, "6502 System Emulator");

    ArgParser arg_parser = ArgParser(argc, argv);

    if (arg_parser.failed())
        return -1;

    AtomKeyboard atom_keyboard;
    int n_cycles_per_60_hz = (int)round(arg_parser.cMHz * 1000000/ 60);
    ALLEGRO_BITMAP* display = al_get_target_bitmap();
    Devices devices(arg_parser.mapFileName, n_cycles_per_60_hz, display, (Keyboard *) &atom_keyboard, arg_parser.debugInfo, arg_parser.program);
   
    
    // Create processor object
    P6502 processor(arg_parser.cMHz, devices, arg_parser.debugInfo);

    al_start_timer(emu_speed_timer);

    
    int cycle_step = 64;
    uint64_t cycle_count = 0;

    // RESET all devices
    for (int d = 0; d < devices.size(); d++) {
        Device* dev = NULL;
        if (devices.getDevice(d, dev)) {
            dev->reset();
        }
    }
    processor.reset();
    cout << "All devices now reset...\n";

    al_clear_to_color(al_map_rgb(0, 0, 0));

    while (true)
    {
 
        cycle_count += cycle_step;

        // advance time for the 6502 until cycle_count has been reached
        processor.advance(cycle_count);

        // advance time for each device (when applicable) until cycle_count has been reached
        for (int d = 0; d < devices.size(); d++) {
            Device* dev = NULL;
            if (devices.getDevice(d, dev)) {
                dev->advance(cycle_count);
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
            break;
        }

    }

    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(emu_speed_timer);
    al_destroy_event_queue(queue);

    return 0;

}