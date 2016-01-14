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
//  Class definition for Xfm (TWindow).
//
//----------------------------------------------------------------------------
#if !defined(xfm_h)              // Sentry, use file only if it's not already included.
#define xfm_h
// Butterworth low-pass filter;
//
class ButterworthLP {
	public:
  	// Constructor:
    // sps: sampling rate, samples/sec
    // fc: cutoff frequency, Hz
    // n_order: order of Butterworth filter. Must be even
    ButterworthLP();
  	ButterworthLP(double sps, double fc, int n_order); 		// Constructor: samples/sec; cutoff frequency
    ~ButterworthLP();		// Destructor
    void Filter(float *data, int npts);		// To filter a complete record
  	double NextElement(double new_sample);
  private:
  	int n_order;
    int n2m1;		// n/2 - 1;
    bool nyquist;	// Exceed Nyquist?
    double sps, fc;
    // create enough space for up to 8th order
  	double a[4][3], b[4][3];		// Coefficients
    double x[4], y[4];						// Place to store previous values
};
class ButterworthHP {
	public:
  	// Constructor:
    // sps: sampling rate, samples/sec
    // fc: cutoff frequency, Hz
    // n_order: order of Butterworth filter. Must be even
    ButterworthHP();
  	ButterworthHP(double sps, double fc, int n_order); 		// Constructor: samples/sec; cutoff frequency
    ~ButterworthHP();		// Destructor
    void Filter(float *data, int npts);		// To filter a complete record
  	double NextElement(double new_sample);
  private:
  	int n_order;
    int n2m1;		// n/2 - 1;
    double sps, fc;
  	double a[4][3], b[4][3];		// Coefficients
    double x[4], y[4];						// Place to store previous values
};

//{{TWindow = Xfm}}
class Xfm : public TWindow {
  public:
    Xfm(TWindow* parent, KSAC *ksac, bool inst_resp = false, const char far* title = 0, TModule* module = 0);
    virtual ~Xfm();
    static void four1(float *data,int nn,int isign);
    static void realft(float *data,int n,int isign);
    static void McClureLongPeriod(float *data, int n, double sps);
    static void McClureLowPass(float *data, int n, double sps, double PeriodFc);
    static void McClureHighPass(float *data, int n, double sps, double PeriodFc);
    static ButterworthLP HelicorderFilterLP;
    static ButterworthLP EventFilterLP;
    static ButterworthHP HelicorderFilterHP;
    static ButterworthHP EventFilterHP;
  private:
    TButton *back;						// Termination button
    void Paint(TDC&, bool, TRect& rect);
    void SetupWindow();
    void EvLButtonUp(UINT, TPoint &p);
    void EvMouseMove(UINT, TPoint &p);
    void EvLButtonDown(uint modKeys, TPoint& point);
    void DrawFrame(TDC &dc);										// Draw frame
    void DrawCurve(TDC &dc);										// Draw fourier transform
    double getPeriod(int x);
    double getAmplitude(int y);
    void CmBack();
    bool CanClose() {return true;}
    TEdit *periodexp;
    TEdit *period;   	// Place to show period as mouse moves
    TEdit *amplitude;	// Place to show amplitude
    TRadioButton *p_radio;
    TRadioButton *f_radio;
    //bool CmSpinGain(TNmUpDown& not);
    bool CmSpinPeriod(TNmUpDown& not);
    void BNClickPeriod();
    void BNClickFrequency();
    TUpDown *spin_period;
    int period_expansion;
    TRect period_box, frequency_box;
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
    double width, height;
    int h_tic;								// Height of tick marks
   	double xsf;								// x-scale factor
 		double ysf;								// y-scale factor
		float *data;							// Transformed data
    double *amp;								// Amplitude data
    double delta_f;						// Frequency interval
    double max_f;							// Maximum frequency
    int npts;									// Number of data points
    double depmax;							// Dependent variable max
    bool inst_response;				// Plotting instrument response?
	DECLARE_RESPONSE_TABLE(Xfm);

};    //{{Xfm}}


#endif  // xfm_h sentry.

