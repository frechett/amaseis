//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         comportdialog.cpp
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of ComPortDialog (TDialog).
//
//----------------------------------------------------------------------------
#include "as1.h"

//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(ComPortDialog, TDialog)
//{{ComPortDialogRSP_TBL_BEGIN}}
  EV_BN_CLICKED(IDCANCEL, Cancel),
  EV_BN_CLICKED(IDOK, Handle_Okay),
//{{ComPortDialogRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{ComPortDialog Implementation}}


static ComPortDialogXfer ComPortDialogData;

ComPortDialog::ComPortDialog(TWindow* parent, TResId resId, TModule* module)
:
    TDialog(parent, resId, module)
{
//{{ComPortDialogXFER_USE}}
  comport = new TEdit(this, IDC_COM_PORT, 8);

  SetTransferBuffer(&ComPortDialogData);
//{{ComPortDialogXFER_USE_END}}

  // INSERT>> Your constructor code here.

}


ComPortDialog::~ComPortDialog()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.

}


bool ComPortDialog::CanClose()
{
  bool result;

  result = TDialog::CanClose();

  // INSERT>> Your code here.

  return result;
}


void ComPortDialog::SetupWindow()
{
  TDialog::SetupWindow();

  // INSERT>> Your code here.
  char buff[8];
  sprintf(buff,"%u",profile->comport);
  comport->SetText(buff);
}


void ComPortDialog::Cancel()
{
  // INSERT>> Your code here.
  CloseWindow();
}


void ComPortDialog::Handle_Okay()
{
  // INSERT>> Your code here.
  // Retrieve the port number
  char buff[8];
  comport->GetText(buff,4);
  int c = atoi(buff);
  if(c > 0)
  profile->comport = c;
  CloseWindow();
}




