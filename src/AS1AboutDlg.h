//----------------------------------------------------------------------------
//  Project AS1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    AS1 Application
//  FILE:         as1aboutdlg.h
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TAS1AboutDlg (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(as1aboutdlg_h)              // Sentry, use file only if it's not already included.
#define as1aboutdlg_h

#include <owl/static.h>

#include "as1.rh"                  // Definition of all resources.


//{{TDialog = TAS1AboutDlg}}
class TAS1AboutDlg : public TDialog {
  public:
    TAS1AboutDlg(TWindow* parent, TResId resId = IDD_ABOUT, TModule* module = 0);
    virtual ~TAS1AboutDlg();

//{{TAS1AboutDlgVIRTUAL_BEGIN}}
  public:
    void SetupWindow();
//{{TAS1AboutDlgVIRTUAL_END}}
};    //{{TAS1AboutDlg}}


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


#endif  // as1aboutdlg_h sentry.
