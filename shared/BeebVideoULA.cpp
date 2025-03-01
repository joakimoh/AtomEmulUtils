#include "BeebVideoULA.h"
#include "CRTC6845.h"
#include "TT5050.h"
#include <iomanip>

BeebVideoULA::BeebVideoULA(
	string name, uint16_t adr, double clockSpeed, ALLEGRO_BITMAP* disp, int dispW, int dispH, DebugInfo  *debugInfo, ConnectionManager* connectionManager
) : VideoDisplayUnit(name, BEEB_VDU_DEV, adr, 0x10, disp, dispW, dispH, 0x0 /* dummy adr */, debugInfo, connectionManager), mCPUClock(clockSpeed)
{
	registerPort("DISEN",		IN_PORT,	0x01, DISPTMG,		&mDISPTMG);
	registerPort("CURSOR",		IN_PORT,	0x01, CURSOR,		&mCURSOR);	
	registerPort("INV",			IN_PORT,	0x01, INV,			&mINV);
	registerPort("RA",			IN_PORT,	0x0f, RA,			&mRA);
	registerPort("CRTC_CLK",	OUT_PORT,	0x01, CRTC_CLK,		&mCRTC_CLK);


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

	Device::reset();

	mScanLine = 0;

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

	if (!initialised()) { 
		mCycleCount += max(1, (int)round(getScanLineDuration() * mCPUClock));
		endCycle = mCycleCount;
		return true; 
	}

	int scan_lines_per_frame = (int) round(getScanLinesPerFrame());
	if (scan_lines_per_frame != mScanLines) {
		mScanLines = scan_lines_per_frame;
		mScanLine = 0;

	}
	int chars_per_line = getCharsPerLine();
	int visible_chars = getLeftBorderChars() + getActiveChars() + getRightBorderChars();
	int active_chars = getActiveChars();
	int top_border_lines = getTopBorderLines();
	int active_lines = getActiveLines();
	int bottom_border_lines = getBottomBorderLines();
	int left_border_chars = getLeftBorderChars();
	int right_border_chars = getRightBorderChars();
	int VS_pos = getVerticalSyncLine();
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
	if (!mCRTC->initialised())
		return true;


	// Check that screen size fits with the current graphics mode
	updateScreenSz();

	if (mScanLine == mVerticalSyncPos) {

		unlockDisplay();

		//cout << "scale " << mScreenW << "x" << mScreenH << " to " << mDisplayWidth << "x" << mDisplayHeight << "\n";

		// Scale the display bitmap including borders to match the size of the display
		al_draw_scaled_bitmap(mDisplayBitmap, 0, 0, mScreenW, mScreenH, 0, 0, mDisplayWidth, mDisplayHeight, 0);

		// Make the updates visible on the display
		al_flip_display();

		// Clear the display
		al_clear_to_color(black);

		//cout << dec << mFrame << ":" << (getCRField(CR_TELETEXT) == 1 ? "teletext" : "Graphics") << ":" << mScreenW << "x" << mScreenH << "\n";

		lockDisplay();

	}

	// Process one scan line (should be 64/128 chars per scan line and the visible part being 20, 40 or 80 chars)
	// Each char is not necessarily the same as character on the screen. "Big Char" is the char visible
	// on the screen and "Char" is actually one byte fetched from memory that only sometimes corresponds
	// to width of a screen char. 

	unsigned int* bitmap_data_p = NULL;
	if (mScanLine == 0)
		bitmap_data_p = (unsigned int*)((char*)mLockedDisplayBitMap->data);
	else
		bitmap_data_p = (unsigned int*)((char*)mLockedDisplayBitMap->data + mLockedDisplayBitMap->pitch * (mScanLine + top_border_lines));

	int pixels_per_byte = mPixelsPerByte;
	int pixel_width = mPixelW;
	bool teletext = getCRField(CR_TELETEXT) == 1;
	if (teletext) {
		pixels_per_byte = 12;
		pixel_width = 1;// mPixelW;
	}

	if (mScanLine == 0)
		// Add Top border
	{
		for (int line = 0; line < top_border_lines; line++) {
			for (int char_pos = 0; char_pos < visible_chars; char_pos++) {
				uint8_t Rs, Gs, Bs;
				for (int big_pixel = 0; big_pixel < pixels_per_byte; big_pixel++) {
					Rs = Gs = Bs = 0;
					uint32_t colour = 0xff00ff00;
					for (int pw = 0; pw < mPixelW && bitmap_data_p != NULL; pw++)
						*bitmap_data_p++ = colour;
				}
			}
		}
	}

	for (int char_pos = 0; char_pos < chars_per_line; char_pos++) {

		// Advance CRTC & TGC one character (visible or not) and get character data (only used for visible char though)
		// the TGC character is only 6 pixels wide whereas the CRTC one is 8 pixels wide!
		// The M6847 R12/R13 value is set as follows:
		// MODE 0 - 6:	screen address / 8 <=> 0,1,2: 0x3000 => 0x600, 3: 0x4000 => 0x800, 4-5: 0xb00, 6: 0x6000 => 0xc00
		// MODE 7:		screen address - (0x7400 ^ 0x2000) => 0x2800 for actual memory address of 0x7c00
		// The same logic applies to setting R14/R15 (cursor position)
		//
		// Mode		CRTC address		Actual Address		Size	Resolution	Big Chars	Colours		Lines		Visible Chars		Pixel W	Pixel R
		// 0		 0x600 -  0xfff		0x3000 - 0x7fff		20k		640x256		80x32		2			312			256		128			1		16 MHz
		// 1		 0x600 -  0xfff		0x3000 - 0x7fff		20k		320x256		40x32		4			312			256		128			2		 8 MHz
		// 2		 0x600 -  0xfff		0x3000 - 0x7fff		20k		160x256		20x32		16			312			256		128			4		 4 MHz
		// 3		 0x800 -  0xfff		0x4000 - 0x7fff		16k		N/A			80x25		2			312			256		64			(1)		16 MHz
		// 4		 0xb00 -  0xfff		0x5800 - 0x7fff		10k		320x256		40x32		2			312			256		64			2		 8 MHz
		// 5		 0xb00 -  0xfff		0x5800 - 0x7fff		10k		160x256		20x32		4			312			256		64			4		 4 MHz
		// 6		 0xc00 -  0xfff		0x6000 - 0x7fff		8k		N/A			40x25		2			312			256		64			(2)		 8 MHz
		// 7		0x2800 - 0x2c00		0x7c00 - 0x7fff		1k		78x75		40x25		8			572			513		64 (768)	(2)		 8 MHz
		//															(468x500) (240x250)
		//																	  (480x500)
		// 
		// Each n x 8(10) pxiel block in modes 0-6 are organised so that each row (0,1,..7(9)) of the n pixel are stored
		// in consecutive memory locations (row 0: a, row 1: a+1,... row 7: a+7). For two-colour modes n = 8 pixels,
		// for 4-colour modes it is 4 pixels and for 16-colour modes it is 2 pixels. That is the reason the screen
		// address above is divided by 8. The raster address will then select each of the 8(10) rows to cover the complete
		// block of n x 8(10) pixels. The raster rows are 8 for modes 0-2,4 & 5 (graphics & char modes) and 10 for
		// modes 3 & 6 (char only modes). The lowest memory address (e.g. 0x5800 for mode 4) will correspond to the
		// upper left corner of the screen.
		//
		// For mode 7 (teletext mode) only complete characters (or graphical symbols) are read at a time and the raster
		// address selects the row of each character (the same screen memory data is present for all raster  rows).
		// There are for mode 7, 19 raster rows.
		// The TCG's page memory input comes from the CRTC (0x7c00 to 0x7fff)
		// The TCG then generates either 6 x 10 pixel character colour data (tgc_data)
		// or 12 x 20 pixel character colour data (if characters are interpolated).
		// The graphics data is always 6 x 10 pixels but encoded as a 2 x 3 sixels (one sixel <=> 3 x 3.3 pixels)
		// The visible screen in the should be 480 × 500 pixels <=> 40 × 25 characters <=> 78 x 54 sixels. However,
		// The Beeb seems to setup the CRTC to generate only 19 * 25 = 475 (rather than the expected 20 *25 = 500) visible scan lines.
		// The full frame rate should correspond to 1/2 the interlaced frame rate, i.e. 50 /2 = 25 Hz but here the Beeb uses 27 Hz 
		// (close enough).
		vector<TT5050::TTColour> tgc_data;

		// Advance the CRT one character at a time
		// If in the active, area, the video memory address containing 
		// character/graphics data is also provided.
		uint16_t crtc_adr, screen_adr;
		bool in_active_area;
		if (!mCRTC->getMemFetchAdr(crtc_adr)) {
			cout << "Failed to get address from the CRTC!\n";
			return false;
		}

		// Is the screen (display) in the active area?	
		uint8_t dis_ena;
		if (!teletext)
			dis_ena = ~(~mDISPTMG | ((mRA >> 3) & 0x1)); // RA3 shall never be set when not in teletext mode as raster lines are 0-7 only
		else
			dis_ena = mDISPTMG; // In teletext mode raster lines are 0-19

		if (!teletext) { // Normally modes 0-6 - raster address selected bytes within an 8 row byte block
			screen_adr = crtc_adr * 8 + (mRA & 0x7);
		}
		else { // Normally mode 7
			screen_adr = crtc_adr + (0x7400 ^ 0x2000); // CRTC MA13 is used to select the SA5050 and is cleared by 0x2000
		}

		

		if (mScanLine < active_lines)
			// Visible active line
		{
			/*
			if (char_pos == 0)
				cout << "\nL" << dec << mScanLine << " ";
			cout << dec << char_pos << ":" << hex << screen_adr << " ";
			*/
			if (dis_ena)
				// Active area of an active line
			{

				if (char_pos == 0)
					// Add left border
				{
					uint8_t Rs, Gs, Bs;
					for (int c = 0; c < left_border_chars; c++) {
						for (int big_pixel = 0; big_pixel < pixels_per_byte; big_pixel++) {
							Rs = Gs = Bs = 0;
							uint32_t colour = 0xff0ff000;
							for (int pw = 0; pw < mPixelW && bitmap_data_p != NULL; pw++)
								*bitmap_data_p++ = colour;
						}
					}
				}

				// Read video memory data
				uint8_t screen_data;
				if (!mVideoMem->read(screen_adr, screen_data)) {
					cout << "Failed to read video memory at address 0x" << hex << screen_adr << "\n";
					return false;
				}

				// For teletext modes, decode video memory data as videotext data
				if (teletext && !mTGC->getScreenData(char_pos == 0, mScanLine == 0 && char_pos == 0, screen_data, tgc_data)) {
					return false;
				}


				// Get cursor configuration
				uint8_t cursor_seg_ena = 0x0;
				if (mCURSOR) {
					if (mCursorSegment < 0)
						mCursorSegment = 0;
					uint8_t cursor_segments = (mControlRegister >> 5) & 0x7;
					cursor_seg_ena = cursor_segments >> (2 - mCursorSegment);
					bool clk_2_Mhz = ((mControlRegister >> 4) & 0x1) != 0;
					uint8_t cs_01_width = (clk_2_Mhz ? 8 : 16);
					uint8_t cs_2_width = (cs_01_width << 1) & 0x1;
					mCursorSegment++;
				}

				uint8_t Rs, Gs, Bs;
				uint8_t R, G, B;

				uint8_t mem_data = screen_data; // not for teletext

				//
				//	Columns	Colours	Big Pixels	Real Pixels		Big Pixel Width		Big Pixels/byte		Bytes/line	
				//	20		16		20*8 = 160	640				4					2					80
				//	40		2		40*8 = 320	640				2					8					40
				//	40		4		40*8 = 320	640				2					4					80
				//	80		2		80*8 = 640	640				1					8					80
				// 
				// Big pixels/byte = 8 * cols / Visible "chars" per line
				//
				for (int big_pixel = 0; big_pixel < pixels_per_byte; big_pixel++) {

					if (!teletext) {

						uint8_t palette_mem_adr = ((mem_data >> 1) & 0x1) | ((mem_data >> 2) & 0x2) | ((mem_data >> 3) & 0x1) | ((mem_data >> 4) & 0x1);
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
						if (big_pixel < tgc_data.size()) {
							Rs = tgc_data[big_pixel].R;
							Gs = tgc_data[big_pixel].G;
							Bs = tgc_data[big_pixel].B;
						}
						else {
							Rs = Gs = Bs = 0;
						}
					}

					R = Rs ^ cursor_seg_ena ^ mINV;
					G = Gs ^ cursor_seg_ena ^ mINV;
					B = Bs ^ cursor_seg_ena ^ mINV;


					// Update display with the R, G & B data
					uint32_t colour = 0xff000000 | (R ? 0x00ff0000 : 0) | (G ? 0x0000ff00 : 0) | (G ? 0x000000ff : 0);
					for (int pw = 0; pw < mPixelW && bitmap_data_p != NULL; pw++)
						*bitmap_data_p++ = colour;

					if (!teletext)
						mem_data = mem_data << 1;
					else {
						R = R << 1;
						G = G << 1;
						B = B << 1;
					}
				}

				if (char_pos == active_chars - 1)
					// Add right border 
				{
					for (int c = 0; c < right_border_chars; c++) {
						uint8_t Rs, Gs, Bs;
						for (int big_pixel = 0; big_pixel < pixels_per_byte; big_pixel++) {
							Rs = Gs = Bs = 0;
							uint32_t colour = 0xff0ff000;
							for (int pw = 0; pw < mPixelW && bitmap_data_p != NULL; pw++)
								*bitmap_data_p++ = colour;
						}
					}
				}
			}

			
		}

		
		
	}

	if (mScanLine == active_lines - 1)
		// Add Bottom border
	{
		for (int line = 0; line < bottom_border_lines; line++) {
			for (int char_pos = 0; char_pos < visible_chars; char_pos++) {
				uint8_t Rs, Gs, Bs;
				for (int big_pixel = 0; big_pixel < pixels_per_byte; big_pixel++) {
					Rs = Gs = Bs = 0;
					uint32_t colour = 0xff00ff00;
					for (int pw = 0; pw < mPixelW && bitmap_data_p != NULL; pw++)
						*bitmap_data_p++ = colour;
				}
			}
		}

		mFrame++;
	}

	mScanLine = (mScanLine + 1) % scan_lines_per_frame;

	return true;
}


bool BeebVideoULA::read(uint16_t adr, uint8_t& data)
{
	// Call parent class to trigger scheduling of other devices when applicable
	if (!VideoDisplayUnit::triggerBeforeRead(adr, data))
		return false;

	// All registers are write-only => return 0xff for read operations
	data = 0xff;

	return true;
}

void BeebVideoULA::updateScreenSz()
{
	// For non-teletext mode the visible screen's actual pixels are 640 x 256
	// For the telelext mode the width is visible horizontal chars * 12.
	// and the height is (visible vertical chars * scan lines per char) which is normally 27 * 19 = 513 (500 = 25 x 20 is used for the chars)
	int pW = mScreenW;
	int pH = mScreenH;
	if (getCRField(CR_TELETEXT)) {
		if (mCRTC->initialised()) {
			int chars_per_line, lines;
			mCRTC->getVisibleCharArea(chars_per_line, lines);
			mScreenW = chars_per_line * 12;
			mScreenH = lines;
			mPixelW = 1;
		}
	}
	else {
		mScreenW = 640;
		mScreenH = 256;
	}

	// resize screen when switching between teletext mode (500 x 480 + borders)  and non-telext modes (640 x 256)
	if (pW != mScreenW || pH != mScreenH) {
		//cout << "create display bitmap " << dec << mScreenW << " x " << mScreenH << "\n";
		unlockDisplay();
		al_destroy_bitmap(mDisplayBitmap);	
		mDisplayBitmap = al_create_bitmap(mScreenW, mScreenH);
		lockDisplay();
	}
}

bool BeebVideoULA::write(uint16_t adr, uint8_t data)
{

	if (!VideoDisplayUnit::selected(adr))
		return false;


	uint16_t a = adr - mMemorySpace.adr;

	if (a == 0) {

		mControlRegister = data;
		if (getCRField(CR_CLOCK_RATE) == 1)
			updatePort(CRTC_CLK, 2);
		else
			updatePort(CRTC_CLK, 1);

		mNCols = (1 << getCRField(CR_N_COLS)) * 10;
		mPixelRate = 1 << (getCRField(CR_N_COLS) + 1); // pixel rate: 2, 4, 8 or 16 MHz
		mPixelW = 16 / mPixelRate; // pixel width in "actual pixels" for a 640 pixel wide screen
		mPixelsPerByte = 8 * mNCols / getVisibleCharsPerLine();

		updateScreenSz();

	}
	else if (a == 1) {
		uint8_t pa = (a >> 4) & 0xf;
		mPaletteMem[pa] = data & 0xf;
	}

	if (mCRTC != NULL && mCRTC->initialised() && (mDebugInfo->dbgLevel & DBG_VERBOSE)) {
		cout << "\n" << dec;
		cout << "Video ULA PixelRate:       " << mPixelRate << " MHz\n";
		cout << "Video ULA PixelWidth:      " << (int) mPixelW << "\n";
		cout << "Video ULA Pixels/byte:     " << (int)mPixelsPerByte << "\n";
		cout << "Video ULA CRTC Clock:      " << mCRTC_CLK << " MHz\n";
		cout << "Video ULA No of cols:      " << mNCols << "\n";
		cout << "Video ULA Teletext:        " << (getCRField(CR_TELETEXT) ? "ON" : "OFF") << "\n";
		cout << "Video ULA Cursor Segments: " << hex << (int)getCRField(CR_CURSOR_SEGMENT) << "\n";
		cout << "\n";
	}
	
	// Call parent class to trigger scheduling of other devices when applicable
	return VideoDisplayUnit::triggerAfterWrite(adr, data);
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
	w = mScreenW;
	h = mScreenH;
	return true;
}

double BeebVideoULA::getScanLineDuration()
{
	if (mCRTC != NULL && mCRTC->initialised())
		return mCRTC->getScanLineDuration();
	else
		return 64.0;
}

double BeebVideoULA::getScanLinesPerFrame()
{
	if (mCRTC != NULL && mCRTC->initialised())
		return mCRTC->getScanLinesPerFrame();
	else
		return 312.0;
}

double BeebVideoULA::getFrameRate()
{
	if (mCRTC != NULL && mCRTC->initialised())
		return mCRTC->getFrameRate();
	else
		return 50.0;
}

int BeebVideoULA::getCharScanLines()
{
	if (mCRTC != NULL && mCRTC->initialised())
		return mCRTC->getCharScanLines();
	else
		return 12;
}

int BeebVideoULA::getVerticalSyncLine()
{
	if (mCRTC != NULL && mCRTC->initialised())
		return mCRTC->getVerticalSyncLine();
	else
		return 0;
}

int BeebVideoULA::getHorizontalSyncPos()
{
	if (mCRTC != NULL && mCRTC->initialised())
		return mCRTC->getHorizontalSyncPos();
	else
		return 51;

}

int BeebVideoULA::getCharsPerLine()
{
	if (mCRTC != NULL && mCRTC->initialised())
		return mCRTC->getCharsPerLine();
	else
		return 40;
}

int BeebVideoULA::getVisibleCharsPerLine()
{
	if (mCRTC != NULL && mCRTC->initialised())
		return mCRTC->getVisibleCharsPerLine();
	else
		return 40;
}

int BeebVideoULA::getScanLine()
{
	return mScanLine;
}

int BeebVideoULA::getLeftBorderChars()
{
	if (mCRTC != NULL && mCRTC->initialised())
		return mCRTC->getLeftBorderChars();
	else
		return 0;
}

int BeebVideoULA::getTopBorderLines()
{
	if (mCRTC != NULL && mCRTC->initialised())
		return mCRTC->getTopBorderLines();
	else
		return 0;
}

int BeebVideoULA::getActiveChars()
{
	if (mCRTC != NULL && mCRTC->initialised())
		return mCRTC->getActiveChars();
	else
		return 40;
}

int BeebVideoULA::getActiveLines()
{
	if (mCRTC != NULL && mCRTC->initialised())
		return mCRTC->getActiveLines();
	else
		return 25;
}

int BeebVideoULA::getRightBorderChars()
{
	if (mCRTC != NULL && mCRTC->initialised())
		return mCRTC->getRightBorderChars();
	else
		return 10;
}

int BeebVideoULA::getBottomBorderLines()
{
	if (mCRTC != NULL && mCRTC->initialised())
		return mCRTC->getBottomBorderLines();
	else
		return 10;
}

int BeebVideoULA::getRetraceLines()
{
	if (mCRTC != NULL && mCRTC->initialised())
		return mCRTC->getRetraceLines();
	else
		return 10;
}

int BeebVideoULA::getRetraceChars()
{
	if (mCRTC != NULL && mCRTC->initialised())
		return mCRTC->getRetraceChars();
	else
		return 10;
}

// Get pointer to other device to be able to call its methods
bool BeebVideoULA::connectDevice(Device* dev)
{
	Device::connectDevice(dev);

	if (dev != NULL && dev->devType == CRTC6845_DEV) {
		mCRTC = (CRTC6845*)dev;
		//cout << "CRTC connected!\n";
	}
	if (dev != NULL && dev->devType == TT_5050_DEV) {
		mTGC = (TT5050*)dev;
		//cout << "TGC connected!\n";
	}

	return true;
}