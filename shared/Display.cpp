#include "Display.h"
#include <cmath>
#include <sstream>
#include <iomanip>
#include "Utility.h"



Display::Display(ALLEGRO_EVENT_QUEUE* queue, VideoFormat& videoFormat, bool EnableHwAcc, double speedFactor, DebugTracing *debugTracing) :
    mDM(debugTracing), mSpeedFactor(speedFactor), mQueue(queue), mHwAcc(EnableHwAcc), mVideoFmt(videoFormat)
{

}


Display::~Display()
{
    al_unregister_event_source(mQueue, al_get_display_event_source(mDisplay));
    al_destroy_display(mDisplay);
    
}


const char* Display::get_format_name(int format)
{
    for (unsigned i = 0; i < NUM_FORMATS; i++) {
        if (formats[i].format == format)
            return formats[i].name;
    }
    return "unknown";
}

bool Display::init()
{
    return init(mVideoFmt);
}


bool Display::init(VideoFormat videoFormat)
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

    mVideoSettings = VideoSettings(videoFormat, mHwAcc);
    Resolution disp_res = mVideoSettings.getVisibleResolution();
    mDisplay = al_create_display(disp_res.width, disp_res.height);
    al_set_window_title(mDisplay, M_WINDOW_TITLE.c_str());
    mDisplayBitmap = al_get_target_bitmap();
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
    if (!mVideoSettings.hwAccelerationEnabled()) {
        al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP);
        cout << "HW graphics acceleration disabled...\n";
    }

    al_clear_to_color(al_map_rgb(0, 0, 0));

    al_register_event_source(mQueue, al_get_display_event_source(mDisplay));

    return true;
}


bool Display::getClipboard()
{
    char* clipboard_content = al_get_clipboard_text(mDisplay);

    if (clipboard_content == nullptr)
        return false;

    stringstream clipboard(clipboard_content);
    al_free(clipboard_content);
    bool stop = false;

	// Clear previous clipboard content
    mClipboard.clear();
    mClipBoardRow = mClipBoardCol = 0;
    mNewLineCharPos = -1;

    int row = 0;
    while (!stop) {
        string line, l;
        if (clipboard.eof())
            break;
        getline(clipboard, line);
        for (int i = 0; i < line.size(); i++) {
            if (line[i] >= 32 && line[i] <= 126)
                l += line[i];
            else if (line[i] == '\n' || line[i] == '\r')
                break;
        }
        mClipboard.push_back(l);
    }

    return true;
}


bool Display::nextClipboardChar(char& c)
{
    // Make sure c always get a value
    c = 0;

	// Check if we are in the middle of outputting the new line characters
    if (mNewLineCharPos >= 0) {
        c = mNewLineChars[mNewLineCharPos++];
        if (mNewLineCharPos == mNewLineChars.size())
            mNewLineCharPos = -1;
        return true;
	}

    // Check for end of line
    if (mClipBoardCol == mClipboard[mClipBoardRow].size()) {
        mClipBoardCol = 0;
        mClipBoardRow++;
        mNewLineCharPos = 0;      
		return nextClipboardChar(c); // Output the first new line character
	}

    // Check for last line
    if (mClipBoardRow >= mClipboard.size() || mClipBoardCol >= mClipboard[mClipBoardRow].size())
        return false;

	// Output the next character in the current line
	c = mClipboard[mClipBoardRow][mClipBoardCol++];
 
	return true;
}


void Display::updateWindowTitle()
{
    Resolution res = mVideoSettings.getTotalResolution();
    int tick_int = (int)trunc(mTickRate);
    int tick_fra = (int)trunc((mTickRate - tick_int)*10);
    int clock_int = (int)trunc(mCPUClockRate);
    int clock_fra = (int)trunc((mCPUClockRate - clock_int) * 10);
    
    string speed_s;
    if (mMeasuredSpeed < 1)
        speed_s = to_string((int)round(mMeasuredSpeed * 100)) + "%";
    else {
        int speed_int = (int)trunc(mMeasuredSpeed);
        int speed_fra = (int)trunc((mMeasuredSpeed - speed_int) * 100);
        stringstream speed_ss;
        speed_ss << speed_int << "." << setw(2) << setfill('0') << dec << speed_fra;
        speed_s = speed_ss.str();
    }
    if (mVideoSettings.format() == VideoFormat::NO_FMT)
        al_set_window_title(
            mDisplay,
            (
                M_WINDOW_TITLE +
                " TICK RATE " + to_string(tick_int) + "." + to_string(tick_fra)  + " Mhz" +
                " CPU CLOCK " + to_string(clock_int) + "." + to_string(clock_fra) + " Mhz" +
                " SPEED " + speed_s +
                " " + mVideoSettings.getFormat() + " " + to_string(res.width) + "x" + to_string(res.height)
                ).c_str()
        );
    else
        al_set_window_title(
            mDisplay,
            (
                M_WINDOW_TITLE +
                " TICK RATE " + to_string(tick_int) + "." + to_string(tick_fra) + " Mhz" + +
                " CPU CLOCK " + to_string(clock_int) + "." + to_string(clock_fra) + " Mhz" +
                " SPEED " + speed_s +
                " FPS " + to_string((int)round(mMeasuredFrameRate)) +
                " " + mVideoSettings.getFormat() + " " + to_string(res.width) + "x" + to_string(res.height)
            ).c_str()   
        );
}


void Display::setTickRate(double tickRate)
{
    mTickRate = tickRate;

}


void Display::setCPUClockRate(double clockRate)
{
    mCPUClockRate = clockRate;
}