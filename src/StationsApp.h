//----------------------------------------------------------------------------
//  Project Stations
//  RunTime Software
//  Copyright © 2007 All Rights Reserved.
//
//  SUBSYSTEM:    Stations Application
//  FILE:         stationsapp.h
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TStationsApp (TApplication).
//
//----------------------------------------------------------------------------
#if !defined(stationsapp_h)              // Sentry, use file only if it's not already included.
#define stationsapp_h

#include <owl/controlb.h>
#include <owl/docking.h>
#include <owl/opensave.h>
#include <owl/rcntfile.h>
#include <owl/static.h>
#include <classlib/arrays.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <math.h>
#include "stationsapp.rh"            // Definition of all resources.
extern void gcarc(
  float sla,    // Station latitude
  float slo,    // Station longitude                                                       float slo,    // Station
  float ela,    // Event latitude
  float elo,    // Event longitude
  float &delta, // Great Circle arc in degrees (returned)
  float &baz);   // Azimuth of event as seen from city (returned)

//
// FrameWindow must be derived to override Paint for Preview and Print.
//
//{{TDecoratedFrame = TSDIDecFrame}}
class TSDIDecFrame : public TDecoratedFrame {
  public:
    TSDIDecFrame(TWindow* parent, const char far* title, TWindow* clientWnd, bool trackMenuSelection = false, TModule* module = 0);
    ~TSDIDecFrame();
};    //{{TSDIDecFrame}}


//{{TApplication = TStationsApp}}
class TStationsApp : public TApplication, public TRecentFiles {
  private:

    void SetupSpeedBar(TDecoratedFrame* frame);
    void ProcessCmdLine(char * CmdLine);
    void RegisterInfo();
    void UnRegisterInfo();

  public:
    TStationsApp();
    virtual ~TStationsApp();

    TOpenSaveDialog::TData FileData;                    // Data to control open/saveas standard dialog.
    void OpenFile(const char* fileName = 0);

    THarbor*        ApxHarbor;

//{{TStationsAppVIRTUAL_BEGIN}}
  public:
    virtual void InitMainWindow();
    virtual void InitInstance();
//{{TStationsAppVIRTUAL_END}}

//{{TStationsAppRSP_TBL_BEGIN}}
  protected:
    void CmFileNew();
    void CmFileOpen();
    void CmHelpAbout();
    int32 CmFileSelected(uint wp, int32 lp);
//{{TStationsAppRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TStationsApp);
};    //{{TStationsApp}}

// Utility functions
bool OpenKey();
bool GetKeyInt(char *name,int &value);
bool SetKeyInt(char *name,CONST int value);
bool GetKeyBool(char *name,bool &value);
bool SetKeyBool(char *name,bool value);
bool CloseKey();


#endif  // stationsapp_h sentry.
