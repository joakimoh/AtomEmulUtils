
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
#include "../shared/AtomKeyBoard.h"


using namespace std;

int main(int argc, const char* argv[])
{
    al_init();
    al_install_keyboard();

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0); // 60 Hz update frequency as a base
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    //ALLEGRO_DISPLAY* disp = al_create_display(720, 576); // PAL screen with 576 (out of 625) visible lines of 720 (out of 768) visible 'pixels' each
    ALLEGRO_DISPLAY* disp = al_create_display(648, 486); // NTSC screen with 486 (out of 525) visible lines of 648 (out of 720?) visible 'pixels' each
    
    ALLEGRO_FONT* font = al_create_builtin_font();

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    bool redraw = true;
    ALLEGRO_EVENT event;

    al_set_window_title(disp, "6502 System Emulator");

    ArgParser arg_parser = ArgParser(argc, argv);

    if (arg_parser.failed())
        return -1;

    AtomKeyBoard atom_keyboard;
    Devices devices(arg_parser.mapFileName, (KeyBoard *) &atom_keyboard, arg_parser.debugInfo, arg_parser.program);
    

    al_start_timer(timer);
    while (true)
    {
        al_wait_for_event(queue, &event);

        atom_keyboard.checkForKey(event);

        if (event.type == ALLEGRO_EVENT_TIMER)
            redraw = true;
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
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
    

    // Create processor object
    P6502 processor(arg_parser.cMHz, devices, arg_parser.debugInfo);

    // Start processor thread (run method of processor)
    std::thread processor_t(&P6502::run, &processor);

    // Wait for processor thread to complete
    processor_t.join();

    return 0;

}