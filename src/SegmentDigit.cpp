/*
  SegmentDigit
*/

#include "Arduino.h"
#include "SegmentDigit.h"

#define SEGMENTS   7
/*
* Define which segment to display for the digits 0-9
*
*
* The segments:
* 0 => top
* 1 => left top
* 2 => right top
* 3 => middle
* 4 => left bottom
* 5 => right bottom
* 6 => bottom 
*/
const boolean digits[][SEGMENTS] = {
  {false, false, false, true, false , false, false},
  {true, true, false, true, true , false, true},
  { false, true, false, false, false, true, false},
  {false, true, false, false, true, false, false},
  {true, false, false, false,true, false,true},
  {false, false, true, false, true, false, false},
  {true, false, true, false, false, false, false},
  {false, true, false, true, true, false, true},
  {false,false,false,false,false,false,false},
  {false,false,false,false,true,false,true}
};

/*
*  Construct
*/
SegmentDigit::SegmentDigit(int screenwidth, int screenheight){
	_screen_width = screenwidth;
	_screen_height = screenheight;
	_y1 = 8;
	_x1 = 8;
	_min_y=_y1;
	_min_x=_x1;
	_nr_digits = 4;
	_dot_y = 0;
}
SegmentDigit::SegmentDigit(int screenwidth, int screenheight, int width){
	SegmentDigit(screenwidth, screenheight, width);
	setWidth(width);
}

/*
*  RegisterCallback, to be implemented for the hardware, has to draw a filled rect. 
* 
* ex: 
* u8g:  ( 2 color lcd )
void drawCallback(int y, int x, int w, int h, boolean del){
  u8g.setColorIndex(del ? 0 : 1 );
  u8g.drawBox(y,x,w,h);
}

* Adafruit_TFTLCD ( 16bit color tft )
void drawCallback(int y, int x, int w, int h, boolean del){
  int color = del ? bg_color : char_color;
  tft.fillRect(y, x, w, h, color);
}
*/
void SegmentDigit::setCallback( void  (*drawRectCallback)(int y, int x, int w, int h, boolean del)){
	_drawRectCallback = drawRectCallback;
}
void SegmentDigit::setNrDigits(int nr){
	_nr_digits = nr;
}
void SegmentDigit::setPosition(int y, int x){
	_y1 = y;
	_x1 = x;
	if ( _y1 < _min_y ){
		_min_y=_y1;
	}
	_min_x=_x1;
	_min_y=_y1;
	setWidth(_segment_width);
}
void SegmentDigit::setInternalPosition(int y, int x){
	_y1 = y;
	_x1 = x;
	if ( _y1 < _min_y ){
		_min_y=_y1;
	}
	_min_x=_x1;
	//_min_y=_y1;
	setWidth(_segment_width);
}
void SegmentDigit::setWidth(int width){
	_segment_width = width;
	_line_w =  _keep_line_w ? _line_w : _segment_width / 6;
	if (_line_w < 1 ){
		_line_w=1;
	}
	_gap =  _keep_gap ? _gap : _segment_width / 2;

	_segment_height=_segment_width+_segment_width/3;
	_y2 = _y1+_segment_width; 
	_x2 = _x1+_segment_height;
	_x3 = _x2+_segment_height;
  
}
void SegmentDigit::drawDoubleDot(int py, boolean del ){
	_drawRectCallback(py ,_x1 + (_segment_height/2), _line_w+2, _line_w+2, del );
	_drawRectCallback(py ,_x2  + (_segment_height/2), _line_w+2, _line_w+2, del );
	
  	setInternalPosition( _last_y + 5*_line_w  , _x1);
}
void SegmentDigit::drawDot(int py,  boolean del ){
	_drawRectCallback(py ,_x3-2, _line_w+2, _line_w+2, del );
  	setInternalPosition( _last_y + 5*_line_w  , _x1);
}
void SegmentDigit::setLineWidth(int width, boolean keep){
	_line_w = width;
	_keep_line_w = keep;
}
void SegmentDigit::setGap(int gap, boolean keep){
	_gap = gap;
	_keep_gap = keep;
}
int SegmentDigit::drawInt(int val){
	int mdigits[_nr_digits];
	for (int i = 0; i< _nr_digits; i++ ){
		mdigits[i] = 0;
	}
	if ( val >= 10000 ) {
		return 0;
	}
	int x = _nr_digits-1;
	while (val && x >= 0){
		mdigits[x]= val % 10;
		val  = ( val - mdigits[x] ) / 10;
		x--;
	}
	for (int i = 0; i<_nr_digits; i++ ) {
		drawDigit(mdigits[i], i);
	}
	return _last_y;
}



int SegmentDigit::drawFloat(float val,int digits){
	_nr_digits = 4;
	return drawFloat(val);
}

int SegmentDigit::drawFloat(float val){

	int my_digits = _nr_digits;
	int dec_point = my_digits;
	int intval = int(val);
	float rest = val - intval;
	int x = _nr_digits-1;
	if ( intval < 10 ) {
		dec_point = 1;
	}else if (intval < 100){
		dec_point = 2;
	}else if (intval < 1000){
		dec_point = 3;
	}
	if ( dec_point != _dec_point){
		deleteAll();
	}

	setNrDigits(dec_point);
	drawInt(intval);
	if (dec_point == my_digits){
		_dec_point = dec_point;
		return _last_y;
	}
	
	_dot_y = _last_y + _line_w;
	drawDot(_dot_y , false );
	int dec = 0;
	if (dec_point == 1 ){
		dec = int( 1000 * rest + 0.5);
		setNrDigits(3);
	}else if (dec_point == 2 ){
		dec = int( 100 * rest + 0.5);
		setNrDigits(2);
	}else if (dec_point == 3 ){
		dec = int( 10 * rest + 0.5);
		setNrDigits(1);
	}
	_dec_point = dec_point;
	drawInt(dec);
	return _last_y;
}


 void SegmentDigit::drawTime(int hi, int lo ){// displays hi:lo, 
 
  	setNrDigits(2);
 	drawInt(hi);
    drawDoubleDot(_last_y + _line_w, false);
    drawInt(lo);
    setPosition(_min_y,_x1);
 }
 void SegmentDigit::drawTime(int h, int m, int s ){// displays hh:mm:ss, 
 	  	setNrDigits(2);
 	drawInt(h);
    drawDoubleDot(_last_y + _line_w, false);
    drawInt(m);
    drawDoubleDot(_last_y + _line_w, false);
    drawInt(s);
    setPosition(_min_y,_x1);
 }

void SegmentDigit::drawDigit(int digit, int offset ) {
	for (int i = 0; i < 7 ; i++ ){
		drawSegment( i,  offset, digits[digit][i]);
	}
}

void SegmentDigit::deleteAll(){

	for (int i=0; i < 4 ; i++){		
		if ( _dec_point == i && _dot_y > 0 ) {
	
			//drawDot(_dot_y , true );
			_dot_y = 0;
		}
		for (int j=0; j<7; j++){
			drawSegment(j,i,true);
	
		}	
	}
    //setPosition(_min_y,_x1);
    Serial.println(_min_y);
	_drawRectCallback(_min_y+2, _min_x, _max_w, _max_h, true );
}

void SegmentDigit::deleteAll(int y, int x, int w , int h){
	 _drawRectCallback(y,x, w, h, true );
}

void SegmentDigit::drawSegment( int idx,  int offset, boolean del){
	int rect[4];
	int dx1 = _x1;
	int dx2 = _x2;
	int dx3 = _x3;
	int dy1 = offset == 0 ? _y1 : ( offset * (_segment_width + _line_w)) +  (offset * _gap ) +_y1;
	int dy2 = offset == 0 ? _y2+_line_w : dy1+_segment_width + _line_w;
	if (idx == 0 ) {
		rect[0] = dy1-_line_w; 
		rect[1] = dx1;
		rect[2] = dy2-dy1+_line_w;
		rect[3] =_line_w ;
	}else if (idx == 1 ) {
		rect[0] = dy1-_line_w; 
		rect[1] = del ? dx1+_line_w : dx1;
		rect[2] =_line_w ;
		rect[3] = dx2-dx1;
	}else if (idx == 2 ) {
		rect[0] = dy2-_line_w; 
		rect[1] = del ? dx1+_line_w : dx1 ;
		rect[2] = _line_w;
		rect[3] = dx2-dx1;
	}else if (idx == 3 ) {
		rect[0] = dy1; 
		rect[1] = dx2;
		rect[2] =dy2-dy1 ;
		rect[3] = _line_w;
	}else if (idx == 4 ) {
		rect[0] = dy1-_line_w; 
		rect[1] = dx2;
		rect[2] = _line_w;
		rect[3] = dx3-dx2+_line_w;
	}else if (idx == 5 ) {
		rect[0] = dy2-_line_w; 
		rect[1] = dx2;
		rect[2] = _line_w;
		rect[3] = dx3-dx2+_line_w;
	}else if (idx == 6 ) {
		rect[0] = dy1-_line_w; 
		rect[1] = dx3 ;
		rect[2] =dy2-dy1 ;
		rect[3] = _line_w;
	}
  
	_last_y = dy2;
	_max_w = (_nr_digits * _line_w + rect[0] + rect[2] - _min_y);
	_max_h = (rect[1] + rect[3] - _min_x);
	_drawRectCallback(rect[0] ,rect[1], rect[2], rect[3], del );
}