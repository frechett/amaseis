//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         comportdialog.h
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for ComPortDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(comportdialog_h)              // Sentry, use file only if it's not already included.
#define comportdialog_h

#include <owl/dialog.h>


#include <owl/edit.h>
#include <owl/commctrl.h>
#include "as1.rh"            // Definition of all resources.
#include <owl/button.h>


//{{TDialog = ComPortDialog}}
struct ComPortDialogXfer {
//{{ComPortDialogXFER_DATA}}
    char    comport[ 255 ];
//{{ComPortDialogXFER_DATA_END}}
};

class ComPortDialog : public TDialog {
  public:
    ComPortDialog(TWindow* parent, TResId resId = IDD_COM_PORT, TModule* module = 0);
    virtual ~ComPortDialog();

//{{ComPortDialogVIRTUAL_BEGIN}}
  public:
    virtual bool CanClose();
    virtual void SetupWindow();
//{{ComPortDialogVIRTUAL_END}}

//{{ComPortDialogXFER_DEF}}
  protected:
    TEdit* comport;

//{{ComPortDialogXFER_DEF_END}}

//{{ComPortDialogRSP_TBL_BEGIN}}
  protected:
    void BNClicked();
    void Cancel();
    void Handle_Okay();
//{{ComPortDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(ComPortDialog);
};    //{{ComPortDialog}}


#endif  // comportdialog_h sentry.

