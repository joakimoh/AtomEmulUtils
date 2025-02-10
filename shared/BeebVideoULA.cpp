#include "BeebVideoULA.h"
#include "CRTC6845.h"
#include "TT5050.h"


BeebVideoULA::BeebVideoULA(
	string name, uint16_t adr, double clockSpeed, ALLEGRO_BITMAP* disp, uint16_t videoMemAdr, DebugInfo debugInfo, ConnectionManager* connectionManager
) : VideoDisplayUnit(name, BEEB_VDU_DEV, adr, 0x100, disp, videoMemAdr, debugInfo, connectionManager), mCPUClock(clockSpeed)
{
	registerPort("DISEN",		IN_PORT,	0x01, DEN,			&mDEN);
	registerPort("CURSOR",		IN_PORT,	0x01, CURSOR,		&mCURSOR);	
	registerPort("INV",			IN_PORT,	0x01, INV,			&mINV);
	registerPort("RA3",			IN_PORT,	0x01, RA3,			&mRA3);


	// Create 640 x 256 display bitmap and clear it
	mDisplayBitmap = al_create_bitmap(640, 256);

	lockDisplay();

}

BeebVideoULA::~BeebVideoULA()
{
}


// Reset device
bool BeebVideoULA::reset()
{
	return true;
}

// Advance until clock cycle stopcycle has been reached
bool BeebVideoULA::advance(uint64_t stopCycle)
{
	uint64_t cycle;
	return advanceLine(cycle);
}

bool BeebVideoULA::advanceLine(uint64_t& endCycle)
{
	int scan_lines_per_frame = (int) round(getScanLinesPerFrame());
	if (scan_lines_per_frame != mScanLines) {
		mScanLines = scan_lines_per_frame;
		mScanLine = 0;
	};
	int VS_pos = getVerticalSyncPos();
	if (VS_pos != mVerticalSyncPos) {
		mVerticalSyncPos = VS_pos;
		mScanLine = 0;
	}

	if (mCRTC == NULL || mTGC == NULL)
		return false;

	uint64_t pCycleCount = mCycleCount;
	mCycleCount += max(1, (int)round(getScanLineDuration() * mCPUClock));
	endCycle = mCycleCount;

	// Exit if the CRTC device is not initialised (a scan line of '1' will tell this)
	if (getScanLineDuration() == 1)
		return true;

	if (mScanLine == mVerticalSyncPos) {

		unlockDisplay();

		// Scale the display bitmap including borders to match the size of the display
		al_draw_scaled_bitmap(mDisplayBitmap, 0, 0, 640, 256, 0, 0, 640, 400, 0);

		// Make the updates visible on the display
		al_flip_display();

		// Clear the display
		al_clear_to_color(black);

		mFrame++;

		lockDisplay();

	}

	// Process one scan line (should be 128 chars per scan line and the visible part being 40 or 80 chars)
	int n_chars = getCharScanLines();
	for (int char_pos = 0; char_pos < n_chars; char_pos++) {

		// Advance CRTC & TGC one character (visible or not) and get character data (only used for visible char though)
		// the TGC character is only 6 pixels wdie whereas the CRTC one is 8 pixels wide!
		uint8_t crtc_data;
		vector<uint32_t> tgc_data;
		mCRTC->updateDataOutput(crtc_data);

		// The TCG's page memory input comes from the CRTC.
		// The TCG then generates 6 pixel colour data (tgc_data).
		mTGC->updateDataOutput(crtc_data, tgc_data);

		uint8_t cursor_seg_ena = 0x0;
		if (mCURSOR) {
			if (mCursorSegment < 0)
				mCursorSegment = 0;
			uint8_t cursor_segments = (mControlRegister >> 5) & 0x7;
			uint8_t cursor_seg_ena = cursor_segments >> (2 - mCursorSegment);
			bool clk_2_Mhz = ((mControlRegister >> 4) & 0x1) != 0;
			uint8_t cs_01_width = (clk_2_Mhz ? 8 : 16);
			uint8_t cs_2_width = (cs_01_width << 1) & 0x1;
			mCursorSegment++;
		}

		uint8_t dis_ena = ~(~mDEN | mRA3);
		if (dis_ena) {

			unsigned int* bitmap_data_p = (unsigned int*)((char*)mLockedDisplayBitMap->data + mLockedDisplayBitMap->pitch * mScanLine);

			uint8_t Rt, Gt, Bt;
			uint8_t Rs, Gs, Bs;
			uint8_t R, G, B;
			bool teletext = (mControlRegister & 0x2) != 0;
			uint8_t mem_data = 0x0;
			int pixels_per_char = 8;

			if (!teletext) {
				mem_data = crtc_data;
			}
			else {
				pixels_per_char = 6;
			}

			for (int big_pixel = 0; big_pixel < pixels_per_char / mPixelW; big_pixel++) {
					
				if (!teletext) {

					uint8_t palette_mem_adr = ((mem_data >> 1) & 0x1) | ((mem_data >> 2) & 0x2) | ((mem_data >>3) & 0x1) | ((mem_data >> 4) & 0x1);
					uint8_t palette_data = mPaletteMem[palette_mem_adr] & 0xf;
					uint8_t Ri = palette_data & 0x1;
					uint8_t Gi = (palette_data >> 1) & 0x1;
					uint8_t Bi = (palette_data >> 2) & 0x1;
					uint8_t F = (palette_data >> 3) & 0x1;
					uint8_t flash = mControlRegister & 0x1;

					Rs = ~(Ri ^ ~(F & flash) & dis_ena);
					Gs = ~(Gi ^ ~(F & flash) & dis_ena);
					Bs = ~(Bi ^ ~(F & flash) & dis_ena);
				}
				else {
					Rs = tgc_data[big_pixel];
					Gs = 0x0;
					Bs = 0x0;
				}

				R = Rs ^ cursor_seg_ena ^ mINV;
				G = Gs ^ cursor_seg_ena ^ mINV;
				B = Bs ^ cursor_seg_ena ^ mINV;


				// Update display with the R, G & B data
				uint32_t colour = 0xff000000 | (R?0x00ff0000:0) | (G?0x0000ff00:0) | (G?0x000000ff:0);
				for (int pw = 0; pw < mPixelW; pw++)
					*bitmap_data_p++ = colour;

				if (!teletext)
					mem_data = mem_data << 1;
				else {
					Rt = Rt << 1;
					Gt = Gt << 1;
					Bt = Bt << 1;
				}
			}

		}

	}

	mScanLine = (mScanLine + 1) % scan_lines_per_frame;

	return true;
}


bool BeebVideoULA::read(uint16_t adr, uint8_t& data)
{
	// Call parent class to trigger scheduling of other devices when applicable
	if (!VideoDisplayUnit::read(adr, data))
		return false;

	// All registers are write-only => return 0xff for read operations
	data = 0xff;

	return true;
}

bool BeebVideoULA::write(uint16_t adr, uint8_t data)
{
	// Call parent class to trigger scheduling of other devices when applicable
	if (!VideoDisplayUnit::write(adr, data))
		return false;

	uint16_t a = adr - mMemorySpace.adr;

	if (a == 0) {
		mControlRegister = data;
		if (mControlRegister & 0x10)
			CRTCClock = 2.0;
		else
			CRTCClock = 1.0;

		mNCols = 1 << ((mControlRegister >> 2) & 0x3) * 10;
		int pixel_rate = 1 << (((mControlRegister >> 2) & 0x3)+1); // pixel rate: 2, 4, 8 or 16 MHz
		mPixelW = 16 / pixel_rate; // pixel width in "actual pixels"


	}
	else if (a == 1) {
		uint8_t pa = (a >> 4) & 0xf;
		mPaletteMem[pa] = data & 0xf;
	}
	return true;
}

bool BeebVideoULA::readGraphicsMem(uint16_t adr, uint8_t& data)
{
	return true;
}

void BeebVideoULA::lockDisplay()
{
	// Save allegro state and lock display bitmap for update
	al_store_state(&mAllegroState, ALLEGRO_STATE_TARGET_BITMAP);
	al_set_target_bitmap(mDisplayBitmap);
	mLockedDisplayBitMap = al_lock_bitmap(mDisplayBitmap, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_WRITEONLY);
}

void BeebVideoULA::unlockDisplay()
{
	// Restore allegro state and unlock display for update
	al_unlock_bitmap(mDisplayBitmap);
	al_restore_state(&mAllegroState);
}

bool BeebVideoULA::getVisibleArea(int& w, int& h)
{
	w = 640;
	h = 256;
	return true;
}

double BeebVideoULA::getScanLineDuration()
{
	if (mCRTC != NULL)
		return mCRTC->getScanLineDuration();
	else
		return 64.0;
}

double BeebVideoULA::getScanLinesPerFrame()
{
	if (mCRTC != NULL)
		return mCRTC->getScanLinesPerFrame();
	else
		return 312.0;
}

double BeebVideoULA::getFrameRate()
{
	if (mCRTC != NULL)
		return mCRTC->getFrameRate();
	else
		return 0.0;
}

int BeebVideoULA::getCharScanLines()
{
	if (mCRTC != NULL)
		return mCRTC->getCharScanLines();
	else
		return 12;
}

int BeebVideoULA::getVerticalSyncPos()
{
	if (mCRTC != NULL)
		return mCRTC->getVerticalSyncPos();
	else
		return 0;
}

// Get pointer to other device to be able to call its methods
bool BeebVideoULA::connectDevice(Device* dev)
{
	Device::connectDevice(dev);

	if (dev != NULL && dev->devType == CRTC6845_DEV)
		mCRTC = (CRTC6845 *) dev;
	if (dev != NULL && dev->devType == TT_5050_DEV)
		mTGC = (TT5050 *) dev;

	return true;
}