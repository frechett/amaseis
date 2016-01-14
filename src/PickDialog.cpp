//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         pickdialog.cpp
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of PickDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "as1.h"


//{{PickDialog Implementation}}

DEFINE_RESPONSE_TABLE1(PickDialog, TDialog)
//{{EventDialogRSP_TBL_BEGIN}}
  EV_COMMAND(IDC_DONE, CmDone),
//{{EventDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


PickDialog::PickDialog(TWindow* parent, TResId resId, TModule* module)
:
    TDialog(parent, resId, module)
{
  // INSERT>> Your constructor code here.
  arrival_number = new TStatic(this,IDC_ARRIVAL_NUMBER);
}


PickDialog::~PickDialog()
{
  Destroy(IDCANCEL);
  delete arrival_number;

  // INSERT>> Your destructor code here.

}

void PickDialog::CmDone()
{
	CmOk();
}


void PickDialog::SetupWindow()
{
  TDialog::SetupWindow();

  // INSERT>> Your code here.

}

