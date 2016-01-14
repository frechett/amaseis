//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         station.cpp
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of Station (TDialog).
//
//----------------------------------------------------------------------------
#include "as1.h"
#include "HelpManual\as1.h"


//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(SeisStation, TDialog)
	EV_COMMAND(IDOK,Handle_Ok),
	EV_COMMAND(IDHELP,Handle_Help),
END_RESPONSE_TABLE;


//{{Station Implementation}}

SeisStation::SeisStation(TWindow* parent, KSAC *sac, TResId resId, TModule* module)
:
    TDialog(parent, resId, module)
{
  elevation = new TEdit(this, IDC_ELEVATION, 16);
  latitude = new TEdit(this, IDC_LATITUDE, 16);
  longitude = new TEdit(this, IDC_LONGITUDE, 16);
  station_code = new TEdit(this, IDC_STATION_CODE,8);
  station_name = new TEdit(this, IDC_STATION_NAME, 64);
  vertical = new TRadioButton(this, IDC_Z, 0);
  north = new TRadioButton(this, IDC_N, 0);
  east = new TRadioButton(this, IDC_E, 0);
  this->sac = sac;
  SetCaption("Seismogram Station");

}

SeisStation::~SeisStation()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.
}

void SeisStation::SetupWindow()
{
  TDialog::SetupWindow();

  char buffer[32];
  sprintf(buffer,"%g",sac->GetSACHV(STEL));
  elevation->SetText(buffer);
  sprintf(buffer,"%lg",sac->GetSACHV(STLA));
  latitude->SetText(buffer);
  sprintf(buffer,"%lg",sac->GetSACHV(STLO));
  longitude->SetText(buffer);
  station_code->SetText(sac->GetSACHV(KSTNM));
  station_name->SetText("unknown");
  // Which component.  From Jeff Barker:
  //    The Sac header variables CMPAZ and CMPINC are component azimuth and
  // incidence angle, respectively.  So, Z, N and E would be:
  // (0 - actually undefined, and 0), (0, 90) and (90, 90).  CMPAZ is
  // sometimes some other angle if the components are not oriented N and E.
  // CMPINC can be -90 if the vertical component has reversed polarity.  By
  // the way, these are reals, not integers.
  float cmpaz = sac->GetSACHV(CMPAZ);
  float cmpinc = sac->GetSACHV(CMPINC);
  north->Uncheck();
  east->Uncheck();
  vertical->Uncheck();
  if(cmpinc == 0) {
  	vertical->Check();
  } else if (cmpinc == 90) {
  	if(cmpaz == 0)
    	north->Check();
    else
    	east->Check();
  }
}

void SeisStation::Handle_Ok()
{
  char buffer[32];
  elevation->GetText(buffer,15);
  sac->SetSACHV(STEL,atof(buffer));
  latitude->GetText(buffer,15);
  sac->SetSACHV(STLA,atof(buffer));
  longitude->GetText(buffer,15);
  sac->SetSACHV(STLO,atof(buffer));
  station_code->GetText(buffer,7);
  sac->SetSACHV(KSTNM,buffer);
  if(vertical->GetState()) {
  	sac->SetSACHV(CMPAZ,0);
    sac->SetSACHV(CMPINC,0);
  } else if(north->GetState()) {
  	sac->SetSACHV(CMPAZ,0);
    sac->SetSACHV(CMPINC,90);
  } else if(east->GetState()) {
  	sac->SetSACHV(CMPAZ,90);
    sac->SetSACHV(CMPINC,90);
  }
  profile->write();
	CmOk();
}

void SeisStation::Handle_Help()
{
  myHtmlHelp(Station_Information);
}

