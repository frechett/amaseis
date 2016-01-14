//----------------------------------------------------------------------------
//  Project AS1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    AS1 Application
//  FILE:         as1.h
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TAS1 (TApplication).
//
//----------------------------------------------------------------------------
#if !defined(as1_h)              // Sentry, use file only if it's not already included.
#define as1_h

#include <owl/owlpch.h>
#include <owl/controlb.h>
#include <owl/docking.h>
#include <owl/opensave.h>
#include <owl/printer.h>
#include <owl/rcntfile.h>
#include <owl/button.h>
#include <owl/radiobut.h>
#include <owl/scrollba.h>
#include <owl/inputdia.h>
#include <owl/dialog.h>
#include <owl/edit.h>
#include <owl/updown.h>
#include <owl/point.h>
#include <owl/buttonga.h>
#include <owl/textgadg.h>
#include <owl/buttonga.h>
#include <owl/module.h>
#include <owl/checkbox.h>
#include <classlib/filename.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <math.h>
#include <iostream.h>
#include <iomanip.h>
#include <string.h>
#include <fstream.h>
#include <complex.h>
#include <classlib\arrays.h>
#include <classlib/cmdline.h>
#include <winsys/registry.h>
#pragma hdrstop
#include "png.h"
#include "writepng.h"
#include "as1.rh"            // Definition of all resources.
#include "progress.h"
#include "wcomm.h"		// For winsock classes
#include "windowprint.h"
#include "as1editfile.h"
#include "profile.h"
#include "child.h"
#include "kutil.h"
#include "ksac.h"
#include "xfm.h"
#include "HourRecord.h"
#include "tcp.h"
#include "ftp\ftp.h"
#include "serialio.h"
#include "plot.h"
#include "pepp.h"
#include "gain.h"
#include "pickdialog.h"
#include "devicedialog.h"
#include "JB.h"
#include "traveltime.h"
#include "Event.h"
#include "Client.h"
#include "input.h"
#include "filter.h"
#include "ztimedialog.h"
#include "scaledialog.h"
#include "apxprint.h"
#include "apxprev.h"
#include "as1editfile.h"
#include "as1aboutdlg.h"
#include "child.h"
#include "station.h"
#include "seisstation.h"
#include "contdialog.h"
#include "eventdialog.h"
#include "depthdialog.h"
#include "showdata.h"
#include "setzero.h"
#include "datedialog.h"
#include "comportdialog.h"
#include "calibrationdialog.h"
#include "xfmlog.h"
#include "magnitude.h"
#include "magnitude_ms.h"
#include "magnitude_mblg.h"
#include "PSN.h"
#include "forwardbackward.h"
#include "HelpManual\as1.h"


extern HINSTANCE appl_inst;
extern Client*  MdiClient;
extern int ver_res, hor_res;	// Vertical and horizontal resolution
extern char working_dir[];
extern char TCPHostAddress[];
extern int TCPHostPort;
extern char FTPAddr[];
extern char FTPUser[];
extern char FTPPW[];
extern char FTPPath[];
#define DEBUG_FILE "AmaSeisLog.txt"
#define PI 3.141592654
#define deg2rad 0.017453292
#define rad2deg 57.29577951
#define radius_earth 6371
// Got following from f:\bc5\include\win32\winuser.h
#define WM_MOUSEWHEEL                   0x020A
void Debug(const char *format,...);
void Debug2(const char *format,...);
//{{TApplication = TAS1}}
class TAS1 : public TApplication, public TRecentFiles {
  private:

    void SetupSpeedBar(TDecoratedMDIFrame* frame);
    void ProcessCmdLine(char * CmdLine);
    void RegisterInfo();
    void UnRegisterInfo();

  public:
    TAS1();
    virtual ~TAS1();

    TOpenSaveDialog::TData  FileData;                   // Data to control open/saveas standard dialog.

    //Client*  MdiClient;

    THarbor*        ApxHarbor;

    // Public data members used by the print menu commands and Paint routine in MDIChild.
    //
    TPrinter*       Printer;                            // Printer support.
    int             Printing;                           // Printing in progress.

//{{TAS1VIRTUAL_BEGIN}}
  public:
    virtual void InitMainWindow();
    virtual void InitInstance();
//{{TAS1VIRTUAL_END}}

//{{TAS1RSP_TBL_BEGIN}}
  protected:
    void CmHelpAbout();
    void EvWinIniChange(char far* section);
    int32 CmFileSelected(uint wp, int32 lp);
//{{TAS1RSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TAS1);
};    //{{TAS1}}

// Merge sort algorithm by Alan Jones
void msort(
	void *p,           /* Index to be sorted */
	int n,             /* Number of items */
	int elwidth,       /* Width of an element (char = 1, int = 2, etc. */
	int (*cmpf)(void *, void *));  /* key comparison function              */

void sortlongint(
	int *p,     /* array of integers to be re-arranged */
	int n,      /* number of items to sort         */
	int dir);

  void sortint(
	unsigned short *p,	/* array of integers to be re-arranged */
	unsigned n,      		/* number of items to sort         */
	int dir);


// select algorithm from Numerical Recipes in C
float select(int k, int n, float arr[]);

extern Profile *profile;
extern FtpThread *ftpthread;
#endif  // as1_h sentry.
