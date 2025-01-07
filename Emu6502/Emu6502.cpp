
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

#include "ArgParser.h"
#include "../shared/P6502.h"
#include "../shared/Codec6502.h"
#include "../shared/AtomKeyboard.h"


using namespace std;

int main(int argc, const char* argv[])
{
    al_init();
    al_install_keyboard();

    ALLEGRO_TIMER* timer_60_hz = al_create_timer(1.0 / 60); // 60 Hz frequency as a base
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    //ALLEGRO_DISPLAY* disp = al_create_display(720, 576); // PAL screen with 576 (out of 625) visible lines of 720 (out of 768) visible 'pixels' each
    ALLEGRO_DISPLAY* disp = al_create_display(648, 486); // NTSC screen with 486 (out of 525) visible lines of 648 (out of 720?) visible 'pixels' each
    
    ALLEGRO_FONT* font = al_create_builtin_font();

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer_60_hz));

    bool redraw = true;
    ALLEGRO_EVENT event;

    al_set_window_title(disp, "6502 System Emulator");

    ArgParser arg_parser = ArgParser(argc, argv);

    if (arg_parser.failed())
        return -1;

    AtomKeyboard atom_keyboard;
    int n_cycles_per_60_hz = (int)round(arg_parser.cMHz * 1000000/ 60);
    Devices devices(arg_parser.mapFileName, n_cycles_per_60_hz, (Keyboard *) &atom_keyboard, arg_parser.debugInfo, arg_parser.program);
    
    vector<Device*> *device_list = NULL;
    if (!devices.getDevices(device_list)) {
        cout << "Failed to get a list of devices!\n";
        return -1;
    }
    
    // Create processor object
    P6502 processor(arg_parser.cMHz, devices, arg_parser.debugInfo);

    al_start_timer(timer_60_hz);

    
    int cycles_step = 10;
    uint64_t cycle_count = 0;

    while (true)
    {
        al_wait_for_event(queue, &event);

        atom_keyboard.checkForKey(event);

        if (event.type == ALLEGRO_EVENT_TIMER) {

            for (int c = 0; c < n_cycles_per_60_hz; c++) {

                cycle_count += cycles_step;

                // advance time for each device (when applicable) until clock cycle cycle_count has been reached
                for (int d = 0; d < device_list->size(); d++) {
                    Device *dev = (*device_list)[d];
                    dev->advance(cycle_count);
                }
                
            }
        }

        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            break;

        if (redraw && al_is_event_queue_empty(queue))
        {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_draw_text(font, al_map_rgb(255, 255, 255), 0, 0, 0, "Hello world!");
            al_flip_display();

            redraw = false;
        }
    }

    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer_60_hz);
    al_destroy_event_queue(queue);

    return 0;

}