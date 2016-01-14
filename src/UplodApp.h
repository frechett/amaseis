//----------------------------------------------------------------------------
//  Project Upload
//  
//  Copyright © 1997. All Rights Reserved.
//
//  SUBSYSTEM:    Upload Application
//  FILE:         uplodapp.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TUploadApp (TApplication).
//
//----------------------------------------------------------------------------
#if !defined(uplodapp_h)              // Sentry, use file only if it's not already included.
#define uplodapp_h

#include <owl/opensave.h>


#include "uplodapp.rh"            // Definition of all resources.
extern void Debug(const char *format,...);


//
// FrameWindow must be derived to override Paint for Preview and Print.
//
//{{TDecoratedFrame = TSDIDecFrame}}
class TSDIDecFrame : public TDecoratedFrame {
  public:
    TSDIDecFrame(TWindow* parent, const char far* title, TWindow* clientWnd, bool trackMenuSelection = false, TModule* module = 0);
    ~TSDIDecFrame();

//{{TSDIDecFrameVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
//{{TSDIDecFrameVIRTUAL_END}}

//{{TSDIDecFrameRSP_TBL_BEGIN}}
  protected:
    void EvTimer(uint timerId);
//{{TSDIDecFrameRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TSDIDecFrame);
};    //{{TSDIDecFrame}}


//{{TApplication = TUploadApp}}
class TUploadApp : public TApplication {
  private:


  public:
    TUploadApp();
    virtual ~TUploadApp();

//{{TUploadAppVIRTUAL_BEGIN}}
  public:
    virtual void InitMainWindow();
//{{TUploadAppVIRTUAL_END}}

//{{TUploadAppRSP_TBL_BEGIN}}
  protected:
    void CmHelpAbout();
//{{TUploadAppRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TUploadApp);
};    //{{TUploadApp}}


#endif  // uplodapp_h sentry.
