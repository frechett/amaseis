//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         calibrationdialog.h
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for CalibrationDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(calibrationdialog_h)              // Sentry, use file only if it's not already included.
#define calibrationdialog_h

//#include <owl/dialog.h>


//#include <owl/edit.h>
//#include <owl/commctrl.h>
//#include "as1.rh"            // Definition of all resources.
//#include <owl/button.h>


class CalibrationDialog : public TDialog {
  public:
    CalibrationDialog(TWindow* parent, TResId resId = IDD_CALIBRATION, TModule* module = 0);
    virtual ~CalibrationDialog();

//{{CalibrationDialogVIRTUAL_BEGIN}}
  public:
    virtual bool CanClose();
    virtual void SetupWindow();
//{{CalibrationDialogVIRTUAL_END}}

//{{CalibrationDialogXFER_DEF}}
  protected:
    TButton* CmCancel;
    TEdit* cal_mass;
    TEdit* mass_of_arm;
    TEdit* cal_distance;
    TEdit* center_of_gravity;

//{{CalibrationDialogXFER_DEF_END}}

//{{CalibrationDialogRSP_TBL_BEGIN}}
  protected:
    void HelpClicked();
    void OKClicked();
    void CancelClicked();
//{{CalibrationDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(CalibrationDialog);
};    //{{CalibrationDialog}}


#endif  // calibrationdialog_h sentry.

