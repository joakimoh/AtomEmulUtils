#ifndef VIDEO_SETTINGS_H
#define VIDEO_SETTINGS_H

enum VideoFormat { PAL_FMT, NTSC_FMT };
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

public:

	VideoSettings(VideoFormat fmt, bool hwAcc);

	Resolution getVisibleResolution() { return mVisiblePixels;  }

	Resolution getVisibleOffset() { return mVisiblePixelsSyncOffset; }

	Resolution getTotalResolution() { return mTotalPixels; }

	Resolution getBlanking() { return mBlanking; }

	Resolution getVideoResolution() { return mVideoPixels; }

	bool hwAccelerationEnabled() { return mHwAcc; }

};









#endif


