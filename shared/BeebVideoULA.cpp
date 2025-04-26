#include "BeebVideoULA.h"
#include "CRTC6845.h"
#include "TT5050.h"
#include <iomanip>
#include "Utility.h"
#include <chrono>
#include <cmath>
#include <bitset>

// 
// The Video ULA sets up the 6847 CRTC.
// 
// The 6847 R12/R13 value is set as follows:
// MODE 0 - 6:	screen address / 8 <=> 0,1,2: 0x3000 => 0x600, 3: 0x4000 => 0x800, 4-5: 0xb00, 6: 0x6000 => 0xc00
// MODE 7:		screen address - (0x7400 ^ 0x2000) => 0x2800 for actual memory address of 0x7c00
// The same logic applies to setting R14/R15 (cursor position)
//
// Mode		CRTC address		Actual Address		Size	Resolution	Visible	Colours		Total		Visible		Total		PixelW*	PixelR	Pixels/byte**
//																		Chars				Lines		Lines		Chars
// 0		 0x600 -  0xfff		0x3000 - 0x7fff		20k		640x256		80x32		2		312			256			128			1		16 MHz	8
// 1		 0x600 -  0xfff		0x3000 - 0x7fff		20k		320x256		40x32		4		312			256			128			2		 8 MHz	4
// 2		 0x600 -  0xfff		0x3000 - 0x7fff		20k		160x256		20x32		16		312			256			128			4		 4 MHz	2
// 3		 0x800 -  0xfff		0x4000 - 0x7fff		16k		N/A			80x25		2		312			256			64			1		16 MHz	8
// 4		 0xb00 -  0xfff		0x5800 - 0x7fff		10k		320x256		40x32		2		312			256			64			2		 8 MHz	8
// 5		 0xb00 -  0xfff		0x5800 - 0x7fff		10k		160x256		20x32		4		312			256			64			4		 4 MHz	4
// 6		 0xc00 -  0xfff		0x6000 - 0x7fff		8k		N/A			40x25		2		312			256			64			2		 8 MHz	8
// 7		0x2800 - 0x2c00		0x7c00 - 0x7fff		1k		78x75		40x25		8		572			513			64 (768)	1		 8 MHz	12
//															(468x500) (240x250)
//																	  (480x500)
// 
// * W.r.t. scaling to either 640 pixels (Modes 0-6) or 480 pixels (mode 7). For Modes 0-6 PixelW = 16/PixelR; for Mode 7 it is '1'.
// ** For modes 0-6 it is 8/log2(#colours) or 8 * #columns / visible line chars; for mode 7 it is 12.
// 
// Each n x 8(10) pixel block in modes 0-6 are organised so that each row (0,1,..7(9)) of the n pixel are stored
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
// The full frame rate should correspond to 1/2 the interlaced field rate, i.e. 50 /2 = 25 Hz

BeebVideoULA::BeebVideoULA(
	string name, uint16_t adr, double cpuclock, uint8_t waitStates, ALLEGRO_DISPLAY* disp, ALLEGRO_BITMAP* dispBitmap, int dispW, int dispH, DebugManager  *debugManager, ConnectionManager* connectionManager
) : VideoDisplayUnit(name, BEEB_VDU_DEV, cpuclock, waitStates, adr, 0x10, dispBitmap, dispW, dispH, 0x0 /* dummy adr */, debugManager, connectionManager), mDisplay(disp)
{
	registerPort("SCROLL_CTRL",	IN_PORT,	0x0f, SCROLL_CTRL,	&mSCROLL_CTRL);
	registerPort("DISEN",		IN_PORT,	0x01, DISPTMG,		&mDISPTMG);
	registerPort("CURSOR",		IN_PORT,	0x01, CURSOR,		&mCURSOR);	
	registerPort("INV",			IN_PORT,	0x01, INV,			&mINV);
	registerPort("RA",			IN_PORT,	0x0f, RA,			&mRA);
	registerPort("CRTC_CLK",	OUT_PORT,	0x3, CRTC_CLK,		&mCRTC_CLK);
	registerPort("RESET",		IN_PORT,	0x01, RESET,		&mRESET);


	// Create 640 x 256 display bitmap and clear it
	mDisplayBitmap = al_create_bitmap(640, 512);

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



	auto video_start = chrono::high_resolution_clock::now();

	bool reset_transition = (mRESET != pRESET);
	pRESET = mRESET;

	if (mRESET == 0 && reset_transition)
		reset();

	uint64_t pCycleCount = mCycleCount;
	double scan_line_duration = getScanLineDuration();
	int cycle_count = max(1, (int)round(scan_line_duration * mCPUClock));
	mCycleCount += cycle_count;
	endCycle = mCycleCount;

	// Make sure internal state is up-to-date and synchonised with that of the CRTC
	if (!validateInternalState(mControlRegister))
		return true;

	int chars_per_line = getCharsPerLine();
	int visible_chars = getLeftBorderChars() + getActiveChars() + getRightBorderChars();
	int active_chars = getActiveChars();
	int top_border_lines = getTopBorderLines();
	int active_lines = getActiveLines();
	int bottom_border_lines = getBottomBorderLines();
	int left_border_chars = getLeftBorderChars();
	int right_border_chars = getRightBorderChars();
	int disp_skew;
	int cursor_skew;
	if (mCRTC != NULL)
		mCRTC->getSkew(disp_skew, cursor_skew);
	else {
		disp_skew = 0;
		cursor_skew = 0;
	}

	int field = fieldScanLineOffset();
	int adjusted_scanline = mScanLine - field;

	int field_rate = (int)round(mCRTC->getFieldRate());
	if (adjusted_scanline == 0) {

		if (mDM->debug(DBG_TIME) && mField == 0) {
			cout << dec << "\n";
			cout << "Field Rate: " << dec << field_rate << "\n";
			cout << "Video ULA total (including CRTC & TCG) - ms per sec: " << mVideoULACnt / 1e6 << "\n";
			cout << "Display update - ms per sec: " << mDispUsCnt / 1e6 << "\n";
			cout << "Memory read - ms per sec: " << mReadCnt / 1e6 << "\n";
			cout << "CRTC update - ms per sec: " << mCRTCnt / 1e6 << "\n";
			cout << "TT update - ms per sec: " << mTTCnt / 1e6 << "\n";
			cout << "Line update - ms per sec: " << mLineCnt / 1e6 << "\n";
			cout << "Left border update - ms per sec: " << mLeftBorderCnt / 1e6 << "\n";
			cout << "Character pixels update - ms per sec: " << mCharPixelCnt / 1e6 << "\n";
			cout << "Right border update - ms per sec: " << mRightBorderCnt / 1e6 << "\n";
			cout << "Top border update - ms per sec: " << mTopBorderCnt / 1e6 << "\n";
			cout << "Bottom border update - ms per sec: " << mBottomBorderCnt / 1e6 << "\n";
			mVideoULACnt = 0;
			mDispUsCnt = 0;
			mCRTCnt = 0;
			mTTCnt = 0;
			mCharPixelCnt = 0;
			mTopBorderCnt = 0;
			mLeftBorderCnt = 0;
			mRightBorderCnt = 0;
			mBottomBorderCnt = 0;
			mReadCnt = 0;
			mLineCnt = 0;
		}

		mField = (mField + 1) % field_rate;
	}

	
	if (mDM->debug(DBG_VDU))
		cout << "\n\nFIELD #" << field << ", SCAN LINE " << dec << mScanLine << " (adjusted to " << adjusted_scanline << ")\n";


	bool teletext = getCRField(CR_TELETEXT) == 1;


	if (adjusted_scanline == mVerticalSyncPos) {
	
		auto disp_start = chrono::high_resolution_clock::now();

		mDM->log(this, DBG_VDU, "UPDATE SCREEN\n");

		unlockDisplay();

		// Scale the display bitmap including borders to match the size of the display
		//al_draw_scaled_bitmap(mDisplayBitmap, 0, 0, mScreenW, mScreenH, 0, 0, mDisplayWidth, mDisplayHeight, 0);
		int width = mScreenW;
		int height = mScreenH;

		al_draw_bitmap(mDisplayBitmap, 0, 0, 0);

		// Make the updates visible on the display	
		al_flip_display();

		// Clear the display
		al_clear_to_color(black);

		lockDisplay();

		auto disp_stop = chrono::high_resolution_clock::now();
		auto disp_dur = chrono::duration_cast<chrono::nanoseconds>(disp_stop - disp_start);
		mDispUsCnt += disp_dur.count();

	}

	

	// Process one scan line (should be 64/128 chars per scan line and the visible part being 20, 40 or 80 chars)
	// Each char is not necessarily the same as character on the screen. "Big Char" is the char visible
	// on the screen and "Char" is actually one byte fetched from memory that only sometimes corresponds
	// to the width of a screen char. 

	unsigned int* max_bitmap_data_p = (unsigned int*)((char*)mLockedDisplayBitMap->data + mLockedDisplayBitMap->pitch * mScreenScanLines);
	unsigned int* bitmap_data_p = NULL;

	if (adjusted_scanline == 0)
		// Add Top border
	{
		auto top_border_start = chrono::high_resolution_clock::now();

		if (mDM->debug(DBG_VDU))
			cout << "DRAW TOP BORDER\n";

		bitmap_data_p = (unsigned int*)((char*)mLockedDisplayBitMap->data);

		uint32_t colour = 0xff0ff000;
		if (field == 1)
			colour = 0xffff0000;
		for (int line = 0; line < top_border_lines; line++) {
			for (int char_pos = 0; char_pos < visible_chars; char_pos++) {
				for (int big_pixel = 0; big_pixel < mPixelsPerCharacter; big_pixel++) {
					for (int pw = 0; pw < mPixelW && bitmap_data_p != NULL && bitmap_data_p < max_bitmap_data_p; pw++) {
						if (field == 1 && line % 2 == 0 || field == 0 && line % 2 == 1)
							*bitmap_data_p++;
						else
							*bitmap_data_p++ = colour;
					}
				}
			}
		}

		auto top_border_stop = chrono::high_resolution_clock::now();
		auto top_border_dur = chrono::duration_cast<chrono::nanoseconds>(top_border_stop - top_border_start);
		mTopBorderCnt += top_border_dur.count();
	}

	bitmap_data_p = (unsigned int*)((char*)mLockedDisplayBitMap->data + mLockedDisplayBitMap->pitch * (mScanLine + top_border_lines));

	

	for (int char_pos = 0; char_pos < chars_per_line; char_pos++) {	

		auto line_start = chrono::high_resolution_clock::now();

		// Advance CRTC & TGC one character (visible or not) and get character data (only used for visible char though)
		// the TGC character is only 12 pixels wide whereas the CRTC one is 8 pixels wide!
		
		vector<TT5050::TTColour> tgc_data(16);

		if (char_pos == 0 && mDM->debug(DBG_VDU))
			cout << "VDU RA = " << dec << (int)mRA << "\n";

		// Advance the CRT one character at a time
		// If in the active, area, the video memory address containing 
		// character/graphics data is also provided.
		uint16_t crtc_adr, screen_adr;
		auto crtc_start = chrono::high_resolution_clock::now();
		if (!mCRTC->getMemFetchAdr(crtc_adr)) {
			cout << "Failed to get address from the CRTC!\n";
			return false;
		}
		auto crtc_stop = chrono::high_resolution_clock::now();
		auto crtc_dur = chrono::duration_cast<chrono::nanoseconds>(crtc_stop - crtc_start);
		mCRTCnt += crtc_dur.count();

		// Is the screen (display) in the active area?	
		uint8_t dis_ena;
		if (!teletext)
			dis_ena = ~(~mDISPTMG | ((mRA >> 3) & 0x1)); // RA3 shall never be set when not in teletext mode as there are only 8 raster lines for modes 0-6
		else
			dis_ena = mDISPTMG; // In teletext mode raster lines are 0-19

		// Calculate screen address
		if (!teletext) { // Normally modes 0-6 - raster address selected bytes within an 8 row byte block
			screen_adr = crtc_adr * 8 + (mRA & 0x7);
		}
		else { // Normally mode 7
			screen_adr = crtc_adr + (0x7400 ^ 0x2000); // CRTC MA13 is used to select the SA5050 and is cleared by 0x2000
		}
		if (screen_adr >= 0x8000) {
			//cout << "SCROLLING - subtracting 0x" << hex << screen_adr << " by 0x" << mHwScrollSub << " for C1:C0 = " << Utility::int2binStr(mC,2) << "\n";
			screen_adr -= mHwScrollSub; // correct for wrap around when hardware scrolling		
		}

		if (adjusted_scanline < active_lines && char_pos >= disp_skew && char_pos < active_chars + disp_skew && (screen_adr >= 0x8000 || screen_adr < 0x3000)) { // exit if the CRTC hasn't been properly initialised yet!
			cout << "CRTC not ready - got address " << hex << (crtc_adr * 8 + (mRA & 0x7)) << " for line " << dec << mScanLine << ", pos " << char_pos << " and for #active chars of " << active_chars << "\n";
			return false;
		}

		if (adjusted_scanline < active_lines)
			// Visible active line
		{
						
			if (dis_ena)
				// Active area of an active line
			{
				
				if (char_pos == disp_skew)
					// Add left border
				{

					auto left_border_start = chrono::high_resolution_clock::now();

					mDM->log(this, DBG_VDU, "DRAW LEFT BORDER\n");		

					uint32_t colour = 0xff0ff000;
					if (field == 1)
						colour = 0xffff0000;
					for (int c = 0; c < left_border_chars; c++) {
						for (int big_pixel = 0; big_pixel < mPixelsPerCharacter; big_pixel++) {
							for (int pw = 0; pw < mPixelW && bitmap_data_p != NULL && bitmap_data_p < max_bitmap_data_p; pw++)
								*bitmap_data_p++ = colour;
						}
					}

					auto left_border_stop = chrono::high_resolution_clock::now();
					auto left_border_dur = chrono::duration_cast<chrono::nanoseconds>(left_border_stop - left_border_start);
					mLeftBorderCnt += left_border_dur.count();
				}

				// Read video memory data
				auto mem_read_start = chrono::high_resolution_clock::now();
				uint8_t screen_data;
				if (!mVideoMem->read(screen_adr, screen_data)) {
					cout << "Failed to read video memory at address 0x" << hex << screen_adr << "\n";
					return false;
				}
				auto mem_read_stop = chrono::high_resolution_clock::now();
				auto mem_read_dur = chrono::duration_cast<chrono::nanoseconds>(mem_read_stop - mem_read_start);
				mReadCnt += mem_read_dur.count();

				// For teletext modes, decode video memory data as videotext data
				if (teletext) {
					auto tt_start = chrono::high_resolution_clock::now();
					if (!mTGC->getScreenData(char_pos == disp_skew, adjusted_scanline == 0 && char_pos == disp_skew, screen_data, tgc_data)) {
						cout << "Failed to get teletext symbol at address 0x" << hex << screen_adr << "\n";
						return false;
					}
					auto tt_stop = chrono::high_resolution_clock::now();
					auto tt_dur = chrono::duration_cast<chrono::nanoseconds>(tt_stop - tt_start);
					mTTCnt += tt_dur.count();
				}


				// Get cursor configuration
				//
				// The video ULA draws each segment in turn, if the corresponding bit is set.
				// Segments 0 and 1 are each 1/40 or 1/80 of the display width.
				// Segment 2 is twice as wide as segment 1 if the CRTC clock rate is 1 MHz; otheriwse it
				// has the same size as segment 1.
				//
				uint8_t cursor_seg_ena = 0x0;
				bool clk_2_Mhz = ((mControlRegister >> 4) & 0x1) != 0;
				uint8_t cursor_segments = (mControlRegister >> 5) & 0x7;
				if (teletext) // Emulate one character delay for teletext data by shifting the cursor segments one step left
					cursor_segments = (cursor_segments << 1) & 0x7;
				if (mCURSOR || mCursorSegment > 0 && (mCursorSegment <= 2 || !clk_2_Mhz && mCursorSegment <= 3)) {
					if (mCursorSegment < 0)
						mCursorSegment = 0;// char_skew; // start segment considers character skew (cursor skew already considered in CURSOR input)
					int shift = (mCursorSegment<=2? mCursorSegment:2);				
					cursor_seg_ena = (cursor_segments >> (2 - shift)) & 0x1;						
					if (mDM->debug(DBG_VDU)) {
						cout << "VDU CURSOR " << dec << (int) mCURSOR << ", cursor ena " << (cursor_seg_ena ? "enabled" : "disabled") <<
							", cursor segments '" << setw(3) << bitset<3>(cursor_segments) << "', cursor segment #" << dec << mCursorSegment << 
							", char col " << char_pos << "\n";
					}
					mCursorSegment++;
				}
				else
					mCursorSegment = -1;

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
				auto char_pixel_start = chrono::high_resolution_clock::now();
				for (int big_pixel = 0; big_pixel < mPixelsPerCharacter; big_pixel++) {

					if (!teletext) {

						// The screen memory byte's b7b5b3b1 is used as address a3a2a1a0 into the 16-bytes palette memory
						// The read palette memory nibble FBiGiRi gives flash state (F) and inverted values of R (Ri), G (Gi) & B (Bi)  
						uint8_t palette_mem_adr = ((mem_data >> 4) & 0x8) | ((mem_data >> 3) & 0x4) | ((mem_data >> 2) & 0x2) | ((mem_data >> 1) & 0x1);
						uint8_t palette_data = mPaletteMem[palette_mem_adr] & 0xf;
						uint8_t Ri = palette_data & 0x1;
						uint8_t Gi = (palette_data >> 1) & 0x1;
						uint8_t Bi = (palette_data >> 2) & 0x1;
						uint8_t F = (palette_data >> 3) & 0x1;
						uint8_t flash = mControlRegister & 0x1;
						uint8_t Fi = 1 - (F & flash);

						Rs = 1-((Ri ^ Fi) & dis_ena);
						Gs = 1-((Gi ^ Fi) & dis_ena);
						Bs = 1-((Bi ^ Fi) & dis_ena);

						// Invert the video if INV is LOW (only for modes 0 to 6 and not for the teletext mode 7)
						// Also scale up from 0:1 to 0:255 for the intensity
						Rs = ((Rs ^ mINV)?255:0);
						Gs = ((Gs ^ mINV)?255:0);
						Bs = ((Bs ^ mINV)?255:0);
						
					}
					else {
						if (big_pixel < tgc_data.size()) {
							Rs = tgc_data[big_pixel].R;
							Gs = tgc_data[big_pixel].G;
							Bs =tgc_data[big_pixel].B;
						}
						else {
							Rs = Gs = Bs = 0;
						}

					}

					// Super-impose the cursor
					uint8_t cursor = (cursor_seg_ena ? 255 : 0);
					R = Rs ^ cursor;
					G = Gs ^ cursor;
					B = Bs ^ cursor;


					// Update display with the R, G & B data
					//uint32_t colour = 0xff000000 | (R ? 0x00ff0000 : 0) | (G ? 0x0000ff00 : 0) | (B ? 0x000000ff : 0);
					uint32_t colour = 0xff000000 | (R << 16) | (G << 8) | B;
					for (int pw = 0; pw < mPixelW && bitmap_data_p != NULL && bitmap_data_p < max_bitmap_data_p; pw++)
						*bitmap_data_p++ = colour;

					if (!teletext)
						mem_data = (mem_data << 1) | 1;
				}
				auto char_pixel_stop = chrono::high_resolution_clock::now();
				auto char_pixel_dur = chrono::duration_cast<chrono::nanoseconds>(char_pixel_stop - char_pixel_start);
				mCharPixelCnt += char_pixel_dur.count();

				if (char_pos == active_chars + disp_skew - 1)
					// Add right border 
				{
					auto right_border_start = chrono::high_resolution_clock::now();
					mDM->log(this, DBG_VDU,"DRAW RIGHT BORDER\n");
					uint32_t colour = 0xff0ff000;
					if (field == 1)
						colour = 0xffff0000;
					for (int c = 0; c < right_border_chars; c++) {
						for (int big_pixel = 0; big_pixel < mPixelsPerCharacter; big_pixel++) {					
							for (int pw = 0; pw < mPixelW && bitmap_data_p != NULL && bitmap_data_p < max_bitmap_data_p; pw++)
								*bitmap_data_p++ = colour;
						}
					}
					auto right_border_stop = chrono::high_resolution_clock::now();
					auto right_border_dur = chrono::duration_cast<chrono::nanoseconds>(right_border_stop - right_border_start);
					mRightBorderCnt += right_border_dur.count();
				}
			}

			
		}	

		auto line_stop = chrono::high_resolution_clock::now();
		auto line_dur = chrono::duration_cast<chrono::nanoseconds>(line_stop - line_start);
		mLineCnt += line_dur.count();
		
	}
	
	

	if (adjusted_scanline == active_lines)
		// Add Bottom border
	{

		auto bottom_border_start = chrono::high_resolution_clock::now();

		if (mDM->debug(DBG_VDU))
			cout << "DRAW BOTTOM BORDER\n";

		bitmap_data_p = (unsigned int*)((char*)mLockedDisplayBitMap->data + mLockedDisplayBitMap->pitch * (adjusted_scanline + top_border_lines));

		uint32_t colour = 0xff00ff00;
		if (field == 1)
			colour = 0xffff0000;
		
		for (int line = 0; line < bottom_border_lines; line++) {
			for (int char_pos = 0; char_pos < visible_chars; char_pos++) {
				uint8_t Rs, Gs, Bs;
				for (int big_pixel = 0; big_pixel < mPixelsPerCharacter ; big_pixel++) {
					Rs = Gs = Bs = 0;

					for (int pw = 0; pw < mPixelW && bitmap_data_p != NULL && bitmap_data_p < max_bitmap_data_p; pw++) {
						if (field == 1 && line % 2 == 0 || field == 0 && line % 2 == 1)
							*bitmap_data_p++;
						else
							*bitmap_data_p++ = colour;
					}
				}
			}
		}

		auto bottom_border_stop = chrono::high_resolution_clock::now();
		auto bottom_border_dur = chrono::duration_cast<chrono::nanoseconds>(bottom_border_stop - bottom_border_start);
		mBottomBorderCnt += bottom_border_dur.count();

	}

	auto video_stop = chrono::high_resolution_clock::now();
	auto video_dur = chrono::duration_cast<chrono::nanoseconds>(video_stop - video_start);
	mVideoULACnt += video_dur.count();

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

bool BeebVideoULA::dump(uint16_t adr, uint8_t& data)
{
	data = 0xff;
	return false;
}

void BeebVideoULA::updateScreenSz()
{
	// For non-teletext mode the visible screen's actual pixels are 640 x 256
	// For the telelext mode the width is visible horizontal chars * 12.
	// and the height is (visible vertical chars * scan lines per char) which is normally 27 * 19 = 513 (500 = 25 x 20 is used for the chars)
	// 
	// The 40x25 6x10-pixel (12x20 interpolated) teletext characters should be displayed in the same area as 40x25 8x10-pixel characters.
	// This means 40*12 x 25*20 = 480 x 500 should be scaled as the 40*8 x 25*10 = 320 x 250 pixels (as for mode 6).
	int pW = mScreenW;
	int pH = mScreenH;

	if (mCRTC->initialised()) {
		int visible_chars_per_line, visible_lines;
		mCRTC->getVisibleCharArea(visible_chars_per_line, visible_lines);
		mScreenW = visible_chars_per_line * mPixelsPerCharacter * mPixelW;
		mScreenH = visible_lines;		
	}
	

	// resize screen when switching between teletext mode (500 x 480 + borders)  and non-telext modes (640 x 256)
	if (pW != mScreenW || pH != mScreenH) {
		int n_active_chars = mCRTC->getActiveChars();
		int n_active_lines = mCRTC->getActiveLines();
		int n_active_W = n_active_chars * mPixelsPerCharacter;
		if (mDM->debug(DBG_VERBOSE))
			cout << "create display bitmap " << dec << mScreenW << " x " << mScreenH << " (" << n_active_W << " x " << n_active_lines << ")\n";
		int width = mScreenW;
		int height = mScreenH;
		al_resize_display(mDisplay, width, height);
		unlockDisplay();
		al_destroy_bitmap(mDisplayBitmap);	
		mDisplayBitmap = al_create_bitmap(mScreenW, mScreenH);
		lockDisplay();
	}
}

bool BeebVideoULA::validateInternalState(uint8_t newControlRegisterValue)
{

	int p_scan_line = mScanLine;
	int p_scan_lines = mScreenScanLines;
	int p_vertical_syn_pos = mVerticalSyncPos;
	int p_pixels_per_byte = mPixelsPerCharacter;

	if (!initialised()) {
		mScanLine = 0;
		mScreenScanLines = 312;
		mVerticalSyncPos = 280;
		return false;
	}

	mScanLine = mCRTC->getScreenScanLine();
	mScreenScanLines = mCRTC->getScreenScanLines();
	mVerticalSyncPos = mCRTC->getVerticalSyncLine();

	if (getCRField(CR_CLOCK_RATE) == 1)
		updatePort(CRTC_CLK, 2);
	else
		updatePort(CRTC_CLK, 1);

	mNCols = (1 << getCRField(CR_N_COLS)) * 10;
	mPixelRate = 1 << (getCRField(CR_N_COLS) + 1); // pixel rate: 2, 4, 8 or 16 MHz

	bool teletext = getCRField(CR_TELETEXT) == 1;

	mPixelW = 16 / mPixelRate; // pixel width in "actual pixels" for a 640 pixel wide screen
	if (teletext)
		mPixelW = 1;

	mPixelsPerCharacter = 8 * mNCols / getVisibleCharsPerLine();
	if (mPixelsPerCharacter > 8) {
		return false;
	}
	if (teletext)
		mPixelsPerCharacter = 16; // as 12 pixels have been stretched to 16 by the TCG

	if ((
		((mControlRegister ^ newControlRegisterValue) & 0xfe) != 0 ||
		mPixelsPerCharacter != p_pixels_per_byte ||
		mScreenScanLines != p_scan_lines ||
		mVerticalSyncPos != p_vertical_syn_pos
		)
		&& (mDM->debug(DBG_VERBOSE))
	) {
		cout << "\n" << dec;
		cout << "Video ULA Control Register: 0x" << hex << (int)mControlRegister << dec << "\n";
		cout << "Video ULA PixelRate:       " << mPixelRate << " MHz\n";
		cout << "Video ULA PixelWidth:      " << (int)mPixelW << "\n";
		cout << "Video ULA Pixels/byte:     " << (int)mPixelsPerCharacter << "\n";
		cout << "Video ULA CRTC Clock:      " << (int)mCRTC_CLK << " MHz\n";
		cout << "Video ULA No of cols:      " << dec << mNCols << "\n";
		cout << "Video ULA Teletext:        " << (getCRField(CR_TELETEXT) ? "ON" : "OFF") << "\n";
		cout << "Video ULA Cursor Segments: " << hex << (int)getCRField(CR_CURSOR_SEGMENT) << "\n";
		cout << "Video ULA Pixel Width:     " << dec << (int)mPixelW << "\n";
		cout << "Video ULA Pixels/byte:     " << dec << mPixelsPerCharacter << "\n";
		cout << "Video ULA Scan Line:       " << dec << mScanLine << "\n";
		cout << "Video ULA Scan Lines:      " << dec << mScreenScanLines << "\n";
		cout << "Video ULA Vertical Sync:   " << dec << mVerticalSyncPos << "\n";
		cout << "\n";
	}

	updateScreenSz();

	return true;
}

bool BeebVideoULA::write(uint16_t adr, uint8_t data)
{

	if (!VideoDisplayUnit::selected(adr))
		return false;

	uint16_t a = adr - mMemorySpace.adr;

	if (a == 0) {

		mControlRegister = data;
		validateInternalState(data);

	}
	else if (a == 1) {
		uint8_t pa = (data >> 4) & 0xf;
		mPaletteMem[pa] = data & 0xf;
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

double BeebVideoULA::getScanLinesPerField()
{
	if (mCRTC != NULL && mCRTC->initialised())
		return mCRTC->getScanLinesPerField();
	else
		return 312.0;
}

int BeebVideoULA::getScreenScanLines()
{
	if (mCRTC != NULL && mCRTC->initialised())
		return mCRTC->getScreenScanLines();
	else
		return 624;
}

double BeebVideoULA::getFieldRate()
{
	if (mCRTC != NULL && mCRTC->initialised())
		return mCRTC->getFieldRate();
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

int BeebVideoULA::getScreenScanLine()
{
	return mCRTC->getScreenScanLine();
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

// Check if interlace is enabled (On)
bool BeebVideoULA::interlaceOn()
{
	return (mCRTC != NULL && mCRTC->interlaceOn());
}

// Advance 1/2 scan line - required for interlace modes as
// each field is usally 312 1/2 (PAL) or 262 1/2 (NTSC) scan lines
// to get 625 (PAL) or 525 (NTSC) scan lines per frame (i.e., a pair of even and odd fields)
// at 50 Hz (PAL) or 60 Hz (NTSC).
bool BeebVideoULA::advanceHalfLine(uint64_t& endCycle)
{
	uint64_t pCycleCount = mCycleCount;
	double half_scan_line_duration = getScanLineDuration() / 2;
	int cycle_count = max(1, (int)round(half_scan_line_duration * mCPUClock));
	mCycleCount += cycle_count;
	endCycle = mCycleCount;

	return true;
}

// Get scan line offset (0 for even field or non-interlaced mode, 1 for odd field)
int BeebVideoULA::fieldScanLineOffset()
{
	if (mCRTC != NULL && mCRTC->initialised())
		return mCRTC->fieldScanLineOffset();
	else
		return 0;
}

void BeebVideoULA::processPortUpdate(int port)
{
	if (port == RESET && mRESET == 0) {
		cout << "Video ULA reset!\n";
		reset();
	}

	if (port != SCROLL_CTRL)
		return;


	//
	// Hardware scrolling
	// 
	// The values of C0 and C1 together determine the value to subtract from the current
	// address when it exceeds 8000
	//      MA12	C1   C0     CRTC address	Screen				Mem		Hz			subract
	//		(a15)								Address   Modes		Sz		Res			const
	//		------------------------------------------------------------------------------------
	//		0		x	x		2800			7c00		7		1k					0400
	//		1		0   0		800				4000		3		16k					4000
	//		1		0   1		c00				6000		6		8k					2000
	//		1		1	0		600				3000		0,1,2	20k		80 bytes	5000
	//		1		1   1		b00				5800		4,5		10k					2800
	//
	// The System VIA's PB(3:0) are connected to the Video ULA's SCROLL_CTRL(3:0) where
	// The three least significant bits selects whether to update C0 (4) or C1 (5)
	// and the most significant bit is the value to assign to C0 or C1.
	// mC = C(1:0) below with C1 as the most significant bit and C0 as the least significant bit.
	//

	uint8_t ctrl_sel = mSCROLL_CTRL & 0x7;
	uint8_t ctrl_val = (mSCROLL_CTRL >> 3) & 0x1;
	uint8_t p_C = mC;
	switch (ctrl_sel) {
	case 4:	// Hardware scrolling - set C0 (See below)
		mC = (mC & 2) | ctrl_val;
		break;
	case 5:	// Hardware scrolling - set C1 (See below)
		mC = (mC & 1) | (ctrl_val << 1);
		break;
	default:
		break;
	}

	uint16_t p_HW_scroll_sub = mHwScrollSub;
	int mode = 0;
	mHwScrollSub = 0x0;
	switch (mC & 0x3) {
	case 0:// Mode 3 - start address 0x4000
		mode = 3;
		mHwScrollSub = 0x4000;
		break;
	case 1: // Mode 6 - start address 0x6000
		mode = 6;
		mHwScrollSub = 0x2000;
		break;
	case 2:// Modes 0,1,2 - start address 0x3000
		mode = 12;
		mHwScrollSub = 0x5000;
		break;
	case 3:// Modes 4,5 - start address 0x5800
		mode = 45;
		mHwScrollSub = 0x2800;
		break;
	default:
		mode = -1;
		break;
	}

	bool teletext = getCRField(CR_TELETEXT) == 1;
	if (teletext) {
		mHwScrollSub = 0x0400;
		mode = 7;
	}

	if (mDM->debug(DBG_VDU) && mHwScrollSub != p_HW_scroll_sub)
		cout << "New HW Scroll constant 0x" << hex << mHwScrollSub << " (0x" << p_HW_scroll_sub << ") for mode " << dec << mode << "\n";

}