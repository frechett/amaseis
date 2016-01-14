//----------------------------------------------------------------------------
//  Project Stations
//  RunTime Software
//  Copyright © 2007 All Rights Reserved.
//
//  SUBSYSTEM:    Stations Application
//  FILE:         stationswindow.h
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TStationsWindow (TWindow).
//
//----------------------------------------------------------------------------
#if !defined(stationswindow_h)              // Sentry, use file only if it's not already included.
#define stationswindow_h

#include "stationsapp.rh"            // Definition of all resources.
#define PI 3.141592654
#define deg2rad 0.017453292
#define rad2deg 57.29577951
#define radius_earth 6371

extern DWORD colors[];   // Array of RGB(r,g,b)
class Station {
	public:
  	Station();
    Station(FILE *fp);
    int operator == (const Station& s);
    	//{return (latitude == s.latitude && longitude == s.longitude); }
      //{ return s.rect && rect && s.rect->Touches(*rect);}

		float latitude, longitude, radius;
    TRect *rect;	// Rectangle into which the triangle is rendered
		char name[32];
		char code[8];
    char tcp_url[48];
    int tcp_port;
    char ftp_addr[48];
    char ftp_user[32];
    char ftp_pw[32];
    char ftp_path[32];
    TPoint point;
	//friend ostream& operator <<  ( ostream&, const Contained& );
};
typedef TIArrayAsVector<Station> StationArray;

class Vector {
	public:
		Vector(float x, float y, float z) {this->v[0] = x; this->v[1] = y; this->v[2] = z;}
    Vector() {v[0] = v[1] = v[2] = 0;}
    float v[3];
  // Inner product
  float operator % (Vector & arg) {
  	float result = v[0]*arg.v[0] + v[1]*arg.v[1] + v[2]*arg.v[2];
    return result;
  }
  Vector operator * (float r) {
  	return Vector(v[0]*r,v[1]*r,v[2]*r);
  }
};


//{{TWindow = TStationsWindow}}
class Progress;
class TStationsWindow : public TWindow {
  public:
    TStationsWindow(TWindow* parent, const char far* title = 0, TModule* module = 0);
    virtual ~TStationsWindow();
    void Render(float psi, float theta, float radius, TRect rect, TDC &dc);		// To draw earth
    static Vector Xform(float *mat[3],Vector v1);		// 3-D transformation
    static Vector IXform(float *mat[3],Vector v1);		// 3-D Inverse transformation
    float i_sin(float x);		// Interpolated sine
    float i_cos(float x);		// Interpolated cosine
    float *xfer[3];
  	float *sin_array;
    float *cos_array;
    float radius;
    TPoint center;
  private:
    void DrawBoundaries(FILE *fp, float radius, TDC &dc);
		void EvLButtonDown(uint modKeys, TPoint& point);
		void EvRButtonDown(uint modKeys, TPoint& point);
		void EvLButtonUp(uint modKeys, TPoint& point);
    void EvMouseMove(uint modKeys, TPoint& point);
    void CmZoomIn();
    void CmZoomOut();
    void getStations();
    TPoint LatLon2Point(float lat, float lon);
  	// Place to hold sines and cosines so we don't have to keep re-computing them
    bool events_drawn;			// Flag so we only draw events once
    TBitmap *bm;		// For saving image
    TBitmap *iris_logo;
    TMemoryDC *mdc;		// Memory device context
    float last_psi, last_theta, last_radius;	// Keep track if there have been changes
    float lat2, lon2;	// Coordinates of last mouse click
    float lat1, lon1;	// Coordinates at center of screen
    float lattemp, lontemp;
    bool mousedown;
    Progress *pro;
  public:
    virtual void SetupWindow();
    virtual void Paint(TDC& dc, bool erase, TRect& rect);
		void EvSize(uint sizeType, TSize& size);
    void EvTimer(uint timerId);
	DECLARE_RESPONSE_TABLE(TStationsWindow);

};    //{{TStationsWindow}}



#endif  // stationswindow_h sentry.
