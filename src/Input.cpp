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

//{{Input Implementation}}

DEFINE_RESPONSE_TABLE1(Input, TDialog)
	EV_COMMAND(IDOK,CmOk),
END_RESPONSE_TABLE;

Input::Input(TWindow* parent, char *caption, char *prompt_text, char *buf,
	int length, TResId resId, TModule* module)
:
    TDialog(parent, resId, module)
{
//{{InputXFER_USE}}
  input = new TEdit(this,IDC_INPUT,48);
  prompt = new TStatic(this,IDC_INPUT_PROMPT);
	this->prompt_text = prompt_text;
  this->caption = caption;
  this->buf = buf;
  this->length = length;
}
void Input::SetupWindow()
{
  TDialog::SetupWindow();
  //SetCaption(caption);
  //prompt->SetText(prompt_text);
}

void Input::CmOk()
{
  TDialog::CmOk();
  input->GetText(buf,length);
}


Input::~Input()
{
  Destroy(IDCANCEL);
  //delete input;
}


