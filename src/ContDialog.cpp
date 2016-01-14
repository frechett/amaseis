//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         contdialog.cpp
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of ContDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "as1.h"
#include "HelpManual\as1.h"

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(ContDialog, TDialog)
//{{ContDialogRSP_TBL_BEGIN}}
	EV_COMMAND(IDOK,Handle_Ok),
	EV_COMMAND(IDHELP,Handle_Help),
	EV_COMMAND(IDC_LOW_PASS_PARAMETERS,CmLowPassParamsClicked),
	EV_COMMAND(IDC_HIGH_PASS_PARAMETERS,CmHighPassParamsClicked),
//{{ContDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{ContDialog Implementation}}

ContDialog::ContDialog(TWindow* parent, TResId resId, TModule* module)
:
    TDialog(parent, resId, module)
{
  DaysToRetainRecords = new TEdit(this, IDC_DAYS_TO_RETAIN, 8);
  decimate = new TEdit(this, IDC_DECIMATE, 8);
  gain = new TEdit(this, IDC_GAIN, 8);
  //lines_per_hour = new TEdit(this, IDC_LINES_PER_HOUR, 8);
  filter = new TCheckBox(this,IDC_FILTER);
  glitch_removal = new TCheckBox(this,IDC_GLITCH_REMOVAL);
  horizontal_time_limit = new TComboBox(this,IDC_HORIZONTAL_TIME_LIMIT);

}

ContDialog::~ContDialog()
{
  Destroy(IDCANCEL);
}

// Values for combo box
static char *c_time_limits[] = {
	"One hour",
  "30 minutes",
  "15 minutes",
  "10 minutes",
  "5 minutes",
  "2 minutes",
  "1 minute"
};
static int c_lph[] = {1,2,4,6,12,30,60};
static int c_n_times = 7;

void ContDialog::SetupWindow()
{
  TDialog::SetupWindow();
  char buffer[8];
  //sprintf(buffer,"%d",profile->lines_per_hour);
	//lines_per_hour->SetText(buffer);

  sprintf(buffer,"%d",profile->DaysToRetainRecords);
	DaysToRetainRecords->SetText(buffer);

  sprintf(buffer,"%d",profile->decimate);
	decimate->SetText(buffer);

  sprintf(buffer,"%g",profile->gain);
	gain->SetText(buffer);

  glitch_removal->SetCheck(profile->glitch_removal ? BF_CHECKED : BF_UNCHECKED);

  filter->SetCheck(profile->helicorder_filter ? BF_CHECKED : BF_UNCHECKED);
  // Map lines per hour to the right string
  int i;
  int lph = profile->lines_per_hour;
  for(i=0;i<c_n_times;i++) {
  	if(c_lph[i] >= lph) break;
  }
  for(int j=0;j<c_n_times;j++) {
  	horizontal_time_limit->AddString(c_time_limits[j]);
  }
  horizontal_time_limit->SetText(c_time_limits[i]);
}

void ContDialog::Handle_Ok()
{
	char buffer[8];
  #if 0
	lines_per_hour->GetText(buffer,7);
  int lph = atoi(buffer);
  if(lph < 1)
  	lph = 1;
  // Allow lph >= 24
  if(lph <= 24) {
  	if((24%lph) != 0) {
  	MessageBox("Lines per hour must divide evenly into 24","AmaSeis",
    	MB_ICONEXCLAMATION);
		lines_per_hour->SetText("1");
    return;
  	}
	}
	profile->lines_per_hour = lph;
  #endif

  int i = horizontal_time_limit->GetSelIndex();
  if(i >= 0 && i <= c_n_times) {
  	profile->lines_per_hour = c_lph[i];
  }
	DaysToRetainRecords->GetText(buffer,7);
  profile->DaysToRetainRecords = atoi(buffer);

	decimate->GetText(buffer,7);
  profile->decimate = atoi(buffer);

	gain->GetText(buffer,7);
  profile->gain = atof(buffer);


  uint c = glitch_removal->GetCheck();
  if(c == BF_CHECKED)
  	profile->glitch_removal = true;
  else
  	profile->glitch_removal = false;

  profile->helicorder_filter = (filter->GetCheck() == BF_CHECKED);
  profile->write();
  CmOk();
}
void ContDialog::CmLowPassParamsClicked()
{
  	Filter(this,profile->LowPassPeriod,profile->EnableLowPass,"Low Pass Filter Dialog").Execute();
    if(profile->LowPassPeriod)
   		Xfm::HelicorderFilterLP = ButterworthLP(profile->samprate,1.0/profile->LowPassPeriod,2);
}
void ContDialog::CmHighPassParamsClicked()
{
  	Filter(this,profile->HighPassPeriod,profile->EnableHighPass,"High Pass Filter Dialog").Execute();
    if(profile->HighPassPeriod)
   		Xfm::HelicorderFilterHP = ButterworthHP(profile->samprate,1.0/profile->HighPassPeriod,2);
}
void ContDialog::Handle_Help()
{
  //WinHelp("AS1.HLP", HELP_CONTEXT, Helicorder_Screen);
  myHtmlHelp(Helicorder_Screen);
}

