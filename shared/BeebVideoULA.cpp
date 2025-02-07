#include "BeebVideoULA.h"


BeebVideoULA::BeebVideoULA(
	string name, uint16_t adr, double clockSpeed, ALLEGRO_BITMAP* disp, uint16_t videoMemAdr, DebugInfo debugInfo, ConnectionManager* connectionManager
) : VideoDisplayUnit(name, BEEB_VDU_DEV, adr, 0x100, disp, videoMemAdr, debugInfo, connectionManager), mCPUClock(clockSpeed)
{
	registerPort("DISEN",		IN_PORT,	0x01, DISEN,		&mDISEN);
	registerPort("CURSOR",		IN_PORT,	0x01, DISEN,		&mCURSOR);	
	registerPort("CRTC_DIN",	IN_PORT,	0xff, CRTC_DIN,		&mCRTC_DIN);
	registerPort("CRTC_DREQ",	OUT_PORT,	0x01, CRTC_DREQ,	&mCRTC_DREQ);
	registerPort("TCG_DREQ",	OUT_PORT,	0x01, TCG_DREQ,		&mTCG_DREQ);
	registerPort("RIN",			IN_PORT,	0xff, RIN,			&mRIN);
	registerPort("GIN",			IN_PORT,	0xff, GIN,			&mGIN);
	registerPort("BIN",			IN_PORT,	0xff, BIN,			&mBIN);
	registerPort("N_CHARS",		IN_PORT,	0xff, N_CHARS,		&mN_CHARS);
	registerPort("SL_DUR",		IN_PORT,	0xff, SL_DUR,		&mSL_DUR);
	registerPort("INV",			IN_PORT,	0x01, INV,			&mINV);
	registerPort("VS",			IN_PORT,	0x01, VS,			&mVS);
	registerPort("FR",			IN_PORT,	0xff, FR,			&mFR);
	registerPort("SL_L",		IN_PORT,	0xff, SL_L,			&mSL_L);
	registerPort("SL_H",		IN_PORT,	0xff, SL_H,			&mSL_H);


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
	mCycleCount += (int) round(mSL_DUR * mCPUClock);
	endCycle = mCycleCount;

	if (mVS && !mNewFrame) {

		mNewFrame = true;

		unlockDisplay();

		// Scale the display bitmap including borders to match the size of the display
		al_draw_scaled_bitmap(mDisplayBitmap, 0, 0, 640, 256, 0, 0, 640, 400, 0);

		// Make the updates visible on the display
		al_flip_display();

		// Clear the display
		al_clear_to_color(black);

		mFieldCount++;

		lockDisplay();

	}

	// Process one scan line (mN_CHARS should be 128 and the visible part 40 or 80 chars)
	for (int char_pos = 0; char_pos < mN_CHARS; char_pos++) {

		updatePort(CRTC_DREQ, 0x1); // Advance the CRTC one char and get visible char data when applicable (CRTC_DIN)
		updatePort(TCG_DREQ, 0x1); // Advance the TCG one char and get GRB data when applicable (RIN, GIB, GIN)

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

		if (mDISEN) {

			unsigned int* bitmap_data_p = (unsigned int*)((char*)mLockedDisplayBitMap->data + mLockedDisplayBitMap->pitch * mScanLine);

			uint8_t Rt, Gt, Bt;
			uint8_t Rs, Gs, Bs;
			uint8_t R, G, B;
			uint8_t mem_data;
			bool teletext = (mControlRegister & 0x2) != 0;

			if (!teletext) {
				mem_data = mCRTC_DIN;
			}
			else {
				// Get TCG RGB data		
				Rt = mRIN;
				Gt = mGIN;
				Bt = mBIN;
			}

			for (int big_pixel = 0; big_pixel < 8 / mPixelW; big_pixel++) {
					
				if (!teletext) {

					uint8_t palette_mem_adr = ((mem_data >> 1) & 0x1) | ((mem_data >> 2) & 0x2) | ((mem_data >>3) & 0x1) | ((mem_data >> 4) & 0x1);
					uint8_t palette_data = mPaletteMem[palette_mem_adr] & 0xf;
					uint8_t Ri = palette_data & 0x1;
					uint8_t Gi = (palette_data >> 1) & 0x1;
					uint8_t Bi = (palette_data >> 2) & 0x1;
					uint8_t F = (palette_data >> 3) & 0x1;
					uint8_t flash = mControlRegister & 0x1;

					Rs = ~(Ri ^ ~(F & flash) & mDISEN);
					Gs = ~(Gi ^ ~(F & flash) & mDISEN);
					Bs = ~(Bi ^ ~(F & flash) & mDISEN);
				}
				else {
					Rs = Rt & 0x1;
					Gs = Gt & 0x1;
					Bs = Bt & 0x1;
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

		updatePort(CRTC_DREQ, 0x0);
		updatePort(TCG_DREQ, 0x0);
	}

	return true;
}


bool BeebVideoULA::read(uint16_t adr, uint8_t& data)
{
	// All registers are write-only => return 0xff for read operations
	data = 0xff;

	return true;
}

bool BeebVideoULA::write(uint16_t adr, uint8_t data)
{
	uint16_t a = adr - mDevAdr;

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
	return (double) mSL_DUR;
}

int BeebVideoULA::getScanLinesPerFrame()
{
	return mSL_L + mSL_H * 256;
}

int BeebVideoULA::getFrameRate()
{
	return mFR;
}