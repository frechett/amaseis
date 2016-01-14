//----------------------------------------------------------------------------
//  Project AS1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    AS1 Application
//  FILE:         child.h
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for Child (TMDIChild).
//
//----------------------------------------------------------------------------
#if !defined(event_h)              // Sentry, use file only if it's not already included.
#define event_h

typedef struct event_data {
  char kevnm[32];	// Event name
  char yyyymmddhh[16];	// Name of event as YYYYMMDDHH for Seismic Waves
  char kstnm[16];	// Station name (code, really)
	float evla;		// Event latitude
  float evlo;		// Event longitude
  float evdp;		// Event depth
  float ev_mb;	// Event body wave magnitude
  float ev_ms;	// Event surface wave magnitude
  float ev_ml;	// Event local magnitude
  float ev_mw;	// Event moment magnitude
  int year, month, day, jday, hour, minute, second, msecond;
  float origin;	// Time, in sec, from origin time to begining of record
} EVENT_DATA;
//{{TMDIChild = Child}}

class PickInstructions : public TDialog {
	public:
  	PickInstructions(TWindow* parent, TResId resId = IDD_PICK_INSTRUCTIONS, TModule* module = 0);
    TStatic *pickInstructions;
    virtual ~PickInstructions();
};

//class Event : public TMDIChild {
class Event : public TWindow {
  public:
    //Event(TMDIClient& parent, const char far* title, KSAC **sacs,int n,
    //	TWindow* clientWnd, bool shrinkToClient = false, TModule* module = 0);
   	Event(TWindow* parent, const char far* title, KSAC **sacs, int n, TModule* module = 0);
    virtual ~Event();
    TScrollBar *scrollbar;
    void Paint(TDC&, bool, TRect& rect);
    int filtered;		// Flag to tell if a filter has been applied
    								// If filtered == 0, no filtering
                    // if filtered == 1, one filter applied
                    // if filtered > 1, multiple filters applied
  private:
    TRect cr, vr, bmargin;
    int cheight, cwidth;
    float cutoff_1;
    float cutoff_2;
    float c0, c1, c2, d1, d2;
    KSAC **ksacs;						// Pointer to the SAC instances
    int n_ksac;							// Number of SAC instances active
    long double zTime;			// Starting time for window in seconds since 1601
    float time_length;			// Length of window in seconds

  protected:
    void SetupWindow();
    void CmFileSave();
    void CmFileOpen();
    void CmFileSaveAs();
		void EvSize(uint sizeType, TSize& size);
    //void CmGain();
    void CmTimeMagnify();
    void CmExtractDisable(TCommandEnabler& tce);
    void CmWriteBitMap();
    void CmEventDialog();
    void CmFilter();
    void CmFilterLP();				// Bob McClure long period filter
    void CmFilterLowPass();		// Bob McClure low pass filter
    void CmFilterHighPass();	// Bob McClure high pass filter
    void CmFilterLPFFT(); 		// Same long period filter using FFT
    void CmUnfilter();
    void CmZTime();
    void CmSeisStation();
    void CmFourierXfm();
    void CmFourierXfmLinear();
    // Calls dialog for filter cutoff
    void CmMedianFilter();
    void CmInstResponse();
    void DrawFrame(TDC &dc,TRect &);		// Draw frame
    void DrawTrace(TDC &dc, float t0 = 0, float t2 = 0);				 		// Draw seismogram trace
    void EvHScroll(UINT code, UINT pos, HWND wnd);
    void CmPick();
    void CmArrival();
    void CmFileClose();
    void CmFilePrint();
    void CmMagnitude();
    void CmMagnitude_ms();
    void CmMagnitude_mbLg();
    void CeContinuous(TCommandEnabler& tce);
    void DrawPick(int i);
    void EvLButtonUp(UINT, TPoint &p);
    void EvMouseMove(UINT, TPoint &p);
    void EvLButtonDown(uint modKeys, TPoint& point);
    void SetMark(TPoint p);
    void CmExtract();
    void Mark(TDC &dc,TPoint p);	// mark new area
    void Mark(TDC &dc);						// mark old area
    void Mark();
    void CmScale();
   	void EvTimer(UINT id);
    void Mark(TPoint p);
    float getTime(int x);
    float getAmplitude(int y);
    int x1, x2;
    int lmargin, margin;
    //TEdit *gainquery;
    PickInstructions *pick_instructions;
    TEdit *timeexp;
    TEdit *time;     	// Place to show time as mouse moves
    TEdit *amplitude;	// Place to show amplitude
    //bool CmSpinGain(TNmUpDown& not);
    bool CmSpinTimex(TNmUpDown& not);
    TEdit *time_accel;
    TButton *back;						// Termination button
    TButton *play;						// Play seismogram in"real time"
    TButton *stop;						// Stop playing
    TUpDown *spin_time_accel;
    float accel;
    int accel_factor;
    bool playing;							// Playing back slowly
    bool CmSpinTimeAccel(TNmUpDown& not);
    //TUpDown *spin_gain;
    TUpDown *spin_time;
    void CmBack();						// Exit
    void CmPlay();						// Play event back in real time
    void CmStop();						// Stop play
    float t0, t1;							// Time frame to display
    int play_increment;				// Timer increment in milliseconds
    PickDialog *pick_dialog;
    //Gain *gain_dialog;
    TravelTime *travel_time;
    void filter();			// Does filtering
    void filterCoeff();	// Sets up for filtering
    void CmTravelTime();
    float time_expansion;
    float time_offset;
    float max_amplitude;
    bool auto_scale;
    //float gain;
    bool picking;		// Flag to indicate if in "pick-arrivals" state
    int pick;				// Which pick are we working on (0 to 9)
    int width, height, y0;	//width, height, and half-height of scaling viewport
    int offset;			// Offset in pixels
    float origin;					// Time offset
    float fsec;						// Offset to start on a minute
    int hour_save;

  	float xsf;			// x-scale factor in
 		float ysf;			// y-scale factor
  	float *data;		// Pointer to data array
    bool down;			// Is mouse down?
    bool enable_lowpass;
    bool enable_highpass;

 DECLARE_RESPONSE_TABLE(Event);
};


#endif  // event_h sentry.
