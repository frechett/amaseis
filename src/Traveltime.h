//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         raveltime.h
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TravelTime (TWindow).
//
//----------------------------------------------------------------------------
#if !defined(raveltime_h)              // Sentry, use file only if it's not already included.
#define raveltime_h

//{{TWindow = TravelTime}}
class TravelTime : public TWindow {
  public:
    TravelTime(TWindow* parent, KSAC *ksac, const char far* title = 0, TModule* module = 0);
    virtual ~TravelTime();
    void SetDepth(float d);
    //void SetKsac(KSAC *k) {ksac = k;}
  private:
    TRect cr, vr;
    KSAC *ksac;
    void Paint(TDC&, bool, TRect& rect);
    void SetupWindow();
    void DrawFrame(TDC &dc);										// Draw frame
    void DrawCurves(TDC &dc);										// Draw seismogram trace
    void DrawBitmap(TDC &dc,TRect *pr = 0);
    void DrawTrace(TDC &dc);
    TRect DrawPicks(TDC &dc);
    void EvLButtonDown(UINT, TPoint &p);
		void EvLButtonUp(UINT, TPoint &p);
		void EvMouseMove(UINT, TPoint &p);
    TBitmap *MakeBitmap(TDC &dc);
    TButton *back;						// Termination button
    float depth;
    void CmBack();						// Exit
  	float xsf;								// x-scale factor
 		float ysf;								// y-scale factor
    float time_length;				// x-axis extent
    float distance_length;		// Y-axis extent
    int y0;
    float width, height;
    TBitmap *bm;      				// Bitmap of screen in area of trace without trace
   	TEXTMETRIC system_tm;     // Text metrics for system font
    int cheight;							// Character height
    int cwidth;								// Character width
    int lmargin;							// left margin
    int rmargin;							// right margin
		int tmargin;							// Top margin
  	int bmargin;							// Bottom margin
    int h_tic;								// Height of tick marks
    bool moving;							// Flag to tell if we are moving
    int x_offset, y_offset;
    TPoint mouse;							// Mouse position
    float picks[10];					// Time of picks
    int n_picks;							// Number of picks

 	DECLARE_RESPONSE_TABLE(TravelTime);
};    //{{TravelTime}}


#endif  // raveltime_h sentry.

