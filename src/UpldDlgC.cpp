//----------------------------------------------------------------------------
//  Project Upload
//
//  Copyright © 1997. All Rights Reserved.
//
//  SUBSYSTEM:    Upload Application
//  FILE:         uplddlgc.cpp
//  AUTHOR:
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TUploadDlgClient (TDialog).
//
//----------------------------------------------------------------------------
#include <owl/owlpch.h>
#include <owl/listbox.h>
#include <classlib/filename.h>
#include "ftp_u.h"
#include "profile.h"
#include "uplodapp.h"
#include "uplddlgc.h"
void Debug(const char *format,...)
{
	static char *log_file = "UploadDebug.txt";
	va_list argptr;
	va_start(argptr,format);
	char buf[512];
  SYSTEMTIME utc;
  GetSystemTime(&utc);
	sprintf(buf,"%02d:%02d:%02d.03d: ",utc.wHour,utc.wMinute,utc.wSecond,utc.wMilliseconds);
	//strcpy(buf,p+11);
	//strcat(buf,": ");
	vsprintf(buf+14,format,argptr);
  FILE *fp = fopen(log_file,"a+t");
  if(fp) {
  	fputs(buf,fp);
   	fputs("\n",fp);
   	fclose(fp);
  }
	va_end(argptr);
}


//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(TUploadDlgClient, TDialog)
//{{TUploadDlgClientRSP_TBL_BEGIN}}
  EV_WM_TIMER,
//{{TUploadDlgClientRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{TUploadDlgClient Implementation}}


//--------------------------------------------------------
// TUploadDlgClient
// ~~~~~~~~~~
// Construction/Destruction handling.
//
//static TUploadDlgClientXfer TUploadDlgClientData;
#define TIMER_ONE 1
#define TIMER_TWO 2
Profile *profile;
TUploadDlgClient::TUploadDlgClient(TWindow* parent, TResId resId, TModule* module)
:
  TDialog(parent, resId, module)
{
//{{TUploadDlgClientXFER_USE}}
  activity = new TListBox(this, IDC_ACTIVITY);

  //SetTransferBuffer(&TUploadDlgClientData);
//{{TUploadDlgClientXFER_USE_END}}

  // INSERT>> Your constructor code here.
  profile = new Profile();
  ftpupload = new FtpUpload(this);
}


TUploadDlgClient::~TUploadDlgClient()
{
  Destroy();
	delete profile;
  delete ftpupload;
}

static bool exists(char *fn)
{
	TFileName tfn = TFileName(fn);
  return tfn.Exists();
}
void TUploadDlgClient::SetupWindow()
{
  TDialog::SetupWindow();
  SetTimer(TIMER_ONE, 1000);	// Tick in one second to send up station info
}


void TUploadDlgClient::EvTimer(uint timerId)
{
  TDialog::EvTimer(timerId);
  static SYSTEMTIME utc, utc_old;
	static char *fn = "upload.txt";
  static int ctr;
  char buffer[100];
  char file[100];
  float samprate;
  if(timerId == TIMER_ONE) {
    KillTimer(TIMER_ONE);
    // Send up station information
  	SetText("Opening up FTP session to %s at %s",profile->ftp_addr,profile->ftp_user);
    // Note: login does a CD to the proper directory
  	if(ftpupload->Login()) {
  		SetText("Can't open FTP session");
    	return;
    }
    SetText("Upload station information");
    ftpupload->PutStationInfo();
    SetText("Close FTP session");
    ftpupload->DoClose();
    // Check for new file once a minute
    SetTimer(TIMER_TWO,6000);
    return;
  }
 	// Keep checking for the upload.txt file
  if(timerId != TIMER_TWO)
  	return;
  if(!exists(fn)) {
  	//if(0 == (ctr++%10)) {
  	//	SetText("%02d:%02d:%02d: No %s file",utc.wHour,utc.wMinute,utc.wSecond,fn);
    //}
    // Once an hour clear the list
    GetSystemTime(&utc);
    if(utc.wHour != utc_old.wHour)
    	activity->ClearList();
    utc_old = utc;
  	return;
  }
  // Open up FTP session
  SetText("Opening up FTP session to %s at %s",profile->ftp_addr,profile->ftp_user);
  if(ftpupload->Login()) {
  	SetText("Can't open FTP session");
    return;
  }
  FILE *fp = fopen(fn,"rt");
  if(fp) {
  	while(fgets(buffer,99,fp)) {
  		if(sscanf(buffer,"Upload:%s",file)) {
    		// Upload file
      	if(exists(file))
  				ftpupload->Upload(file);
    	} else
    	if(sscanf(buffer,"Samples/Second:%g",&samprate)) {
    		// Upload samples per second
      	ftpupload->PutSPS(samprate);
    	}
  	}
  	fclose(fp);
  }
  SetText("Close FTP session");
  ftpupload->DoClose();
  //SetText("FTP closed. Remove upload.txt");
  remove(fn);
  //SetText("upload.txt removed");
}

void TUploadDlgClient::SetText(const char *format,...)
{
	static int ctr;
	va_list argptr;
	va_start(argptr,format);
	char buf[512];
	vsprintf(buf,format,argptr);
	va_end(argptr);
  Debug(buf);
  activity->InsertString(buf,-1);
  activity->SetSelIndex(ctr++);
}

