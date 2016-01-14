//----------------------------------------------------------------------------
//  Project AS1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    AS1 Application
//  FILE:         as1editfile.h
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TAS1EditFile (TEditFile).
//
//----------------------------------------------------------------------------
#if !defined(as1editfile_h)              // Sentry, use file only if it's not already included.
#define as1editfile_h

#include <owl/editfile.h>

#include "as1.rh"            // Definition of all resources.


//{{TEditFile = TAS1EditFile}}
class TAS1EditFile : public TEditFile {
  public:
    TAS1EditFile(TWindow* parent = 0, int id = 0, const char far* text = 0, int x = 0, int y = 0, int w = 0, int h = 0, const char far* fileName = 0, TModule* module = 0);
    virtual ~TAS1EditFile();

//{{TAS1EditFileVIRTUAL_BEGIN}}
  public:
    virtual void Paint(TDC& dc, bool erase, TRect& rect);
    virtual void SetupWindow();
    virtual bool SaveAs();
//{{TAS1EditFileVIRTUAL_END}}
//{{TAS1EditFileRSP_TBL_BEGIN}}
  protected:
    void EvGetMinMaxInfo(MINMAXINFO far& minmaxinfo);
//{{TAS1EditFileRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TAS1EditFile);
};    //{{TAS1EditFile}}


#endif  // as1editfile_h sentry.
