//----------------------------------------------------------------------------
//  Project AS1
//  RunTime Software
//  Copyright � 2000. All Rights Reserved.
//
//  SUBSYSTEM:    AS1 Application
//  FILE:         apxprev.h
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TApxPreviewWin (Print Preview).
//
//----------------------------------------------------------------------------
#if !defined(apxprev_h)   // Sentry, use file only if it's not already included.
#define apxprev_h

#include <owl/controlb.h>
#include <owl/preview.h>

#include "apxprint.h"
#include "as1.rh"


//{{TDecoratedFrame = TApxPreviewWin}}
class TApxPreviewWin : public TDecoratedFrame {
  public:
    TApxPreviewWin(TWindow* parentWindow, TPrinter* printer, TWindow* currWindow, const char far* title, TLayoutWindow* client);
   ~TApxPreviewWin();

    int             PageNumber, FromPage, ToPage;

    TWindow*        CurrWindow;
    TControlBar*    PreviewSpeedBar;
    TPreviewPage*   Page1;
    TPreviewPage*   Page2;
    TPrinter*       Printer;

    TPrintDC*       PrnDC;
    TSize*          PrintExtent;
    TApxPrintout*   Printout;

  private:
    TLayoutWindow*  Client;

    void SpeedBarState();
    void PPR_PreviousEnable(TCommandEnabler& tce);
    void PPR_NextEnable(TCommandEnabler& tce);
    void PPR_Previous();
    void PPR_Next();
    void PPR_OneUp();
    void PPR_TwoUpEnable(TCommandEnabler& tce);
    void PPR_TwoUp();
    void PPR_Done();
    void CmPrintEnable(TCommandEnabler& tce);
    void CmPrint();

//{{TApxPreviewWinVIRTUAL_BEGIN}}
  protected:
    virtual void SetupWindow();
//{{TApxPreviewWinVIRTUAL_END}}

//{{TApxPreviewWinRSP_TBL_BEGIN}}
  protected:
    void EvClose();
//{{TApxPreviewWinRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TApxPreviewWin);
};    //{{TApxPreviewWin}}

#endif  // apxprev_h
