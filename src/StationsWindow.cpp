//----------------------------------------------------------------------------
//  Project Stations
//  RunTime Software
//  Copyright © 2007 All Rights Reserved.
//
//  SUBSYSTEM:    Stations Application
//  FILE:         stationswindow.cpp
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TStationsWindow (TWindow).
//
//----------------------------------------------------------------------------

#include <owl/pch.h>
#include <owl/static.h>
#include <stdio.h>
#include <classlib/arrays.h>
#include <process.h>
#include "stationsapp.rh"
#include "plot.h"
#include "profile.h"
#include "progress.h"
#include "stationsapp.h"
#include "stationswindow.h"
#include "ftp_stations.h"
static void triangle(TPoint p, TDC &dc);
Profile *profile;
void Debug(const char *format,...)
{
  va_list argptr;
  va_start(argptr,format);
  char buf[512];
  SYSTEMTIME utc;
  GetSystemTime(&utc);
	sprintf(buf,"%02d:%02d:%02d.%03d: ",utc.wHour,utc.wMinute,utc.wSecond,utc.wMilliseconds);
	//strcpy(buf,p+11);
	//strcat(buf,": ");
	vsprintf(buf+13,format,argptr);
  FILE *fp = fopen("Stations.txt","a+t");
  //FILE *fp = fopen("As1Debug.txt","a+t");
  if(fp) {
  	fputs(buf,fp);
   	fputs("\n",fp);
   	fclose(fp);
  }
	va_end(argptr);
}


Station::Station()
{
	latitude = longitude = 0;
  *name = 0;
  *code = 0;
  tcp_port = 0;
  *tcp_url = 0;
  *ftp_addr = 0;
  *ftp_pw = 0;
  *ftp_user = 0;
  *ftp_path = 0;
  rect = 0;
  point = TPoint(0,0);
}

// Constructor for reading a list of statins from an XML file.
// Example:
// <station
// 	latitude="42.01"
//  longitude="-76.02"
//  name="Binghamton University"
//  code="SBNY"
// />
//
int Station::operator == (const Station& s)
{
	bool ret = true;
	if(s.rect == 0)
  	ret = false;
  else if(rect == 0)
  	ret = false;
	else {
  	ret = s.rect->Touches(*rect);
  }
  return ret;
}
static void removeQuote(char *s)
{
	int len = strlen(s);
  len--;
  if(s[len] == '\"')
  	s[len] = 0;
}
Station::Station(FILE *fp)
{
	char buff[80];
  latitude = longitude = 0;
  tcp_port = 0;
  strcpy(name,"");
  *tcp_url = 0;
  *code = 0;
  rect = 0;
  point = TPoint(0,0);
  bool sta = false;
  char *p;
  // Note that this is a "poor man's parser"
  // All attribute names must be in lower case.
  // Each attribute must be on its own line.
	while(fgets(buff,80,fp)) {
  	// Remove new-line character at end
    buff[strlen(buff)-1] = 0;
   	if(strstr(buff,"/>"))
     	break;
   	p = strstr(buff,"latitude");
   	if(p) {
    	sscanf(p,"latitude=\"%f\"",&latitude);
      continue;
    }
   	p = strstr(buff,"longitude");
   	if(p) {
    	sscanf(p,"longitude=\"%f\"",&longitude);
      continue;
    }
   	p = strstr(buff,"longitude");
   	if(p) {
    	sscanf(p,"longitude=\"%f\"",&longitude);
      continue;
    }
   	p = strstr(buff,"tcp_port");
   	if(p) {
    	sscanf(p,"tcp_port=\"%d\"",&tcp_port);
      continue;
    }
   	p = strstr(buff,"name");
   	if(p) {
    	strcpy(name,p+6);
      removeQuote(name);
      continue;
    }
   	p = strstr(buff,"code");
   	if(p) {
    	sscanf(p,"code=\"%s\"",code);
      removeQuote(code);
      continue;
    }
   	p = strstr(buff,"tcp_url");
   	if(p) {
    	sscanf(p,"tcp_url=\"%s\"",tcp_url);
      removeQuote(tcp_url);
      continue;
    }
    p = strstr(buff,"ftp_addr");
   	if(p) {
    	sscanf(p,"ftp_addr=\"%s\"",ftp_addr);
      removeQuote(ftp_addr);
      continue;
    }
    p = strstr(buff,"ftp_user");
   	if(p) {
    	sscanf(p,"ftp_user=\"%s\"",ftp_user);
      removeQuote(ftp_user);
      continue;
    }
    p = strstr(buff,"ftp_pw");
   	if(p) {
    	sscanf(p,"ftp_pw=\"%s\"",ftp_pw);
      removeQuote(ftp_pw);
      continue;
    }
    p = strstr(buff,"ftp_path");
   	if(p) {
    	sscanf(p,"ftp_path=\"%s\"",ftp_path);
      removeQuote(ftp_path);
      continue;
    }
  }
}

static StationArray stations(10,0,1);


DEFINE_RESPONSE_TABLE1(TStationsWindow, TWindow)
//{{WorldRSP_TBL_BEGIN}}
	EV_WM_SIZE,
	EV_WM_LBUTTONDOWN,
	EV_WM_RBUTTONDOWN,
	EV_WM_LBUTTONUP,
  EV_WM_MOUSEMOVE,
  EV_COMMAND(CM_ZOOM_IN,		CmZoomIn),
  EV_COMMAND(CM_ZOOM_OUT,   CmZoomOut),
  //EV_WM_TIMER,
//{{WorldRSP_TBL_END}}
END_RESPONSE_TABLE;

//{{TStationsWindow Implementation}}



//--------------------------------------------------------
// TStationsWindow
// ~~~~~~~~~~
// Construction/Destruction handling.
//
TStationsWindow::TStationsWindow(TWindow* parent, const char far* title, TModule* module)
:
  TWindow(parent, title, module)
{
  // INSERT>> Your constructor code here.
  sin_array = new float[361];
  cos_array = new float[361];
  for(int i=0;i<3;i++) {
  	xfer[i] = new float[3];
  }
  int i;
  for(i=0;i<=90;i++) {
  	float ang_rad = deg2rad*i;
    cos_array[i] = cos(ang_rad);
    sin_array[i] = sin(ang_rad);
  }
  for(i=91;i<=180;i++) {
  	sin_array[i] = sin_array[180-i];
    cos_array[i] = -cos_array[180-i];
  }
  for(i=181;i<=270;i++) {
  	sin_array[i] = -sin_array[i-180];
    cos_array[i] = -cos_array[i-180];
  }
  for(i=270;i<=360;i++) {
  	sin_array[i] = -sin_array[360-i];
    cos_array[i] = cos_array[360-i];
  }
  bm = 0;
  mdc = 0;
  last_theta = last_psi = last_radius = 0;
  pro = 0;

}


TStationsWindow::~TStationsWindow()
{
  Destroy();

  // INSERT>> Your destructor code here.
  Destroy(IDCANCEL);
 	delete [] sin_array;
  delete [] cos_array;
  for(int i=0;i<3;i++) {
  	delete [] xfer[i];
  }
  if(bm)
  	delete bm;
  if(mdc)
  	delete mdc;
}
void TStationsWindow::SetupWindow()
{
  TWindow::SetupWindow();
  // Get coordinate of center of screen
  int temp;
  lat1 = 40;
  lon1 = -96;
  radius = 3210.;
  OpenKey();
  if(GetKeyInt("Latitude",temp))
  	lat1 = temp;
  if(GetKeyInt("Longitude",temp))
  	lon1 = temp;
  if(GetKeyInt("Radius",temp))
  	radius = ((float)temp)/10.0;
  CloseKey();
  mousedown = false;
  profile = new Profile();
	getStations();
}
extern TSDIDecFrame* frame;

void TStationsWindow::getStations()
{
	// We used to get the xml file from the disk. Now get via ftp from IRIS.
	char buff[80];
  // The following downloads. If you change the 1 to a 0, it will use
  // the xml file in the directory.
  // If there is no password, don't try to download
  if(strlen(profile->ftp_pw)) {
  	pro = new Progress(this);
  	pro->Create();
  	FtpStations *ftpstations = new FtpStations(pro);
  	if(ftpstations->Login()) {
  		pro->SetText("Unable to download IRISEdStations.xml");
    	Sleep(5000);
  	} else
  		pro->SetText("IRISEdStations.xml has been downloaded");
  	ftpstations->DoClose();
  	Sleep(500);
  	pro->SetText("+ zoom in, - zoom out, draw mouse to rotate");
  	Sleep(1000);
  	delete pro;
  }

	Station *station;

	FILE *fp = fopen("IRISEdStations.xml","rt");
  if(fp) {
   	while(fgets(buff,80,fp)) {
     	strlwr(buff);
      if(strstr(buff,"<irisedstations>")) {
       	while(fgets(buff,80,fp)) {
         	strlwr(buff);
          if(strstr(buff,"</irisedstations>")) {
          	fclose(fp);
            return;
          }
          if(strstr(buff,"<station")) {
 						station = new Station(fp);
     				stations.Add(station);
          }
        }
      }
    }
    fclose(fp);
  } else {
  	MessageBeep(MB_ICONASTERISK);
  	pro = new Progress(this);
  	pro->Create();
  	pro->SetText("Unable to load IRISEdStations.xml");
    Sleep(5000);
    delete pro;
  }
}

void TStationsWindow::CmZoomIn()
{
	radius *= 1.25;
  OpenKey();
  SetKeyInt("Radius",(int)radius*10);
	CloseKey();
  Invalidate();
}
void TStationsWindow::CmZoomOut()
{
	radius *= 0.8;
  OpenKey();
  SetKeyInt("Radius",(int)radius*10);
	CloseKey();
  Invalidate();
}
void TStationsWindow::EvSize(uint sizeType, TSize& size)
{
	TWindow::EvSize(sizeType,size);
	Invalidate();
}
void TStationsWindow::EvRButtonDown(uint modKeys, TPoint& point)
{
	TWindow::EvRButtonDown(modKeys, point);
}

void TStationsWindow::EvLButtonUp(uint modKeys, TPoint& point)
{
	mousedown = false;
  OpenKey();
  SetKeyInt("Latitude",(int)lat1);
  SetKeyInt("Longitude",(int)lon1);
  SetKeyInt("Radius",(int)radius*10);
	CloseKey();
	Invalidate();
}
void TStationsWindow::EvLButtonDown(uint modKeys, TPoint& point)
{
	mousedown = true;
	TPoint p = point - center;
  // Determine x (out of page) coordinate
  float x = radius*radius -p.x*p.x - p.y*p.y;
  if(x > 0) {
   	x = sqrt(x);
		Vector v(x,p.x,-p.y);
    v = IXform(xfer,v);
    v = v*(1.0/radius);
    float latitude = asin(v.v[2]);
    latitude *= rad2deg;
		float longitude = atan2(v.v[1],v.v[0]);
		longitude *= rad2deg;
    // See if this click is on a station triangle
    // Compute great-circle-arc distance to each stations
    int n_stations = stations.GetItemsInContainer();
    for(int i=0;i<n_stations;i++) {
    	Station *sta = stations[i];
    	//float delta, baz;
    	//gcarc(sta->latitude,sta->longitude,latitude,longitude,delta,baz);
      //if(delta < 0.25) {	// Are we within a 0.5 degrees?
      if(sta->rect->Contains(point)) {
      	// Have to decide if this is via FTP download or TCP client
        // If the FTP parameters are set, we'll go that way
        Debug("%s:%s:%s",sta->ftp_addr,sta->ftp_user,sta->ftp_pw);
        if(strlen(sta->code)) {
        	char args[128];
        	if(strlen(sta->code) && sta->tcp_port && strlen(sta->tcp_url)) {
      			sprintf(args,"as1.exe %s %d %s",sta->code,sta->tcp_port,sta->tcp_url);
      			// Terminate this program and switch to as1.exe running
      			// in its own directory
      			// The directory is sta->code
      			execl("as1.exe",args,NULL);
          }
          else if(	strlen(sta->ftp_addr) &&
          	 				strlen(sta->ftp_user) &&
                   	strlen(sta->ftp_pw)		&&
                    strlen(sta->ftp_path)) {
      			sprintf(args,"as1.exe %s 0 0 %s %s %s %s",sta->code,sta->ftp_addr,
            	sta->ftp_user,sta->ftp_pw,sta->ftp_path);
      			execl("as1.exe",args,NULL);
          } else
        		MessageBox("Parameters not set for this station.");
        } else {
        	MessageBox("Station code not set for this station");
        }
      }
    }
    // Didn't get a station. record lat and lon
    lat2 = lattemp = latitude;
    lon2 = lontemp = longitude;
  }
}
void TStationsWindow::EvMouseMove(uint modKeys, TPoint& point)
{
	if(!mousedown)
  	return;
  TWindow::EvMouseMove(modKeys, point);
	TPoint p = point - center;
  // Determine x (out of page) coordinate
  float x = radius*radius -p.x*p.x - p.y*p.y;
  if(x > 0) {
   	x = sqrt(x);
		Vector v(x,p.x,-p.y);
    v = IXform(xfer,v);
    v = v*(1.0/radius);
    float latitude = asin(v.v[2]);
    latitude *= rad2deg;
		float longitude = atan2(v.v[1],v.v[0]);
		longitude *= rad2deg;
    if(longitude != lon2 || latitude != lat2) {
    	// Mouse moved. Redraw.
      float delta_lon = longitude - lon2;
      lon2 = longitude;
      float delta_lat = latitude - lat2;
      lat2 = latitude;
      lat1 -= delta_lat;
      lon1 -= delta_lon;
      // abs() only works for int.
      //if(lattemp-lat1 > 1.0 ||
      //	lontemp-lon1 > 1.0	||
      //  lat1-lattemp > 1.0	||
      //  lon1-lontemp > 1.0) {
      //	lattemp = lat1;
      //  lontemp = lon1;
      //	Invalidate();
      //}
    }
  }
}
void TStationsWindow::Paint(TDC& dc, bool erase, TRect& rect)
{
  TWindow::Paint(dc, erase, rect);
  TRect crect = GetClientRect();
  //int w = crect.Width();
  // Draw a U.S. view centered at 96 deg west longitude and 40 deg north latitude
  // Scale size of earth to window
  //Render(lon1, lat1, w*1.3, crect,dc);
  Render(lon1, lat1, radius, crect,dc);
}
void TStationsWindow::Render(float psi,	// Longitude
	float theta,	// latitude
  float radius,
  TRect rect,

  TDC &dc)
{
	this->radius = radius;
	// Find center of rectangle
  center = TPoint(rect.Width()/2,rect.Height()/2);
  dc.TextRect(rect,TColor(0,0,0));
  // Create transformation matric
  float psi_rad = psi*deg2rad;
  // Not sure why I need the minus sign but if I don't use it, I get
  // the wrong N/S hemisphere
  float theta_rad = -theta*deg2rad;
  float s_psi = sin(psi_rad);
  float c_psi = cos(psi_rad);
  float s_theta = sin(theta_rad);
  float c_theta = cos(theta_rad);
  xfer[0][0] = c_theta*c_psi;	xfer[0][1] = c_theta*s_psi;  	xfer[0][2] = -s_theta;
  xfer[1][0] = -s_psi; 				xfer[1][1] = c_psi;  					xfer[1][2] = 0;
  xfer[2][0] = s_theta*c_psi; xfer[2][1] = s_theta*s_psi; 	xfer[2][2] = c_theta;
  // See if we have to draw or if we have the image saved in a bitmap
  if(last_psi != psi ||
  	 last_theta != theta ||
     last_radius != radius)
  {
  // Draw lines of constant latitude from 75 deg south to 75 deg north
  static TPoint points[361];	// Number of points in a circle
  float z1;
  float r;
  bool down;
  // Draw a circle around the whole earth but just fill rectangle if earth
  // is bigger than the rectangle
  dc.SelectObject(TBrush(TColor(255,255,255)));
  if(radius > rect.Width()) {
  	dc.Rectangle(rect);
  } else {
  	dc.Ellipse(center.x-radius,center.y-radius,center.x+radius,center.y+radius);
  }
  for(int lon = 0;lon<=360;lon++) {
   	TPoint p = TPoint(radius*cos_array[lon],radius*sin_array[lon]) + center;
   	if(lon == 0)
   		dc.MoveTo(p);
 	 	else
  		dc.LineTo(p);
  }
 	for(int lat = -75;lat<=75;lat += 15) {
  	if(lat >= 0) {
    	z1 = radius*sin_array[lat];
    }
    else {
    	z1 = -radius*sin_array[-lat];
    }
   	r = radius*cos_array[abs(lat)];
    bool down = false;		// Keep track of pen-up, pen-down
  	for(int lon = 0;lon<=360;lon++) {
   		Vector vector = Vector(r*cos_array[lon],r*sin_array[lon],z1);
      vector = Xform(xfer,vector);
      // The x-axis is pointing toward the user
      if(vector.v[0] < 0) {
      	if(down)
        	down = false;
        continue;
      }
      // y-axis is to the right
      // z-azis is down
      TPoint p = TPoint(vector.v[1],-vector.v[2]) + center;
      if(!down) {
      	dc.MoveTo(p);
        down = true;
      }
      else
      	dc.LineTo(p);
    }
  }
  // Draw a circle around the whole earth
  for(int lon = 0;lon<=360;lon++) {
   	TPoint p = TPoint(radius*cos_array[lon],radius*sin_array[lon]) + center;
   	if(lon == 0)
   		dc.MoveTo(p);
 	 	else
  		dc.LineTo(p);
  }
  // Draw lines of constant longitude
  for(int lon = 0;lon< 180;lon += 15) {
  	down = false;
  	for(int lat = 0;lat < 360;lat++) {
 			Vector vector = Vector(radius*cos_array[lon]*cos_array[lat],
      											radius*sin_array[lon]*cos_array[lat],
                            radius*sin_array[lat]);
      vector = Xform(xfer,vector);
      if(vector.v[0] < 0) {
      	if(down)
        	down = false;
        continue;
      }
      TPoint p = TPoint(vector.v[1],-vector.v[2]) + center;
      if(!down) {
      	dc.MoveTo(p);
        down = true;
      }
      else
      	dc.LineTo(p);
    }
  }
  static char *mapfiles[] =
  	{"COAST","ISLAND","STATE",
     "COUNTRY","LAKE","RIVER"};
  char filename[16];
  // The files COAST.S1, ... COAST.S5 are in different resolutions
	// Figure out what resolution we want
  //static char *res[] = {".S1",".S2",".S3",".S4"};
  //int re = (4*rect.Height())/radius;
  //if(re >= 4)
  //	re = 3;
  //char *ext = res[re];
  static char *ext = ".S2";
  for(int i=0;i<6;i++) {
  	strcpy(filename,mapfiles[i]);
    strcat(filename,ext);
  	FILE *fp = fopen(filename,"rb");
  	if(fp) {
  		DrawBoundaries(fp,radius,dc);
    	fclose(fp);
  	}
  }
  #if 0
  // Don't bother with scale
  // Draw scale.  Compute pixels per km.
  float pix_per_km = (float)radius/radius_earth;
  // See how many km a scale bar 1/10 of the width of the screen would be
  float k_bar = rect.right/(10*pix_per_km);
  // Round to a power of 10
  float a1 = log10(k_bar);
  // Get fractional part and integer part
  int ip = a1;
  float a2 = a1 - ip;
  a2 = pow(10,a2);
  int p = 10;
  if(a2 < 2)
  	p = 2;
  else if (a2 < 5)
  	p = 5;

  int k_10 = p*pow10(ip);
  // See how long the bar will be.
  int bar_len = k_10*pix_per_km;
  int x = 35;
  //int x = (rect.right - bar_len)/2;
  dc.SelectObject(TPen(colors[BLACK]));
  int y = rect.bottom-45;
  int yy = y+20;
  TRect scale_rec = TRect(x-30,y,x+bar_len+30,rect.bottom-5);
  y += 10;
  dc.SelectObject(TBrush(colors[WHITE]));
  dc.Rectangle(scale_rec);
  //dc.TextRect(scale_rec,colors[WHITE]);
  dc.MoveTo(x,y);
  dc.LineTo(x,yy);
  dc.LineTo(x+bar_len,yy);
  dc.LineTo(x+bar_len,y);
  writexy(dc,x,yy,0,"0",BLACK,-1,1,2);
  char temp[24];
  sprintf(temp,"%d km",k_10);
  writexy(dc,x+bar_len,yy,0,temp,BLACK,-1,1,2);
  #endif
  	// Save result in a bitmap
    #if 0
    if(mdc)
    	delete mdc;
    mdc = new TMemoryDC(dc);
  	// Create a compatible bitmap
    if(bm)
    	delete bm;
  	bm = new TBitmap(dc,rect.Width(),rect.Height());
  	mdc->SelectObject(*bm);
  	// Copy bits from DC to memory DC
  	mdc->BitBlt(rect,dc,rect.TopLeft());
    last_psi = psi;
    last_theta = theta;
    last_radius = radius;
    #endif

  }
  //else {
  	// Draw from bitmap
  //	dc.BitBlt(rect,*mdc,rect.TopLeft());
  //}
  #if 0
  // Compute location of epicenter if there is one
  TPoint epicenter;
  if(event_lat != 0) {
    epicenter = LatLon2Point(event_lat,event_lon);
    star(epicenter,dc);
  	circle(epicenter,0,5,dc);
  }
  #endif
  // Draw stations
  // First, zero out defining rectangle in each one.
  int ns = stations.GetItemsInContainer();
  for(int i=0;i<ns;i++) {
  	Station *sta = stations[i];
    delete sta->rect;
    sta->rect = 0;
    sta->point = TPoint(0,0);
  }
  for(int i=0;i<ns;i++) {
    // Get latitude and longitude
    Station *sta = stations[i];
    float st_lat = sta->latitude;
    float st_lon = sta->longitude;
    sta->point = LatLon2Point(st_lat,st_lon);
    sta->rect = new TRect(sta->point+TSize(-7,-20),sta->point+TSize(7,4));
    // See if this triangle overlaps another one.
    //if(i == 0)
    //	continue;
    bool touching = true;
    while(touching) {
    	int j;
    	for(j=0;j<i;j++) {
    		Station *sta_j = stations[j];
      	//if(sta_j == sta) {
        if(sta_j->rect && sta->rect) {
        	if(sta_j->rect->Touches(*sta->rect)) {
      			// They touch
          	sta->rect->Offset(0,5);
          	sta->point += TPoint(0,5);
          	break;
        	}
      	}
    	}
      if(j == i)
      	touching = false;
    }
    triangle(sta->point,dc);
    // Write station name
    char name[24];
    sprintf(name,"%s:%s",sta->code,sta->name);
    writexy(dc,sta->point.x+10,sta->point.y-12,0,name,RED,-1,1,1);
    //
  }
  // Write a "help" section
  writexy(dc,10,10,0,"Zoom in: +, Zoom out: -, Pan: click and drag",BLACK,-1,-1,1);
}
static TColor map_colors[] = {
	TColor(0,0,0),      // 0: Default - black
  TColor(0,127,0),		// 1: Coast - green
	TColor(127,0,0),		// 2: Country - red
  TColor(0,0,127),
  TColor(127,127,0),	// 4: Brown - state
  TColor(0,127,0),		// 5: Island - green
  TColor(0,0,127),		// 6: Lake - blue
  TColor(0,0,127),		// 7: River - blue
  TColor(0,0,0)
};
/******************************************
** draw geographic boundaries
**
** Input: ifp           File pointer for map file
**				TDC						Device context
*****************************************/
void TStationsWindow::DrawBoundaries(FILE *ifp,float radius, TDC &dc)
{
	int n;                // Number of bytes in buffer
	int jj;
	int size_idat;        // Size of buffer
	short *idat;            // Pointer to buffer
	int nbytes;           // Number of bytes per data point
												// nbytes = 4 for minute resolution
												// nbytes = 6 for second resolution
	short xs, ys;
  //short xmx, ymn, ymx, xmn;
	short npts;
	short icode;
	int iread;
	int color;
	TPoint *tpoints;        // Array of point to plot
  static in_use = false;		// Sentry
  if(in_use)
  	return;
  in_use = true;

	/*****************************************************************
	**  read in CODE,NPTS,XMAX,XMIN,YMAX,YMIN
	**  Use low level _read for performance
	** If resolve == 1, resolution is seconds.  Else minutes.
	** When drawing a flat map on screen, we let Turbo do the clipping.
	** When drawing a map in perspective, we have to clip on the
	** rectangular box's boundaries
	*****************************************************************/
  // Following now done in map.cpp since doing it here we only capture the last file
  // drawn rather than all of them
	nbytes = 4;
  int poly_ct;
	size_idat = 6*sizeof(short);
	idat = (short *) malloc(size_idat); // Room for header
  Vector v;
	// fd = fileno(ifp);
	while(fread(idat,2,6,ifp))
	{
		icode=(short)(idat[0]/1000);
		npts=idat[1];
		//xmx=idat[2];
		//xmn=idat[3];
		//ymx=idat[4];
		//ymn=idat[5];
    color = icode % 8;

   	dc.SelectObject(TPen(map_colors[color],1));

		/**********************************************************
		** If not in plot regions, skip past data to next segment
		** If data is in plot region, go read the data and convert to floating point
    **********************************************************/

    n = npts*nbytes;
    // The x-y points give the corners of the "rectangle" for this segment.
    // See if this will be visible.  If not, skip ahead.
    #if 0
    int n_hidden = 0;
    for(i = 2;i<6; i += 2) {
   		x = idat[i]/60.0;
	 		c_lon = i_cos(x);
   		s_lon = i_sin(x);
   		x = idat[i+1]/60.0;
   		c_lat = i_cos(x);
   		s_lat = i_sin(x);
  		v = Vector(c_lon*c_lat,s_lon*c_lat,s_lat);
    	v = v*radius;
  		v = Xform(xfer,v);
    	if(v.v[0] >= 0)
      	break;
      n_hidden++;
    }
    if(n_hidden >= 2) {
      fseek(ifp,(long)n,SEEK_CUR);
      continue;
    }
    #endif
    // Keep adjusting size if this block is biggest
    if(size_idat < n) {
      idat = (short *) realloc(idat,n);
      size_idat = n;
    }
    long fileptr = ftell(ifp);
    iread = fread(idat,1,n,ifp);
    if(iread > 0)
    {
      tpoints = new TPoint[npts];
      if(fileptr < 1000L)
        fileptr++;							// What's this all about?
      poly_ct = 0;
      for(jj=0;jj<npts;jj++)
      {
        // Coordinates are stored as a 2-byte integer
        xs=idat[jj*2];
        ys=idat[jj*2+1];
        float x = xs/60.0;
				float c_lon = i_cos(x);
        float s_lon = i_sin(x);
        x = ys/60.0;
        float c_lat = i_cos(x);
        float s_lat = i_sin(x);
        v =  Vector(c_lon*c_lat,s_lon*c_lat,s_lat);
        v = v*radius;
  			v = Xform(xfer,v);
        if(v.v[0] < 0) {
        	if(poly_ct) {
          	dc.Polyline(tpoints,poly_ct);
            poly_ct = 0;
          }
        	continue;
        }
	      tpoints[poly_ct] = TPoint(v.v[1],-v.v[2]) + center;
        poly_ct++;
      }
    	if(poly_ct) {
      	if(color) {
          dc.Polyline(tpoints,poly_ct);
        }
      }
     	delete tpoints;
    }
  }
  free(idat);
  in_use = false;
}

float TStationsWindow::i_sin(float x) {
	int sign = 1;
	if(x < 0) {
  	sign = -1;
    x = -x;
  }
	int xi = x;		// Get integer part
  float y = sin_array[xi];
  y = y+(x-xi)*(sin_array[xi+1]-y);
  return y*sign;
}
// Interpolate for cosine
float TStationsWindow::i_cos(float x) {
	if(x < 0) {
    x = -x;
  }
	int xi = x;		// Get integer part
  float y = cos_array[xi];
  y = y+(x-xi)*(cos_array[xi+1]-y);
  return y;
}
// Convert a latitude and longitude to a point on the screen
TPoint TStationsWindow::LatLon2Point(float lat, float lon)
{
	float c_lon = i_cos(lon);
  float s_lon = i_sin(lon);
  float c_lat = i_cos(lat);
  float s_lat = i_sin(lat);
	Vector v = Vector(c_lon*c_lat,s_lon*c_lat,s_lat);
  v = v*radius;
 	v = Xform(xfer,v);
  return TPoint(v.v[1],-v.v[2]) + center;
}

// Draw a circle
#if 0
static void circle(TPoint p, int color, int r,TDC &dc)
{
  //dc.SelectObject(TPen(rms_colors[rms]));
	//dc.SelectObject(TBrush(rms_colors[rms]));
  dc.Ellipse(p.x-r,p.y-r,p.x+r,p.y+r);

  //rms = 12 - rms;
  //dc.SelectObject(TBrush(TColor::White,BS_NULL));
  //dc.SelectObject(TPen(colors[BLUE]));
	//dc.SelectObject(TBrush(colors[BLUE]));
  //dc.MoveTo(p);
  //dc.BeginPath();
  //dc.AngleArc(p,r,90,30*rms);
  //dc.LineTo(p);
  //dc.EndPath();
  //dc.FillPath();
}
#endif
// Draw a red triangle
static void triangle(TPoint p, TDC &dc)
{
	dc.SelectObject(TBrush(TColor(255,0,0)));
  dc.SelectObject(TPen(TColor(0,0,0)));
  static TPoint tri[3] = {TPoint(0,-8),TPoint(7,4),TPoint(-7,4)};
  TPoint pp[3];
  for(int i=0;i<3;i++) {
  	pp[i] = tri[i]+p;
  }
  dc.Polygon(pp,3);
}
// Draw a black triangle
#if 0
static void box(TPoint p, TDC &dc)
{
	dc.SelectObject(TBrush(TColor(0,0,0)));
  dc.SelectObject(TPen(TColor(0,0,0)));
  static TPoint bx[3] = {TPoint(0,-6),TPoint(5,3),TPoint(-5,3)};
  TPoint pp[3];
  for(int i=0;i<3;i++) {
  	pp[i] = bx[i]+p;
  }
  dc.Polygon(pp,4);
}
#endif
// Draw a BLUE star
#if 0
static void star(TPoint p, TDC &dc)
{
	dc.SelectObject(TBrush(TColor(0,0,255)));
  dc.SelectObject(TPen(TColor(0,0,0)));
  static TPoint tri[10] =
  	{TPoint(0,-6),TPoint(2,-2),TPoint(6,-2),
  	 TPoint(3,1), TPoint(4,6), TPoint(0,3),
     TPoint(-4,6),TPoint(-3,1),TPoint(-6,-2),
     TPoint(-2,-2)};
  TPoint pp[10];
  for(int i=0;i<10;i++) {
  	pp[i] = tri[i]+p;
  }
  dc.Polygon(pp,10);
}
#endif
// Transformation from earth coordinates to screen coordinates
Vector TStationsWindow::Xform(float *mat[3],Vector v1) {
	Vector res;
	for(int i=0;i<3;i++) {
		Vector v2(mat[i][0],mat[i][1],mat[i][2]);
    res.v[i] = v2 % v1;
  }
  return res;
}
// Inverse transformation.  Same matrix but rows and columns reversed
Vector TStationsWindow::IXform(float *mat[3],Vector v1) {
	Vector res;
	for(int i=0;i<3;i++) {
		Vector v2(mat[0][i],mat[1][i],mat[2][i]);
    res.v[i] = v2 % v1;
  }
  return res;
}
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



