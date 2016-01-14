//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         arrivaldialog.cpp
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of ArrivalDialog (TDialog).
//
//----------------------------------------------------------------------------
#include <owl/pch.h>

#include "arrivaldialog.h"


//{{ArrivalDialog Implementation}}


ArrivalDialog::ArrivalDialog(TWindow* parent, TResId resId, TModule* module)
:
    TDialog(parent, resId, module)
{
  // INSERT>> Your constructor code here.

}


ArrivalDialog::~ArrivalDialog()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.

}

