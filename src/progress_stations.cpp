//----------------------------------------------------------------------------
//  Project rsw
//
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    rsw.apx Application
//  FILE:         progress.cpp
//  AUTHOR:
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of Progress (TDialog).
//
//----------------------------------------------------------------------------
#include <owl/pch.h>
#include <owl/static.h>
#include <stdio.h>
#include "stationsapp.rh"
#include "progress.h"
//{{Progress Implementation}}

Progress::Progress(TWindow* parent, TResId resId, TModule* module)
:
    TDialog(parent, resId, module)
{
//{{ProgressXFER_USE}}
  msg = new TStatic(this, IDC_PROGRESS_MSG, 64);
 	//pro->SetRange(0, 100);
  //pro->SetLed(4, 80);
  //pro->SetStep(8);
}


Progress::~Progress()
{
  Destroy(IDCANCEL);
  delete msg;

  // INSERT>> Your destructor code here.

}

void Progress::SetText(const char *format,...)
{
	va_list argptr;
	va_start(argptr,format);
	char buf[64];
	vsprintf(buf,format,argptr);
	va_end(argptr);
  msg->SetText(buf);
}

