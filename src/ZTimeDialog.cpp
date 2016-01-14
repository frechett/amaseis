//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         ztimedialog.cpp
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of ZTimeDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "as1.h"

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(ZTimeDialog, TDialog)
//{{ZTimeDialogRSP_TBL_BEGIN}}
  //EV_BN_CLICKED(IDCANCEL, BNClicked),
  EV_BN_CLICKED(IDOK, OkClicked),
//{{ZTimeDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{ZTimeDialog Implementation}}


//static ZTimeDialogXfer ZTimeDialogData;

ZTimeDialog::ZTimeDialog(TWindow* parent, SYSTEMTIME &st, TResId resId, TModule* module)
:
    TDialog(parent, resId, module)
{
//{{ZTimeDialogXFER_USE}}
  day = new TEdit(this, IDC_Z_DAY, 3);
  hour = new TEdit(this, IDC_Z_HOUR, 3);
  month = new TEdit(this, IDC_Z_MONTH, 3);
  year = new TEdit(this, IDC_Z_YEAR, 5);
  seconds = new TEdit(this, IDC_Z_SECONDS, 7);
  minute = new TEdit(this, IDC_Z_MINUTE, 3);

  //SetTransferBuffer(&ZTimeDialogData);
//{{ZTimeDialogXFER_USE_END}}

  // INSERT>> Your constructor code here.
  pst = &st;
}


ZTimeDialog::~ZTimeDialog()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.

}


void ZTimeDialog::SetupWindow()
{
  TDialog::SetupWindow();

  // INSERT>> Your code here.
  char buff[16];
  sprintf(buff,"%d",pst->wYear);
  year->SetText(buff);
  sprintf(buff,"%d",pst->wMonth);
  month->SetText(buff);
  sprintf(buff,"%d",pst->wDay);
  day->SetText(buff);
  sprintf(buff,"%d",pst->wHour);
  hour->SetText(buff);
  sprintf(buff,"%d",pst->wMinute);
  minute->SetText(buff);
  float s = pst->wSecond + 0.001*pst->wMilliseconds;
  sprintf(buff,"%f",s);
  seconds->SetText(buff);
}


void ZTimeDialog::OkClicked()
{
  // INSERT>> Your code here.
	char buffer[16];
	year->GetText(buffer,7);
  pst->wYear = (WORD)atoi(buffer);

  month->GetText(buffer,7);
  pst->wMonth = (WORD)atoi(buffer);

	day->GetText(buffer,7);
  pst->wDay = (WORD)atoi(buffer);

	hour->GetText(buffer,7);
  pst->wHour = (WORD)atoi(buffer);

  minute->GetText(buffer,7);
  pst->wMinute = (WORD)atoi(buffer);

  seconds->GetText(buffer,7);
  float s = atof(buffer);
  pst->wSecond = (WORD)s;
  pst->wMilliseconds = 1000*(s - (int)s);

	CmOk();

}

