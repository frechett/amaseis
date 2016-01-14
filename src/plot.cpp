/*********************************************
** Plotting routines for Seismic
**
** Alan L. Jones
** Used in DOS Seismic: 6/25/90
** Converted to Windows Seismic: 7/6/95
**
*********************************************/
#include "as1.h"


/********************************************
** Global variables defined here and used in
** other modules
********************************************/
TEXTMETRIC my_tm;

DWORD colors[] = {
  RGB(0,0,0),				// Dummy black
  RGB(0,0,0),       // Black
  RGB(0,0,192),     // Blue
  RGB(0,192,0),     // Green
  RGB(0,128,128),   // Cyan
  RGB(128,0,0),     // Red
  RGB(128,0,128),   // Magenta
  RGB(128,128,0),   // Brown
  RGB(192,192,192), // Light gray
  RGB(128,128,128), // Dark Gray
  RGB(0,0,255),     // Light Blue
  RGB(0,255,0),     // Light Green
  RGB(0,255,255),   // Light Cyan
  RGB(255,0,0),     // Light Red
  RGB(255,0,255),   // Light Magenta
  RGB(255,255,0),   // Yellow
  RGB(255,255,255), // White
  RGB(0,0,0)        // Another black.  Need this when drawing
                    // map boundaries since 0 means no lines
};
// Following for saving and restoring viewport
static TPoint save_org;
void pushvp(TDC &dc)
{
  save_org = dc.GetViewportOrg();
}
void popvp(TDC &dc)
{
  dc.SetViewportOrg(save_org);
}

/**********************
** viewport()
**
** Set viewport and clipping in user coordinates
**********************/
void viewport(TDC &dc,int x0,int y0,int x1,int y1,int clip)
{
  // int vpheight = y1 - y0;
  // int vpwidth =  x1 - x0;
  if(clip) {
    dc.SetWindowOrg(TPoint(0,0));
    TRegion region(TRect(x0,y0,x1,y1));
    dc.SelectClipRgn(region);
  }
  	dc.SetWindowOrg(TPoint(x0,y0));
}
void viewport(TDC &dc,TRect r,int clip)
{
  // int vpheight = y1 - y0;
  // int vpwidth =  x1 - x0;
  if(clip) {
    dc.SetWindowOrg(TPoint(0,0));
    TRegion region(r);
    dc.SelectClipRgn(region);
  }
  TPoint p = r.TopLeft();
  dc.SetWindowOrg(-p);
}

void setviewport(TDC &dc,TRect r,int clip)
{
  // int vpheight = y1 - y0;
  // int vpwidth =  x1 - x0;
  if(clip) {
    dc.SetViewportOrg(TPoint(0,0));
    TRegion region(r);
    dc.SelectClipRgn(region);
  }
  TPoint p = r.TopLeft();
  dc.SetViewportOrg(p);
}


/**************************
** plot(x,y,pen)
**
** Draw a line to (x,y) with pen color set by pen
**************************/
void plot(
  TDC &dc,
  int x,
  int y,
  int color,
  int width,
  int style)
{
  if(color) {
    TPen pen(colors[color],width,style);
    dc.SelectObject(pen);
    dc.LineTo(x,y);
  }
  else dc.MoveTo(x,y);
}
void plot(
  TDC &dc,
  TPoint p,
  int color,
  int width,
  int style)
{
  if(color) {
    TPen pen(colors[color],width,style);
    dc.SelectObject(pen);
    dc.LineTo(p);
  }
  else dc.MoveTo(p);
}


/**************************
** fill_(TDC,npts,array,color,fill_color)
**
** Fill polygon.
**************************/
void fill_(
  TDC &dc,
  int npts,
  TPoint *array,
  int color,
  int fill_color)
{
  if(fill_color == WHITE)
    dc.SelectStockObject(WHITE_BRUSH);
  else
    dc.SelectObject(TBrush(colors[fill_color]));

  if(color == WHITE)
    dc.SelectStockObject(WHITE_PEN);
  else
    dc.SelectObject(TPen(colors[color]));
  dc.Polygon(array,npts);
}

// Draw polyline
void polyline(
  TDC &dc,
  int npts,
  TPoint *array,
  int color,
  int thickness)
{
  dc.SelectObject(TPen(colors[color],thickness));
  dc.Polyline(array,npts);
}

/****************************
** number(dc,x,y,size,yss,p1,p2)
**
** Put the number 'yss' at x,y
****************************/
void number(
  TDC &dc,
  int x,
  int y,
  TFont *font,
  double yss,    /* Number to output */
  int h,        /* Left-right justification */
                /* 0: left, 1: center, 2: right */
  int v,        /* Top-bottom justification */
                /* 0: bottom, 2: top */
  int color)
{
  char string[10];
  sprintf(string,"%g",yss);
  writexy(dc,x,y,font,string,color,-1,h,v);
}

/****************************
** writexy(x,y,size,text,pen,bg,hor_justify,ver_justify)
**
** Returns rectangle which contains text
** if color == -1, do not do actual writing but return rect
** 1997/07/15: Allow for multiple lines of output.  NL character
** 		specified by "\n"
****************************/
TRect writexy(
	TDC &dc,
	int x,
	int y,
  TFont *font,
	char *ptext,
	int color,
	int bg,       // Background color.  If bg is negative, then don't write
								// background
	int hor_j,    // horizontal justification -- left=0, center=1, right=2
	int ver_j,    // Vertical justification-- bottom = 0, center=1, top= 2
	int drop,     // Size of drop shadow
	int drop_color) // color of crop shadow
{
	TEXTMETRIC plot_tm;
	TColor tc = dc.GetTextColor();
	TColor bc = dc.GetBkColor();
  TSize vExt = dc.GetViewportExt();
  TSize wExt = dc.GetWindowExt();
  drop *= wExt.cx/vExt.cx;

	// Make a copy of text so we don't destroy the original when we
	// wipe out new lines when handling muli-line text
	char *text = strdup(ptext);
	if(font)
		dc.SelectObject(*font);
	else
		dc.SelectStockObject(SYSTEM_FONT);
	dc.GetTextMetrics(plot_tm);
	int space = plot_tm.tmAveCharWidth/2;
	int h = plot_tm.tmAscent;
	if(color >= 0) {
		if(bg>=0) {
			dc.SetBkMode(OPAQUE);
			dc.SetBkColor(colors[bg]);
		} else {
			dc.SetBkMode(TRANSPARENT);
		}
	}
  UINT align = 0;
	switch(hor_j) {
	case 0: align |= TA_LEFT; x += space; break;
  case 1: align |= TA_CENTER; break;
	case 2: align |= TA_RIGHT; x -= space; break;
  }
  switch(ver_j) {
	case 2:  y += h; break;
  case 1:
    // Windows does not have concept of centered vertically
    // Do computations ourself based on TextMetrics
    y = y + h/3;
    break;
	case 0: y -= plot_tm.tmDescent; 
		break;
  }
	align |= TA_BASELINE;
	dc.SetTextAlign(align);
	// If text is white draw a drop shadow

	// rect1 is rectangle enclosing all text
	TRect r = TRect(0,0,0,0);
	char *p, *pt;
	pt = text;
	int y1 = y;
	while(pt) {
		// Check for new line "\n"
		if(NULL != (p = strstr(pt,"\\n"))) {
			*p = 0;
		}
		if(color >=0)
		{
			if(drop) {
				dc.SetTextColor(colors[drop_color]);
				dc.TextOut(x+drop,y1+drop,pt);
			}
			dc.SetTextColor(colors[color]);
			dc.TextOut(x,y1,pt);
		}
		// Return text rectangle
		TSize size =
			dc.GetTextExtent(pt,strlen(pt))+TSize(drop,drop);
		// Adjust for alignment
		int x1 = x;
		switch(hor_j) {
		case 1: x1 -= size.cx/2; break;
		case 2: x1 -= size.cx; break;
		}
		TRect r1 = TRect(TPoint(x1,y1-h),size);
		r1 = r1.InflatedBy(1,1);
		// Get union of rectangles
		if(r.Width() != 0) {
			r |= r1;
		} else
			r = r1;

		if(p) {
			p += 2;
			pt = p;
			y1 += plot_tm.tmHeight+plot_tm.tmExternalLeading;
		}
		else
			pt = 0;
	}
	#if 0
	// Return text rectangle
	TSize size =
		dc.GetTextExtent(text,strlen(text))+TSize(drop,drop);
	// Adjust for alignment
	switch(hor_j) {
	case 1: x -= size.cx/2; break;
	case 2: x -= size.cx; break;
	}
	TRect r = TRect(TPoint(x,y-h),size);
	r = r.InflatedBy(1,1);
	#endif
	if(font)
		dc.RestoreFont();
	dc.SetTextColor(tc);
	dc.SetBkColor(bc);
	free(text);
	return r;
}
TRect writexy(
	TDC &dc,
	TPoint p,
  TFont *font,
	char *ptext,
	int color,
	int bg,       // Background color.  If bg is negative, then don't write
								// background
	int hor_j,    // horizontal justification -- left=0, center=1, right=2
	int ver_j,    // Vertical justification-- bottom = 0, center=1, top= 2
	int drop,     // Size of drop shadow
	int drop_color) // color of crop shadow
{
	int x = p.X();
  int y = p.Y();
  return writexy(dc,x,y,font,ptext,color,bg,hor_j,ver_j,drop,drop_color);
}

/*******************
** rect(dc,xmin,ymin,xmax,ymax,pen)
**
** Draw rectangle on screen with solid color
*******************/
void rect(TDC &dc,int xmin, int ymin, int xmax, int ymax,int color)
{
  TBrush brush(colors[color]);
  TPen pen(colors[color]);
  dc.SelectObject(pen);
  dc.SelectObject(brush);
  dc.Rectangle(xmin,ymin,xmax,ymax);
}
void rect(TDC &dc,TRect r,int color)
{
  TBrush brush(colors[color]);
  TPen pen(colors[color]);
  dc.SelectObject(pen);
  dc.SelectObject(brush);
  dc.Rectangle(r);
}

// Draw border of an exdented or indented button
void border(
	TDC &dc,      // Device context
  TRect &box,   // Rectangle to put border in
  TColor *bg,   // Color of face
  bool in,      // If true, button is pushed in so shadows reversed
	int w,        // Width of border
  int face)     // default is face = 0 in which case nothing happens
                // if face != 0, draw a face this wide using color bg
{
  // Fill in box
	if(bg != 0 && face == 0)
		dc.TextRect(box,*bg);

	if(w==0)
		return;

	TRect bx1 = box.InflatedBy(-w,-w);

	TPoint points[7];
	dc.SelectStockObject(NULL_PEN);
	TColor highlight(colors[LIGHTGRAY]);
	TColor shadow(colors[BLACK]);
	points[0] = box.TopLeft();
	points[1] = box.BottomLeft();
	points[2] = bx1.BottomLeft();
	points[3] = bx1.TopLeft();
	points[4] = bx1.TopRight();
	points[5] = box.TopRight();
	points[6] = box.TopLeft();
	if(in) {
		dc.SelectObject(TBrush(shadow));
	}
	else {
		dc.SelectObject(TBrush(highlight));
	}
	dc.Polygon(points,7);
	dc.RestoreBrush();

	points[0] = box.BottomRight();
	points[1] = box.TopRight();
	points[2] = bx1.TopRight();
	points[3] = bx1.BottomRight();
	points[4] = bx1.BottomLeft();
	points[5] = box.BottomLeft();
	points[6] = box.BottomRight();
	if(!in) {
		dc.SelectObject(TBrush(shadow));
	}
	else {
		dc.SelectObject(TBrush(highlight));
	}
	dc.Polygon(points,7);
	dc.RestoreBrush();

	if(face) {
		dc.SelectObject(TBrush(*bg));
		dc.PatBlt(bx1.left,bx1.top,bx1.Width(),face);
		dc.PatBlt(bx1.left,bx1.top,face,bx1.Height());
		dc.PatBlt(bx1.left,bx1.bottom,bx1.Width(),-face);
		dc.PatBlt(bx1.right,bx1.top,-face,bx1.Height());
		dc.RestoreBrush();
	}
	dc.RestorePen();
}

