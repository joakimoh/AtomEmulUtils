#ifndef VIDEO_H
#define VIDEO_H

#include <sstream>
#include <string>

using namespace std;

class VideoTiming {

public:

	double fieldFreq;
	double fieldDuration;
	double linesPerFrame;
	double pixelsPerLine;
	double frameFreq;
	double linesPerField;
	double lineDuration;
	double vBlanking;
	double hzBlanking;
	double vBlankinglines;
	double hzBlankingPixels;
	double visibleLineDuration;
	double visiblePixelsPerLine;
	double visibleLines;
	double hzPixelduration;

	VideoTiming(double f = 60, double lines = 525, double lineWidth = 720, double vtBlankingP = 0.08, double hzBlankingP = 0.172) {

		fieldFreq = f;
		linesPerFrame = lines;
		pixelsPerLine = lineWidth;
		frameFreq = fieldFreq / 2;
		linesPerField = linesPerFrame / 2;
		fieldDuration = 1 / fieldFreq;
		lineDuration = (1 / fieldFreq) / linesPerField;
		vBlanking = vtBlankingP * 1 / fieldFreq;
		hzBlanking = hzBlankingP * lineDuration;
		visibleLineDuration = lineDuration - hzBlanking;
		visiblePixelsPerLine = visibleLineDuration / lineDuration * pixelsPerLine;
		vBlankinglines = vBlanking / fieldDuration * linesPerField;
		visibleLines = linesPerField - vBlankinglines;
		hzPixelduration = lineDuration / pixelsPerLine;
	}

	std::string toString()
	{
		stringstream sout;

		sout << "Frame freq: " << frameFreq << ", Field freq: " << fieldFreq << "\n";
		sout << "Lines per frame: " << linesPerFrame << ", lines per field: " << linesPerField <<
			", line width : " << pixelsPerLine << "\n";
		sout << "Vertical blanking: " << vBlanking * 1e6 << " us <=> " << vBlankinglines << " lines\n";
		sout << "Horizontal blanking " << hzBlanking * 1e6 << " us\n";
		sout << "Visible lines: " << visibleLines << "\n";
		sout << "Horizontal pixel duration: " << hzPixelduration * 1e6 << " us\n";

		return sout.str();
	}

};




#endif