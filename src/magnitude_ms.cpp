//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         magnitude.cpp
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of Ms Magnitude (TDialog).
//
//----------------------------------------------------------------------------
#include "as1.h"
#include "HelpManual\as1.h"


/***********************
// Routine to compute Ms
// MS = log10(A/T) + 1.66*log10(D) + 3.3   % 20 deg < D < 160 deg; 18 <= T <= 22 s
// Input:
// amp: amplitude in micrometers
// t: period in seconds
// delta: great-circle arc distance in degrees
// MS (returned)
***********************/
static float msmag(float amp, float t, float delta) throw (char *)
{
	if(t < 18 || t > 22) throw "Period must be between 18 and 22 seconds";
  if(amp == 0) return 0;
  if(delta < 20 || delta > 160) throw "Great circle arc distance must\n"
  									"be between 20 and 160 degrees.";

  return log10(amp/t) + 1.66*log10(delta) + 3.3;
}
//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(Magnitude_ms, TDialog)
//{{MagnitudeRSP_TBL_BEGIN}}
  EV_BN_CLICKED(IDHELP, HandleHelp),
  EV_BN_CLICKED(IDOK, HandleOK),
  EV_BN_CLICKED(IDC_COMPUTE, Process),
//{{MagnitudeRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{Magnitude_ms Implementation}}


Magnitude_ms::Magnitude_ms(TWindow* parent, TResId resId, TModule* module)
:
    TDialog(parent, resId, module)
{
//{{MagnitudeXFER_USE}}
  raw_amplitude = new TEdit(this, IDC_RAW_AMPLITUDE, 16);
  period = new TEdit(this, IDC_PERIOD, 16);
  inst_response = new TStatic(this, IDC_INST_RESPONSE, 16);
  calibration_file = new TEdit(this, IDC_CALIBRATION_FILE, 48);
  amplitude_microns = new TStatic(this, IDC_AMPLITUDE_MICRONS, 16);
  gca = new TEdit(this, IDC_GCARC, 16);
  magnitude = new TStatic(this, IDC_MB, 16);
  response_table = new TStatic(this,IDC_RESPONSE_TABLE,255);

  //SetTransferBuffer(&MagnitudeData);
//{{MagnitudeXFER_USE_END}}

}


Magnitude_ms::~Magnitude_ms()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.
  delete raw_amplitude;
  delete period;
  delete inst_response;
  delete calibration_file;
  delete amplitude_microns;
  delete gca;
  delete magnitude;
  delete response_table;
}


void Magnitude_ms::SetupWindow()
{
  TDialog::SetupWindow();

  // INSERT>> Your code here.
  //TransferData(tdSetData);
  char buf[48];
  sprintf(buf,"%g",profile->ms_amp);
  raw_amplitude->SetText(buf);
  sprintf(buf,"%g",profile->ms_per);
  period->SetText(buf);
  sprintf(buf,"%g",profile->inst_res);
  inst_response->SetText(buf);
  sprintf(buf,"%g",profile->gca);
  gca->SetText(buf);
  calibration_file->SetText(profile->calibration_file);
  profile->inst_res = 0;
  char *line = DisplayResponseTable(profile->calibration_file);
	if(line) {
    response_table->SetText(line);
  } else {
  	MessageBox("Can't open calibration file.");
  }

  Process();

}


void Magnitude_ms::HandleOK()
{
  // INSERT>> Your code here.
  Process();
  CloseWindow();

}


void Magnitude_ms::HandleHelp()
{
  // INSERT>> Your code here.
  //WinHelp("AS1.HLP", HELP_CONTEXT, Calibration);
  myHtmlHelp(Calibration);

}


void Magnitude_ms::HandleCancel()
{
  // INSERT>> Your code here.
	CloseWindow();
}

void Magnitude_ms::Process()
{
	// TransferData doesn't work.  That is, it doesn't hold the data in the
  // static structure.
  //TransferData(tdGetData);
  char buf[48];
  char cal_file[48];
  raw_amplitude->GetText(buf,15);
  profile->ms_amp = atof(buf);
  period->GetText(buf,15);
  profile->ms_per = atof(buf);
  // Read calibration file
  calibration_file->GetText(cal_file,47);
  profile->inst_res = InterpolateResponse(cal_file,profile->ms_per);
  if(profile->inst_res == 0) {
  	sprintf(buf,"Calibration file \"%s\" not found.",cal_file);
  	MessageBox(buf,"File not found");
    return;
  }
  sprintf(buf,"%g",profile->inst_res);
  inst_response->SetText(buf);

  gca->GetText(buf,15);
  profile->gca = atof(buf);
  if(profile->inst_res != 0) {
  	float am = profile->ms_amp/profile->inst_res;
    sprintf(buf,"%.3g",am);
    amplitude_microns->SetText(buf);
    float ms = 0;
    try {
    	ms = msmag(am,profile->ms_per,profile->gca);
    } catch (char *p) {
    	MessageBox(p);
    }
    sprintf(buf,"%.1f",ms);
    magnitude->SetText(buf);
  }
}


