//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         gain.cpp
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of Gain (TDialog).
//
//----------------------------------------------------------------------------
#include "as1.h"

/***********************************
** Edit Strings Dialog for editing list of file names
***********************************/
DEFINE_RESPONSE_TABLE2(Gain, TDialog,TWindow)
	EV_COMMAND(IDOK,Handle_Ok),
	//EV_COMMAND(IDHELP,Handle_Help),
  EV_UDN_DELTAPOS(IDC_UD_GAIN, SpinGain),
	EV_UDN_DELTAPOS(IDC_UD_X, SpinX),
END_RESPONSE_TABLE;

//{{Gain Implementation}}
Gain::Gain(TWindow* parent, TResId resId, TModule* module)
:
    TDialog(parent, resId, module)
{
  // INSERT>> Your constructor code here.
  gain = new TEdit(this,IDC_GAIN);
  time_expansion = new TEdit(this,IDC_TIME_MAGNIFY);
  spin_gain = new TUpDown(this,IDC_UD_GAIN);
  spin_x = new TUpDown(this,IDC_UD_X);
  g = 1;
  this->parent = parent;
}

void Gain::SetupWindow()
{
	TWindow::SetupWindow();
  spin_gain->SetRange(1,30);
  spin_x->SetRange(1,20);
}
void Gain::SetGain(float g)
{
	char buffer[24];
	sprintf(buffer,"%g",g);
	gain->SetText(buffer);
  int x = (log10(g) + 1)/log10(1.2);
  spin_gain->SetPos(x);
}
void Gain::SetX(float x)
{
	char buffer[24];
	sprintf(buffer,"%g",x);
	time_expansion->SetText(buffer);
	spin_x->SetPos(x);
}
float Gain::GetGain()
{
	return g;
}
float Gain::GetExpansion()
{
	return (float) spin_x->GetPos();
}
bool Gain::SpinGain(TNmUpDown& not)
{
	char buffer[16];
  int x = not.iPos + not.iDelta;
  if(x < 1) x = 1;
  spin_gain->SetPos((int)x);
  g = 0.1*pow(1.2,x);
	sprintf(buffer,"%.2g",g);
	gain->SetText(buffer);
  parent->Invalidate();
  return true;
}
bool Gain::SpinX(TNmUpDown& not)
{
  int x = not.iPos + not.iDelta;
  if(x < 1) x = 1;
  SetX(x);
  parent->Invalidate();
  return true;
}


// Comes here when user presses OK
void Gain::Handle_Ok()
{
	// CmOk();
}



Gain::~Gain()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.

  delete gain;
  delete time_expansion;
  delete spin_gain;
  delete spin_x;
}

