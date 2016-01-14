//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         contdialog.h
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for ContDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(contdialog_h)              // Sentry, use file only if it's not already included.
#define contdialog_h

#include <owl/dialog.h>


#include <owl/commctrl.h>
#include "as1.rh"            // Definition of all resources.
#include <owl/button.h>
#include <owl/edit.h>


class ContDialog : public TDialog {
  public:
    ContDialog(TWindow* parent, TResId resId = IDD_CONTINUOUS, TModule* module = 0);
    virtual ~ContDialog();

//{{ContDialogXFER_DEF}}
  protected:
    TEdit* lines_per_hour;
    TEdit* DaysToRetainRecords;
    TEdit* decimate;
    TEdit* gain;
    TComboBox *horizontal_time_limit;
    TCheckBox *filter;
    TCheckBox *glitch_removal;
    void CmLowPassParamsClicked();
    void CmHighPassParamsClicked();


//{{ContDialogXFER_DEF_END}}

//{{ContDialogVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
//{{ContDialogVIRTUAL_END}}

//{{ContDialogRSP_TBL_BEGIN}}
  protected:
    void Handle_Ok();
    void Handle_Help();
//{{ContDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(ContDialog);
};    //{{ContDialog}}


#endif  // contdialog_h sentry.

