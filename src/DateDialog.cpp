//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         datedialog.cpp
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of DateDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "as1.h"

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(DateDialog, TDialog)
//{{DateDialogRSP_TBL_BEGIN}}
  EV_BN_CLICKED(IDOK, HandleOK),
//{{DateDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{DateDialog Implementation}}



DateDialog::DateDialog(TWindow* parent, SYSTEMTIME &st, TResId resId, TModule* module)
:
    TDialog(parent, resId, module)
{
//{{DateDialogXFER_USE}}
  year = new TEdit(this, IDC_D_YEAR, 16);
  month = new TEdit(this, IDC_D_MONTH, 16);
  hour = new TEdit(this, IDC_D_HOUR, 16);
  day = new TEdit(this, IDC_D_DAY, 16);

  // INSERT>> Your constructor code here.
	pst = &st;
}


DateDialog::~DateDialog()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.

}


void DateDialog::SetupWindow()
{
  TDialog::SetupWindow();

  // INSERT>> Your code here.
  char buff[16];
  int y = pst->wYear;
  int m = pst->wMonth;
  int d = pst->wDay;
  int h = pst->wHour;
  sprintf(buff,"%d",y);
  year->SetText(buff);
  sprintf(buff,"%d",m);
  month->SetText(buff);
  sprintf(buff,"%d",d);
  day->SetText(buff);
  sprintf(buff,"%d",h);
  hour->SetText(buff);
}


void DateDialog::HandleOK()
{
	char buffer[8];
	year->GetText(buffer,7);
  pst->wYear = atoi(buffer);

  month->GetText(buffer,7);
  pst->wMonth = atoi(buffer);

	day->GetText(buffer,7);
  pst->wDay = atoi(buffer);

	hour->GetText(buffer,7);
  pst->wHour = atof(buffer);

	CmOk();
}

