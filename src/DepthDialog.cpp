//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         depthdialog.cpp
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of DepthDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "as1.h"


//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(DepthDialog, TDialog)
//{{DepthDialogRSP_TBL_BEGIN}}
  EV_BN_CLICKED(IDOK, BNClicked),
//{{DepthDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{DepthDialog Implementation}}



DepthDialog::DepthDialog(TWindow* parent, float evdp, TResId resId, TModule* module)
:
    TDialog(parent, resId, module)
{
//{{DepthDialogXFER_USE}}
  depth = new TEdit(this, IDC_DEPTH, 255);
  d = evdp;
  if(d < 0)
  	d = 33;
//{{DepthDialogXFER_USE_END}}

  // INSERT>> Your constructor code here.
}


DepthDialog::~DepthDialog()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.
  delete depth;
}


void DepthDialog::BNClicked()
{
  // INSERT>> Your code here.
  char buffer[16];
  depth->GetText(buffer,15);
  d = atof(buffer);
  profile->write();
  CmOk();

}


void DepthDialog::SetupWindow()
{
  TDialog::SetupWindow();

  // INSERT>> Your code here.
  char buffer[16];
  sprintf(buffer,"%g",d);
  depth->SetText(buffer);

}

