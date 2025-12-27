#include "Display.h"



Display::Display(ALLEGRO_EVENT_QUEUE* queue, VideoFormat& videoFormat, bool EnableHwAcc, double speedFactor, DebugManager *debugManager) :
    mDM(debugManager), mSpeedFactor(speedFactor), mQueue(queue)
{

    if (!initDisplay(videoFormat, EnableHwAcc)) {
        cout << "Failed to initialise the Display!\n";
        throw runtime_error("Failed to initialise the Display!\n");
    }

    al_register_event_source(mQueue, al_get_display_event_source(mDisplay));

}

Display::~Display()
{
    al_unregister_event_source(mQueue, al_get_display_event_source(mDisplay));

    al_destroy_display(mDisplay);

    delete mVideoSettings;
    
}

const char* Display::get_format_name(int format)
{
    for (unsigned i = 0; i < NUM_FORMATS; i++) {
        if (formats[i].format == format)
            return formats[i].name;
    }
    return "unknown";
}

bool Display::initDisplay(VideoFormat& videoFormat, bool EnableHwAcc)
{
#ifdef ALLEGRO_GTK_TOPLEVEL
    //al_set_new_display_flags(ALLEGRO_RESIZABLE | ALLEGRO_GTK_TOPLEVEL | ALLEGRO_WINDOWED);
    al_set_new_display_flags(ALLEGRO_GTK_TOPLEVEL | ALLEGRO_WINDOWED);
#else
    al_set_new_display_flags(ALLEGRO_RESIZABLE);
#endif

    // Some optimisation tried to speed up the al_flip_display()  operation.
    // Doesn't seem to have any effect though...
    //al_set_new_display_option(ALLEGRO_SINGLE_BUFFER, true, ALLEGRO_REQUIRE);
    //al_set_new_display_option(ALLEGRO_VSYNC, 1, ALLEGRO_REQUIRE);

    mVideoSettings = new VideoSettings(videoFormat, EnableHwAcc);
    Resolution disp_res = mVideoSettings->getVisibleResolution();
    mDisplay = al_create_display(disp_res.width, disp_res.height);
    al_set_window_title(mDisplay, "6502 System Emulator");
    ALLEGRO_BITMAP* mDisplayBitmap = al_get_target_bitmap();
    int disp_fmt = al_get_bitmap_format(mDisplayBitmap);
    if (disp_fmt != ALLEGRO_PIXEL_FORMAT_ARGB_8888 && disp_fmt != ALLEGRO_PIXEL_FORMAT_XRGB_8888) {
        cout << "Unsupported bitmap format " << get_format_name(disp_fmt) << " (" << dec << disp_fmt << ")\n";
        return false;
    }
    int disp_bitmap_flags = al_get_bitmap_flags(mDisplayBitmap);
    //cout << "ALLEGRO_MEMORY_BITMAP = 0x" << hex << ALLEGRO_MEMORY_BITMAP << "\n";
    //cout << "ALLEGRO_NO_PRESERVE_TEXTURE = 0x" << hex << ALLEGRO_NO_PRESERVE_TEXTURE << "\n";
    //cout << "ALLEGRO_VIDEO_BITMAP = 0x" << hex << ALLEGRO_VIDEO_BITMAP << "\n";
    //cout << "Display bitmap flags = 0x" << hex << disp_bitmap_flags << "\n";
    al_set_new_bitmap_flags(ALLEGRO_NO_PRESERVE_TEXTURE); // Greatly improves performance for Windows!!!
    if (!mVideoSettings->hwAccelerationEnabled()) {
        al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
        cout << "HW graphics acceleration disabled...\n";
    }

    al_clear_to_color(al_map_rgb(0, 0, 0));

    return true;
}