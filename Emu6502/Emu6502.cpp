
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


using namespace std;

int main(int argc, const char* argv[])
{
    al_init();
    al_install_keyboard();
    al_init_primitives_addon();
    al_init_image_addon();

    ALLEGRO_TIMER* timer_60_hz = al_create_timer(1.0 / 60); // 60 Hz frequency as a base
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_DISPLAY* disp = al_create_display(648, 486); 
    

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
    ALLEGRO_BITMAP* display = al_get_target_bitmap();
    Devices devices(arg_parser.mapFileName, n_cycles_per_60_hz, display, (Keyboard *) &atom_keyboard, arg_parser.debugInfo, arg_parser.program);
   
    
    // Create processor object
    P6502 processor(arg_parser.cMHz, devices, arg_parser.debugInfo);

    al_start_timer(timer_60_hz);

    
    int cycles_step = 10;
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
    
    ALLEGRO_BITMAP* display_bitmap = al_create_bitmap(256, 192);
    ALLEGRO_BITMAP* char_row_bitmap = al_create_bitmap(8, 12);
    
   
    while (true)
    {
        al_wait_for_event(queue, &event);

        atom_keyboard.checkForKey(event);

        if (event.type == ALLEGRO_EVENT_TIMER) {

            for (int c = 0; c < n_cycles_per_60_hz; c++) {

                cycle_count += cycles_step;

                // advance time for the 6502 until clock cycle cycle_count has been reached
                processor.advance(cycle_count);

                // advance time for each device (when applicable) until clock cycle cycle_count has been reached
                for (int d = 0; d < devices.size(); d++) {
                    Device* dev = NULL;
                    if (devices.getDevice(d, dev)) {
                        dev->advance(cycle_count);
                    }
                }
                
                
            }
        }

        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            break;

        if (redraw && al_is_event_queue_empty(queue))
        {
         
            al_clear_to_color(al_map_rgb(0, 0, 0));
            int symbol = 0;
            
            // Save display area
            ALLEGRO_BITMAP* display = al_get_target_bitmap();

            // clear 256 x 192 pixel bitmap area
            al_set_target_bitmap(display_bitmap);
            al_clear_to_color(al_map_rgb(0, 0, 0));

            // Draw 16 rows with 32 characters on each row on display bitmap
            ALLEGRO_COLOR green = al_map_rgb(0, 0xff, 0);
            
             for (int row = 0; row < 16; row++) {
                for (int col = 0; col < 32; col++) {  
                    VDU6847::CharDef char_symbol = VDU6847::mCharRom[symbol];
                    symbol = (symbol + 1) % 64;               
                    al_set_target_bitmap(char_row_bitmap);
                    al_clear_to_color(al_map_rgb(0, 0, 0));
                    for (int char_pixel_row = 0; char_pixel_row < 12; char_pixel_row++) {
                        uint8_t char_row_bitmap = char_symbol.rows[char_pixel_row];
                        float y = char_pixel_row;
                        for (int char_pixel_col = 0; char_pixel_col < 8; char_pixel_col++) {
                            float x = char_pixel_col;
                            if (char_row_bitmap & 0x80) {
                                al_put_pixel(x, y, green);
                            }
                            char_row_bitmap = char_row_bitmap << 1;
                        }
                    }
                    al_set_target_bitmap(display_bitmap);
                    al_draw_bitmap(char_row_bitmap, col * 8, row * 12, 0);
                }
            }
            

            // Direct drawing back to the display
            al_set_target_bitmap(display);
            
            // Draw the 256 x 192 display bitmap while scaling it to 646 x 486
            al_draw_scaled_bitmap(display_bitmap, 0, 0, 256, 192, 0, 0, 648, 486, 0);

            // Make the updates visible on the display
            al_flip_display();

            
            redraw = false;
        }
        
    }

    /*
    al_destroy_bitmap(display_bitmap);
    al_destroy_bitmap(char_row_bitmap);
    */

    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer_60_hz);
    al_destroy_event_queue(queue);

    return 0;

}