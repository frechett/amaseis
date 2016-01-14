//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         datedialog.h
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for DateDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(datedialog_h)              // Sentry, use file only if it's not already included.
#define datedialog_h

#include <owl/dialog.h>


#include <owl/edit.h>
#include <owl/commctrl.h>
#include "as1.rh"            // Definition of all resources.


//{{TDialog = DateDialog}}
#if 0
struct DateDialogXfer {
//{{DateDialogXFER_DATA}}
    char    year[ 255 ];
    char    month[ 255 ];
    char    hour[ 255 ];
    char    day[ 255 ];
//{{DateDialogXFER_DATA_END}}
};
#endif

class DateDialog : public TDialog {
  public:
    DateDialog(TWindow* parent, SYSTEMTIME &st, TResId resId = IDD_DATEDIALOG, TModule* module = 0);
    virtual ~DateDialog();

//{{DateDialogVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
//{{DateDialogVIRTUAL_END}}

//{{DateDialogRSP_TBL_BEGIN}}
  protected:
    void HandleOK();
    SYSTEMTIME *pst;
//{{DateDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(DateDialog);

//{{DateDialogXFER_DEF}}
  protected:
    TEdit* year;
    TEdit* month;
    TEdit* hour;
    TEdit* day;

//{{DateDialogXFER_DEF_END}}
};    //{{DateDialog}}


#endif  // datedialog_h sentry.

