//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         setzero.cpp
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of SetZero (TDialog).
//
//----------------------------------------------------------------------------
#include "as1.h"


//
// Build a response table for all messages/commands handled by the application.
//

//{{SetZero Implementation}}

DEFINE_RESPONSE_TABLE1(SetZero, TDialog)
	EV_COMMAND(IDOK,CmOk),
END_RESPONSE_TABLE;

SetZero::SetZero(TWindow* parent, TResId resId, TModule* module)
:
    TDialog(parent, resId, module)
{
//{{SetZeroXFER_USE}}
  //Ok = new TButton(this, IDOK);
  zerolevel = new TEdit(this,IDC_LEVEL,12);
  visible = false;

}
void SetZero::SetupWindow()
{
  TDialog::SetupWindow();
  char buf[24];
  sprintf(buf,"%d",profile->zero_level);
  zerolevel->SetText(buf);
}

void SetZero::CmOk()
{
	char buf[24];
  zerolevel->GetText(buf,12);
	profile->zero_level = atoi(buf);
  profile->write();		// Save change
  TDialog::CmOk();
}


SetZero::~SetZero()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.
  delete zerolevel;

}




bool SetZero::ShowWindow(int cmdShow)
{
  bool result;

  result = TDialog::ShowWindow(cmdShow);

  // INSERT>> Your code here.
  if(result == true)
  	visible = true;
  else
  	visible = false;
  return result;
}

