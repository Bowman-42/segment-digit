/*
  SegmentDigit
*/
#ifndef SegmentDigit_h
#define SegmentDigit_h

#include "Arduino.h"


class SegmentDigit
{
  	public:
		SegmentDigit(int screenwidth, int screenheight);
		SegmentDigit(int screenwidth, int screenheight, int width);
		void setNrDigits(int nr);
		void setPosition(int y, int x);
		void setInternalPosition(int y, int x);
		void setWidth(int width);
		void setGap(int gap, boolean keep);
		void setLineWidth(int width, boolean keep);
		int drawInt(int val);
		int drawFloat(float val);
		int drawFloat(float val,int digits);
		void drawTime(int hi, int lo );// displays hi:lo, 
		void drawTime(int h, int m, int s );// displays hh:mm:ss, 
		void deleteAll();
		void deleteAll(int y, int x, int w, int h);
		void setCallback( void  (*drawRectCallback)(int y, int x, int w, int h, boolean del));
  	private:
		int _screen_width;
		int _screen_height;
		int _segment_width;
		int _segment_height;
		int _nr_digits;
		int _y1;
		int _y2;
		int _x1;
		int _x2;
		int _x3;
		int _gap;
		int _line_w;
		int _last_y;
		int _min_y;
		int _min_x;
		int _max_w;
		int _max_h;
		int _dec_point;
		int _dot_y;
		boolean _keep_line_w;
		boolean _keep_gap;
		void drawDot(int py, boolean del );
		void drawDoubleDot(int py, boolean del );
		void drawSegment(int idx,  int offset, boolean del);
		void drawDigit(int digit, int offset );
		void  (*_drawRectCallback)(int y, int x, int w, int h, boolean del);
    
};

#endif