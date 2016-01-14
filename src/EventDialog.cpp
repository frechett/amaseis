//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         eventdialog.cpp
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of EventDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "as1.h"
#include "HelpManual\as1.h"

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(EventDialog, TDialog)
//{{EventDialogRSP_TBL_BEGIN}}
  EV_COMMAND(IDOK, Handle_Ok),
  EV_COMMAND(IDHELP, Handle_Help),
  EV_COMMAND(IDC_CREATE_EVT, Handle_Evt),
//{{EventDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{EventDialog Implementation}}
EventDialog::EventDialog(TWindow* parent, EVENT_DATA &ed,
	TResId resId, TModule* module)
:
    TDialog(parent, resId, module)
{
//{{EventDialogXFER_USE}}
	ped = &ed;
  event_name				= new TEdit(this, IDC_EVENT_NAME, 32);
  event_depth 			= new TEdit(this, IDC_EVENT_DEPTH, 16);
  event_latitude 		= new TEdit(this, IDC_EVENT_LATITUDE, 16);
  event_longitude 	= new TEdit(this, IDC_EVENT_LONGITUDE, 16);
  event_mb 					= new TEdit(this, IDC_EVENT_MB, 8);
  event_ms 					= new TEdit(this, IDC_EVENT_MS, 8);
  event_ml 					= new TEdit(this, IDC_EVENT_ML, 8);
  event_mw 					= new TEdit(this, IDC_EVENT_MW, 8);
  event_year 				= new TEdit(this, IDC_EVENT_YEAR, 8);
  event_month				= new TEdit(this, IDC_EVENT_MONTH, 8);
  event_day  				= new TEdit(this, IDC_EVENT_DAY, 8);
  event_hour 				= new TEdit(this, IDC_EVENT_HOUR, 8);
  event_minute			= new TEdit(this, IDC_EVENT_MINUTE, 8);
  event_second			= new TEdit(this, IDC_EVENT_SECOND, 8);
  event_origin			= new TStatic(this, IDC_ORIGIN, 24);
  origin_year				= new TEdit(this, IDC_ORIGIN_YEAR, 8);
  origin_month			= new TEdit(this, IDC_ORIGIN_MONTH, 8);
  origin_day 				= new TEdit(this, IDC_ORIGIN_DAY, 8);
  origin_hour				= new TEdit(this, IDC_ORIGIN_HOUR, 8);
  origin_minute			= new TEdit(this, IDC_ORIGIN_MINUTE, 8);
  origin_second			= new TEdit(this, IDC_ORIGIN_SECOND, 8);
  seismic_waves			= new TEdit(this, IDC_SEISMIC_WAVES, 48);
  // Clear out yyyymmddhh
  strcpy(ped->yyyymmddhh,"");
}


EventDialog::~EventDialog()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.

}
void EventDialog::SetupWindow()
{
  TDialog::SetupWindow();
  char buffer[32];
  if('-' == *ped->kevnm)
    strcpy(ped->kevnm,"Name");
	event_name->SetText(ped->kevnm);
  if(ped->evla < -90)
  	ped->evla = 0;
 	sprintf(buffer,"%g",ped->evla);
	event_latitude->SetText(buffer);
  if(ped->evlo < -180)
  	ped->evlo = 0;
 	sprintf(buffer,"%g",ped->evlo);
	event_longitude->SetText(buffer);
  if(ped->evdp < -1234)
  	ped->evdp = 0;
 	sprintf(buffer,"%g",ped->evdp/1000);
	event_depth->SetText(buffer);
  if(ped->ev_mb < 0) ped->ev_mb = 0;
  sprintf(buffer,"%g",ped->ev_mb);
  event_mb->SetText(buffer);
  if(ped->ev_ms < 0) ped->ev_ms = 0;
  sprintf(buffer,"%g",ped->ev_ms);
  event_ms->SetText(buffer);
  if(ped->ev_ml < 0) ped->ev_ml = 0;
  sprintf(buffer,"%g",ped->ev_ml);
  event_ml->SetText(buffer);
  if(ped->ev_mw < 0) ped->ev_mw = 0;
  sprintf(buffer,"%g",ped->ev_mw);
  event_mw->SetText(buffer);
  sprintf(buffer,"%d",ped->year);
  event_year->SetText(buffer);
  sprintf(buffer,"%d",ped->month);
  event_month->SetText(buffer);
  sprintf(buffer,"%d",ped->day);
  event_day->SetText(buffer);
  sprintf(buffer,"%d",ped->hour);
  event_hour->SetText(buffer);
  sprintf(buffer,"%d",ped->minute);
  event_minute->SetText(buffer);
	float sec = ped->second;
  sec += 0.001*ped->msecond;
  sprintf(buffer,"%.1f",sec);
  event_second->SetText(buffer);
  sprintf(buffer,"%.1f",ped->origin);
  event_origin->SetText(buffer);
  // Compute year, month, day, hour, minute, second
  // for beginning of seismogram
  SYSTEMTIME st;
  st.wYear = ped->year;
  st.wMonth = ped->month;
  st.wDay = ped->day;
  st.wHour = ped->hour;
  st.wMinute = ped->minute;
  st.wSecond = ped->second;
  st.wMilliseconds = ped->msecond;
 	FILETIME ft;
  LARGE_INTEGER li, li2;
  // Copy to a FILETIME structure
  SystemTimeToFileTime(&st,&ft);
  memcpy(&li,&ft,sizeof(LARGE_INTEGER));
  // Time in LARGE_INTEGER is stored in units of 100 nano-seconds
  // Subtract off origin time
  li2.QuadPart = 0.5 + 10*ped->origin; // Time is now in 0.1 seconds
  LONGLONG l2 = li2.QuadPart;
  l2 = l2*1000000L;
  // Subtract.  2005.01.30: Have to add since origin time is
  // adjustment from record time to event time.
  li.QuadPart = li.QuadPart+l2;
  memcpy(&ft,&li,sizeof(LARGE_INTEGER));
	FileTimeToSystemTime(&ft,&st);
  // Convert back to year, month, etc.
  sprintf(buffer,"%d",st.wYear);
  origin_year->SetText(buffer);
  sprintf(buffer,"%d",st.wMonth);
  origin_month->SetText(buffer);
  sprintf(buffer,"%d",st.wDay);
  origin_day->SetText(buffer);
  sprintf(buffer,"%d",st.wHour);
  origin_hour->SetText(buffer);
  sprintf(buffer,"%d",st.wMinute);
  origin_minute->SetText(buffer);
  sprintf(buffer,"%.1f",st.wSecond+0.001*st.wMilliseconds);
  origin_second->SetText(buffer);
  seismic_waves->SetText(profile->seismic_waves);
}

void EventDialog::Process()
{
	char buffer[64];
	event_name->GetText(ped->kevnm,31);
  event_latitude->GetText(buffer,8);
  ped->evla = atof(buffer);
  event_longitude->GetText(buffer,8);
  ped->evlo = atof(buffer);
  event_depth->GetText(buffer,8);
  ped->evdp = 1000*atof(buffer);
  event_mb->GetText(buffer,8);
  ped->ev_mb = atof(buffer);
  event_ms->GetText(buffer,8);
  ped->ev_ms = atof(buffer);
  event_ml->GetText(buffer,8);
  ped->ev_ml = atof(buffer);
  event_mw->GetText(buffer,8);
  ped->ev_mw = atof(buffer);
  event_year->GetText(buffer,8);
  ped->year = atoi(buffer);
  event_month->GetText(buffer,8);
  ped->month = atoi(buffer);
  event_day->GetText(buffer,8);
  ped->day = atoi(buffer);
  ped->jday = julian(ped->year,ped->month-1,ped->day);
  event_hour->GetText(buffer,8);
  ped->hour = atoi(buffer);
  event_minute->GetText(buffer,8);
  ped->minute = atoi(buffer);
  event_second->GetText(buffer,8);
  float s = atof(buffer);
  ped->second = (int) s;
  ped->msecond = ((int)(1000*s))%1000;
  event_origin->GetText(buffer,12);
  ped->origin = atof(buffer);

  origin_year->GetText(buffer,8);
  year = atoi(buffer);
  origin_month->GetText(buffer,8);
  month = atoi(buffer);
  origin_day->GetText(buffer,8);
  day = atoi(buffer);
  origin_hour->GetText(buffer,8);
  hour = atoi(buffer);
  origin_minute->GetText(buffer,8);
  minute = atoi(buffer);
  origin_second->GetText(buffer,8);
  second = atof(buffer);
  double record_time = sec_1901(ped->year,ped->month,ped->day,
  	ped->hour,ped->minute,ped->second,ped->msecond);
  double origin_time = sec_1901(year,month,day,
  	hour,minute,(int)second,(int)(1000*(second - (int)second)));
  // Subtract off number of seconds between origin and start of record
  ped->origin = origin_time - record_time;
  sprintf(buffer,"%g",ped->origin);
  event_origin->SetText(buffer);
  seismic_waves->GetText(profile->seismic_waves,47);
}
void EventDialog::Handle_Ok() {
	Process();
  CmOk();
}
extern deblank(char *string);
void EventDialog::Handle_Evt()
{
	Process();
  // Put *.evt file in Seismic Waves
  // Make directory
  // Put this SAC file in the directory
 	char fn[64];
 	char buff[64];
  if(strlen(profile->seismic_waves)) {
  	// Construct file name: YYYYMMDDHHMM.evt
    sprintf(ped->yyyymmddhh,"%04d%02d%02d%02d",year,month,day,hour,minute);
  	sprintf(fn,"%s\\%s.evt",profile->seismic_waves,ped->yyyymmddhh);
  	FILE *fp = fopen(fn,"wt");
  	if(fp) {
   		fprintf(fp,"EVENT NAME: %s\n",ped->kevnm);
    	fprintf(fp,"EVENT LATITUDE: %10.4f\n",ped->evla);
    	fprintf(fp,"EVENT LONGITUDE: %10.4f\n",ped->evlo);
    	fprintf(fp,"EVENT DEPTH: %10.1f\n",0.001*ped->evdp);
    	// Find maximum magnitude
    	float mag = 0;
    	float mag1 = ped->ev_ms;
    	mag = (mag1 > mag) ? mag1 : mag;
    	mag1 = ped->ev_mb;
    	mag = (mag1 > mag) ? mag1 : mag;
    	mag1 = ped->ev_mw;
    	mag = (mag1 > mag) ? mag1 : mag;
    	mag1 = ped->ev_ml;
    	mag = (mag1 > mag) ? mag1 : mag;
    	mag1 = ped->ev_ms;
    	mag = (mag1 > mag) ? mag1 : mag;
    	fprintf(fp,"EVENT MAGNITUDE: %6.1f\n",mag);
    	fprintf(fp,"EVENT DATE: %04d/%02d/%02d\n",year,month,day);
    	fprintf(fp,"ORIGIN TIME: %02d:%02d:%02.1f\n",
    		hour,minute,second);
    	fprintf(fp,"TIME ACCELERATION: %d\n",40);
    	fprintf(fp,"TIME WINDOW: %d\n",1800);
    	fprintf(fp,"TIME LIMIT: %d\n",3600);
    	fprintf(fp,"VIEW ANGLE: %d\n",-135);
      strcpy(buff,ped->kstnm);
      // In SAC, station name has blanks at end to fill out to 8 chars.
      deblank(buff);
      fprintf(fp,"STATION FILE: %s.sac\n",buff);
    	fclose(fp);
    	char dn[64];
    	strcpy(dn,fn);
    	// Pull out directory name
    	char *p = strchr(dn,'.');
    	if(p)
    		*p = 0;
      //sprintf(buf,"File %s created.",fn);
      //MessageBox(buf);
      // Now create the directory
      CreateDirectory(dn,0);
      //if(CreateDirectory(dn,0) == 0) {
			//	sprintf(buff,"Problem creating directory: %s",dn);
      //  MessageBox(buff);
	   	//}
    } else {
      sprintf(buff,"Problem creating file: %s",fn);
      MessageBox(buff);
    }
    CmOk();
  }
}

void EventDialog::Handle_Help()
{
  //WinHelp("AS1.HLP", HELP_CONTEXT, Event_Screen);
  myHtmlHelp(Event_Screen);
}


