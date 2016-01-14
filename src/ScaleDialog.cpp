//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         scaledialog.cpp
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of ScaleDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "as1.h"
//#include <owl/pch.h>

//#include "scaledialog.h"


//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(ScaleDialog, TDialog)
//{{ScaleDialogRSP_TBL_BEGIN}}
  EV_COMMAND(IDOK, OKClicked),
//{{ScaleDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{ScaleDialog Implementation}}


static ScaleDialogXfer ScaleDialogData;

ScaleDialog::ScaleDialog(TWindow* parent, bool &auto_scale, float &max_amplitude, TResId resId, TModule* module)
:
    TDialog(parent, resId, module)
{
//{{ScaleDialogXFER_USE}}
  c_auto_scaling = new TCheckBox(this, IDC_AUTO_SCALING, 0);
  e_max_amplitude = new TEdit(this, IDC_MAX_AMPLITUDE, 16);

  SetTransferBuffer(&ScaleDialogData);
//{{ScaleDialogXFER_USE_END}}

  // INSERT>> Your constructor code here.
	this->auto_scale = &auto_scale;
  this->max_amplitude = &max_amplitude;
}


ScaleDialog::~ScaleDialog()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.

}


void ScaleDialog::OKClicked()
{
  // INSERT>> Your code here.
  uint c = c_auto_scaling->GetCheck();
  if(c == BF_CHECKED)
  	*auto_scale = true;
  else
  	*auto_scale = false;
  char buf[16];
  e_max_amplitude->GetText(buf,15);
  *max_amplitude = atof(buf);
  CmOk();

}


void ScaleDialog::SetupWindow()
{
  TDialog::SetupWindow();

  // INSERT>> Your code here.
	c_auto_scaling->SetCheck(*auto_scale? BF_CHECKED : BF_UNCHECKED);
  char buf[16];
  sprintf(buf,"%g",*max_amplitude);
  e_max_amplitude->SetText(buf);

}

