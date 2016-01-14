//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         raveltime.cpp
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TravelTime (TWindow).
//
//----------------------------------------------------------------------------
#include "as1.h"


//{{TravelTime Implementation}}
DEFINE_RESPONSE_TABLE1(TravelTime, TWindow)
  EV_WM_LBUTTONDOWN,
  EV_WM_LBUTTONUP,
  EV_WM_MOUSEMOVE,
  EV_COMMAND(CM_BACK,					CmBack),
END_RESPONSE_TABLE;



TravelTime::TravelTime(TWindow* parent, KSAC *ksac, const char far* title, TModule* module)
:
    TWindow(parent, title, module)
{
  // INSERT>> Your constructor code here.
  Attr.Style = WS_MAXIMIZE | WS_MAXIMIZEBOX	| WS_BORDER | WS_CAPTION;

  back = 0;
  bm = 0;
  moving = false;
  //time_length = 60*60;
  //distance_length = 150;
 	int npts = ksac->GetSACHV(NPTS);
  float sps = 1/ksac->GetSACHV(DELTA);
  time_length = 1.5*npts/sps;
  // Round to multiple of 10
  time_length = 10*(int)(time_length/10);
  if(time_length > 5400)
  	time_length = 5400;
  if(time_length < 300)
  	time_length = 300;
  distance_length = time_length/25;
  if(distance_length < 5)
  	distance_length = 5;
  distance_length = 10*(int)(distance_length/10);
  if(distance_length > 180)
  	distance_length = 180;
  this->ksac = ksac;

  TClientDC dc(*this);
  dc.GetTextMetrics(system_tm);
  cheight = system_tm.tmHeight;
  cwidth = system_tm.tmAveCharWidth;
  lmargin = 8*cwidth;
  rmargin = 2*cwidth;
  tmargin = cheight/2;
  bmargin = 2.5*cheight;
  x_offset = 0;
  y_offset = cheight*15;
  SetCursor(0,IDC_SIZEALL);
}


TravelTime::~TravelTime()
{
  //Destroy(IDCANCEL);
  if(back)
  	delete back;
  if(bm)
  	delete bm;
}
void TravelTime::SetupWindow()
{
  // Default SetUpWindow processing.
  //
  TWindow::SetupWindow();
}

void TravelTime::SetDepth(float d)
{
	depth = d;
  // Delete bitmap since we might have a new depth
  if(bm) {
  	delete bm;
    bm = 0;
  }
}


void TravelTime::CmBack()
{
  ShowWindow(SW_HIDE);
}

void TravelTime::EvLButtonDown(UINT, TPoint &p)
{
	mouse = p;
  moving = true;
}
void TravelTime::EvLButtonUp(UINT, TPoint &)
{
	moving = false;
  back->Invalidate();
}
void TravelTime::EvMouseMove(UINT, TPoint &p)
{
	static TRect oldrect;
  if(oldrect == TRect(0,0,0,0))
  	oldrect = TRect(cr);
	if(moving) {
		x_offset += p.x - mouse.x;
    y_offset += p.y - mouse.y;
    mouse = p;
    TClientDC dc(*this);
    DrawBitmap(dc,&oldrect);
    oldrect = DrawPicks(dc);
    Validate();
  }
}
//---------------------------
// DrawCurves
#define NUMWAVES 6
void TravelTime::DrawCurves(TDC &dc)
{
  JB *P_wave;			// P arrival
  JB *pP_wave;		// pP arrival
  JB *PP_wave;		// PP	arrival
  JB *S_wave;			// S arrival
  JB *sS_wave;		// sS arrival
  JB *SS_wave;		// SS arrival

	static char *lbl[] = {"P","pP","PP","S","sS","SS"};
  // Have to put "_" in front of pP and sS since in Windows/DOS
  // the names are the same as PP and SS
  P_wave 		= new JB("P",depth);
  pP_wave 	= new JB("_pP",depth);
  PP_wave 	= new JB("PP",depth);
  S_wave 		= new JB("S",depth);
  sS_wave 	= new JB("_SS",depth);
  SS_wave 	= new JB("SS",depth);

  setviewport(dc,vr,1);
  int top = vr.Top() + height - 140*ysf;
  TRect vre(vr.Left(),top,vr.Right(),vr.Bottom());
  //ysf = height/distance_length;
  //xsf = width/time_length;
  #if 0
 	float p = ksac->GetTn(0);
  if(p != 0 && p != -12345.) {
  	x_offset = -(p-600)*xsf;
  }
  #endif
  y0 = height/2;
  float *y[NUMWAVES];
  y[0] = P_wave->wave;
  y[1] = pP_wave->wave;
  y[2] = PP_wave->wave;
  y[3] = S_wave->wave;
  y[4] = sS_wave->wave;
  y[5] = SS_wave->wave;
  int len[NUMWAVES];
  len[0] = P_wave->len;
  len[1] = pP_wave->len;
  len[2] = PP_wave->len;
  len[3] = S_wave->len;
  len[4] = sS_wave->len;
  len[5] = SS_wave->len;
  for(int j = 0;j<NUMWAVES;j++) {
    // Skip pP and sS if at zero depth or close to it
    if(j == 1 || j == 4) {
    	if(depth < 16.5)
        continue;
    }
  	float xxx = 0;
  	float yyy = 0;
  	float xxx2;
  	float yyy2;
  	plot(dc,0,height);
    float *yy = y[j];
    int len1 = len[j];
    float xlabel = 0;
    float ylabel = 0;
    // flag tells us when to start drawing
    bool flag = false;
  	for(int i=0;i<len1;i++) {
      float yytmp = yy[i];
      if(yytmp == 0)
      	continue;
  		xxx2 = xxx;
    	yyy2 = yyy;
    	xxx = yytmp*xsf;
      yyy = height - i*ysf;
      if(flag) {
      	// If P wave, plot in red
        if(j >= 0 && j <= 2)
					plot(dc,xxx,yyy,LIGHTBLUE,2);
        else
        	plot(dc,xxx,yyy,LIGHTGREEN,2);
	    } else {
				// Move to first point
  			plot(dc,xxx,yyy,0);
      	flag = true;
      }
      if(yyy < 0) break;
      if(xxx > width) break;
    	xlabel = xxx;
    	ylabel = yyy;
    }
    if(xlabel > width)
    	xlabel = width - cwidth;
    if(ylabel < 0)
    	ylabel = cheight;
    #if 1
    // Extend, by extrapolation, to difracted wave
    // Chop at 140 degrees
    float yd = yyy2-yyy;
    float xd = xxx-xxx2;
    setviewport(dc,vre,1);
    setviewport(dc,vr,0);
    if(yd != 0)
      plot(dc,xxx+yyy*xd/yd,0,BLACK,1,PS_DOT);
    setviewport(dc,vr,1);
    #endif
    // Move P and S labels up a bit
    int yadjust = 0;
    if(j == 0 || j == 3)
    	yadjust = cheight;
    // Move PP and SS labels even more
    if(j == 1 || j == 4)
    	yadjust = 2*cheight;
    // Label this curve
    if(xlabel <= 0)
    	continue;
    if(xlabel > width)
    	writexy(dc,width-cwidth,ylabel+cheight,0,lbl[j],BLACK,-1,2,2);
    else if(yyy < 0)
    	writexy(dc,xlabel,cheight/2,0,lbl[j],BLACK,-1,0,2);
    else
    	writexy(dc,xlabel,ylabel+yadjust,0,lbl[j],BLACK,-1,0,2);
  }
  // Draw surface waves
  // Love: (G)
  // Plot from 15 minutes on
  float t0 = 900;
  //float sw_vel = 4.3/111.1;
  float sw_vel = 0.03148; // Degrees per second. Provided by Bob McClure. 2007.08.03.
  int gg = height - ysf*t0*sw_vel;
  plot(dc,t0*xsf,gg);
  gg = height - ysf*time_length*sw_vel;
  plot(dc,width,gg,BLACK,1);
  int x_sw = width;
  if(gg < 0) {
  	gg = 0;
    x_sw = xsf*(height/(ysf*sw_vel));
  }
 	writexy(dc,x_sw-cwidth,gg,0,"Love Surface Waves",BLACK,-1,2,2);

  // Rayleigh
  // Plot Rayleigh from 4 minutes on
  t0 = 240;
  //sw_vel = 3.8/111.1;
  sw_vel = 0.02753;		// Degrees/sec provided by Bob McClure
  gg = height - ysf*t0*sw_vel;
  plot(dc,t0*xsf,gg);
  gg = height - ysf*time_length*sw_vel;
  plot(dc,width,gg,BLACK,1);
  x_sw = width;
  if(gg < 0) {
  	gg = 0;
    x_sw = xsf*(height/(ysf*sw_vel));
  }
 	writexy(dc,x_sw-cwidth,gg+cheight,0,"Rayleigh Surface Waves",BLACK,-1,2,2);
  delete P_wave;
  delete pP_wave;
  delete PP_wave;
  delete S_wave;
  delete sS_wave;
  delete SS_wave;
}

//----------------
// DrawFrame()
//
void TravelTime::DrawFrame(TDC &dc)
{

  char temp[64];
  TColor tmpcol;
  cr = GetClientRect();
  dc.TextRect(cr,colors[DARKGRAY]);

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
  // Find maximum pick to determine time_length
  #if 0
  for(int i=0;i<10;i++) {
  	float p = ksac->GetTn(i);
    if(p != 0 && p != -12345.0) {
    	if(p>time_length)
      	time_length = p;
    } else
      break;
  }
  // Round time_length
  time_length /= 600;
  time_length *= 600;
  #endif
  width = vr.Width();
  height = vr.Height();
  setviewport(dc,cr,1);
  xsf = width/time_length;
  ysf = height/distance_length;

  dc.TextRect(vr,colors[WHITE]);
  border(dc,vr.InflatedBy(3,3),0,true,3,0);
  // tmpcol = TColor(colors[DARKGRAY]);
  // Draw vertical axis
  int y;
  int dec = 10;
  if(distance_length < 11)
  	dec = 1;
  else if(distance_length < 21)
  	dec = 2;
  for(int i=0;i<= distance_length;i += dec) {
  	y = tmargin+height - ysf*i;
    sprintf(temp,"%d",i);
    writexy(dc,cwidth*7,y,0,temp,WHITE,-1,2,1,2,BLACK);
  }
  // Create a rotated font
  TFont *rt = new TFont("Arial",18,0,900,0,FW_BOLD);
  int x = lmargin - 5*cwidth;
 	y = tmargin+height/2;
  writexy(dc,x,y,rt,"Great circle arc distance in degrees",WHITE,-1,1,0,2,BLACK);
  delete rt;


  setviewport(dc,vr,1);
	// Draw tick marks
  for(int i=0;i< distance_length;i += dec) {
  	int y = height - i*ysf;
		plot(dc,0,y);
   	plot(dc,6,y,BLACK,1);
    int x = vr.Width();
 		plot(dc,x,y);
   	plot(dc,x-6,y,BLACK,1);
  }
  float time_inc = 300;
  if(time_length < 910)
  	time_inc = 60;
  else if(time_length < 181)
  	time_inc = 120;
  for(int i=time_inc;i<time_length;i += time_inc) {
  	int x = i*xsf;
    if(x <= vr.Width()) {
  		plot(dc,x,vr.Height()-6);
    	plot(dc,x,vr.Height(),BLACK,1);
  		plot(dc,x,0);
    	plot(dc,x,6,BLACK,1);
  	}
  }
  // Write the minutes at the bottom
  setviewport(dc,cr,1);
	setviewport(dc,bm,0);
  for(int i=0;i<=time_length;i += time_inc) {
  	int x = i*xsf;
    if(x >= 0 && x <= vr.Width()) {
  		sprintf(temp,"%d",i/60);
    	writexy(dc,x,cheight/4,0,temp,WHITE,-1,1,2,BLACK);
  	}
  }
  writexy(dc,vr.Width()/2,cheight*1.25,0,"Minutes",WHITE,-1,1,2,BLACK);
  setviewport(dc,cr,1);
  // Draw a rectangle around entire window
}
//extern LOGPALETTE *loadLogPalette();

TBitmap *TravelTime::MakeBitmap(TDC &dc)
{
	// Set viewport origin to traces region
  setviewport(dc,cr,1);
	TMemoryDC *dc_mem = new TMemoryDC(dc);
 	dc_mem->SetBkMode(TRANSPARENT);
	// Create device dependent bitmap
	// Create bitmap
	TBitmap	*bm_mem = new TBitmap(dc,vr.Width(),vr.Height());
	// Connect it to memory DC
	dc_mem->SelectObject(*bm_mem);
	// Set the coordinates to upper-left
  dc_mem->SetViewportOrg(0,0);
  dc.SetViewportOrg(vr.TopLeft());
  dc.SelectClipRgn(TRegion(cr));
  dc_mem->BitBlt(TRect(0,0,vr.Width(),vr.Height()),dc,TPoint(0,0),SRCCOPY);
  delete dc_mem;
  return bm_mem;
}

TRect TravelTime::DrawPicks(TDC &dc)
{
  setviewport(dc,vr,1);
  height = vr.Height();
  plot(dc,0,y_offset);
  plot(dc,width,y_offset,RED,1);
  char temp[16];
  // Compute degrees
  double deg = (float)(height-y_offset)/ysf;
  sprintf(temp,"%.4g",deg);
 	writexy(dc,5,y_offset,0,temp,RED,-1,0,0);
  for(int i=0;i<10;i++) {
  	float p = ksac->GetTn(i);
    if(p != 0 && p != -12345.0) {
    	int x = xsf*p;
      x += x_offset;
      plot(dc,x,y_offset+h_tic);
      plot(dc,x,y_offset-h_tic,RED,2);
    } else
      break;
  }
  DrawTrace(dc);
  TRect rect(0,y_offset-h_tic-1,vr.Width(),y_offset+h_tic+1);
  return rect;
}
//---------------------------
// DrawTrace
//
// h: is height of window for trace
void TravelTime::DrawTrace(TDC &dc)
{
 	int npts = ksac->GetSACHV(NPTS);
 	float *data = ksac->GetData();
  float sps = 1/ksac->GetSACHV(DELTA);
 	plot(dc,x_offset,y_offset,0,1);
 	plot(dc,x_offset,y_offset,BLACK,1);
 	// Compute scaling factor
 	ksac->GetMinMax();
 	float depmin = ksac->GetSACHV(DEPMIN);
 	float depmax = ksac->GetSACHV(DEPMAX);
  float depmen = ksac->GetSACHV(DEPMEN);

  //if(depmin < 0)
  //	depmin = -depmin;
 	//float depmen = ksac->GetSACHV(DEPMEN);
 	// See if set
 	float scale = 1;
  float xxsf = xsf/sps;
 	if(depmin != -12345.0 && depmax != -12345.0) {
		// Scaling
    if((depmax-depmen) > (depmen-depmin)) {
    		scale = h_tic/(depmax-depmen);
    } else {
    		scale = h_tic/(depmen-depmin);
    }
  }
 	// Draw trace
  // Decimate to speed up redrawing
  int dec = 0.2/xxsf;
  if(dec < 1)
  	dec = 1;
  dc.SelectObject(TPen(TColor(RGB(255,0,0))));
 	for(int i=0;i<npts;i += dec) {
  	dc.LineTo(x_offset+i*xxsf,y_offset-scale*(data[i]-depmen));
  }
  dc.SelectObject(TPen(TColor(RGB(0,0,0))));
}


// Redraw all or part of the screen.
// Rectangle pr is the part to refresh.  If pr ==0, to the
// entire viewport.
void TravelTime::DrawBitmap(TDC &dc, TRect *pr)
{
  setviewport(dc,cr,1);
	TMemoryDC *dc_mem = new TMemoryDC(dc);
	dc_mem->SetBkMode(TRANSPARENT);
	// Connect bitmap to memory DC
	dc_mem->SelectObject(*bm);
  if(pr == 0) {
		// Set the coordinates to upper-left of viewport for both DCs
 		//dc_mem->SetViewportOrg(0,0);
		dc.BitBlt(vr,*dc_mem,TPoint(0,0),SRCCOPY);
  } else {
		// Set the coordinates to upper-left of viewport for both DCs
 		setviewport(dc,vr,0);
		dc.BitBlt(*pr,*dc_mem,pr->TopLeft(),SRCCOPY);
  }
  dc_mem->RestoreObjects();                //Roger tried
  delete dc_mem;
}

void TravelTime::Paint(TDC&dc, bool, TRect& )
{
	DrawFrame(dc);
  if(bm == 0) {
  	DrawCurves(dc);
    bm = MakeBitmap(dc);
  }
  else {
  	DrawBitmap(dc);
  }
 	DrawPicks(dc);
}




