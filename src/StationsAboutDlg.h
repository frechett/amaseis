//----------------------------------------------------------------------------
//  Project Stations
//  RunTime Software
//  Copyright © 2007 All Rights Reserved.
//
//  SUBSYSTEM:    Stations Application
//  FILE:         stationsaboutdlg.h
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TStationsAboutDlg (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(stationsaboutdlg_h)              // Sentry, use file only if it's not already included.
#define stationsaboutdlg_h

#include <owl/static.h>

#include "stationsapp.rh"                  // Definition of all resources.


//{{TDialog = TStationsAboutDlg}}
class TStationsAboutDlg : public TDialog {
  public:
    TStationsAboutDlg(TWindow* parent, TResId resId = IDD_ABOUT, TModule* module = 0);
    virtual ~TStationsAboutDlg();

//{{TStationsAboutDlgVIRTUAL_BEGIN}}
  public:
    void SetupWindow();
//{{TStationsAboutDlgVIRTUAL_END}}
};    //{{TStationsAboutDlg}}


// Reading the VERSIONINFO resource.
//
class TProjectRCVersion {
  public:
    TProjectRCVersion(TModule* module);
    virtual ~TProjectRCVersion();

    bool GetProductName(LPSTR& prodName);
    bool GetProductVersion(LPSTR& prodVersion);
    bool GetCopyright(LPSTR& copyright);
    bool GetDebug(LPSTR& debug);

  protected:
    uint8 far*  TransBlock;
    void far*   FVData;

  private:
    // Don't allow this object to be copied.
    //
    TProjectRCVersion(const TProjectRCVersion&);
    TProjectRCVersion& operator = (const TProjectRCVersion&);
};


#endif  // stationsaboutdlg_h sentry.
