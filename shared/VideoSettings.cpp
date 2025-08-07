#include "VideoSettings.h"
#include <iostream>
#include <string>
#include <cmath>

using namespace std;

VideoSettings::VideoSettings(VideoFormat fmt, bool hwAcc): mHwAcc(hwAcc)
{
	int screen_scan_lines;
	int total_hz_pixels;
	switch (fmt) {
	case PAL_FMT:
		screen_scan_lines = 625;
		mFieldrate = 50;
		break;
	case NTSC_FMT:
		screen_scan_lines = 525;
		mFieldrate = 60;
		break;
	default:
		throw runtime_error("Illegal format '" + to_string((int)fmt));
		break;
	}

	// Calculate vertical borders
	double vt_blanking = 0.08 * screen_scan_lines;							// PAL: 50 lines, NTSC: 42
	double vt_video_content = screen_scan_lines - vt_blanking;				// PAL: 575 lines, NTSC: 482 lines
	double vt_visible_content = 0.9 * screen_scan_lines;					// PAL: 562.5 lines, NTSC: 472.5 lines (90% visible on a CRTC based on observation)
	double vt_visible_offset = (vt_video_content - vt_visible_content) / 2;	// PAL: 6.25 lines, NTSC: 4.75 lines
	int vt_visible_sync_offset = (int)round(vt_blanking + vt_visible_offset + screen_scan_lines) % screen_scan_lines;

	// Calculate total resolution while preserving an aspect ratio of 4:3 and with the assumption that 73% out of the line duration is visible 
	double hz_visible_content = vt_visible_content * 4 / 3;					// PAL: 750 pixels, NTSC: 630 pixels
	total_hz_pixels = (int) round(hz_visible_content / 0.73);				// PAL: 1027 pixels, NTSC: 863 pixels

	// Calculate horizontal borders
	double hz_front_porch_spec = 0.0258 * total_hz_pixels;					// PAL:26.5 pixels, NTSC: 22.3 pixels
	double hz_back_porch_spec = 0.0891 * total_hz_pixels;					// PAL: 91.5 pixels, NTSC: 76.9 pixels
	double hz_sync_width_spec = 0.0734 * total_hz_pixels;					// PAL: 75.4 pixels, NTSC: 63.3 pixels
	double hz_blanking_spec = 0.1883 * total_hz_pixels;						// PAL: 193.5 pixels, NTSC: 162.5 pixels
	double hz_video_content = total_hz_pixels - hz_blanking_spec;

	double hz_visible_offset = (hz_video_content - hz_visible_content) / 2;
	int hz_visible_sync_offset = (int)round(hz_sync_width_spec + hz_back_porch_spec + hz_visible_offset + total_hz_pixels) % total_hz_pixels;

	// Define pixels resolution and visible offsets
	mTotalPixels = { total_hz_pixels, screen_scan_lines }; // This is the line & field durations expressed in "pixels"
	mVisiblePixelsSyncOffset = { hz_visible_sync_offset, vt_visible_sync_offset };
	mVisiblePixels = { (int)round(hz_visible_content),  (int)round(vt_visible_content)}; // This is the visible part of the line & field
	mBlanking = { (int) round(hz_blanking_spec) , (int) round(vt_blanking) };
	mVideoPixels = { (int)round(hz_video_content) , (int)round(vt_video_content) }; // This is the video part of the line & field

}
