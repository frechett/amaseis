//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         xfm.h
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for XfmLog (TWindow).
//
//----------------------------------------------------------------------------
#if !defined(xfmlog_h)              // Sentry, use file only if it's not already included.
#define xfmlog_h

//{{TWindow = XfmLog}}
class XfmLog : public TWindow {
  public:
    XfmLog(TWindow* parent, KSAC *ksac, bool inst_resp = false, const char far* title = 0, TModule* module = 0);
    virtual ~XfmLog();
  private:
    TButton *back;						// Termination button
    void Paint(TDC&, bool, TRect& rect);
    void SetupWindow();
    void EvLButtonUp(UINT, TPoint &p);
    void EvMouseMove(UINT, TPoint &p);
    void EvLButtonDown(uint modKeys, TPoint& point);
    void DrawFrame(TDC &dc);										// Draw frame
    void DrawCurve(TDC &dc);										// Draw fourier transform
    float getAmplitude(int y);
    void CmBack();
    bool CanClose() {return true;}
    TEdit *amplitude;			// Place to show amplitude
    TEdit *period_box;		// Place to show period
    TEdit *frequency_box;	// Place to show frequency
   	TEXTMETRIC system_tm;     // Text metrics for system font
    int cheight;							// Character height
    int cwidth;								// Character width
    int lmargin;							// left margin
    int rmargin;							// right margin
		int tmargin;							// Top margin
  	int bmargin;							// Bottom margin
    int x_offset, y_offset;
    TRect cr, vr;
    int y0;
    int x0;
    float width, height;
    int h_tic;								// Height of tick marks
   	float xsf;								// x-scale factor
 		float ysf;								// y-scale factor
		float *data;							// Transformed data
    float *amp;								// Amplitude data
    float delta_f;						// Frequency interval
    float max_f;							// Maximum frequency
    float min_x, max_x;
    float min_y, max_y;
    int npts;									// Number of data points
    float depmax;							// Dependent variable max
    float depmin;							// Dependent variable min
    bool inst_response;				// Plotting instrument response?
	DECLARE_RESPONSE_TABLE(XfmLog);

};    //{{XfmLog}}


#endif  // xfm_h sentry.

