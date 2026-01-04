#ifndef VIDEO_SETTINGS_H
#define VIDEO_SETTINGS_H
#include <string>

using namespace std;

enum VideoFormat { PAL_FMT, NTSC_FMT, NO_FMT };
typedef struct struct_Resolution { int width; int height; } Resolution;

class VideoSettings {

private:

	int mFieldrate = 1;
	Resolution mTotalPixels = { 0 };
	Resolution mVisiblePixels = { 0 };
	Resolution mVisiblePixelsSyncOffset = { 0 };
	Resolution mBlanking = { 0 };
	Resolution mVideoPixels = { 0 };

	bool mHwAcc = true;

	VideoFormat mFmt = PAL_FMT;

public:

	VideoSettings(VideoFormat fmt, bool hwAcc);
	VideoSettings() {};

	Resolution getVisibleResolution() { return mVisiblePixels;  }

	Resolution getVisibleOffset() { return mVisiblePixelsSyncOffset; }

	Resolution getTotalResolution() { return mTotalPixels; }

	Resolution getBlanking() { return mBlanking; }

	Resolution getVideoResolution() { return mVideoPixels; }

	string getFormat() { return (mFmt == PAL_FMT ? "PAL" : (mFmt == NTSC_FMT? "NTSC" : "Headless")); }

	bool hwAccelerationEnabled() { return mHwAcc; }

	int getFieldRate() {
		return mFieldrate;
	}

};









#endif


