//----------------------------------------------------------------------------
//  Project AS1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    AS1 Application
//  FILE:         client.h
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for Client (TMDIClient).
//
//----------------------------------------------------------------------------
#if !defined(client_h)              // Sentry, use file only if it's not already included.
#define client_h

//#include "as1.rh"            // Definition of all resources.

// Timer ids
enum TimerIDs {
	TIMER_INIT = 5,
	TIMER_SECOND,		// Ticks every second
	TIMER_GET_SAMPRATE,
	TIMER_MINUTE,
  TIMER_EVENT
};

// Class to display a "Please wait" message
class PleaseWait : public TDialog {
  public:
    PleaseWait(TWindow* parent, TResId resId = IDD_WAIT, TModule* module = 0);
    virtual ~PleaseWait();
};


//{{TMDIClient = Client}}
class Client : public TMDIClient {
  public:
    Client(TModule* module = 0);
    virtual ~Client();
    //void OpenFile(const char* fileName = 0);

    void writetime(SYSTEMTIME utc,TDC &dc);		// Write current time
    void PrintPaint(TDC&, bool, TRect& rect);
    void Paint(TDC&, bool, TRect& rect);
    // Contains current data being digitized
   	HourRecord *hourrecord;						// Current hour record
  	//Event *event;											// Event produced from continuous record of loaded from file
		KSAC **OpenEvent(int &n);	// Uses Explorer-like file open dialog
		void OpenMRUEvent(char *filename);
    TCPClient *tcpclient;				// TCP connection instant if we are a client
    int GetOffset() {return h_offset;}
    void SetOffset(int offset);

//{{ClientVIRTUAL_BEGIN}}
  private:
		KSAC ** OpenEvent(char *filename);
  	// Our continuous record window which is the first child window
    virtual void SetupWindow();
    void SetFileFilter();
    void CmFileOpen();
    void CmFileClose();
    //void CmFilter();
    void CmGain();
    bool CanClose();
    bool canclose;
    void CmFilePrint();
    void CmWriteBitMap();
    void CmFilePrintSetup();
    void CmPrintEnable(TCommandEnabler& tce);
    void CmExtract();		// Extract an earthquake
    void CmDelete();		// Delete these hours
    void CeDeleteEnable(TCommandEnabler& tce);
    long EvMouseWheel(UINT wParam, LONG lParam);
    void CreateEq(KSAC **ksac,int n);
    void EvSize(uint sizeType, TSize& size);
    void CmStation();
    void CmSetZero();
    void CmCloseChildren();
    void CmShowData();
    void CmHelpTopics();
    void CmHelpAbout();
    void CmDevice();
    void CmComPort();
    void CmCalibration();
    void CmNow();
    void EvLButtonDown(UINT, TPoint &p);
    void EvLButtonUp(UINT, TPoint &p);
    void EvMouseMove(UINT, TPoint &p);
    void EvTimer(UINT id);
    //void DrawNewData(TDC &dc);									// Draw new data
    void DrawNewData(TDC &dc, TMemoryDC &memory_dc);									// Draw new data
    int DrawFrame(TDC &dc,TRect &rect);				// Draw frame. Returns left margin
    void DrawTraces(TDC &dc,TRect &rect);										// Draw seismogram traces
    void PrintFrame(TDC &dc,TRect &rect);				// Print frame
    void PrintTraces(TDC &dc,TRect &rect);			// Printseismogram traces
    void CmForwardBackward();										// Move day, week, month, year
    void SetMark(TPoint p);
    void Mark(TDC &dc,TPoint p);	// mark new area
    void Mark(TDC &dc);						// mark old area
    void Mark();
    void Mark(TPoint p);
    //void CmUpload24Hours();
    //void CeUpload24HoursEnable(TCommandEnabler& tce);

    void EvVScroll(UINT code, UINT pos, HWND wnd);
    void GetMargins(TDC &dc,int &lmargin, int &rmargin,int &cwidth,int &cheight);
    TRect GetVR(TRect cr,int lmargin, int rmargin, int cheight);			// Get scaling viewport
    TRect GetVR(TDC &dc,TRect cr);
    TRect GetBM();			// Get bottom margin
    float GetXsf(TDC &dc,TRect cr);							// Get x-scaling factor
    float GetYsf(TDC &dc,TRect cr);
    float GetHsf(TDC &dc,TRect cr);							// Hour
    float GetHsf();
    bool digitizing;														// Flag
		TPoint invXform(int h, int x,int width,float xsf);
    TPoint Xform(int i, float d, int hour,float xsf,float ysf, float hsf);				// Transform
    float XformHour(int i, float xsf);
    //TPoint Xsecform(int i, int d, int hour,float xsf, float xsec_sf,float ysf); 		// Same as Xform but i in seconds
    SerialIO *serialio;
    TScrollBar *scrollbar;
    TBitmap *iris_bmap;
		// FILE *fp_debug;

    // Amount scrolled
    int h_offset;
    int max_scroll;
    int this_minute;
    int timer_rate;
    int refresh_interval;
    int ctr;					// Counts timer ticks
    TRect time_w;			// Time window
    TRect mouse_w;		// Show time for mouse position

    // Following are for marking a section for copying
    bool down;
    bool painting;		// true when in Paint()
    int line1, line2, x1, x2;		// End points of section
    int minute_ctr;		// Keeps track of minutes for saving screen image

    // enum for calculating sample rate
    enum CompSampRate {
    	START,
      COMPUTING,
      DONE
    };
    bool getSR;				// Flag
    bool jump_mouse;		// Flag so don't do mouse stuff until things are initialized
    CompSampRate csr;

    void CmContinuous();
    void CmDateTime();
    int lines_per_screen;
    PleaseWait *pleasewait;
    TCPThread *tcpthread;				// Thread handling client requests
    ListenThread *listenthread;	// Listen for connection attempts
    TMemoryDC *memory_dc;
    TBitmap *memory_bm;

//{{ClientRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(Client);
};    //{{Client}}

// WriteBitMap is outside class definition since it is used in client.cpp
// and in Event.cpp
extern void WriteBitMap(TWindow *parent,char *filename);
extern void ErrorBox(TWindow *p,UINT res,char *s);
extern char *GetResString(UINT Id);



#endif  // client_h sentry.
