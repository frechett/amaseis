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
DEFINE_RESPONSE_TABLE1(Station, TDialog)
//{{StationRSP_TBL_BEGIN}}
	EV_COMMAND(IDOK,Handle_Ok),
	EV_COMMAND(IDHELP,Handle_Help),
//{{StationRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{Station Implementation}}

Station::Station(TWindow* parent, TResId resId, TModule* module)
:
    TDialog(parent, resId, module)
{
//{{StationXFER_USE}}
  elevation = 		new TEdit(this, IDC_ELEVATION, 16);
  latitude = 			new TEdit(this, IDC_LATITUDE, 16);
  longitude = 		new TEdit(this, IDC_LONGITUDE, 16);
  station_code = 	new TEdit(this, IDC_STATION_CODE,8);
  station_name = 	new TEdit(this, IDC_STATION_NAME, 64);
  network_id = 		new TEdit(this, IDC_NETWORK_ID, 8);
  vertical = 			new TRadioButton(this, IDC_Z, 0);
  north = 				new TRadioButton(this, IDC_N, 0);
  east = 					new TRadioButton(this, IDC_E, 0);
  // INSERT>> Your constructor code here.
  SetCaption("This Station");
}

Station::~Station()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.
}

void Station::SetupWindow()
{
  TDialog::SetupWindow();

  // INSERT>> Your code here.
  char buffer[32];
  sprintf(buffer,"%g",profile->elevation);
  elevation->SetText(buffer);
  sprintf(buffer,"%lg",profile->latitude);
  latitude->SetText(buffer);
  sprintf(buffer,"%lg",profile->longitude);
  longitude->SetText(buffer);
  station_code->SetText(profile->station_code);
  station_name->SetText(profile->station_name);
  network_id->SetText(profile->network_id);
	char c = profile->component;
  c = (char)toupper(c);
  north->Uncheck();
  east->Uncheck();
  vertical->Uncheck();
  if(c =='Z')
  	vertical->Check();
  if(c =='N')
  	north->Check();
  if(c =='E')
  	east->Check();

}

void Station::Handle_Ok()
{
  char buffer[32];
  elevation->GetText(buffer,15);
  sscanf(buffer,"%g",&profile->elevation);
  latitude->GetText(buffer,15);
  sscanf(buffer,"%lg",&profile->latitude);
  longitude->GetText(buffer,15);
  sscanf(buffer,"%lg",&profile->longitude);
  station_code->GetText(profile->station_code,7);
  station_name->GetText(profile->station_name,63);
  network_id->GetText(profile->network_id,7);
  if(vertical->GetState())
  	profile->component = 'Z';
  if(north->GetState())
  	profile->component = 'N';
  if(east->GetState())
  	profile->component = 'E';

	CmOk();
}


void Station::Handle_Help()
{
  //WinHelp("AS1.HLP", HELP_CONTEXT, Station_Information);
  myHtmlHelp(Station_Information);
}

