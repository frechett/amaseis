//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         pickdialog.h
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for PickDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(pickdialog_h)              // Sentry, use file only if it's not already included.
#define pickdialog_h

//{{TDialog = PickDialog}}
class PickDialog : public TDialog {
  public:
    PickDialog(TWindow* parent, TResId resId = IDD_PICK, TModule* module = 0);
    virtual ~PickDialog();
    TStatic *arrival_number;

//{{PickDialogVIRTUAL_BEGIN}}
  private:
    virtual void SetupWindow();
    TButton *done;
    void CmDone();

	DECLARE_RESPONSE_TABLE(PickDialog);

//{{PickDialogVIRTUAL_END}}
};    //{{PickDialog}}


#endif  // pickdialog_h sentry.

