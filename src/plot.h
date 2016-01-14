#if !defined(plot_h)              // Sentry, use file only if it's not already included.
#define plot_h

// Following for writing text
#define LEFT 0
#define CENTER 1
#define RIGHT 2
#define BOTTOM 0
#define TOP 2
/**************************
** Prototypes for plot package
**************************/
enum COLORS {
  NONE,
  BLACK,
  BLUE,
  GREEN,
  CYAN,
  RED,
  MAGENTA,
  BROWN,
  LIGHTGRAY,
  DARKGRAY,
  LIGHTBLUE,
  LIGHTGREEN,
  LIGHTCYAN,
  LIGHTRED,
  LIGHTMAGENTA,
  YELLOW,
  WHITE
  };

extern DWORD colors[];   // Array of RGB(r,g,b)


void plot(TDC &dc,int x, int y, int color = 0, int width = 1, int style=PS_SOLID);
void plot(TDC &dc,TPoint p, int color = 0, int width = 1,int style=PS_SOLID);

void fill_(TDC &dc,int npts,TPoint *array,int color,int fill_color);
void polyline(TDC &dc,int npts,TPoint *array,int color,int thickness);
int flush(void);
void viewport(TDC &dc,int x0,int y0,int x1,int y1,int clip);
void viewport(TDC &dc,TRect r,int clip);
void setviewport(TDC &dc,TRect r,int clip);
void number(TDC &dc,int x,int y,TFont *font,double yss,int h,int v,int pen);
void number_ns(TDC &dc,int x,int y,TFont *font,double yss,int h,int v,int pen,bool ns);
void fat_circle(TDC &dc,int x,int y,int radius,int color);
void border(
	TDC &dc,      // Device context
  TRect &box,   // Rectangle to put border in
  TColor *bg,   // Color of face
  bool in,      // If true, button is pushed in so shadows reversed
	int w,        // Width of border
  int face = 0);     // de

TRect writexy(TDC &dc,int x,int y,TFont *font,char *text,int pen,int bg,
  int hor_j,int ver_j,int drop = 0,int drop_color = 0);
TRect writexy(TDC &dc,TPoint p,TFont *font,char *text,int pen,int bg,
  int hor_j,int ver_j,int drop = 0,int drop_color = 0);
// int textfontheight(char *text);
// int textfontwidth(char *text);
void rect(TDC &dc,int xmin,int ymin,int xmax,int ymax,int pen);
void rect(TDC &dc,TRect,int pen);

//void dot1(TDC &dc,int x,int y,int pen_res);

void pushvp(TDC &dc);
void popvp(TDC &dc);
#endif

