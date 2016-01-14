//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         depthdialog.h
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for DepthDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(depthdialog_h)              // Sentry, use file only if it's not already included.
#define depthdialog_h

#include <owl/dialog.h>


#include <owl/commctrl.h>
#include "as1.rh"            // Definition of all resources.
#include <owl/edit.h>


//{{TDialog = DepthDialog}}
struct DepthDialogXfer {
//{{DepthDialogXFER_DATA}}
    char    depth[ 255 ];
//{{DepthDialogXFER_DATA_END}}
};

class DepthDialog : public TDialog {
  public:
    DepthDialog(TWindow* parent, float evdp, TResId resId = IDD_DEPTH, TModule* module = 0);
    virtual ~DepthDialog();

//{{DepthDialogXFER_DEF}}
  protected:
    TEdit* depth;
    float d;		// Depth

//{{DepthDialogXFER_DEF_END}}

//{{DepthDialogVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
    float GetDepth(){return d;}
//{{DepthDialogVIRTUAL_END}}

//{{DepthDialogRSP_TBL_BEGIN}}
  protected:
    void BNClicked();
//{{DepthDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(DepthDialog);
};    //{{DepthDialog}}


#endif  // depthdialog_h sentry.

