//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         devicedialog.h
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for DeviceDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(devicedialog_h)              // Sentry, use file only if it's not already included.
#define devicedialog_h

#include <owl/dialog.h>


#include <owl/combobox.h>
#include <owl/commctrl.h>
#include "as1.rh"            // Definition of all resources.


//{{TDialog = DeviceDialog}}
struct DeviceDialogXfer {
//{{DeviceDialogXFER_DATA}}
    TComboBoxData  combo;
//{{DeviceDialogXFER_DATA_END}}
};

class DeviceDialog : public TDialog {
  public:
    DeviceDialog(TWindow* parent, TResId resId = IDD_DEVICE, TModule* module = 0);
    virtual ~DeviceDialog();

//{{DeviceDialogVIRTUAL_BEGIN}}
  public:
    virtual bool CanClose();
    virtual void SetupWindow();
//{{DeviceDialogVIRTUAL_END}}

//{{DeviceDialogRSP_TBL_BEGIN}}
  protected:
    void BNClicked();
    void BNOkay();
    void HandleHelp();
//{{DeviceDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(DeviceDialog);

//{{DeviceDialogXFER_DEF}}
  protected:
    TComboBox* combo;

//{{DeviceDialogXFER_DEF_END}}
};    //{{DeviceDialog}}


#endif  // devicedialog_h sentry.

