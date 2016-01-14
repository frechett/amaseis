//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         xfm.cpp
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of XfmLog (TWindow).
//
//----------------------------------------------------------------------------
#include "as1.h"
#define twopi 6.283185306
// Gravity
#define g 980.0

//{{XfmLog Implementation}}
DEFINE_RESPONSE_TABLE1(XfmLog, TWindow)
//{{XfmLogRSP_TBL_BEGIN}}
  EV_WM_LBUTTONDOWN,
  EV_WM_LBUTTONUP,
  EV_WM_MOUSEMOVE,
  EV_COMMAND(CM_BACK,								CmBack),
//{{XfmLogRSP_TBL_END}}
END_RESPONSE_TABLE;


XfmLog::XfmLog(TWindow* parent, KSAC *ksac, bool inst_response, const char far* title, TModule* module)
:
    TWindow(parent, title, module)
{
  // Override the default window style for TWindow.
  //
      Attr.Style = WS_BORDER | WS_CAPTION | WS_MAXIMIZEBOX | WS_MAXIMIZE ;

  // INSERT>> Your constructor code here.
  amplitude = 0;
  frequency_box = 0;
  period_box = 0;
 	npts = ksac->GetSACHV(NPTS);
  float mean = ksac->GetSACHV(DEPMEN);
  float delta = ksac->GetSACHV(DELTA);
  //depmax = ksac->GetSACHV(DEPMAX);
  //depmin = ksac->GetSACHV(DEPMIN);
  float sps = 1/delta;
  // Do Fourier transform.
  int n = 2;
  while(n < npts)
  	n *= 2;
  // The Fourier transform routine from Numerical Methods is
  // in origin 1.
  delta_f = sps/n;
  data = new float[n];
  // Zero data
  memset(data,0,n*sizeof(float));
  memcpy(data,ksac->GetData(),npts*sizeof(float));
  for(int i = 0; i<npts;i++)
  	data[i] -= mean;
  // Have to pass n/2 to realft routine.
  // The "data-1" is because the realft routine is in origin 1
  npts = n/2;
  // Temporary code.  Put in an impulse.
  //data[0] = 1;
  //for(int i=1;i<n;i++)
  //	data[i] = 0;
  Xfm::realft(data-1,npts,1);

  this->inst_response = inst_response;

  max_f = sps/2;
  // Compute parameters for instrument response
  float aa = 0;
  if(inst_response) {
		aa = g*profile->cal_mass*profile->cal_distance;
    aa /= 2*profile->center_of_gravity*profile->mass_of_arm;
    // aa is in cm/s**2.  Convert to MKS
    aa /= 100;
    aa = pow(twopi,3)/aa;
		// Convert to microns
    aa *=0.000001;

  }
  // Compute amplitude as a function of frequency
  amp = new float[npts];
  int sr_npts = sqrt(npts);
  for(int i=0;i<npts;i++) {
  	int j = 2*i;
  	//float a = delta*sqrt(pow(data[j],2) + pow(data[j+1],2));
    // 2008.01.28: Normalize. Take out delta and divide by the
    // square root of N.
  	float a = sqrt(pow(data[j],2) + pow(data[j+1],2));
    a /= sr_npts;
    if(a>0)
    	a = log10(a);
    else
    	a = 1;
    amp[i] = a;
    if(inst_response) {
    	if(i*delta_f < 2)
    		amp[i] *= aa*pow(i*delta_f,3);
      else
      	amp[i] = 0;
    }
  }
  FILE *fp = fopen("EventLogFT.csv","wt");
    if(fp) {
  	fprintf(fp,"FREQUENCY,LOG AMPLITUDE\n");
    float t = 0;
    float *d = amp;
    for(int i=0;i<npts;i++) {
    	fprintf(fp,"%f,%f\n",t,*d++);
      t += delta_f;
    }
  	fclose(fp);
  }
// Response function (temporary)
  //for(int i=0;i<npts;i++)
  //	amp[i] *= pow((i/delta),3);
  // Find minimum and maximum values
  depmax = -1e20;
  depmin = 1e20;
  for(int i=0;i<npts;i++) {
  	float d = amp[i];
    if(d > depmax)
    	depmax = d;
    if(d < depmin)
    	depmin = d;
  }

  TClientDC dc(*this);
  dc.GetTextMetrics(system_tm);
  cheight = system_tm.tmHeight;
  cwidth = system_tm.tmAveCharWidth;
  lmargin = 8*cwidth;
  rmargin = 2*cwidth;
  tmargin = cheight/2;
  bmargin = 5*cheight;
  x_offset = 0;
  y_offset = cheight*15;
	back = 0;
}

void XfmLog::SetupWindow()
{
	TWindow::SetupWindow();
}

void XfmLog::CmBack()
{
  //ShowWindow(SW_HIDE);
  CloseWindow();
}
XfmLog::~XfmLog()
{
  //Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.
  delete amp;
  delete data;
}

void XfmLog::EvMouseMove(UINT, TPoint &p)
{
 	char buffer[32];
  // Get time
  if(!vr.Contains(p))
  	return;
 	int y = vr.bottom - p.y;
 	float d = y/ysf + min_y;
 	sprintf(buffer,"%.3g",pow(10,d));
  if(amplitude)
  	amplitude->SetText(buffer);
  d = (p.x-vr.left)/xsf;
  d = pow(10,d+x0);
  sprintf(buffer,"%.3g",d);
  if(frequency_box)
  	frequency_box->SetText(buffer);
  if(d != 0) {
  	sprintf(buffer,"%.3g",1/d);
  	if(period_box)
  		period_box->SetText(buffer);
  }
}
void XfmLog::EvLButtonUp(UINT, TPoint &p)
{
}
void XfmLog::EvLButtonDown(uint modKeys, TPoint& point)
{
}

void XfmLog::DrawCurve(TDC &dc)
{
  viewport(dc,vr,1);
  int y0 = vr.Height();
  dc.BeginPath();

 	// Plot against frequency
  float f = y0-ysf*(amp[0]-min_y);
 	plot(dc,0,f);
 	plot(dc,0,f, BLACK,PS_SOLID);
 	int x;
 	for(int i=1;i<npts;i++) {
   	f = log10(i*delta_f) - x0;
 		x = xsf*f;
		dc.LineTo(x,y0-ysf*(amp[i]-min_y));
  }
  dc.EndPath();
  dc.StrokePath();
}

void XfmLog::DrawFrame(TDC &dc)
{

  char temp[64];
  TColor tmpcol;
  cr = GetClientRect();
  dc.TextRect(cr,colors[DARKGRAY]);
	plot(dc,cr.Left(),cr.Bottom());
 	plot(dc,cr.Right(),cr.Bottom(),BLACK,5);

 	// Compute working area of screen
	vr = TRect(lmargin,tmargin,cr.Right()-rmargin,cr.Bottom()-bmargin);
  height = vr.Height();
  h_tic = height/5;
  // Bottom margin
  TRect bm = TRect(vr.Left(),vr.Bottom(),cr.Right(),cr.Bottom());
  if(back == 0) {
  	back = new TButton(this,CM_BACK,"Back",
    	vr.Left()+cheight,vr.Top()+cheight,9*cwidth,2*cheight);
    back->Create();
  }
  width = vr.Width();
  height = vr.Height();
  setviewport(dc,cr,1);

  dc.TextRect(vr,colors[WHITE]);
  border(dc,vr.InflatedBy(3,3),0,true,3,0);
  // tmpcol = TColor(colors[DARKGRAY]);
  // Figure out amplitude scaling
 	// Scaling
  // Vertical scale on left
  float tic = 1;
  // Find limits to end on tic marks
  max_y = ceil(depmax);
  min_y = floor(depmin);
  if(max_y - min_y > 4)
  	min_y = max_y-4;
  if(max_y != min_y)
  	ysf = vr.Height()/(max_y-min_y);
  else
  	ysf = vr.Height();
  int x = 0;
  int x1 = vr.Width();
  int y0 = vr.Height();
 	viewport(dc,cr,1);
 	viewport(dc,vr,0);

  // Draw tic marks and write vertical labels
  for(float f=min_y;f<=max_y;f += tic) {
  	int y = y0-(f-min_y)*ysf;
    if(y >= 0 && y <= vr.Height()) {
			plot(dc,x,y);
   		plot(dc,x1,y,BLACK,1);
  		sprintf(temp,"%g",pow(10,f));
    	writexy(dc,-5,y,0,temp,WHITE,-1,2,1,2,BLACK);
      // Draw lines between major tick marks
      // draw at 2, 4, 6, 8
      for(float yt = 2;yt <= 9;yt += 1) {
      	int yy = y - log10(yt)*ysf;
        plot(dc,x,yy);
        plot(dc,x1,yy,BLUE,1,PS_DOT);
      }
  	}
  }

  // Create a rotated font
  TFont *rt = new TFont("Arial",18,0,900,0,FW_BOLD);
  x = - 5*cwidth;
 	int y = tmargin+height/2;
  if(inst_response)
  	writexy(dc,x,y,rt,"Counts/micrometer",WHITE,-1,1,0,2,BLACK);
  else
  	writexy(dc,x,y,rt,"Counts",WHITE,-1,1,0,2,BLACK);
  delete rt;


  // Figure out horizontal scaling
	max_x = ceil(log10(delta_f*npts));
  min_x = floor(log10(delta_f));
  // Don't handle frequencies below 0.01 (100 sec)
  if(min_x < -2)
  	min_x = -2;
  tic = 1;
  x0 = min_x;
  // Find limits to end on tic marks
  float x2 = max_x - min_x;
  if(x1 != 0)
  	xsf = vr.Width()/x2;
  else
  	xsf = vr.Width();

	// Draw tick marks
  int y1 = vr.Height();
  for(float f=min_x;f<=max_x;f += tic) {
  	int x = (f-x0)*xsf;
    if(x >= 0 && x <= vr.Width()) {
			plot(dc,x,y0);
   		plot(dc,x,0,BLACK,1);
      for(float xt = 2;xt <= 9;xt += 1) {
      	int xx = x + log10(xt)*xsf;
        plot(dc,xx,y0);
        plot(dc,xx,0,BLUE,1,PS_DOT);
      }
    }
  }
	// Write the period at the bottom
  viewport(dc,cr,1);
	viewport(dc,bm,0);
  for(float f=min_x;f<=max_x;f += tic) {
  	int x = (f-x0)*xsf;
    if(x >= 0 && x <= vr.Width()) {
  		sprintf(temp,"%g",pow(10,f));
    	writexy(dc,x,cheight/4,0,temp,WHITE,-1,1,2,2,BLACK);
  	}
  }
  static char *title = "Frequency (Hz)";
  writexy(dc,vr.Width()/2,cheight*1.25,0,title,WHITE,-1,1,2,2,BLACK);
 	x = bm.Left();
  x2 = x + cwidth*4;		// position for time spin control
	// Compute positions for frequency, period and amplitude display boxes
  int x_amp = bm.Right() - cwidth*16; // Position for amplitude display
  int x_per = x_amp - cwidth*16;
  int x_freq = x_per - cwidth*16;
  y = bm.Bottom() - 3*cheight;
  viewport(dc,cr,1);
	writexy(dc,x_amp,y,0,"Amplitude",WHITE,-1,0,2);
	writexy(dc,x_per,y,0,"Period (s)",WHITE,-1,0,2);
	writexy(dc,x_freq,y,0,"Frequency (Hz)",WHITE,-1,0,2);
  // Create font since system font draws too small on printer
  TFont *font = new TFont("MS Sans Serif",cheight,0,0,0,FW_BOLD);

  delete font;
  if(amplitude == 0) {
  	y += 1.25*cheight;
    amplitude = new TEdit(this,CM_AMPLITUDE,"",x_amp,y,cwidth*10,cheight+3);
    amplitude->Create();
    period_box = new TEdit(this,CM_AMPLITUDE,"",x_per,y,cwidth*10,cheight+3);
    period_box->Create();
    frequency_box = new TEdit(this,CM_AMPLITUDE,"",x_freq,y,cwidth*10,cheight+3);
    frequency_box->Create();
  }
}

void XfmLog::Paint(TDC&dc, bool, TRect& )
{
	DrawFrame(dc);
 	DrawCurve(dc);
}


