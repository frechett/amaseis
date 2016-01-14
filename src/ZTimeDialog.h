//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         ztimedialog.h
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for ZTimeDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(ztimedialog_h)              // Sentry, use file only if it's not already included.
#define ztimedialog_h

#include <owl/dialog.h>


#include <owl/edit.h>
#include <owl/commctrl.h>
#include "as1.rh"            // Definition of all resources.

#if 0
//{{TDialog = ZTimeDialog}}
struct ZTimeDialogXfer {
//{{ZTimeDialogXFER_DATA}}
    char    day[ 255 ];
    char    hour[ 255 ];
    char    month[ 255 ];
    char    year[ 255 ];
    char    seconds[ 255 ];
    char    minute[ 255 ];
//{{ZTimeDialogXFER_DATA_END}}
};
#endif

class ZTimeDialog : public TDialog {
  public:
    ZTimeDialog(TWindow* parent, SYSTEMTIME &st, TResId resId = IDD_ZTIME_DIALOG,
    	TModule* module = 0);
    virtual ~ZTimeDialog();

//{{ZTimeDialogVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
//{{ZTimeDialogVIRTUAL_END}}

//{{ZTimeDialogRSP_TBL_BEGIN}}
  protected:
    void OkClicked();
    SYSTEMTIME *pst;
//{{ZTimeDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(ZTimeDialog);

//{{ZTimeDialogXFER_DEF}}
  protected:
    TEdit* day;
    TEdit* hour;
    TEdit* month;
    TEdit* year;
    TEdit* seconds;
    TEdit* minute;

//{{ZTimeDialogXFER_DEF_END}}
};    //{{ZTimeDialog}}


#endif  // ztimedialog_h sentry.

