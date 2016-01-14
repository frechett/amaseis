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
//  Source file for implementation of Xfm (TWindow).
//
//----------------------------------------------------------------------------
#include "as1.h"
#define twopi 6.283185306
#define PI 3.141592653
// Gravity
#define g 980.0

//{{Xfm Implementation}}
DEFINE_RESPONSE_TABLE1(Xfm, TWindow)
//{{XfmRSP_TBL_BEGIN}}
  EV_WM_LBUTTONDOWN,
  EV_WM_LBUTTONUP,
  EV_WM_MOUSEMOVE,
  EV_UDN_DELTAPOS(CM_SPINPERIOD, 		CmSpinPeriod),
  EV_COMMAND(CM_BACK,								CmBack),
  EV_BN_CLICKED(CM_RADIO_PERIOD, 		BNClickPeriod),
  EV_BN_CLICKED(CM_RADIO_FREQUENCY, BNClickFrequency),
//{{XfmRSP_TBL_END}}
END_RESPONSE_TABLE;


Xfm::Xfm(TWindow* parent, KSAC *ksac, bool inst_response, const char far* title, TModule* module)
:
    TWindow(parent, title, module)
{
  // Override the default window style for TWindow.
  //
      Attr.Style = WS_BORDER | WS_CAPTION | WS_MAXIMIZEBOX | WS_MAXIMIZE ;

  // INSERT>> Your constructor code here.
 	npts = ksac->GetSACHV(NPTS);
  double mean = ksac->GetSACHV(DEPMEN);
  double delta = ksac->GetSACHV(DELTA);
  if(delta == 0)
  	delta = 1;
  double sps = 1/delta;
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

  realft(data-1,npts,1);  // From Numerical methods book

  this->inst_response = inst_response;

  max_f = sps/2;
  // Compute parameters for instrument response
  double aa = 0;
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
  amp = new double[npts];
  for(int i=0;i<npts;i++) {
  	int j = 2*i;
  	amp[i] = delta*sqrt(pow(data[j],2) + pow(data[j+1],2));
    if(inst_response) {
    	if(i*delta_f < 2)
    		amp[i] *= aa*pow(i*delta_f,3);
      else
      	amp[i] = 0;
    }
  }
  FILE *fp = fopen("EventFT.csv","wt");
    if(fp) {
  	fprintf(fp,"FREQUENCY,AMPLITUDE\n");
    float t = 0;
    double *d = amp;
    for(int i=0;i<npts;i++) {
    	fprintf(fp,"%f,%fl\n",t,*d++);
      t += delta_f;
    }
  	fclose(fp);
  }
  // Response function (temporary)
  //for(int i=0;i<npts;i++)
  //	amp[i] *= pow((i/delta),3);
  // Find minimum and maximum values
  depmax = -1e20;
  for(int i=0;i<npts;i++) {
  	double d = amp[i];
    if(d > depmax) depmax = d;
  }
  if(depmax == 0)
  	depmax = 1;

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
  periodexp = 0;
  period_expansion = 1;
  p_radio = f_radio = 0;
}

void Xfm::SetupWindow()
{
	TWindow::SetupWindow();
}

void Xfm::CmBack()
{
  //ShowWindow(SW_HIDE);
  CloseWindow();
}
Xfm::~Xfm()
{
  //Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.
  delete amp;
  delete data;
  delete periodexp;
  delete f_radio;
  delete p_radio;
}

void Xfm::BNClickPeriod()
{
	profile->x_period = true;
  Invalidate();
}
void Xfm::BNClickFrequency()
{
	profile->x_period = false;
  Invalidate();
}

void Xfm::EvMouseMove(UINT, TPoint &p)
{
 	char buffer[32];
  // Get time
  if(!vr.Contains(p))
  	return;
 	int y = vr.bottom - p.y;
  if(period) {
  	double per = getPeriod(p.x);
    sprintf(buffer,"%.4g",per);
  	period->SetText(buffer);
  }
  if(amplitude) {
  	double d = y/ysf;
  	sprintf(buffer,"%.4g",d);
    amplitude->SetText(buffer);
  }
}
void Xfm::EvLButtonUp(UINT, TPoint &p)
{
}
void Xfm::EvLButtonDown(uint modKeys, TPoint& point)
{
}

// Convert position to a period
double Xfm::getPeriod(int x) {
	double per = 0;
  if(xsf != 0) {
  	per = (x-vr.Left())/xsf;
  }
  return per;
}


void Xfm::DrawCurve(TDC &dc)
{
  viewport(dc,vr,1);
  int y0 = vr.Height();
  dc.BeginPath();
  if(profile->x_period) {
  	// Plot against period
  	// Draw curve from 1 to npts.  That is, leave out the DC term
  	plot(dc,xsf/delta_f,y0-ysf*amp[1]);
  	int x;
  	for(int i=1;i<npts;i++) {
    	double f = i*delta_f;
  		x = xsf/f;
			//dc.LineTo(x,y0-twopi*ysf*f*amp[i]);
			dc.LineTo(x,y0-ysf*amp[i]);
  	}
  } else {
  	// Plot against frequency
  	plot(dc,0,y0-ysf*amp[0]);
  	int x;
  	for(int i=0;i<npts;i++) {
    	double f = i*delta_f;
  		x = xsf*f;
			//dc.LineTo(x,y0-twopi*ysf*f*amp[i]);
			dc.LineTo(x,y0-ysf*amp[i]);
  	}
  }
  dc.EndPath();
  dc.StrokePath();
}

void Xfm::DrawFrame(TDC &dc)
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
  width = vr.Width();
  height = vr.Height();
  setviewport(dc,cr,1);
  ysf = height/depmax;

  dc.TextRect(vr,colors[WHITE]);
  border(dc,vr.InflatedBy(3,3),0,true,3,0);
  // tmpcol = TColor(colors[DARKGRAY]);
  // Figure out amplitude scaling
 	// Scaling
  // Vertical scale on left
  double denom = depmax;
  double range = denom;
  double dtic = 1;
	double d = log10(range);		// How many decades?
  int dec;
  if(d>0)
		dec = d;
  else
   	dec = d-1;
  d = pow10(dec);
  if(d == 0)
  	d = 1;
  double tic = 1;
  double r2 = range/(10*d);
  if(r2 < .2) tic = .2;
  else if(r2 < .5) tic = .5;
  // Compute tic mark values in amplitude unitss
  dtic = tic*d;
  // Find limits to end on tic marks
  double x2 = dtic*ceil(depmax/dtic);
  ysf = vr.Height()/x2;
  int x = 0;
  int x1 = 6;
  int y0 = vr.Height();
 	viewport(dc,cr,1);
 	viewport(dc,vr,0);

  // Draw tic marks
  for(double f=0;f<=depmax;f += dtic) {
  	int y = y0-f*ysf;
    if(y >= 0 && y <= vr.Height()) {
			plot(dc,x,y);
   		plot(dc,x1,y,BLACK,1);
    }
  }
  //setviewport(dc,cr,1);
  for(double f=0;f<=depmax;f += dtic) {
  	int y = y0-f*ysf;
    if(y >= 0 && y <= vr.Height()) {
  		sprintf(temp,"%g",f);
    	writexy(dc,-5,y,0,temp,WHITE,-1,2,1,2,BLACK);
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


  // Figure out period (1/frequency) scaling
  // Maximum period is 1/delta_f
  //denom = max_f;
  if(profile->x_period) {
  	range = 1/(period_expansion*delta_f);
  } else
  	range = (delta_f*npts)/period_expansion;
  dtic = 1;
	d = log10(range);		// How many decades?
  dec;
  if(d>0)
		dec = d;
  else
   	dec = d-1;
  d = pow10(dec);
  if(d == 0)
  	d = 1;
  tic = 1;
  r2 = range/(10*d);
  if(r2 < .2) tic = .2;
  else if(r2 < .5) tic = .5;
  // Compute tic mark values in period units
  dtic = tic*d;
  // Find limits to end on tic marks
  x2 = dtic*ceil(range/dtic);
  xsf = vr.Width()/x2;

	// Draw tick marks
  int y1 = vr.Height();
  for(double f=0;f<=range;f += dtic) {
  	int x = f*xsf;
    if(x >= 0 && x <= vr.Width()) {
			plot(dc,x,y0);
   		plot(dc,x,y0-5,BLACK,1);
    }
  }
	// Write the period at the bottom
  viewport(dc,cr,1);
	viewport(dc,bm,0);
  for(double f=0;f<=range;f += dtic) {
  	int x = f*xsf;
    if(x >= 0 && x <= vr.Width()) {
  		sprintf(temp,"%g",f);
    	writexy(dc,x,cheight/4,0,temp,WHITE,-1,1,2,2,BLACK);
  	}
  }
  static char *title[] = {"Period (s)","Frequency (Hz)"};
  writexy(dc,vr.Width()/2,cheight*1.25,0,title[!profile->x_period],WHITE,-1,1,2,2,BLACK);
 	x = bm.Left();
  x2 = x + cwidth*4;		// position for time spin control
  int x4 = bm.Right() - cwidth*10; // Position for amplitude display
  int x3 = x4 - 12*cwidth;	//  Position for time display
  y = bm.Top()+1.5*cheight;
  viewport(dc,cr,1);
  // Create font since system font draws too small on printer
  TFont *font = new TFont("MS Sans Serif",cheight,0,0,0,FW_BOLD);
 	//writexy(dc,x2+2*cwidth,y+cheight/4,font,"Gain",YELLOW,-1,0,2,2,BLUE);

  //int xx = x + cwidth*6;		// x-position for time expansion control

 	writexy(dc,x+6*cwidth,y+cheight/4,font,"Expansion",YELLOW,-1,0,2,2,BLUE);
  delete font;
	if(periodexp == 0) {
  	int yy = y+cheight/4;
  	periodexp = new TEdit(this,CM_TIMEEXPANSION,"",x,yy,cwidth*4,cheight+3,16);
    periodexp->Create();
  	period = new TEdit(this,CM_TIME,"",x3,yy,cwidth*8,cheight+3,16);
    period->Create();
  	amplitude = new TEdit(this,CM_AMPLITUDE,"",x4,yy,cwidth*8,cheight+3,16);
    amplitude->Create();
    spin_period = new TUpDown(this,CM_SPINPERIOD,x2,y,cwidth*2,2*cheight);
    spin_period->Create();
    spin_period->SetRange(0,30);
    x += 20*cwidth;
    int w = cwidth*16;
    yy += cheight;
    p_radio = new TRadioButton(this,CM_RADIO_PERIOD,"vs period",x,yy,w,cheight);
    p_radio->Create();
    period_box = TRect(x,yy,x+w,yy+cheight);
    period_box.Inflate(1,1);
    p_radio->SetCheck(profile->x_period ? BF_CHECKED : BF_UNCHECKED);
    x += cwidth*18;
    f_radio = new TRadioButton(this,CM_RADIO_FREQUENCY,"vs frequency",x,yy,w,cheight);
    f_radio->Create();
    frequency_box = TRect(x,yy,x+w,yy+cheight);
    frequency_box.Inflate(1,1);
    f_radio->SetCheck(profile->x_period ? BF_UNCHECKED : BF_CHECKED);
  }
  sprintf(temp,"%d",period_expansion);
  periodexp->SetText(temp);
  plot(dc,period_box.TopLeft());
  plot(dc,period_box.TopRight(),BLACK);
  plot(dc,period_box.BottomRight(),BLACK);
  plot(dc,period_box.BottomLeft(),BLACK);
  plot(dc,period_box.TopLeft(),BLACK);

  plot(dc,frequency_box.TopLeft());
  plot(dc,frequency_box.TopRight(),BLACK);
  plot(dc,frequency_box.BottomRight(),BLACK);
  plot(dc,frequency_box.BottomLeft(),BLACK);
  plot(dc,frequency_box.TopLeft(),BLACK);
}
bool Xfm::CmSpinPeriod(TNmUpDown& not)
{
	char buffer[16];
  int x = not.iPos + not.iDelta;
  if(x < 0) x = 0;
  spin_period->SetPos((int)x);
	period_expansion = 0.5+pow(2,x);
	sprintf(buffer,"%d",period_expansion);
	periodexp->SetText(buffer);
  Invalidate();
  return true;
}

void Xfm::Paint(TDC&dc, bool, TRect& )
{
	DrawFrame(dc);
 	DrawCurve(dc);
}

#define SWAP(a,b) tempr=(a);(a)=(b);(b)=tempr
void Xfm::four1(float *data,int nn,int isign)
{
	int n,mmax,m,j,istep,i;
	double wtemp,wr,wpr,wpi,wi,theta;
	double tempr,tempi;

	n=nn << 1;
	j=1;
	for (i=1;i<n;i+=2) {
		if (j > i) {
			SWAP(data[j],data[i]);
			SWAP(data[j+1],data[i+1]);
		}
		m=n >> 1;
		while (m >= 2 && j > m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	}
	mmax=2;
	while (n > mmax) {
		istep=2*mmax;
		theta=6.28318530717959/(isign*mmax);
		wtemp=sin(0.5*theta);
		wpr = -2.0*wtemp*wtemp;
		wpi=sin(theta);
		wr=1.0;
		wi=0.0;
		for (m=1;m<mmax;m+=2) {
			for (i=m;i<=n;i+=istep) {
				j=i+mmax;
				tempr=wr*data[j]-wi*data[j+1];
				tempi=wr*data[j+1]+wi*data[j];
				data[j]=data[i]-tempr;
				data[j+1]=data[i+1]-tempi;
				data[i] += tempr;
				data[i+1] += tempi;
			}
			wr=(wtemp=wr)*wpr-wi*wpi+wr;
			wi=wi*wpr+wtemp*wpi+wi;
		}
		mmax=istep;
	}
}

void Xfm::realft(float *data,int n,int isign)
{
	int i,i1,i2,i3,i4,n2p3;
	double c1=0.5,c2,h1r,h1i,h2r,h2i;
	double wr,wi,wpr,wpi,wtemp,theta;

	theta=3.141592653589793/(double) n;
	if (isign == 1) {
		c2 = -0.5;
		four1(data,n,1);
	} else {
		c2=0.5;
		theta = -theta;
	}
	wtemp=sin(0.5*theta);
	wpr = -2.0*wtemp*wtemp;
	wpi=sin(theta);
	wr=1.0+wpr;
	wi=wpi;
	n2p3=2*n+3;
	for (i=2;i<=n/2;i++) {
		i4=1+(i3=n2p3-(i2=1+(i1=i+i-1)));
		h1r=c1*(data[i1]+data[i3]);
		h1i=c1*(data[i2]-data[i4]);
		h2r = -c2*(data[i2]+data[i4]);
		h2i=c2*(data[i1]-data[i3]);
		data[i1]=h1r+wr*h2r-wi*h2i;
		data[i2]=h1i+wr*h2i+wi*h2r;
		data[i3]=h1r-wr*h2r+wi*h2i;
		data[i4] = -h1i+wr*h2i+wi*h2r;
		wr=(wtemp=wr)*wpr-wi*wpi+wr;
		wi=wi*wpr+wtemp*wpi+wi;
	}
	if (isign == 1) {
		data[1] = (h1r=data[1])+data[2];
		data[2] = h1r-data[2];
	} else {
		data[1]=c1*((h1r=data[1])+data[2]);
		data[2]=c1*(h1r-data[2]);
		four1(data,n,-1);
	}
}
#undef SWAP
// Translation of Bob McClure's long period filter translated from
// Visual BASIC
// 2007.04.27: Modified to Bob McClure's new algorithm
void Xfm::McClureLongPeriod(float *DataWord, int DataSamples, double SamplesperSecond)
{
/**************
Dim DataSum As Single
Dim ip, n, sum1 As Single, sum2 As Single, Plp As Single, n1, nRep, Tc1
Dim BiasRegister0 As Single
Dim DeltaBiasRegister0 As Single
Dim Deltadataregister As Single
Dim Sample As Single, SampleBlock As Single, SampleBare As Single
Dim Tc0 As Single, SigmaF As Single, OmegaF As Single, Omega2F As Single
Dim Omega2P As Single, OmegaP As Single, SigmaP As Single, Qf As Single
Msg = "Have you correctly set up sensor period, damping, and filter period?"
Response = MsgBox(Msg, vbYesNo)
If Response = vbNo Then Exit Sub
Pp = Val(Text2) 'Pendulum period
Qp = Val(Text3) 'Pendulum Q
Pf = Val(Text4) 'Long filter period
Tc0 = Pf * HD.SamplesperSecond 'DC blocking time constant=long filter period
Tc1 = Pp * HD.SamplesperSecond 'Pendulum period in samples
'If Tc1 < 5 Then
'   Msg$ = "Sensor period should be at least 5 sample periods!"
'   MsgBox (Msg$)
'   Exit Sub
'End If
'(Tc1 is number of iterations in one pendulum period. The filter should have
'at least 20, so we adjust the number of iterations in the filter loop to
'achieve this.)
nRep = 20 \ Tc1
nRep = nRep + 1
Qf = 0.707
OmegaF = 2 * PI / (nRep * Pf * HD.SamplesperSecond)
Omega2F = OmegaF * OmegaF
SigmaF = OmegaF / Qf
OmegaP = 2 * PI / (nRep * Pp * HD.SamplesperSecond)
// Omega2P = OmegaP * OmegaP
// SigmaP = OmegaP / Qp
Omega2P = OmegaP * OmegaP - Omega2F 'CHANGED<<<<<<
SigmaP = OmegaP / Qp - SigmaF       'CHANGED<<<<<<DataSum = 0

For n = 0 To DataSamples - 1
  DataSum = DataSum + DataWord(n)
Next n
BiasRegister0 = DataSum / (DataSamples)

For n = 0 To DataSamples - 1
  Sample = DataWord(n)

 'samplebare provides unfiltered feedthrough
  SampleBare = Sample
 'Remove bias with long time-constant filter
 'sampleblock provides  bias-filtered feedthrough
  BiasRegister0 = BiasRegister0 + SampleBare / Tc0
  SampleBlock = SampleBare - BiasRegister0
  BiasRegister0 = BiasRegister0 - BiasRegister0 / Tc0
  #if 0 old:
  For n1 = 1 To nRep
   'AllPass, extended period
    Sample = SampleBlock + sum1 * SigmaP + sum2 * Omega2P
    sum1 = SampleBlock + sum1 - sum1 * SigmaF - sum2 * Omega2F
    sum2 = sum2 + sum1
  Next n1
  #endif
  For n1 = 1 To nRep
   'AllPass, extended period
    Sample = SampleBlock + sum1 * SigmaP + Oldsum2 * Omega2P
    sum1 = SampleBlock + sum1 - sum1 * SigmaF - Oldsum2 * Omega2F
    Oldsum2 = sum2
    sum2 = sum2 + sum1
  Next n1

  DataWord(n) = Sample
Next n
'Fname = Mid(Fname, 1, Len(Fname) - 4) & "LP" & Text4 & ".psn"
HD.Datatype = 2
U6Script = U6Script & "Sensor Period:" & Text2 & "s   Sensor Q:" & _
Text3 & "  Filtered Period:" & Text4 & "s,"
Text7 = "Long Period Filtering Done. Save your work before exiting program."
mnuItemSave.Enabled = True
mnuItemPlot_Click
End Sub
******************/
	double DataSum, sum1, sum2, Oldsum2, Plp;
  int ip, n, n1, nRep, Tc1;
  double BiasRegister0, DeltaBiasRegister0, Deltadataregister;
  double Sample, SampleBlock, SampleBare;
  double Tc0, SigmaF, OmegaF, Omega2F;
  double Omega2P, OmegaP, SigmaP, Qf;
	//double Pp = 2;				// Pendulum period
	//double Qp = 0.707; 	//Pendulum Q
	//double Pf = 10;			// Long filter period
  double Pf = profile->Pf;
  double Pp = profile->Pp;
  double Qp = profile->Qp;
  sum1 = sum2 = Oldsum2 = 0;	// Added 2006.05.02. Hadn't been initializing
  if(DataSamples == 0)
  	return;
  if(SamplesperSecond <= 0)
  	return;
  if(Pf <= 0 || Pp <= 0 || Qp <= 0)
  	return;
	Tc0 = Pf * SamplesperSecond; // 'DC blocking time constant=long filter period
	Tc1 = Pp * SamplesperSecond; //'Pendulum period in samples
	//if(Tc1 < 5) {
	//   Msg$ = "Sensor period should be at least 5 sample periods!"
	//   MsgBox (Msg$)
  //   Exit Sub
	//End If
	//(Tc1 is number of iterations in one pendulum period. The filter should have
	//at least 20, so we adjust the number of iterations in the filter loop to
	//achieve this.)
	nRep = 20/Tc1;
	nRep = nRep + 1;
	Qf = 0.707;
	OmegaF = twopi / (nRep * Pf * SamplesperSecond);
	Omega2F = OmegaF * OmegaF;
	SigmaF = OmegaF / Qf;
	OmegaP = twopi / (nRep * Pp * SamplesperSecond);
	Omega2P = OmegaP * OmegaP - Omega2F;
	SigmaP = OmegaP / Qp - SigmaF;
	DataSum = 0;

	for(n = 0;n < DataSamples; n++)
  	DataSum = DataSum + DataWord[n];

	BiasRegister0 = DataSum / (DataSamples);

	for(n = 0;n < DataSamples;n++) {
  	Sample = DataWord[n];

 		//'samplebare provides unfiltered feedthrough
  	SampleBare = Sample;
 		//'Remove bias with long time-constant filter
 		//'sampleblock provides  bias-filtered feedthrough
  	BiasRegister0 = BiasRegister0 + SampleBare / Tc0;
  	SampleBlock = SampleBare - BiasRegister0;
  	BiasRegister0 = BiasRegister0 - BiasRegister0 / Tc0;
  	for(n1 = 1; n1 <= nRep; n1++) {
   		//	'AllPass, extended period
    	Sample = SampleBlock + sum1 * SigmaP + Oldsum2 * Omega2P;
    	sum1 = SampleBlock + sum1 - sum1 * SigmaF - Oldsum2 * Omega2F;
      Oldsum2 = sum2;
    	sum2 = sum2 + sum1;
  	}
 		DataWord[n] = Sample;
  }
}
void Xfm::McClureLowPass(float *DataWord, int DataSamples, double SamplesperSecond,
	double PeriodFc)
{
/***************************************
// From Bob's help notes:
2. LOW PASS FILTER:
  The Low Pass Filter attenuates high frequency noise.
  It does lowpass filtering forward in time and then backward in time.
  The result is the frequency response of a 2 pole or 4 pole filter,
  but without any time delay distortion. Four filter types are implemented:
  (type 0) two simple real poles (RC filtering),
  (type 1) four simple poles,
  (type 2) the Bessel filter, and
  (type 3), the Butterworth filter.
  The sharpness of rolloff increases from type 0 to type 3.
Private Sub mnuItemLowPass_Click()
Dim OmegaC, Z$, SamplePeriod, Ts, Fc, Sps, bite As Byte, OmegaS, OmegaCT
Dim nFirst, nLast, nLoop, nindex, DataIn As Single, DataOut As Single
Msg = "Have you correctly set up the lowpass filter type and corner period?"
Response = MsgBox(Msg, vbYesNo)
If Response = vbNo Then Exit Sub
Sps = HD.SamplesperSecond
OmegaS = 2 * PI * Sps
SamplePeriod = 1 / Sps
Z$ = Text5
If Z$ = "" Then Exit Sub
If Val(Z$) < 5 * SamplePeriod Then
  PeriodFc = Round(5 * SamplePeriod, 1)
  Text5 = Str(PeriodFc)
Else
  PeriodFc = Val(Z$)
End If
OmegaC = 2 * PI / PeriodFc
OmegaCT = OmegaC * SamplePeriod
Select Case FilterType
  Case Is < 2
   'Real poles
    SigmaPole(0) = OmegaCT
    SigmaPole(1) = OmegaCT
  Case Is = 2
    'Bessel
    OmegaPole(0) = 0.4 * OmegaCT
    SigmaPole(0) = 1.32 * OmegaCT
    OmegaPole(1) = 1.215 * OmegaCT
    SigmaPole(1) = 0.915 * OmegaCT
  Case Is = 3
    'Butterworth
    OmegaPole(0) = 0.9329 * OmegaCT
    SigmaPole(0) = 0.3827 * OmegaCT
    OmegaPole(1) = 0.3827 * OmegaCT
    SigmaPole(1) = 0.9239 * OmegaCT
  End Select
If FilterType < 2 Then 'real pole filter
  B1(0) = Exp(-SigmaPole(0))
  B1(1) = Exp(-SigmaPole(1))
  GainF(0) = 1 - B1(0)
  GainF(1) = 1 - B1(1)
Else  'complex pole filter
  B1(0) = 2 * Exp(-SigmaPole(0)) * Cos(OmegaPole(0))
  B2(0) = -Exp(-2 * SigmaPole(0))
  GainF(0) = 1 - B1(0) - B2(0)
  B1(1) = 2 * Exp(-SigmaPole(1)) * Cos(OmegaPole(1))
  B2(1) = -Exp(-2 * SigmaPole(1))
  GainF(1) = 1 - B1(1) - B2(1)
End If
nFirst = 0
nLast = DataSamples - 1

For nLoop = 0 To 1
For nindex = nFirst To nLast
  If nLoop = 0 Then
    n = nindex
  Else
    n = nLast - nindex
  End If
  If FilterType > 1 Then
    Sample2(0) = Sample1(0)
    Sample1(0) = sample0(0)
    sample0(0) = B1(0) * Sample1(0) + B2(0) * Sample2(0) + GainF(0) * DataWord(n)
    DataOut = 0.5 * (sample0(0) + Sample1(0))
    If FilterType < 5 Then
      DataIn = DataOut
      Sample2(1) = Sample1(1)
      Sample1(1) = sample0(1)
      sample0(1) = B1(1) * Sample1(1) + B2(1) * Sample2(1) + GainF(1) * DataIn
      DataWord(n) = 0.5 * (sample0(1) + Sample1(1))
    Else
      DataWord(n) = DataOut
    End If
  Else
    Sample1(0) = sample0(0)
    sample0(0) = B1(0) * Sample1(0) + GainF(0) * DataWord(n)
    If FilterType = 0 Then
      DataWord(n) = 0.5 * (sample0(0) + Sample1(0))
    Else
      Sample1(1) = sample0(1)
      sample0(1) = B1(1) * Sample1(1) + GainF(1) * sample0(0)
      DataWord(n) = 0.5 * (sample0(1) + Sample1(1))
    End If
  End If
Next nindex
Next nLoop

HD.Datatype = 2
Z$ = "LPF" & Trim(Str(FilterType)) + Trim(Str(PeriodFc)) + ".psn"
'Fname = Mid(Fname, 1, Len(Fname) - 4) + Z$
'U6Script = U6Script & "LPF type " & Trim(Str(FilterType)) & " Corner=" _
'           & Trim(Str(PeriodFc)) & "s zero lag,"
U6Script = U6Script & "Zero lag LPF: " & FilterDescription(FilterType) & _
" Corner Period:" & Trim(Str(PeriodFc)) & "s,"
Text7 = "Low Pass Filtering done. Save your work before exiting program."
mnuItemSave.Enabled = True
mnuItemPlot_Click
End Sub
************************************************/
	// Input:
  // FilterType: 0, 1, 2, or 3
  // PeriodFc: Cutoff period
	double OmegaC, SamplePeriod, Sps, OmegaS, OmegaCT;
  //unsigned char bite;
	int nFirst, nLast, nLoop, nindex;
  double DataIn, DataOut;
  double OmegaPole[2];
  double SigmaPole[2];
  double B1[2], B2[2];
  double GainF[2];
  double Sample0[2], Sample1[2], Sample2[2];
  int FilterType = 3;	// Set for Butterworth filter
  Sps = SamplesperSecond;
  if(Sps == 0)
  	Sps = 1;
	SamplePeriod = 1 / Sps;
  // If PeriodFc produces a cutoff more than Nyquist, don't do any filtering
  if(PeriodFc < 2*SamplePeriod)
  	return;
  if(PeriodFc < 5* SamplePeriod)
  	PeriodFc = 5*SamplePeriod;
	//OmegaS = twopi * Sps;
	OmegaC = twopi / PeriodFc;
	OmegaCT = OmegaC * SamplePeriod;

	switch (FilterType) {
  case 0: case 1:  	//Real poles
    SigmaPole[0] = OmegaCT;
    SigmaPole[1] = OmegaCT;
    break;
  case 2:						// Bessel
    OmegaPole[0] = 0.4 * OmegaCT;
    SigmaPole[0] = 1.32 * OmegaCT;
    OmegaPole[1] = 1.215 * OmegaCT;
    SigmaPole[1] = 0.915 * OmegaCT;
    break;
  case 3:						// Butterworth
    OmegaPole[0] = 0.9329 * OmegaCT;
    SigmaPole[0] = 0.3827 * OmegaCT;
    OmegaPole[1] = 0.3827 * OmegaCT;
    SigmaPole[1] = 0.9239 * OmegaCT;
    break;
  }
	if(FilterType < 2) { // Then real pole filter
  	B1[0] = exp(-SigmaPole[0]);
  	B1[1] = exp(-SigmaPole[1]);
  	GainF[0] = 1 - B1[0];
  	GainF[1] = 1 - B1[1];
  } else {						// complex pole filter
  	B1[0] = 2 * exp(-SigmaPole[0]) * cos(OmegaPole[0]);
  	B2[0] = -exp(-2 * SigmaPole[0]);
  	GainF[0] = 1 - B1[0] - B2[0];
  	B1[1] = 2 * exp(-SigmaPole[1]) * cos(OmegaPole[1]);
  	B2[1] = -exp(-2 * SigmaPole[1]);
  	GainF[1] = 1 - B1[1] - B2[1];
  }
	nFirst = 0;
	nLast = DataSamples - 1;

	for(nLoop = 0;nLoop<=1;nLoop++) {
		for(nindex = nFirst;nindex<=nLast;nindex++) {
    	int n;
  		if(nLoop == 0)
    		n = nindex;
  		else
    		n = nLast - nindex;

  		if(FilterType > 1) {
    		Sample2[0] = Sample1[0];
    		Sample1[0] = Sample0[0];
    		Sample0[0] = B1[0] * Sample1[0] + B2[0] * Sample2[0] + GainF[0] * DataWord[n];
    		DataOut = 0.5 * (Sample0[0] + Sample1[0]);
    		if(FilterType < 5) {
      		DataIn = DataOut;
      		Sample2[1] = Sample1[1];
      		Sample1[1] = Sample0[1];
      		Sample0[1] = B1[1] * Sample1[1] + B2[1] * Sample2[1] + GainF[1] * DataIn;
      		DataWord[n] = 0.5 * (Sample0[1] + Sample1[1]);
      	} else {
      		DataWord[n] = DataOut;
      	}
    	}
  		else {
    		Sample1[0] = Sample0[0];
    		Sample0[0] = B1[0] * Sample1[0] + GainF[0] * DataWord[n];
    		if(FilterType == 0)
      		DataWord[n] = 0.5 * (Sample0[0] + Sample1[0]);
    		else {
      		Sample1[1] = Sample0[1];
      		Sample0[1] = B1[1] * Sample1[1] + GainF[1] * Sample0[0];
      		DataWord[n] = 0.5 * (Sample0[1] + Sample1[1]);
    		}
      }
    }
  }
}
void Xfm::McClureHighPass(float *DataWord, int DataSamples, double SamplesperSecond, double Pf)
{
/*************************************************
Private Sub mnuItemHighPass_Click()
Dim DataSum As Single, BiasRegister0 As Single, DeltaBiasRegister0 As Single
Dim n, sum1 As Single, sum2 As Single, Pf As Single, nRep, n1
Dim Sample As Single, SampleBare As Single
Dim Tc0 As Single, SigmaF As Single, OmegaF As Single, Omega2F As Single
Dim SampleBlock As Single, Qf As Single
Msg = "Have you correctly set high pass filter period?"
Response = MsgBox(Msg, vbYesNo)
If Response = vbNo Then Exit Sub
Pf = Val(Text6) 'Filter period
Tc0 = Pf * HD.SamplesperSecond 'DC blocking time constant=short filter period
If Tc0 < 5 Then
   Msg$ = "Filter period must be at least 5 sample periods!"
   MsgBox (Msg$)
   Exit Sub
End If
'(Tc0 is number of iterations in one filter period. The filter should have
'at least 20, so we adjust the number of iterations in the filter loop to
'achieve this.)
nRep = 20 \ Tc0
nRep = nRep + 1
Qf = 0.707
OmegaF = 2 * PI / (nRep * Pf * HD.SamplesperSecond)
Omega2F = OmegaF * OmegaF
SigmaF = OmegaF / Qf

For n = 0 To DataSamples - 1
   'samplebare provides unfiltered feedthrough
  SampleBare = DataWord(n)
 'Remove bias with long time-constant filter
  BiasRegister0 = BiasRegister0 + SampleBare / Tc0
  DeltaBiasRegister0 = BiasRegister0 / Tc0

 'sampleblock provides  bias-filtered feedthrough
  SampleBlock = SampleBare - BiasRegister0
  BiasRegister0 = BiasRegister0 - DeltaBiasRegister0
  For n1 = 1 To nRep  'High Pass filter
    Sample = sum1 'Short Period
    sum1 = SampleBlock + sum1 - sum1 * SigmaF - sum2 * Omega2F
    sum2 = sum2 + sum1
  Next n1
DataWord(n) = sum1 - Sample
Next n
HD.Datatype = 2
U6Script = U6Script & "HPF: " & " Corner Period:" & Trim(Str(Pf)) & "s,"
Text7 = "High Pass Filtering done. Save your work before exiting program."
mnuItemSave.Enabled = True
mnuItemPlot_Click
End Sub
**************************************************/
	double BiasRegister0, DeltaBiasRegister0;
	int  	n;
	int 	nRep, n1;
	double Sample, SampleBare;
  double sum1, sum2;
	double	Tc0, SigmaF, OmegaF, Omega2F;
	double SampleBlock, Qf;
	Tc0 = Pf * SamplesperSecond;	// DC blocking time constant=short filter period
	if(Tc0 < 5) {
   return;
  }
	nRep = 20/Tc0;
	nRep = nRep + 1;
	Qf = 0.707;
	OmegaF = twopi / (nRep * Pf * SamplesperSecond);
	Omega2F = OmegaF * OmegaF;
	SigmaF = OmegaF / Qf;
  sum1 = sum2 = 0;
  BiasRegister0 = 0;

	for(n = 0;n < DataSamples; n++) {
   	//samplebare provides unfiltered feedthrough
  	SampleBare = DataWord[n];
 		//Remove bias with long time-constant filter
  	BiasRegister0 = BiasRegister0 + SampleBare / Tc0;
  	DeltaBiasRegister0 = BiasRegister0 / Tc0;

 		//sampleblock provides  bias-filtered feedthrough
  	SampleBlock = SampleBare - BiasRegister0;
  	BiasRegister0 = BiasRegister0 - DeltaBiasRegister0;
  	for(n1 = 1;n1 <= nRep; n1++) {	//High Pass filter
    	Sample = sum1;		//Short Period
    	sum1 = SampleBlock + sum1 - sum1 * SigmaF - sum2 * Omega2F;
    	sum2 = sum2 + sum1;
 	 	}
		DataWord[n] = sum1 - Sample;
	}
}

/******************
// From Bob McClure from a book
static double IIR(double NewSample, int NCoef, double *ACoef, double *Bcoef)
{
	static double y[NCoef+1]; //output samples
  static double x[NCoef+1]; //input samples
  int n;

  //shift the old samples
  for(n=NCoef; n>0; n--) {
     x[n] = x[n-1];
     y[n] = y[n-1];
  }

  //Calculate the new output
  x[0] = NewSample;
  y[0] = ACoef[0] * x[0];
  for(n=1; n<=NCoef; n++)
  	y[0] += ACoef[n] * x[n] - BCoef[n] * y[n];

  return y[0];
}
*****************/

ButterworthLP Xfm::HelicorderFilterLP;
ButterworthLP Xfm::EventFilterLP;
// Default constructor
ButterworthLP::ButterworthLP()
{
	ButterworthLP(1,0,2);
}
// Constructor  for ButterworthLP low-pass filter
// This is taken from web site of Young-Hoo Kwon, 1998
// http://kwon3d.com/theory/filtering/fil.html
ButterworthLP::ButterworthLP(double sps, double fc, int n_order)
{
	this->sps = sps;
  this->fc = fc;
  this->n_order = n_order;
  n2m1 = n_order/2 - 1;
  nyquist = false;
  //for(int k=0;k<=n2m1;k++) {
  //	a[k] = new double(3);
  //	b[k] = new double(3);
  //}
  // Compute frequency ratio but check for fc == 0 first.
  // If fc == 0, the result is that everything is zero.
  if(fc == 0 || fc > 0.3*sps) {	// fc = 0, or exceed Nyquist
  	for(int k = 0;k<=n2m1;k++) {
      for(int i=0;i<3;i++) {
      	a[k][i] = 0;
        b[k][i] = 0;
      }
    }
    if(fc > 0.3*sps) {
    	a[0][0] = 1;	// No filtering if fc > Nyquist
      nyquist = true;
    }
  }
  else {
  	double fr = sps/fc;
    double omega = tan(PI/fr);
    double omega2 = omega*omega;
  	// Compute the coefficients
  	for(int k=0;k<=n2m1;k++) {
    	double d = 2*cos(PI*(2*k+1)/((double)(2*n_order)));
    	d *= omega;
    	//double c = 2*cos(PI*(2*k+1)/((double)(2*n_order)))*omega + omega2;
      double c = 1 + d + omega2;
      b[k][2] = (1 - d + omega2)/c;
      b[k][1] = 2*(omega2 - 1)/c;
      a[k][0] = omega2/c;
      a[k][2] = a[k][0];
      a[k][1] = 2*a[k][0];
  	}
  }
}
ButterworthLP::~ButterworthLP()
{
  //for(int k=0;k<=n2m1;k++) {
  //	delete a[k];
  //	delete b[k];
  //}
}
void ButterworthLP::Filter(float *data, int npts)
{
	// Set inital x and y to initial value
  if(nyquist)
  	return;
  double z = 0;
  if(npts)
  	z = data[0];
  for(int i=0;i<3;i++) {
  	x[i] = y[i] = z;
  }
  for(int i=0;i<npts;i++) {
  	data[i] = NextElement(data[i]);
  }
}
double ButterworthLP::NextElement(double new_sample)
{
  //Calculate the new output
  //shift the old samples
  for(int n=2; n>0; n--) {
     x[n] = x[n-1];
     y[n] = y[n-1];
  }
  for(int k = 0;k <= n2m1;k++) {
   	if (new_sample == 0) new_sample = 0.1;  //Roger 5/5/06  Try to eliminate underflow
  	x[0] = new_sample;
  	y[0] = a[k][0] * x[0];
  	for(int n=1; n<=2; n++)
  		y[0] += a[k][n] * x[n] - b[k][n] * y[n];

  	new_sample = y[0];
  }
  return new_sample;
}

ButterworthHP Xfm::HelicorderFilterHP;
ButterworthHP Xfm::EventFilterHP;
// Default constructor
ButterworthHP::ButterworthHP()
{
	ButterworthHP(1,1,2);
}
// Constructor  for ButterworthHP low-pass filter
// This is taken from web site of Young-Hoo Kwon, 1998
// http://kwon3d.com/theory/filtering/fil.html
// The highpass filter is just like the low pass
// but with a[1] negative
ButterworthHP::ButterworthHP(double sps, double fc, int n_order)
{
	this->sps = sps;
  this->fc = fc;
  this->n_order = n_order;
  n2m1 = n_order/2 - 1;
  //for(int k=0;k<=n2m1;k++) {
  //	b[k] = new double(3);
  //	a[k] = new double(3);
  //}
  // Compute frequency ratio but check for fc == 0 first.
  // If fc is greater than Nyquist, the result is that everything is zero.
  if(fc > 0.5*sps) {
  	for(int k = 0;k<=n2m1;k++) {
      for(int i=0;i<3;i++) {
        b[k][i] = 0;
        a[k][i] = 0;
      }
    }
  }
  else {
  	double fr = sps/fc;
    double omega = tan(PI/fr);
    double omega2 = omega*omega;
  	// Compute the coefficients
  	for(int k=0;k<=n2m1;k++) {
    	double d = 2*cos(PI*(2*k+1)/((double)(2*n_order)));
    	d *= omega;
    	//double c = 2*cos(PI*(2*k+1)/((double)(2*n_order)))*omega + omega2;
      double c = 1 + d + omega2;
      b[k][2] = (1 - d + omega2)/c;
      b[k][1] = 2*(omega2 - 1)/c;
      a[k][0] = 1/c;
      a[k][2] = a[k][0];
      a[k][1] = -2*a[k][0];
  	}
  }
}
ButterworthHP::~ButterworthHP()
{
  //for(int k=0;k<=n2m1;k++) {
  //	delete a[k];
  //	delete b[k];
  //}
}
void ButterworthHP::Filter(float *data, int npts)
{
	// Set inital x and y to initial value
  double z = 0;
  if(npts)
  	z = data[0];
  for(int i=0;i<3;i++) {
  	x[i] = y[i] = z;
  }
  for(int i=0;i<npts;i++) {
  	data[i] = NextElement(data[i]);
  }
}
double ButterworthHP::NextElement(double new_sample)
{
  //Calculate the new output
  //shift the old samples
  for(int n=2; n>0; n--) {
     x[n] = x[n-1];
     y[n] = y[n-1];
  }
  for(int k = 0;k <= n2m1;k++) {
   	if (new_sample == 0) new_sample = 0.1;  //Roger 5/5/06  Try to eliminate underflow
  	x[0] = new_sample;
  	y[0] = a[k][0] * x[0];
  	for(int n=1; n<=2; n++)
  		y[0] += a[k][n] * x[n] - b[k][n] * y[n];
  	new_sample = y[0];
  }
  return new_sample;
}

