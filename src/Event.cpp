//----------------------------------------------------------------------------
//  Project AS1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    AS1 Application
//  FILE:         child.cpp
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of Child (TMDIChild).
//
//----------------------------------------------------------------------------
#include "as1.h"
/***********************
** gcarc()
**
** Compute great circle arc between station and event
** Stolen from FORTRAN program of Francis Wu
**********************/
#define d2r 0.017453292
void gcarc(
  float sla,    // Station latitude
  float slo,    // Station longitude                                                       float slo,    // Station
  float ela,    // Event latitude
  float elo,    // Event longitude
  float &delta, // Great Circle arc in degrees (returned)
  float &baz)   // Azimuth of event as seen from city (returned)
{
//
// Using vector identity
//  (axb).(axc)=(b.c)-(a.b)(a.c)
// to compute central angle (distance delta), az (azimuth of staion
// viewed from source) and baz (azimuth of earthquake viewed from the
// station
//
  float cd, sd, cbaz;
  float slar, dlor, elar;
  float csla, sela;
  float cela, cdlo;
  float ssla;

  slar = (90.- sla) * d2r;
  dlor = (slo - elo) * d2r;
  elar = (90.- ela) * d2r;
  ssla = sin(slar);
  csla = cos(slar);
  sela = sin(elar);
  cela = cos(elar);
  cdlo = cos(dlor);
  cd = ssla*sela*cdlo+csla*cela;
  delta = acos(cd)/d2r;
  sd = sqrt(1.-cd*cd);
  cbaz = (cela-cd*csla)/(sd*ssla);
  if(cbaz>=1)
    baz=0.;
  else if (cbaz<=-1)
    baz= 180.;
  else
    baz = acos(cbaz)/d2r;
  // Adjust for quadrant
  cbaz = elo-slo;
  if(0 < cbaz || cbaz < 180)
    baz = -baz;
  // Convert to postive angles
  if(baz<0)
    baz = 360+baz;
}
//{{Event Implementation}}
PickInstructions::PickInstructions(TWindow* parent, TResId resId, TModule* module)
:
    TDialog(parent, resId, module)
{
  pickInstructions = new TStatic(this, IDC_PICK_INSTRUCTIONS);
}


PickInstructions::~PickInstructions()
{
  Destroy(IDCANCEL);
}


//DEFINE_RESPONSE_TABLE1(Event, TMDIChild)
DEFINE_RESPONSE_TABLE1(Event, TWindow)
	EV_WM_HSCROLL,
  EV_WM_SIZE,
	//EV_COMMAND(CM_GAIN,							CmGain),
	EV_COMMAND(CM_SETTINGS_EVENT,		CmEventDialog),
  EV_COMMAND(CM_FILESAVE, 				CmFileSave),
  EV_COMMAND(CM_FILECLOSE, 				CmFileClose),
  EV_COMMAND(CM_FILESAVEAS, 			CmFileSaveAs),
  EV_COMMAND(CM_FFT, 							CmFourierXfm),
  EV_COMMAND(CM_FFT_LINEAR, 			CmFourierXfmLinear),
  EV_COMMAND(CM_INST_RESPONSE,		CmInstResponse),
  EV_COMMAND(CM_MAGNITUDE,				CmMagnitude),
  EV_COMMAND(CM_MAGNITUDE_MS,			CmMagnitude_ms),
  EV_COMMAND(CM_MAGNITUDE_MBLG,		CmMagnitude_mbLg),
  EV_COMMAND(CM_FILTER, 					CmFilterLowPass),
  EV_COMMAND(CM_FILTER_HIGH_PASS,	CmFilterHighPass),
  EV_COMMAND(CM_FILTER_LONG, 			CmFilterLP),
  EV_COMMAND(CM_UNFILTER, 				CmUnfilter),
  EV_COMMAND(CM_MEDIAN_FILTER,		CmMedianFilter),
  EV_COMMAND(CM_BITMAP, 					CmWriteBitMap),
  //EV_COMMAND(CM_ZTIME,						CmZTime),
	EV_COMMAND_ENABLE(CM_EXTRACT_EQ,CmExtractDisable),
  EV_COMMAND(CM_ARRIVAL, 					CmArrival),
  EV_COMMAND(CM_TRAVEL_TIME,			CmTravelTime),
  EV_COMMAND(CM_S_STATION,				CmSeisStation),
  EV_UDN_DELTAPOS(CM_SPINTIMEX, 	CmSpinTimex),
  EV_UDN_DELTAPOS(CM_SPIN_TIME_ACCEL, 	CmSpinTimeAccel),
  EV_COMMAND(CM_BACK,							CmBack),
  EV_COMMAND(CM_PLAY,							CmPlay),
  EV_COMMAND(CM_STOP,							CmStop),
  EV_COMMAND(CM_SCALE,						CmScale),
  EV_COMMAND(CM_FILEPRINT,				CmFilePrint),
	EV_COMMAND(CM_EXTRACT_EQ,				CmExtract),
  EV_COMMAND_ENABLE(CM_CONTINUOUS,CeContinuous),
  EV_COMMAND_ENABLE(CM_SHOW_DATA,	CeContinuous),
  EV_COMMAND_ENABLE(CM_SET_ZERO,	CeContinuous),
  EV_COMMAND_ENABLE(CM_COM_PORT,	CeContinuous),
  EV_COMMAND_ENABLE(CM_FORWARD_BACKWARD,CeContinuous),
  EV_COMMAND_ENABLE(CM_DATE_TIME,	CeContinuous),
  EV_COMMAND_ENABLE(CM_DEVICE,		CeContinuous),
  EV_COMMAND_ENABLE(CM_NOW,				CeContinuous),

	EV_WM_LBUTTONDOWN,
 	EV_WM_MOUSEMOVE,
  EV_WM_RBUTTONDOWN,
  EV_WM_LBUTTONUP,
  EV_WM_TIMER,
END_RESPONSE_TABLE;

//--------------------------------------------------------
// Event
// ~~~~~~~~~~
// Construction/Destruction handling.
//
//Event::Event(TMDIClient& parent, const char far* title, KSAC **sacs,int n,
//		TWindow* clientWnd,	bool shrinkToClient,
//		TModule* module) :
//  TMDIChild(parent, title, clientWnd,	shrinkToClient, module)
//{
Event::Event(TWindow *parent, const char far* title, KSAC **sacs, int n,
	TModule* module)
:
  TWindow(parent, title, module)
{

  // INSERT>> Your constructor code here.
  Attr.Style |= WS_HSCROLL;
  Attr.Style |= WS_MAXIMIZE | WS_CHILD;
  Attr.Style &= (~WS_THICKFRAME) & (~WS_HSCROLL) & (~WS_VSCROLL);
		//& (~WS_OVERLAPPED)
		//& (~WS_MAXIMIZEBOX)
		//& (~WS_BORDER)
  ;
  scrollbar = 0;
  time_expansion = 1;
  time_offset = 0;
  //gain = 1;
  back = 0;
  ksacs = sacs;
  n_ksac = n;
  filtered = 0;
  play_increment = 200;		// Milliseconds for timer
  auto_scale = true;
  max_amplitude = 100;
  // Set cutoff to sampling frequency as a default
  //if(n_ksac > 0) {
  //  float delta = ksacs[0]->GetSACHV(DELTA);
  //  if(delta > 0)
  	cutoff_2 = profile->bp_high_cutoff;
    cutoff_1 = profile->bp_low_cutoff;
  //}
  picking = false;
  pick_instructions = 0;
  travel_time = 0;
  //gainquery = 0;
  timeexp = 0;
  time = 0;
  play = 0;
  stop = 0;
  time_accel = 0;
  accel = 1;
  accel_factor = 0;
  amplitude = 0;
  playing = false;
  spin_time = 0;
  x1 = x2 = 0;
  down = false;
  pick_dialog = 0;
  spin_time_accel = 0;
  data = 0;
  enable_lowpass = enable_highpass = false;
  profile->event_filter_type = Profile::NO_FILTER;
}


Event::~Event()
{
  // Destroy(IDCANCEL);  Roger took this out and added:
 	this->Destroy();                    //Roger added "this"

  // INSERT>> Your destructor code here.
	if(this->scrollbar)
  	delete this->scrollbar;
  // Roger delete following lines
  // 2003.12.08 Alan put back in. Roger said this code
  // Wasn't getting executed but I found it is
  if(ksacs) {
  	for(int i=0;i<n_ksac;i++)
    	if(ksacs[i])
  			delete ksacs[i];
  }
  if(back)
  	delete back;
	delete pick_instructions;
  //delete gain_dialog;
  //if(gainquery)
  //	delete gainquery;
  //if(spin_gain)
  //	delete spin_gain;
  if(timeexp)
  	delete timeexp;
  if(play)
  	delete play;
  if(stop)
  	delete stop;
  if(time_accel)
  	delete time_accel;
  if(spin_time_accel)
  	delete spin_time_accel;
  if(time)
  	delete time;
  if(amplitude)
  	delete amplitude;
  if(spin_time)
  	delete spin_time;
  //if(pick_dialog)
  //	delete pick_dialog;
  if(travel_time)
  	delete travel_time;
}

void Event::SetupWindow()
{
  // Default SetUpWindow processing.
  //TMDIChild::SetupWindow();
  TWindow::SetupWindow();
  //gain_dialog->Create();
  //gain_dialog->SetGain(gain);
  //gain_dialog->SetX(time_expansion);
  //gainquery->Create();
  //SetCaption(ksacs[0]->GetFileName());
  //SetIconSm(GetApplication(), IDI_MDIAPPLICATION);
  //pick_dialog->Create();
  //pick_dialog->ShowWindow(SW_HIDE);		// Hide this window until we need it
  //travel_time->SetKsac(ksacs[0]);
  //travel_time->ShowWindow(SW_HIDE);
  // char junk[48];
  // sprintf(junk,": %d",sizeof(Header24));
  // MdiClient->MessageBox("In event SetupWindow");
  // 2007.02.13: Write a *.csv file of this data
  FILE *fp = fopen("EventData.csv","wt");
	KSAC *ksac = ksacs[0];
  int npts = ksac->GetSACHV(NPTS);
  float delta = ksac->GetSACHV(DELTA);
  float *data = ksac->GetData();
  if(fp) {
  	fprintf(fp,"TIME,AMPLITUDE\n");
    float t = 0;
    float *d = data;
    for(int i=0;i<npts;i++) {
    	fprintf(fp,"%f,%f\n",t,*d++);
      t += delta;
    }
  	fclose(fp);
  }

}

void Event::CeContinuous(TCommandEnabler& tce)
{
	tce.Enable(false);
}
void Event::EvSize(uint sizeType, TSize& size)
{
	if(timeexp) {
  	delete timeexp;
    timeexp = 0;
    delete spin_time;
    spin_time = 0;
    delete time;
    time = 0;
    delete amplitude;
    amplitude = 0;
  }
  if(scrollbar) {
  	delete scrollbar;
    scrollbar = 0;
  }
	Invalidate();
}

#if 0
// Don't use. This is set in EventDialog
// Keep around for example of using SACTimeToSystemTime
void Event::CmZTime()
{
	KSAC *ksac = ksacs[0];
	SYSTEMTIME st = ksac->SACTimeToSystemTime();
  if (ZTimeDialog(this,st).Execute() == IDOK) {
  	ksac->SystemTimeToSACTime(st);
    Invalidate();
  }
}
#endif

void Event::CmFourierXfmLinear()
{
	KSAC *sac = ksacs[0];
  TWindow *xfm = new Xfm(this,sac);
  xfm->Attr.Style |= WS_MAXIMIZE;
  xfm->Create();
  xfm->ShowWindow(SW_SHOW);
  //MessageBox("Done FFT");
}
void Event::CmFourierXfm()
{
	KSAC *sac = ksacs[0];
  TWindow *xfm = new XfmLog(this,sac);
  xfm->Attr.Style |= WS_MAXIMIZE;
  xfm->Create();
  xfm->ShowWindow(SW_SHOW);
  //MessageBox("Done FFT");
}
void Event::CmInstResponse()
{
	KSAC *sac = ksacs[0];
  TWindow *xfm = new Xfm(this,sac,true);
  xfm->Attr.Style |= WS_MAXIMIZE;
  xfm->Create();
  xfm->ShowWindow(SW_SHOW);
  //MessageBox("Done FFT");
}
void Event::CmBack()
{
  CmFileClose();
}
void Event::CmPlay()
{
	playing = true;
  if(play) {
  	play->Destroy();
    delete play;
    Invalidate();
    play = 0;
    playing = true;
    SetTimer(TIMER_EVENT,play_increment);
    t0 = t1 = 0;
  }
}

void Event::CmStop()
{
	playing = false;
  if(stop) {
  	stop->Destroy();
    delete stop;
    stop = 0;
    t0 = t1 = 0;
    playing = false;
    KillTimer(TIMER_EVENT);
    Invalidate();
  }
}

void Event::EvTimer(UINT id) {
 	TClientDC dc(*this);
  t1 += accel*play_increment*.001;
	DrawTrace(dc, t0, t1);
  t0 = t1;
}

void Event::CmSeisStation()
{
	if(ksacs) {
  	KSAC *sac = ksacs[0];
    SeisStation *ss = new SeisStation(this,sac);
    int ii = ss->Execute();
    delete ss;
    if(ii == IDOK) {
    	Invalidate();
    }
	}
}

void Event::CmExtract()		// Extract an earthquake
{
	if(playing)
  	return;
	// Compute limits
  if(x1 == x2) {
  	MessageBox("Mark an area before extracting");
    return;
  }
  if(x1 > x2) {
  	int xtmp = x2;
    x2 = x1;
    x1 = xtmp;
  }
  down = false;
  // Convert to time
  float t1 = (x1-vr.Left()+offset)/xsf;
  if(t1 < 0)
  	t1 = 0;
  float t2 = (x2-vr.Left()+offset)/xsf;
  if(t2 < 0)
  	t2 = 0;
  if(ksacs) {
  	for(int i=0;i<n_ksac;i++)
    {
    	if(ksacs[i]) {
  			KSAC *ksac = ksacs[i];
        float *data = ksac->GetData();
        float delta = ksac->GetSACHV(DELTA);
				float sps = 1/delta;
  			int n1 = t1*sps;
  			int npts = 1 + (t2 - t1)*sps;
        // Create some space for the new data
        //float *data2 = new float[npts];
        //for(int j=0;j<npts;j++)
        //	data2[j] = data[n1+j];
        //delete data;
        ksac->SetData((float *)&data[n1],npts);
        delete data;
        // Extract the Original data
        data = ksac->PSACDataOriginal;
        ksac->PSACDataOriginal = new float[npts];
        memcpy(ksac->PSACDataOriginal,(float *)&data[n1],npts*sizeof(float));
        delete data;
        //ksac->ResetOriginal();
        // Adjust for the time offset
        ksac->AdjustZTime(t1);
  			ksac->SetSACHV(E,(float)(delta*(npts-1)));
        // Adjust picks
        for(int i=0;i<10;i++) {
  				float p = ksac->GetTn(i);
   		 		if(p != 0 && p != -12345.0) {
          	p -= t1;
            ksac->PutTn(i,p);
    			} else
     	 			break;
        }
      }
    }
    x1 = 0;
    x2 = 0;
  	time_expansion = 1;
  	time_offset = 0;
    Invalidate();
  }
}

void Event::CmFileClose()
{
	//Destroy();
	//CloseWindow();
  // Roger Sparks took out above lines and added following:
  this->~Event(); //Roger tried this, seems to work very well, now Amaseis closes smoothly
}

bool Event::CmSpinTimex(TNmUpDown& not)
{
	char buffer[16];
  int x = not.iPos + not.iDelta;
  if(x < 1) x = 1;
  spin_time->SetPos((int)x);
	time_expansion = x;
	sprintf(buffer,"%g",time_expansion);
	timeexp->SetText(buffer);
  Invalidate();
  return true;
}

bool Event::CmSpinTimeAccel(TNmUpDown& not)
{
	char buffer[16];
  int x = not.iPos + not.iDelta;
  if(x < 0) {
  	x = 0;
  }
	spin_time_accel->SetPos((int)x);
  accel_factor = x;
  accel = pow(2,accel_factor);
  sprintf(buffer,"%g",accel);
  time_accel->SetText(buffer);
  //t0 = 0;		// So the screen will get redrawn
  //Invalidate();
  return true;
}

//--------------------------------------------------------
// Client
// ~~~~~~~~~~
// Menu File Print command
//
void Event::CmFilePrint()
{
	if(playing)
  	return;
  // Create Printer object if not already created.
  //
  TAS1* theApp = TYPESAFE_DOWNCAST(GetApplication(), TAS1);
  if (theApp) {
    if (!theApp->Printer)
      theApp->Printer = new TPrinter(theApp);
		/********************/
  	if (theApp->Printer) {

    	TWindowPrintout printout("AmaSeis Event", this);
    	//
    	// Using OWL's banding support under Win95 _when_ the EXE's marked 4.0
    	// (under Options|Project|Linker|General) will cause the print to fail.
    	//
    	if(!TSystem::IsWin95())
      	printout.SetBanding(false);
    	theApp->Printer->Print(this, printout, true);
  	}
  }
}


#define UNDEF -12345.0

void Event::CmTravelTime()
{
	if(playing)
  	return;
	// Hide the gain-filter dialog
  //gain_dialog->ShowWindow(SW_HIDE);
  // Dialog to get depth
  down = false;
  x1 = x2 = 0;
  float depth = ksacs[0]->GetSACHV(EVDP)/1000;
  if(depth == 0 || depth == -12345.0)
  	depth = 33;
  DepthDialog *dd = new DepthDialog(this,depth);
  if(dd->Execute() == IDOK) {
  	depth = dd->GetDepth();
    delete dd;			// Added 2003.11.05.  Roger Sparks found it missing
    ksacs[0]->SetSACHV(EVDP,depth*1000);
  }
  picking = false;
  if(travel_time)
  	delete travel_time;
  travel_time = new TravelTime(this,ksacs[0],"Travel Time Curves");
  travel_time->Create();
	travel_time->SetDepth(depth);
  travel_time->ShowWindow(SW_SHOW);
}

// Set up for picking arrivals
void Event::CmArrival()
{
  if(picking)
  	picking = false;	// Flag to show we are in picking mode
  else {
		// Initialize all pick times
  	for(int k=0;k<n_ksac;k++)
  	ksacs[k]->ClearPickTimes();
  	pick = 0;
  	picking = true;
  }
  if(picking) {
  	if(pick_instructions == 0) {
  		pick_instructions = new PickInstructions(this);
      pick_instructions->Create();
    	pick_instructions->pickInstructions->SetText(
      "Select arrivals with mouse.\n"
  		"Then click on Travel Time Curves.\n"
  		"When you click on PICK again, old picks are erased.");
    }
    pick_instructions->ShowWindow(SW_SHOW);
  } else {
  	if(pick_instructions) {
    	pick_instructions->ShowWindow(SW_HIDE);
    }
  }
  Invalidate();
}
void Event::CmMagnitude()
{
  Magnitude(this).Execute();
}
void Event::CmMagnitude_ms()
{
	Magnitude_ms(this).Execute();
}
void Event::CmMagnitude_mbLg()
{
	Magnitude_mbLg(this).Execute();
}

void Event::EvMouseMove(UINT, TPoint &p)
{
	if(down && !picking) {
    Mark();
    Mark(p);
  }
 	char buffer[32];
  // Get time
  if(!vr.Contains(p))
  	return;
  if(n_ksac == 0)
  	return;
 	int y = p.y - vr.top;
 	int kk = (p.y*n_ksac)/vr.Height();
  if(kk >= n_ksac)
  	kk = n_ksac-1;
  if(kk < 0)
  	kk = 0;
 	KSAC *ksac = ksacs[kk];
  if(ksac == NULL)
    return;
  if(time) {
  	float t = getTime(p.x);
    // 2003.09.22: Compute resolution to display based on criteria suggested
    // by John Lahr:
    // 2004:11.01: Not getting enough resolution.  Improve it.  See below.
    /***************
    Maximum resolution to be shown on screen:
		Sample rate	Displayed Time Resolution
			1 - 9 Hz		1 s
			10 - 99 Hz		0.1s
			> 100 Hz		0.01s
    // New
		Sample rate	Displayed Time Resolution
			1 - 5 Hz		1 s
			5 - 99 Hz		0.1s
			> 100 Hz		0.01s
		Resolution to be shown, but not greater than the maximum determined by the sample rate:
		Pixel Delta Time	Displayed Time Resolution
		(seconds/pixel)
			PDT .gt. 0.1			1 s
			PDT .le.  0.1 and .gt. 0.01	0.1 s
			PDT .le.  0.01			0.01 s
    *********************/
    int res = 0;
    float sps = 1/ksac->GetSACHV(DELTA);
    if(sps >= 5 && sps <= 100) {
    	if(xsf > 5)
      	res = 1;
    } else if (sps > 100) {
    	if(xsf > 50)
      	res = 2;
      else if(xsf >5)
      	res = 1;
    }
   	switch(res) {
    case 0:
    	// no places to right of decimal
    	if(t >= 0) {
  			// Round and convert to ASCII
  			int ss = t + 0.5;
  			int mm = ss/60;
  			int hh = +hour_save+mm/60;
  			mm = mm%60;
  			ss = ss%60;
  			sprintf(buffer,"%d:%02d:%02d",hh,mm,ss);
    	} else
    		strcpy(buffer,"0:00:00");
    	break;
    case 1:
    	// One place to right of decimal
      if(t >= 0) {
  			// Round and convert to ASCII
  			int ss = 10*t + 0.5;
        int f = ss%10;
        ss /= 10;
  			int mm = ss/60;
  			int hh = +hour_save+mm/60;
  			mm = mm%60;
  			ss = ss%60;
  			sprintf(buffer,"%d:%02d:%02d.%1d",hh,mm,ss,f);
    	} else
    		strcpy(buffer,"0:00:00.0");
    	break;
    case 2:
    	// Two places to right of decimal
      if(t >= 0) {
  			// Round and convert to ASCII
  			int ss = 100*t + 0.5;
        int f = ss%100;
        ss /= 100;
  			int mm = ss/60;
  			int hh = +hour_save+mm/60;
  			mm = mm%60;
  			ss = ss%60;
  			sprintf(buffer,"%d:%02d:%02d.%02d",hh,mm,ss,f);
    	} else
    		strcpy(buffer,"0:00:00.00");
    	break;
    }
  	time->SetText(buffer);
  }
  if(amplitude) {
  	// Scale factors are stored in ksac
  	float d = 0;
  	if(ksac->scale) {
  		d = ksac->y_offset + (ksac->y0 - y)/ksac->scale;
  	}
  	sprintf(buffer,"%.4g",d);
    amplitude->SetText(buffer);
  }
}
// record where mouse is when it is set down
void Event::SetMark(TPoint p)
{
	if(playing)
  	return;
  x1 = x2 = p.x;
}

void Event::Mark()
{
	if(playing)
  	return;
	TClientDC dc(*this);
	Mark(dc);
}

void Event::Mark(TDC &dc, TPoint p)
{
	if(playing)
  	return;
	x2 = p.x;
  Mark(dc);
}

void Event::Mark(TPoint p)
{
	if(playing)
  	return;
	TClientDC dc(*this);
  Mark(dc,p);
}

// Mark the section
void Event::Mark(TDC &dc)
{
	if(playing)
  	return;
  TRect cr = GetClientRect();
   	// Compute working area of screen
	TRect vr = TRect(lmargin,margin,cr.Right()-margin,cr.Bottom()-5*cheight);
  viewport(dc,cr,0);
 	if(x1 == x2)
  	return;
	dc.PatBlt(TRect(x1,vr.top,x2,vr.bottom),DSTINVERT);
}

void Event::CmScale()
{
  if (ScaleDialog(this,auto_scale,max_amplitude).Execute() == IDOK) {
  	Invalidate();
  }
}
void Event::EvLButtonUp(UINT, TPoint &)
{
	down = false;
}

// Convert position to a time
float Event::getTime(int x) {
	float t = 0;
  if(xsf != 0) {
  	//t = fsec-origin+(x-vr.Left()+offset)/xsf;
  	t = fsec+(x-vr.Left()+offset)/xsf;
  }
  return t;
}
void Event::EvLButtonDown(uint /* modKeys */, TPoint& point)
{
	if(playing)
  	return;
	if(picking) {
		// Find where the person clicked
  	int x = point.x;
    int kk = (point.y*n_ksac)/vr.Height();
    KSAC *ksac = ksacs[kk];
    if(ksac == NULL)
      return;
		//float t = (x-vr.Left()+offset)/xsf;
    float t = getTime(x);
    // Determine which waveform it is
    //if(kk >= n_ksac)
    //	kk = 0;
  	// We have the time.  Convert to a datapoint
  	//int i = t/delta;
  	// Stuff it into a SAC Tn.  PutTn finds first one available
    // Returns one used
    //for(int k=0;k<n_ksac;k++)
  	//ksac->PutTn(pick,t-fsec+origin);
  	ksac->PutTn(pick,t-fsec);
    //static float fff = fsec;
  	//ksac->PutTn(pick,t);
    pick++;
    //char temp[32];
    //sprintf(temp,"Pick Number %d",pick);
    //pick_dialog->arrival_number->SetText(temp);
    if(pick >= 10)
    	picking = false;
    else {
    	// Draw this pick.
      DrawPick(pick-1);
    }
  }
  else
	if(down == false)
		Mark();
	down = true;
  SetMark(point);
}


void Event::DrawPick(int i)
{
 	TClientDC dc(*this);
  for(int k=0;k<n_ksac;k++) {
  	float t = ksacs[k]->GetTn(i);
  	if(t == 0 || t == UNDEF)
  		continue;
  	int h = height/(6*n_ksac);			// height of mark
		//int x = vr.Left()+((t-fsec)*xsf)-offset;
		int x = vr.Left()+((t)*xsf)-offset;
    int k_height = height/n_ksac;
    y0 = k_height*(k+0.5);
  	plot(dc,x,y0+h,0,2);
  	plot(dc,x,y0-h,RED,2);
  }
}

void Event::CmFileOpen()
{
	if(playing)
  	return;
	int n = 0;
	//KSAC ** t_ksacs = Client::OpenEvent(n);
	KSAC ** t_ksacs = MdiClient->OpenEvent(n);
  if(n) {
		// Delete existing files
    for(int k=0;k<n_ksac;k++)
    	delete ksacs[k];
    ksacs = t_ksacs;
    n_ksac = n;
  }
  // Set default cutoffs
  //if(n_ksac > 0) {
  //  float delta = ksacs[0]->GetSACHV(DELTA);
  //  if(delta > 0)
  	cutoff_2 = profile->bp_high_cutoff;
    cutoff_1 = profile->bp_low_cutoff;
  //}
  Invalidate();
}


void Event::CmFileSave()
{
	if(playing)
  	return;
  for(int k=0;k<n_ksac;k++) {
  	KSAC *ksac = ksacs[k];
 		if(strncmp(ksac->GetFileName(),"untit",5) == 0) {
  		CmFileSaveAs();
      continue;
    }
  	if(ksac->GetFileType() == SACFILE) {
   		ksac->WriteSAC();
    }
   	else if(ksac->GetFileType() == PEPPFILE){
   		PEPP *pepp = new PEPP(ksac);
     	pepp->WritePEPP();
     	delete pepp;
    }
   	else if(ksac->GetFileType() == PSNFILE){
   		PSN *psn = new PSN(ksac);
     	psn->WritePSN();
     	delete psn;
    }
  }
}

void Event::CmFileSaveAs()
{
	if(playing)
  	return;
  if(n_ksac == 1) {
  	bool ret = false;  	KSAC *ksac = ksacs[0];
  	TAS1* theApp = TYPESAFE_DOWNCAST(GetApplication(), TAS1);

  	// Display standard Save dialog box
  	//
  	// Set filter
  	theApp->FileData.Flags = OFN_NOCHANGEDIR | OFN_EXPLORER | OFN_HIDEREADONLY |
    	OFN_OVERWRITEPROMPT;
  	theApp->FileData.SetFilter(
  		"SAC files (*.sac )|*.sac|"
    	"PEPP files (*.set)|*.set|"
  		"PSN files (*.psn;)|*.psn|");
  	theApp->FileData.DefExt = "sac";


  	*(theApp->FileData.FileName) = 0;
    PEPP *pepp;
    PSN *psn;
  	if (TFileSaveDialog(this, theApp->FileData).Execute() == IDOK) {
    	SetCaption(theApp->FileData.FileName);
    	// See if we should write a SAC file or a PEPP file
    	switch(theApp->FileData.FilterIndex) {
    	default:	// SAC
    		ksac->WriteSAC(theApp->FileData.FileName);
      	break;
    	case 2:	// PEPP
    		// Convert this SAC file internally to PEPP format
     		pepp = new PEPP(ksac);
      	pepp->WritePEPP(theApp->FileData.FileName);
      	delete pepp;
      	break;
    	case 3:	// PSN
    		// Convert this SAC file internally to PSN format
     		psn = new PSN(ksac);
      	psn->WritePSN(theApp->FileData.FileName);
      	delete psn;
      	break;
    	}
    	// Add to MRU (Most Recently Used) files list
  		theApp->SaveMenuChoice(theApp->FileData.FileName);
    }
  } else {
  	MessageBox("Can only save one file");
  }
}

void Event::CmWriteBitMap() {
	if(playing)
  	return;
  TAS1* theApp = TYPESAFE_DOWNCAST(GetApplication(), TAS1);

  theApp->FileData.Flags = OFN_NOCHANGEDIR | OFN_EXPLORER | OFN_HIDEREADONLY |
  	OFN_OVERWRITEPROMPT;
  theApp->FileData.SetFilter("png files (*.png)|*.png");
  theApp->FileData.DefExt = "png";

  if (TFileOpenDialog(this,theApp->FileData).Execute() == IDOK) {
  	UpdateWindow();
    WriteBitMap(this,theApp->FileData.FileName);
  }
  theApp->FileData.SetFilter("SAC files (*.sac.z)|*.sac.z|All Files (*.*)|*.*|");
  theApp->FileData.DefExt = "sac.z";
}


void Event::EvHScroll(UINT code, UINT pos, HWND wnd)
{
  TWindow::EvHScroll(code,pos,wnd); // perform default WM_HSCROLL processing
  time_offset = 0.01*(float)scrollbar->GetPosition();
  if(time_expansion < 1)
  	time_expansion = 1;
  time_offset = time_offset*(time_expansion - 1)/time_expansion;
  Invalidate();
}

void Event::CmUnfilter()
{
	// Do it for each seismogram
  for(int k=0;k<n_ksac;k++) {
  	// Get this seismogram
  	KSAC *ksac = ksacs[k];
    ksac->Restore();
  }
  filtered = 0;
  Invalidate();
}

// Long Period filter
void Event::CmFilterLP()
{
	FilterLP *f = new FilterLP(this);
  if(f->Execute() == IDOK) {
    filtered++;
    profile->event_filter_type = Profile::LONG_PERIOD;
    KSAC *ksac = ksacs[0];
 		int npts = ksac->GetSACHV(NPTS);
  	float delta = ksac->GetSACHV(DELTA);
  	float *data = new float[npts];
    if(delta == 0)
    	delta = 1;
    float sps = 1/delta;
  	// Zero data
  	memset(data,0,npts*sizeof(float));
  	memcpy(data,ksac->GetData(),npts*sizeof(float));
    Xfm::McClureLongPeriod(data, npts, sps);
    ksac->SetData(data,npts);
    delete data;
  }
  Invalidate();
  delete f;
}
void Event::CmFilterLowPass()
{
	Filter *f = new Filter(this,profile->EventLowPassPeriod,enable_lowpass,
  	"Low Pass Filter Dialog");
  if(f->Execute() == IDOK) {
  	delete f;
  	if(enable_lowpass == false) {
    	CmUnfilter();
    	return;
    }
    filtered++;
    if(profile->event_filter_type == Profile::HIGH_PASS)
    	profile->event_filter_type = Profile::BAND_PASS;
    else
    	profile->event_filter_type = Profile::LOW_PASS;
    KSAC *ksac = ksacs[0];
 		int npts = ksac->GetSACHV(NPTS);
  	float delta = ksac->GetSACHV(DELTA);
    if(delta == 0)
    	delta = 1;
    float sps = 1/delta;
    Xfm::EventFilterLP = ButterworthLP(sps,1.0/profile->EventLowPassPeriod,2);
    if(profile->EventLowPassPeriod < 2*delta)
			return;
  	float *data = new float[npts];
  	// Zero data
  	memset(data,0,npts*sizeof(float));
  	memcpy(data,ksac->GetData(),npts*sizeof(float));
    //Xfm::McClureLowPass(data, npts, sps,profile->EventLowPassPeriod);
    Xfm::EventFilterLP.Filter(data,npts);
    ksac->SetData(data,npts);
    delete data;
  }
  Invalidate();
}
void Event::CmFilterHighPass()
{
	Filter *f = new Filter(this,profile->EventHighPassPeriod,enable_highpass,
  	"High Pass Filter Dialog");
  if(f->Execute() == IDOK) {
  	delete f;
  	if(enable_highpass == false) {
    	CmUnfilter();
    	return;
    }
    // Setup filter
    filtered++;
    if(profile->event_filter_type == Profile::LOW_PASS)
    	profile->event_filter_type = Profile::BAND_PASS;
    else
    	profile->event_filter_type = Profile::HIGH_PASS;
    KSAC *ksac = ksacs[0];
 		int npts = ksac->GetSACHV(NPTS);
  	float delta = ksac->GetSACHV(DELTA);
    if(delta == 0)
    	delta = 1;
    float sps = 1/delta;
    Xfm::EventFilterHP = ButterworthHP(sps,1.0/profile->EventHighPassPeriod,2);
    // If violate Nyquist, get zero response, i.e. no filtering
    if(profile->EventHighPassPeriod <= 2*delta)
    	return;

  	float *data = new float[npts];
  	// Zero data
  	memset(data,0,npts*sizeof(float));
  	memcpy(data,ksac->GetData(),npts*sizeof(float));
    //Xfm::McClureHighPass(data, npts, sps,profile->EventHighPassPeriod);
    Xfm::EventFilterHP.Filter(data,npts);
    ksac->SetData(data,npts);
    delete data;
  }
  Invalidate();
}
#if 0
void Event::CmFilter()
{
	Filter *f = new Filter(this,cutoff_1,cutoff_2);
	if(f->Execute() == IDOK) {
		cutoff_1 = f->GetCutoff_1();
    profile->bp_low_cutoff = cutoff_1;
		cutoff_2 = f->GetCutoff_2();
    profile->bp_high_cutoff = cutoff_2;
    // comment out IIR Filter.  See below for DFT filter
    #if 0
    float delta = ksacs[0]->GetSACHV(DELTA);
    if(cutoff_1 == 0 || cutoff_2 > 0.5/delta) {
    	// Not band pass
    	filterCoeff();
    	filter();
    } else {
    	// Band pass.  Do HP followed by LP
      cutoff_1 = 0;
      // High pass
      filterCoeff();
      filter();
      cutoff_1 = profile->bp_low_cutoff;
      // Low pass
      cutoff_2 = 1/delta;
      filterCoeff();
      filter();
      cutoff_2 = profile->bp_high_cutoff;
    }
    #endif
    #if 0
    filterCoeff();
    filter();
    filtered++;
    #endif
    filtered++;
    // Use FFT for filtering
    // Do Fourier transform.
    // Just do first trace for now
    KSAC *ksac = ksacs[0];

 		int npts = ksac->GetSACHV(NPTS);
  	//float mean = ksac->GetSACHV(DEPMEN);
  	float delta = ksac->GetSACHV(DELTA);
    float sps = 1.0/delta;
    Xfm::McClureLowPass(ksac->GetData(),npts,sps);
    #if 0
  	int n = 2;
    // Find next power of 2
  	while(n < npts)
  		n *= 2;
  	float *data = new float[n];
    float delta_f = 1/(delta*n);
  	// Zero data
  	memset(data,0,n*sizeof(float));
  	memcpy(data,ksac->GetData(),npts*sizeof(float));
  	// Have to pass n/2 to realft routine.
  	// The "data-1" is because the realft routine is in origin 1
    int nn=n/2;
    Xfm::realft(data-1,nn,1);
    // Now have the complex Fourier transform in data
 		// Filter
    // Use gaussian filter for low pass
    // Use S-shaped curve for low pass
    float norm_freq = delta_f/cutoff_2;
    float hp_coef = cutoff_1/delta_f;
    float freq = 0;
    if(cutoff_1 == 0) {
    	profile->event_filter_type = Profile::LOW_PASS;
      // Use 4th order ButterworthLP
      complex s, h;
      complex a1 = 2.613126;
      complex a2 = 3.414214;
      complex a3 = a1;
      complex tt;
      //h = 1;
      for(int i=0;i<nn;i++) {
      	int j = 2*i;
        freq += norm_freq;
        s = complex(0,freq);
        h = 1 + s*(a1 + s*(a2 + s*a3));
        tt = complex(data[j],data[j+1])/h;
        data[j] = real(tt);
        data[j+1] = imag(tt);
      }
 	  } else if(cutoff_2 > 0.5/delta) {
    	profile->event_filter_type = Profile::HIGH_PASS;
    	float norm_freq = delta_f/cutoff_1;
      // Just use a wall filter
      for(int i=0;i<nn;i++) {
      	int j = 2*i;
        freq += delta_f;
        if(freq <= cutoff_1) {
        	data[j] = 0;
        	data[j+1] = 0;
        }
      }
    } else {
    	profile->event_filter_type = Profile::BAND_PASS;
      for(int i=0;i<nn;i++) {
      	int j = 2*i;
        freq += delta_f;
        if(freq <= cutoff_1 || freq >= cutoff_2) {
        	data[j] = 0;
        	data[j+1] = 0;
        }
      }
    }
    #if 0
    float fil;
    for(int i=0;i<nn;i++) {
  		int j = 2*i;
      float freq = i*exp_coef;
      if(freq < 4)
      	fil = exp(-freq*freq);
      else
      	fil = 0;
      // High pass
      if(i < hp_coef) {
        float f = (i*delta_f/cutoff_1);
        fil *= f*f*(3 - 2*f);
      }
      data[j] *=fil;
      data[j+1] *=fil;
    }
    #endif
    Xfm::realft(data-1,nn,-1);
    // Have to mulitply result by 1/n
    for(int i = 0;i<npts;i++)
    	data[i] *= 1.0/nn;
    // Move back into data
    ksac->SetData(data,npts);
    delete data;
    Invalidate();
	}
  delete f;
  #endif
  }
  Invalidate();
}
#endif
#define NPS 7
#define NPS2 3
// Array to be sorted
//static int index[NPS];
// Keys
static float pts[NPS];
// Compare function for sorting integers based on floating point keys
#if 0
static int cmpf(void *p1, void *p2)
{
	int i;
	float k1 = *(float *)p1;
	float k2 = *(float *)p2;
  if(k1==k2) i = 0;
  else i = (k1<k2) ? -1 : 1;
  return i;
}
#endif

// Seven-point Median filter
// For each point, take the point and 3 points on either side.  Sort and
// pick out the median.  If at either end, replicate the end point enough
// times to get 7.
void Event::CmMedianFilter()
{
  static int np = 7;
  //static int np2 = 4;		// One-half rounded up
  static int np3 = 3;   // one-half rounded down
  int jj;
  // Do it for each seismogram
  for(int k=0;k<n_ksac;k++) {
  	// Get this seismogram
  	int npts = ksacs[k]->GetSACHV(NPTS);
  	if(npts > 0) {
    	// Filter all the points
  		float *data = ksacs[k]->GetData();
      // Make second, duplicate array
      float *data2 = new float[npts];
      memcpy(data2,data,npts*sizeof(float));
  		for(int i=0;i<npts;i++) {
      	// Pull out the seven points
        for(int j=0;j<np;j++) {
        	jj = i+j-np3;
          if(jj < 0)
            pts[j] = data2[0];
          else if(j >=npts)
          	pts[j] = data2[npts-1];
          else
          	pts[j] = data2[jj];
        }
        // Sort them in order to find the median
        // First set up the index
        //for(int k=0;k<np;k++)
       	//	index[k] = k;
        //msort(pts,np,sizeof(float),cmpf);
        // Pull out median
        //data[i] = pts[np3];
        // Instead of sorting, select out the median
        // The "pts-1" is to convert from our zero-origin
        // to the Numerical recipe's one-origin
        data[i]= select(4,7,pts-1);
      }
      delete []data2;
  	}
  }
  Invalidate();
}


// Filter coefficients
void Event::filterCoeff()
{
  // If cutoff_1 == 0, use low pass
  // If cutoff_2 > nyquist, use high pass
  // If both okay, use bandpass
  float delta = ksacs[0]->GetSACHV(DELTA);
 	float a = cutoff_1*delta;
 	float b = cutoff_2*delta;
  if(a == 0) {					// Low pass
		c0 = b/(1+b);
 		c1 = c0;
 		d1 = (1-b)/(1+b);
    d2 = 0;
  }
  else if (b > 0.5) {		// High pass
  	c0 = 1/(1+a);
    c1 = -c0;
    d1 = (1-a)/(1+a);
    d2 = 0;
  }
  else {								// Bandpass
  	float opa = 1+a;
    float oma = 1-a;
    float opb = 1+b;
    float omb = 1-b;
  	c0 = b/(opa*opb);
    c1 = 0;
    c2 = -c0;
    d1 = (opa*omb + oma*opb)/(opa*opb);
    d2 = -(oma*omb)/(opa*opb);
	}
}

void Event::filter()
{
	// If cutoff_1 is zero and cutoff_2
  // is larger than sampling frequency, don't do anything
  if(cutoff_1 == 0 && cutoff_2 > 0.5/ksacs[0]->GetSACHV(DELTA))
  	return;
  for(int k=0;k<n_ksac;k++) {
  	int npts = ksacs[k]->GetSACHV(NPTS);
  	if(npts > 0) {
  		float *data = ksacs[k]->GetData();
    	float last1 = data[0];
      float last2 = data[0];
      // Do point 0
			float x = c0*data[0];
      x += c1*last1;
      data[0] = x;
      // Do point 1
			x = c0*data[1];
      x += c1*last1 + c2*last2+d1*data[0];
      last1 = data[1];
    	data[1] = x;
      // And now the rest
  		for(int i=2;i<npts;i++) {
				float x = c0*data[i];
    		x += c1*last1 + c2*last2 + d1*data[i-1] + d2*data[i-2];
        last2 = last1;
      	last1 = data[i];
    		data[i] = x;
      }
  	}
  }
}


#if 0
void Event::CmGain()
{
	if(gain_dialog == 0) {
		gain_dialog = new Gain(this);
    gain_dialog->Create();
  	gain_dialog->SetGain(gain);
  	gain_dialog->SetX(time_expansion);
  }
}
#endif
/********************************************************/
/* Removes trailing blanks from a string variable, and  */
/*   return the string length.                          */
/*                                                      */
/*   Expected call:  deblank(string);                   */
/*                                                      */
/*   int deblank(string);                               */
/*   char *string;                                      */
/********************************************************/
int deblank(
	char *string)
{
  int i;
	/* Deblank the string */
  i=-1;
  while (string[++i]);
  while ((--i>=0) && (string[i] == ' '))
    string[i] = '\0';
  return(i+1);
}

void Event::CmEventDialog()
{
	// Load up variables
	EVENT_DATA ed;
  KSAC *ksac = ksacs[0];
  strcpy(ed.kevnm,ksac->GetSACHV(KEVNM));
  strcpy(ed.kstnm,ksac->GetSACHV(KSTNM));
  ed.evla 	= ksac->GetSACHV(EVLA);
  ed.evlo 	= ksac->GetSACHV(EVLO);
  ed.evdp 	= ksac->GetSACHV(EVDP);
  ed.year 	= ksac->GetSACHV(NZYEAR);
  ed.jday 	= ksac->GetSACHV(NZJDAY);
  // compute month and day based on year and julian day
  month_day(ed.jday,ed.year,ed.month,ed.day);
  ed.hour 	= ksac->GetSACHV(NZHOUR);
  ed.minute= ksac->GetSACHV(NZMIN);
  ed.second= ksac->GetSACHV(NZSEC);
  ed.msecond	= ksac->GetSACHV(NZMSEC);
  ed.ev_mb = ksac->GetSACHV(EV_MB);
  ed.ev_ms = ksac->GetSACHV(EV_MS);
  ed.ev_ml = ksac->GetSACHV(EV_ML);
  ed.ev_mw = ksac->GetSACHV(EV_MW);
  //ed.origin = ksac->GetSACHV(B);
  ed.origin = ksac->GetSACHV(O);

  if (EventDialog(this,ed).Execute() == IDOK) {
  	ksac->SetSACHV(KEVNM,ed.kevnm);
  	ksac->SetSACHV(EVLA,ed.evla);
  	ksac->SetSACHV(EVLO,ed.evlo);
  	ksac->SetSACHV(EVDP,ed.evdp);
    ksac->SetSACHV(NZYEAR,(long)ed.year);
    ksac->SetSACHV(NZJDAY,(long)ed.jday);
    ksac->SetSACHV(NZHOUR,(long)ed.hour);
    ksac->SetSACHV(NZMIN,(long)ed.minute);
    ksac->SetSACHV(NZSEC,(long)ed.second);
    ksac->SetSACHV(NZMSEC,(long)ed.msecond);
    ksac->SetSACHV(EV_MB,ed.ev_mb);
    ksac->SetSACHV(EV_MS,ed.ev_ms);
    ksac->SetSACHV(EV_ML,ed.ev_ml);
    ksac->SetSACHV(EV_MW,ed.ev_mw);
    //ksac->SetSACHV(B,ed.origin);
    ksac->SetSACHV(O,ed.origin);

    float arc, baz;
    //gcarc(profile->latitude,profile->longitude,
    gcarc(ksac->GetSACHV(STLA),ksac->GetSACHV(STLO),
      ed.evla,ed.evlo,arc,baz);
    ksac->SetSACHV(GCARC,arc);
    // See if we should write the sac file to Seismic Waves
    if(strlen(ed.yyyymmddhh) != 0) {
      char buf[64];
      char tmpfn[64];
      strcpy(tmpfn,ksac->GetFileName());
      // File of form: c:\sw\yyyymmddhh\BNY.sac
      char fn[16];
      strcpy(fn,ksac->GetSACHV(KSTNM));
      deblank(fn);
      sprintf(buf,"%s\\%s\\%s.sac",
      	profile->seismic_waves,ed.yyyymmddhh,fn);
      ksac->WriteSAC(buf);
      ksac->PutFileName(tmpfn);
      sprintf(tmpfn,"Event %s established for Seismic Waves",ksac->GetSACHV(KEVNM));
      MessageBox(tmpfn);
    }
    Invalidate();
  }
}

void Event::CmExtractDisable(TCommandEnabler& tce)
{
  // If there is something to extract, enable this menu item
	tce.Enable(x1 != x2);
}

//---------------------------
// DrawTrace
void Event::DrawTrace(TDC &dc, float t0, float t1)
{
  viewport(dc,vr,1);
  width = vr.Width();
  height = vr.Height();
  int k_height = height/n_ksac;
  for(int k=0;k<n_ksac;k++) {
    KSAC *ksac = ksacs[k];
  	int y0 = k_height*k;
  	int npts = ksac->GetSACHV(NPTS);
  	//xsf = time_expansion*width/(float)npts;
  	data = ksac->GetData();
  	offset = (time_offset)*width*time_expansion;
  	// Compute scaling factor
  	ksac->GetMinMax();
  	float depmin = ksac->GetSACHV(DEPMIN);
  	float depmax = ksac->GetSACHV(DEPMAX);
    float depmen = ksac->GetSACHV(DEPMEN);
  	// See if set
  	float scale = 1;
    float tic = 1;
    int d;
    float y1;
    float y2 = -1;
    float dtic;
    if(!auto_scale) {
    	depmin = depmen - max_amplitude;
      depmax = depmen + max_amplitude;
    }
  	if(depmin != UNDEF && depmax != UNDEF) {
  		// Scaling
    	float denom = depmax-depmin;
      if(denom == 0)
      	denom = 1;
  		float range = denom;
  		if(range != 0) {
  			int dec = log10(range);		// How many decades?
    		d = pow10(dec);
        if(d == 0)
        	d = 1;
    		float r2 = range/(10*d);
    		if(r2 < .2) tic = .2;
    		else if(r2 < .5) tic = .5;
        // Compute tic mark values in depmin, depmax units
        dtic = tic*d;
        // Find limits to end on tic marks
        if(auto_scale) {
        	y1 = dtic*floor((depmin-depmen)/dtic);
        	y2 = dtic*ceil((depmax-depmen)/dtic);
        }
        else {
        	y1 = depmin-depmen;
          y2 = depmax-depmen;
        }
        if(y1 == y2) {
        	// avoid a zero-divide
        	y2 += 2*tic;
          y1 -= 2*tic;
      	}
        denom = y2-y1;
      	scale = k_height/denom;
      }
  	}
  	// Draw trace
  	//float y = y0;
  	viewport(dc,vr,1);
    int off = ksac->sec_offset*xsf-offset;
    float y3 = depmen+y2;
    //float y3 = y2;
    float delta = ksac->GetSACHV(DELTA);
    // Compute beginning data point based on t0
    int d0 = t0/delta;
    if(d0 >= npts)
    	return;
    float xsec_sf = xsf*delta;
    int d1;
    if(t1 == 0)
    	d1 = npts;
    else {
    	d1 = 1+ t1/delta;
      if(d1 >= npts)
      	d1 = npts-1;
    }
    dc.BeginPath();
  	plot(dc,d0*xsec_sf+off,y0-scale*(data[d0]-y3),0,1);
  	plot(dc,d0*xsec_sf+off,y0-scale*(data[d0]-y3),BLACK,1);
    ksac->y0 = y0;
    ksac->scale = scale;
    // Compute ending datapoint based on t1
    ksac->y_offset = y2;
    // Roger Sparks suggested following change to keep from overrunning array
  	//for(int i=d0;i<=d1;i++) {
  	for(int i=d0;i<d1;i++) {
  		dc.LineTo(i*xsec_sf+off,y0-scale*(data[i]-y3));
  	}
    dc.EndPath();
    dc.StrokePath();
  	// Draw picks if any
  	for(int i=0;i<10;i++) {
  		DrawPick(i);
  	}
  	// Print vertical scale if it hasn't already been drawn
    if(t0 != 0)
    	continue;
  	// Put about 5 tick marks
  	viewport(dc,cr,1);
  	viewport(dc,vr,0);
    //int ymin = k*k_height;
    //int ymax = (k+1)*k_height;
  	TFont *font = new TFont("MS Sans Serif",cheight,0,0,0,FW_BOLD);
   	for(float y= 0; y <= y2; y += dtic*n_ksac) {
      char temp[16];
     	sprintf(temp,"%.5g",y);
     	float yt = y0-scale*(y-y2);
      writexy(dc,-cwidth,yt,font,temp,WHITE,-1,2,1,2,BLACK);
     	plot(dc,0,yt);
     	plot(dc,cwidth,yt,BLACK,1);
    }
   	for(float y= -dtic*n_ksac; y >= y1; y -= dtic*n_ksac) {
      char temp[16];
     	sprintf(temp,"%.5g",y);
     	float yt = y0-scale*(y-y2);
      writexy(dc,-cwidth,yt,font,temp,WHITE,-1,2,1,2,BLACK);
     	plot(dc,0,yt);
     	plot(dc,cwidth,yt,BLACK,1);
    }
    delete font;
  }
}

//----------------
// DrawFrame()
//
void Event::DrawFrame(TDC &dc,TRect &cr)
{
 	TEXTMETRIC system_tm;     // Text metrics for system font
  // Run through all the SAC events to see minimum starting time and
  // maximum ending time
  long double zTime0 = 1e100;
  long double endTime = 0;
  for(int k=0;k<n_ksac;k++) {
  	KSAC *ksac = ksacs[k];
  	if(zTime0 > ksac->zTime)
    	zTime0 = ksac->zTime;
    long double endTime2 =
    	ksac->zTime + ksac->GetSACHV(NPTS)*ksac->GetSACHV(DELTA);
    if(endTime < endTime2)
    	endTime = endTime2;
  }
  time_length = (float)(endTime - zTime0);
  // Go through the events again figuring out each ones offset
  for(int k=0;k<n_ksac;k++) {
  	KSAC *ksac = ksacs[k];
    ksac->sec_offset = (float)(ksac->zTime - zTime0);
  }
  char temp[64];
  TColor tmpcol;
  dc.TextRect(cr,colors[DARKGRAY]);
  dc.GetTextMetrics(system_tm);
  cheight = system_tm.tmHeight;
  cwidth = system_tm.tmAveCharWidth;
  lmargin = 10*cwidth;
  margin = 2*cwidth;
  TRect rm;

  // Create font since system font draws too small on printer
  TFont *font = new TFont("MS Sans Serif",cheight,0,0,0,FW_BOLD);

 	// Compute working area of screen
	vr = TRect(lmargin,margin,cr.Right()-margin,cr.Bottom()-5*cheight);
  // Bottom margin
  bmargin = TRect(vr.Left(),vr.Bottom(),cr.Right(),cr.Bottom());
  //long npts = ksac->GetSACHV(NPTS);
  //sprintf(temp,"npts: %ld",npts);
  //delta = ksac->GetSACHV(DELTA);
  height = vr.Height();
  int k_height = height/n_ksac;


  // Compute scaling factors.  X-scale factor. Converts from data points to screen points
  // xsf units are pixels/datapoint
	//float xsf = (float)(vr.Width())/((float)npts);
  // xsec_sf converts from seconds to screen coordinates
  // pixels/second

  if(time_expansion < 1)
  	time_expansion = 1;
  //float time_length = npts*delta;
  // The x-scale factor is in units of pixels/second
	xsf = (float)(time_expansion*vr.Width())/time_length;
 	// Y-scale factor for data
  viewport(dc,cr,1);

  dc.TextRect(vr,colors[WHITE]);
  int x = cwidth/2;
  border(dc,vr.InflatedBy(x,x),0,true,x,0);
  // Bottom margin area
  tmpcol = TColor(colors[DARKGRAY]);
  viewport(dc,vr,1);

  // Write station and date information
  x = 0.25*vr.Width();
  int y;
  // Print out station name and components
	for(int k=0;k<n_ksac;k++) {
  	y = 2*cheight + k*k_height;
  	char name[16], component[16];
  	strcpy(name,ksacs[k]->GetSACHV(KSTNM));
 		strcpy(component,ksacs[k]->GetSACHV(KCMPNM));
  	// Don't print station name if it is "-12345", the default value
  	if(name[0] != '-')
  		sprintf(temp,"%s  %s",name,component);
  	else
  		sprintf(temp,"Component: %s",component);
 		writexy(dc,x,y,font,temp,BLACK,-1,0,0);
	}
  y = 3*cheight;
  // Print out lat and lon
  sprintf(temp,"Lat: %.5g Lon: %.5g",
  	ksacs[0]->GetSACHV(STLA),ksacs[0]->GetSACHV(STLO));
  writexy(dc,x,y,font,temp,BLACK,-1,0,0);
  y += cheight;
  // Print date and time
  KSAC *ksac = ksacs[0];
  int year = ksac->GetSACHV(NZYEAR);
  int month = 0;
  int day = 0;
  int jul = ksac->GetSACHV(NZJDAY);
  month_day(jul,year,month,day);
  int hour = ksac->GetSACHV(NZHOUR);
  hour_save = hour;
  int minute = ksac->GetSACHV(NZMIN);
  int sec = ksac->GetSACHV(NZSEC);
  fsec = minute*60+sec + 0.001*ksac->GetSACHV(NZMSEC);
  origin = ksac->GetSACHV(O);
  sprintf(temp,"%04d/%02d/%02d %02d:%02d:%02d",year,month,day,hour,minute,sec);
 	writexy(dc,x,y,font,temp,BLACK,-1,0,0);
  y += cheight;
  float sr = 1/ksac->GetSACHV(DELTA);
  sprintf(temp,"Sample rate: %g",sr);
 	writexy(dc,x,y,font,temp,BLACK,-1,0,0);
  y += cheight;
  float nq = 0.5*sr;
  if(filtered == 2 && profile->event_filter_type ==Profile::BAND_PASS ) {
  		sprintf(temp,"Bandpass filter. Corners: %.3g Hz (%.3g s) and %.3g Hz (%.3g s)",
    		1/profile->EventLowPassPeriod, profile->EventLowPassPeriod,
    		1/profile->EventHighPassPeriod, profile->EventHighPassPeriod);
  } else
  if(filtered == 1) {
  	switch(profile->event_filter_type) {
    case Profile::LOW_PASS:
  		sprintf(temp,"Lowpass filter. Corner: %.3g Hz (%.3g s)",
    		1/profile->EventLowPassPeriod, profile->EventLowPassPeriod);
      break;
    case Profile::HIGH_PASS:
  		sprintf(temp,"Highpass filter. Corner: %.3g Hz (%.3g s)",
    		1/profile->EventHighPassPeriod, profile->EventHighPassPeriod);
      break;
    case Profile::LONG_PERIOD:
      sprintf(temp,"Long period filter. Nat. freq: %g s, Q: %g, LP %g s",
      	profile->Pp, profile->Qp, profile->Pf);
      break;
    default:
    	sprintf(temp,"No filtering");
    }
  } else
  if(filtered > 1) {
  	sprintf(temp,"Multiple filters applied");
  } else
 		strcpy(temp,"No filtering");
  writexy(dc,x,y,font,temp,BLACK,-1,0,0);
  y += cheight;
  // Great circle arc
  #if 0
  float arc = ksac->GetSACHV(GCARC);
 	float lat = ksac->GetSACHV(EVLA);
  float lon = ksac->GetSACHV(EVLO);
  if(abs(lat) < 0.001 && abs(lon) < 0.001) {
  	sprintf(temp,"Event latitude: %.4g Event longitude: %.4g",
    	lat, lon);
  	writexy(dc,x,y,font,temp,BLACK,-1,0,0);
    y += cheight;
    if(arc < 0.001) {
    	float baz;
    	gcarc(profile->latitude,profile->longitude,
      	lat,lon,arc,baz);
    	ksac->SetSACHV(GCARC,arc);
    }
  	sprintf(temp,"Great-circle arc: %.4g degrees",arc);
  	writexy(dc,x,y,font,temp,BLACK,-1,0,0);
    y += cheight;

  }
  #endif
	// Display filter coefficients
  //float sps = 1/ksac->GetSACHV(DELTA);
  //sprintf(temp,"Sample rate: %.3g, Npts: %ld",sps,npts);
  //writexy(dc,x,y,font,temp,BLACK,-1,0,0);
  //y += cheight;
  //char *p = ksac->GetSACHV(KEVNM);
  //if(strncmp(p,"-12345",6) != 0) {
 	//	writexy(dc,x,y,font,p,BLACK,-1,0,0);
  //  y += cheight;
  //}
  // See if event parameters are set
  #if 1
  if(ksac->GetSACHV(EVLA) != -12345.0) {
  	if(ksac->GetSACHV(EVLA) != 0) {
      float lat, lon, depth;
      lat = ksac->GetSACHV(EVLA);
      lon = ksac->GetSACHV(EVLO);
      depth = ksac->GetSACHV(EVDP)/1000;
      sprintf(temp,"Lat: %g, Lon: %g, Depth: %g km",lat,lon,depth);
      writexy(dc,x,y,font,temp,BLACK,-1,0,0);
      y += cheight;
  		float arc = 0;
   		float baz;
   		gcarc(ksac->GetSACHV(STLA),ksac->GetSACHV(STLO),
     		lat,lon,arc,baz);
   		ksac->SetSACHV(GCARC,arc);
  		sprintf(temp,"Great-circle arc: %.4g degrees",arc);
  		writexy(dc,x,y,font,temp,BLACK,-1,0,0);
    	y += cheight;
      float mag = ksac->GetSACHV(EV_MS);
      if(mag != 0 && mag != -12345.0) {
      	sprintf(temp,"Ms: %.1f",mag);
      	writexy(dc,x,y,font,temp,BLACK,-1,0,0);
      	x += cwidth*8;
      }
      mag = ksac->GetSACHV(EV_MB);
      if(mag != 0 && mag != -12345.0) {
      	sprintf(temp,"mb: %.1f",mag);
      	writexy(dc,x,y,font,temp,BLACK,-1,0,0);
      	x += cwidth*8;
      }
      mag = ksac->GetSACHV(EV_MW);
      if(mag != 0 && mag != -12345.0) {
      	sprintf(temp,"Mw: %.1f",mag);
      	writexy(dc,x,y,font,temp,BLACK,-1,0,0);
      	x += cwidth*8;
      }
      mag = ksac->GetSACHV(EV_ML);
      if(mag != 0 && mag != -12345.0) {
      	sprintf(temp,"Ml: %.1f",mag);
      	writexy(dc,x,y,font,temp,BLACK,-1,0,0);
      }
    }
  }
  #endif
  int offset = time_offset*vr.Width()*time_expansion;
  int time_inc;
  if(time_length > 1800) // If greater than 1/2 hour, every 5 minutes
  	time_inc = 300;
	else if(time_length > 600)	// If greater than 1 hour, every minute
  	time_inc = 60;
  else if(time_length > 60) // if greater than 1 minute, every 0.5 minutes
  	time_inc = 30;
  else if(time_length > 30) // if greater than 30 s, every 15 seconds
  	time_inc = 15;
  else if(time_length > 10) // if greater than 10 s, every  5 s
  	time_inc = 5;
  else
  	time_inc = 1;
  // Compute where the first tic mark should be based on the origin time
  //float off = fsec - origin;
  float off = fsec;

  for(int i=0;i<time_length+off;i += time_inc) {
  	int x = (i-off)*xsf - offset;
    if(x > 0 && x < vr.Width()) {
  		plot(dc,x,vr.Height()-6);
    	plot(dc,x,vr.Height(),BLACK,1);
  		plot(dc,x,0);
    	plot(dc,x,cwidth,BLACK,1);
  	}
  }
  // Write the minutes at the bottom
  viewport(dc,GetClientRect(),1);
  viewport(dc,bmargin,1);
  viewport(dc,bmargin,0);
  int off1 = time_inc*((int)(off/time_inc));
  float off2 = off - off1;
  for(int i=0;i<=time_length+off;i += time_inc) {
  	int x = (i-off)*xsf - offset;
    if(x >= 0 && x < vr.Width()) {
    	if(time_inc < 60)
  			sprintf(temp,"%d:%02d",i/60,i%60);
      else
  			sprintf(temp,"%d",i/60);
    	writexy(dc,x,cheight/2,font,temp,WHITE,-1,1,2,BLACK);
  	}
  }
  sprintf(temp,"MINUTES since %02d:00:00 UTC",hour);
  writexy(dc,vr.Width()/2,cheight*1.5,font,temp,WHITE,-1,1,2,BLACK);
  if(back == 0) {
  	back = new TButton(this,CM_BACK,"Back",
    	vr.Left()+cheight,vr.Top()+cheight,9*cwidth,2*cheight);
    back->Create();
  }
  if(playing) {
   	int x = bmargin.Width()/4;
    int y = bmargin.Top()+1.75*cheight;
  	if(stop == 0) {
    	stop = new TButton(this,CM_STOP,"Stop",x,
      	y,12*cwidth,1.5*cheight);
      stop->Create();
    }
    if(spin_time_accel == 0) {
    	x += 12*cwidth;
      time_accel = new TEdit(this,CM_TIME_ACCEL,"",x,y+cheight/4,cwidth*5,cheight+3,8);
      time_accel->Create();
      x += 5*cwidth;
    	spin_time_accel = new TUpDown(this,CM_SPIN_TIME_ACCEL,x,
      	y,cwidth*2,2*cheight);
    	spin_time_accel->Create();
    	spin_time_accel->SetRange(0,7);
      spin_time_accel->Create();
      spin_time_accel->SetPos(accel_factor);
      //x += 3*cwidth;
    }
 		writexy(dc,bmargin.Width()/4+17*cwidth,cheight*2,font,"Speed",YELLOW,-1,2,2,BLUE);
  } else {
  	if(play == 0) {
    	int y = bmargin.Top()+1.75*cheight;
    	play = new TButton(this,CM_PLAY,"Play record",bmargin.Width()/4,
      	y,12*cwidth,1.5*cheight);
      play->Create();
    }
    if(time_accel) {
      delete time_accel;
      time_accel = 0;
      spin_time_accel->Destroy();
      delete spin_time_accel;
      spin_time_accel = 0;
    }
  }
  if(time_accel) {
  	char buffer[16];
    accel = pow(2,accel_factor);
  	sprintf(buffer,"%g",accel);
  	time_accel->SetText(buffer);
  }
  delete font;
}

void Event::Paint(TDC&dc, bool, TRect& )
{
  //gain = gain_dialog->GetGain();
  //time_expansion = gain_dialog->GetExpansion();
  if(pick_instructions) {
  	pick_instructions->ShowWindow(picking ? SW_SHOW : SW_HIDE);
  }
	cr = GetClientRect();
  if(time_expansion < 1)
  	time_expansion = 1;
	if(time_expansion <= 1) {
  	if(scrollbar) {
    	delete scrollbar;
      scrollbar = 0;
    }
  }
  else {
		if(scrollbar == 0) {
  		scrollbar = new TScrollBar(this,CM_SCROLLBAR,0,cr.Bottom()-16,cr.Width(),0,true);
  		scrollbar->Create();
      scrollbar->ShowWindow(SW_SHOW);
  	}
  }
  if(scrollbar) {
  	scrollbar->ShowWindow(SW_SHOW);
  	time_offset = 0.01*(float)scrollbar->GetPosition();
  } else
  	time_offset = 0;
  time_offset = time_offset*(time_expansion - 1)/time_expansion;
  // See if we have a high-resolution device such as a printer
  // where the Viewport extent is greater than the Windows extent.
  // If so, make the Window larger
  TSize vExt = dc.GetViewportExt();
  TSize cExt = dc.GetWindowExt();
  if(vExt.cx > cr.Width()) {
  	cr = TRect(TPoint(0,0),vExt);
  }
	DrawFrame(dc,cr);
 	int x = bmargin.Left();
  int x2 = x + cwidth*4;		// position for time spin control
  int x4 = bmargin.Right() - cwidth*14; // Position for amplitude display
  int x3 = x4 - 14*cwidth;	//  Position for time display
  int y = bmargin.Top()+1.5*cheight;
  viewport(dc,cr,0);
  // Create font since system font draws too small on printer
  TFont *font = new TFont("MS Sans Serif",cheight,0,0,0,FW_BOLD);
 	//writexy(dc,x2+2*cwidth,y+cheight/4,font,"Gain",YELLOW,-1,0,2,2,BLUE);

  //int xx = x + cwidth*6;		// x-position for time expansion control

 	writexy(dc,x+6*cwidth,y+cheight/4,font,"Time Expansion",YELLOW,-1,0,2,2,BLUE);
  delete font;
	if(timeexp == 0) {
  	int yy = y+cheight/4;
    #if 0
  	gainquery = new TEdit(this,CM_GAINQUERY,"",x,yy,cwidth*6,cheight+3,16);
    gainquery->Create();
    spin_gain = new TUpDown(this,CM_SPINGAIN,x2,y,cwidth*2,2*cheight);
    spin_gain->Create();
    #endif
  	timeexp = new TEdit(this,CM_TIMEEXPANSION,"",x,yy,cwidth*4,cheight+3,16);
    timeexp->Create();
  	time = new TEdit(this,CM_TIME,"",x3,yy,cwidth*11,cheight+3,16);
    time->Create();
  	amplitude = new TEdit(this,CM_AMPLITUDE,"",x4,yy,cwidth*12,cheight+3,16);
    amplitude->Create();
    spin_time = new TUpDown(this,CM_SPINTIMEX,x2,y,cwidth*2,2*cheight);
    spin_time->Create();
    spin_time->SetRange(1,30);
  }
  char buffer[16];
  sprintf(buffer,"%g",time_expansion);
  timeexp->SetText(buffer);
  t0 = 0;
  if(!playing)
  	DrawTrace(dc);
	//if(picking) {
  //  viewport(dc,vr,0);
  //	writexy(dc,100,50,0,"Select arrivals with mouse.",BLACK,-1,0,0);
  //	writexy(dc,100,65,0,"Then click on Travel Time Curves",BLACK,-1,0,0);
  //	writexy(dc,100,80,0,"When you click on PICK again, old picks are erased.",BLACK,-1,0,0);
  //}
}

