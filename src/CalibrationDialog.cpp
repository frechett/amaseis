//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         calibrationdialog.cpp
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of CalibrationDialog (TDialog).
//
//----------------------------------------------------------------------------
//#include <owl/pch.h>
#include "as1.h"
#include "HelpManual\as1.h"

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(CalibrationDialog, TDialog)
//{{CalibrationDialogRSP_TBL_BEGIN}}
  EV_COMMAND(IDCANCEL, CancelClicked),
  EV_COMMAND(IDOK, OKClicked),
  EV_COMMAND(IDHELP, HelpClicked),
//{{CalibrationDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{CalibrationDialog Implementation}}

CalibrationDialog::CalibrationDialog(TWindow* parent, TResId resId, TModule* module)
:
    TDialog(parent, resId, module)
{
//{{CalibrationDialogXFER_USE}}
  CmCancel = new TButton(this, IDCANCEL);
  mass_of_arm = new TEdit(this, IDC_MOMENT_INERTIA, 16);
  cal_mass = new TEdit(this, IDC_CAL_MASS, 16);
  cal_distance = new TEdit(this, IDC_CAL_DISTANCE, 16);
  center_of_gravity = new TEdit(this, IDC_ARM_LENGTH, 16);

//{{CalibrationDialogXFER_USE_END}}

  // INSERT>> Your constructor code here.

}


CalibrationDialog::~CalibrationDialog()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.

}
void CalibrationDialog::SetupWindow()
{
  TDialog::SetupWindow();

  // INSERT>> Your code here.
  char buff[16];
  sprintf(buff,"%g",profile->cal_mass);
  cal_mass->SetText(buff);
	sprintf(buff,"%g",profile->cal_distance);
  cal_distance->SetText(buff);
  sprintf(buff,"%g",profile->mass_of_arm);
  mass_of_arm->SetText(buff);
  sprintf(buff,"%g",profile->center_of_gravity);
  center_of_gravity->SetText(buff);
}



bool CalibrationDialog::CanClose()
{
  bool result;

  result = TDialog::CanClose();

  // INSERT>> Your code here.

  return result;
}



void CalibrationDialog::HelpClicked()
{
  // INSERT>> Your code here.
    //WinHelp("AS1.HLP", HELP_CONTEXT, Calibration);
    myHtmlHelp(Calibration);
}


void CalibrationDialog::OKClicked()
{
  // INSERT>> Your code here.
  char buff[16];
  cal_mass->GetText(buff,15);
  profile->cal_mass = atof(buff);
  cal_distance->GetText(buff,15);
  profile->cal_distance = atof(buff);
  mass_of_arm->GetText(buff,15);
  profile->mass_of_arm = atof(buff);
  center_of_gravity->GetText(buff,15);
  profile->center_of_gravity = atof(buff);
  CloseWindow();
}


void CalibrationDialog::CancelClicked()
{
  // INSERT>> Your code here.
  CloseWindow();

}

