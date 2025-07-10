#ifndef DISPLAY_H
#define DISPLAY_H

#include <allegro5/allegro_image.h>
#include "allegro5/allegro_native_dialog.h"
#include "VideoSettings.h"
#include "GUI.h"
#include "DebugManager.h"

class Display {

	/*
	*    Simple (incomplete) test of pixel format conversions.
	*
	*    This should be made comprehensive.
	*/
	typedef struct FORMAT
	{
		int format;
		char const* name;
	} FORMAT;

#define NUM_FORMATS ALLEGRO_NUM_PIXEL_FORMATS
	const FORMAT formats[ALLEGRO_NUM_PIXEL_FORMATS] = {
	   {ALLEGRO_PIXEL_FORMAT_ANY, "any"},
	   {ALLEGRO_PIXEL_FORMAT_ANY_NO_ALPHA, "no alpha"},
	   {ALLEGRO_PIXEL_FORMAT_ANY_WITH_ALPHA, "alpha"},
	   {ALLEGRO_PIXEL_FORMAT_ANY_15_NO_ALPHA, "15"},
	   {ALLEGRO_PIXEL_FORMAT_ANY_16_NO_ALPHA, "16"},
	   {ALLEGRO_PIXEL_FORMAT_ANY_16_WITH_ALPHA, "16 alpha"},
	   {ALLEGRO_PIXEL_FORMAT_ANY_24_NO_ALPHA, "24"},
	   {ALLEGRO_PIXEL_FORMAT_ANY_32_NO_ALPHA, "32"},
	   {ALLEGRO_PIXEL_FORMAT_ANY_32_WITH_ALPHA, "32 alpha"},
	   {ALLEGRO_PIXEL_FORMAT_ARGB_8888, "ARGB8888"},
	   {ALLEGRO_PIXEL_FORMAT_RGBA_8888, "RGBA8888"},
	   {ALLEGRO_PIXEL_FORMAT_ARGB_4444, "ARGB4444"},
	   {ALLEGRO_PIXEL_FORMAT_RGB_888, "RGB888"},
	   {ALLEGRO_PIXEL_FORMAT_RGB_565, "RGB565"},
	   {ALLEGRO_PIXEL_FORMAT_RGB_555, "RGB555"},
	   {ALLEGRO_PIXEL_FORMAT_RGBA_5551, "RGBA5551"},
	   {ALLEGRO_PIXEL_FORMAT_ARGB_1555, "ARGB1555"},
	   {ALLEGRO_PIXEL_FORMAT_ABGR_8888, "ABGR8888"},
	   {ALLEGRO_PIXEL_FORMAT_XBGR_8888, "XBGR8888"},
	   {ALLEGRO_PIXEL_FORMAT_BGR_888, "BGR888"},
	   {ALLEGRO_PIXEL_FORMAT_BGR_565, "BGR565"},
	   {ALLEGRO_PIXEL_FORMAT_BGR_555, "BGR555"},
	   {ALLEGRO_PIXEL_FORMAT_RGBX_8888, "RGBX8888"},
	   {ALLEGRO_PIXEL_FORMAT_XRGB_8888, "XRGB8888"},
	   {ALLEGRO_PIXEL_FORMAT_ABGR_F32, "ABGR32F"},
	   {ALLEGRO_PIXEL_FORMAT_ABGR_8888_LE, "ABGR(LE)"},
	   {ALLEGRO_PIXEL_FORMAT_RGBA_4444, "RGBA4444"}
	};

	ALLEGRO_DISPLAY* mDisplay = NULL;
	ALLEGRO_BITMAP* mDisplayBitmap = NULL;
	VideoSettings *mVideoSettings = NULL;

	ALLEGRO_EVENT_QUEUE* mQueue = NULL;

	DebugManager  *mDM = NULL;

	double mSpeedFactor = 1;


	const char* get_format_name(int format);
	bool initDisplay(VideoFormat& videoFormat, bool EnableHwAcc);

public:
	Display(ALLEGRO_EVENT_QUEUE* queue, VideoFormat& videoFormat, bool EnableHwAcc, double speedFactor, DebugManager* debugManager);
	~Display();

	VideoSettings *getVideoSettings() { return mVideoSettings; }
	ALLEGRO_DISPLAY * getDisplay() { return mDisplay;  }
	ALLEGRO_BITMAP* getDisplayBitmap() { return mDisplayBitmap; }
};

#endif