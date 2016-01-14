//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         devicedialog.cpp
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of DeviceDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "as1.h"
#include "HelpManual\as1.h"

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(DeviceDialog, TDialog)
//{{DeviceDialogRSP_TBL_BEGIN}}
  EV_BN_CLICKED(IDCANCEL, BNClicked),
  EV_BN_CLICKED(IDOK, BNOkay),
  EV_BN_CLICKED(IDHELP, HandleHelp),
//{{DeviceDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{DeviceDialog Implementation}}


static DeviceDialogXfer DeviceDialogData;

DeviceDialog::DeviceDialog(TWindow* parent, TResId resId, TModule* module)
:
    TDialog(parent, resId, module)
{
//{{DeviceDialogXFER_USE}}
  combo = new TComboBox(this, IDC_COMBODEVICE, 1);
  DeviceDialogData.combo.Clear();
  DeviceDialogData.combo.AddString("AS-1");
  DeviceDialogData.combo.AddString("DATQ145");
  DeviceDialogData.combo.AddString("DATQ151");
  DeviceDialogData.combo.AddString("DATQ154");
  DeviceDialogData.combo.AddString("DATQ155");
  DeviceDialogData.combo.AddString("DATQ158");
  DeviceDialogData.combo.AddString("DATQ194");
  DeviceDialogData.combo.AddString("EQ1");
  DeviceDialogData.combo.AddString("INFILTECQM1");
  DeviceDialogData.combo.AddString("Rockwave HS-3");
  DeviceDialogData.combo.AddString("Rockwave VS1");
  DeviceDialogData.combo.AddString("SEPUK1");

  SetTransferBuffer(&DeviceDialogData);
//{{DeviceDialogXFER_USE_END}}

  // INSERT>> Your constructor code here.

}


DeviceDialog::~DeviceDialog()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.

}


void DeviceDialog::BNClicked()
{
  // INSERT>> Your code here.
  CloseWindow();

}


void DeviceDialog::BNOkay()
{
  // INSERT>> Your code here.
  profile->device = (Profile::Device) (combo->GetSelIndex()+1);
  profile->write();
  CloseWindow();
}


bool DeviceDialog::CanClose()
{
  bool result;

  result = TDialog::CanClose();

  // INSERT>> Your code here.

  return result;
}


void DeviceDialog::SetupWindow()
{
  TDialog::SetupWindow();

  // INSERT>> Your code here.
  // Set selected device
  combo->SetSelIndex(profile->device-1);

}


void DeviceDialog::HandleHelp()
{
  // INSERT>> Your code here.
  //WinHelp("AS1.HLP", HELP_CONTEXT, Devices);
  myHtmlHelp(Devices);
}

