//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         showdata.cpp
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of ShowData (TDialog).
//
//----------------------------------------------------------------------------
#include "as1.h"


//
// Build a response table for all messages/commands handled by the application.
//

//{{ShowData Implementation}}

DEFINE_RESPONSE_TABLE1(ShowData, TDialog)
	EV_COMMAND(IDOK,CmOk),
END_RESPONSE_TABLE;

ShowData::ShowData(TWindow* parent, TResId resId, TModule* module)
:
    TDialog(parent, resId, module)
{
//{{SetZeroXFER_USE}}
  //Ok = new TButton(this, IDOK);
  level = new TStatic(this,IDC_LEVEL);
  visible = false;

}

void ShowData::Show(int n)
{
	//if(visible) {
		char buffer[16];
  	sprintf(buffer,"%d",n);
  	level->SetText(buffer);
  //}
}
void ShowData::CmOk()
{
	ShowWindow(SW_HIDE);
}


ShowData::~ShowData()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.
  delete level;

}




bool ShowData::ShowWindow(int cmdShow)
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

