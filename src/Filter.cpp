//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         filter.cpp
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of Filter (TDialog).
//
//----------------------------------------------------------------------------
#include "as1.h"
#include "HelpManual\as1.h"

//
// Low Pass Filter
//
DEFINE_RESPONSE_TABLE2(Filter, TDialog,TWindow)
	EV_COMMAND(IDOK,Handle_Ok),
	EV_COMMAND(IDHELP,Handle_Help),
	EV_COMMAND(IDC_LOW_PASS_S,CmSec),
	EV_COMMAND(IDC_LOW_PASS_HZ,CmHz),
	EV_COMMAND(IDC_LOW_PASS_UPDATE,CmUpdate),
	//EV_COMMAND(IDHELP,Handle_Help),
END_RESPONSE_TABLE;


Filter::Filter(TWindow* parent, float &period, bool &enable, const char *caption,
	TResId resId, TModule* module)
:
    TDialog(parent, resId, module)
{
  // INSERT>> Your constructor code here.
  Edit_Period = new TEdit(this,IDC_PERIOD);
  Edit_Frequency = new TEdit(this,IDC_FREQ);
  sec = new TRadioButton(this,IDC_SEC);
  hz = new TRadioButton(this,IDC_HZ);
	enable_cb = new TCheckBox(this,IDC_ENABLE);
  this->period = period;
  this->pperiod = &period;
  this->enable = enable;
  this->penable = &enable;
  this->caption = caption;
}


Filter::~Filter()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.
  delete Edit_Period ;
  delete Edit_Frequency;
  delete sec;
  delete hz;
  delete enable_cb;
}

void Filter::SetupWindow()
{
	TWindow::SetupWindow();
  SetCaption(caption);
  if(strncmp(caption,"Low",3))
  	enable_cb->SetCaption("Filter out low frequencies");
  else
  	enable_cb->SetCaption("Filter out high frequencies");

  char buffer[24];
	sprintf(buffer,"%.3g",period);
	Edit_Period->SetText(buffer);
  if(period < 0.001)
  	if(profile->samprate)
    	period = 1/profile->samprate;
	sprintf(buffer,"%.3g",1/period);

  Edit_Frequency->SetText(buffer);
  if(enable)
  	enable_cb->SetCheck(BF_CHECKED);
  Process();
}

void Filter::PostProcess()
{
	int len = 15;
	char p[16];
  if(sec->GetCheck() == BF_CHECKED) {
		Edit_Period->GetText(p,len);
		period = atof(p);
    if(period < 0.001)
    	period = profile->samprate;
		sprintf(p,"%.3g",1/period);
  	Edit_Frequency->SetText(p);
  } else {
		Edit_Frequency->GetText(p,len);
    period = atof(p);
    if(period != 0)
    	period = 1/period;
    else
    	period = 0;
		sprintf(p,"%.3g",period);
		Edit_Period->SetText(p);
  }
}

void Filter::CmUpdate()
{
	PostProcess();
}
void Filter::Process()
{
  if(profile->LowPassSec) {
  	Edit_Frequency->SetReadOnly(true);
  	Edit_Period->SetReadOnly(false);
  	sec->SetCheck(BF_CHECKED);
  } else {
  	Edit_Period->SetReadOnly(true);
  	Edit_Frequency->SetReadOnly(false);
  	hz->SetCheck(BF_CHECKED);
  }
}

// Comes here when user presses OK
void Filter::Handle_Ok()
{
	PostProcess();
	*pperiod = period;
  if(enable_cb->GetCheck() == BF_CHECKED)
  	enable = true;
  else
  	enable = false;
  *penable = enable;
	CmOk();
}

void Filter::CmSec()
{
	profile->LowPassSec = true;
  Process();
  Invalidate();
}

void Filter::CmHz()
{
	profile->LowPassSec = false;
  Process();
  Invalidate();
}

void Filter::Handle_Help()
{
  //WinHelp("AS1.HLP", HELP_CONTEXT, Filter_Help);
  myHtmlHelp(Filter_Help);
}

#if 0

//{{Filter Implementation}}
DEFINE_RESPONSE_TABLE2(Filter, TDialog,TWindow)
	EV_COMMAND(IDOK,Handle_Ok),
	EV_COMMAND(IDHELP,Handle_Help),
	//EV_COMMAND(IDHELP,Handle_Help),
END_RESPONSE_TABLE;


Filter::Filter(TWindow* parent, float fcutoff_1, float fcutoff_2, TResId resId, TModule* module)
:
    TDialog(parent, resId, module)
{
  // INSERT>> Your constructor code here.
  cutoff_1 = new TEdit(this,IDC_CUTOFF_1);
  cutoff_2 = new TEdit(this,IDC_CUTOFF_2);
  this->fcutoff_1 = fcutoff_1;
  this->fcutoff_2 = fcutoff_2;
}


Filter::~Filter()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.
  delete cutoff_1;
  delete cutoff_2;

}

void Filter::SetupWindow()
{
	char buffer[24];
	TWindow::SetupWindow();
	sprintf(buffer,"%.3g",fcutoff_1);
	cutoff_1->SetText(buffer);
	sprintf(buffer,"%.3g",fcutoff_2);
	cutoff_2->SetText(buffer);
}

// Comes here when user presses OK
void Filter::Handle_Ok()
{
	int len = 15;
	char p[16];
	// Put data back into ctrl
	cutoff_1->GetText(p,len);
	fcutoff_1 = atof(p);
	cutoff_2->GetText(p,len);
	fcutoff_2 = atof(p);
	CmOk();
}

void Filter::Handle_Help()
{
  //WinHelp("AS1.HLP", HELP_CONTEXT, Filter_Help);
  myHtmlHelp(Filter_Help);
}

#endif
//
// FilterFP
//
DEFINE_RESPONSE_TABLE2(FilterLP, TDialog,TWindow)
	EV_COMMAND(IDOK,Handle_Ok),
	EV_COMMAND(IDHELP,Handle_Help),
	//EV_COMMAND(IDHELP,Handle_Help),
END_RESPONSE_TABLE;


FilterLP::FilterLP(TWindow* parent, TResId resId, TModule* module)
:
    TDialog(parent, resId, module)
{
  // INSERT>> Your constructor code here.
  Edit_Pp = new TEdit(this,IDC_LP_NP);
  Edit_Qp = new TEdit(this,IDC_LP_Q);
  Edit_Pf = new TEdit(this,IDC_LP_LP);
}


FilterLP::~FilterLP()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.
  delete Edit_Pp;
  delete Edit_Qp;
  delete Edit_Pf;

}

void FilterLP::SetupWindow()
{
	char buffer[24];
	TWindow::SetupWindow();
	sprintf(buffer,"%.3g",profile->Pp);
	Edit_Pp->SetText(buffer);
	sprintf(buffer,"%.3g",profile->Qp);
	Edit_Qp->SetText(buffer);
	sprintf(buffer,"%.3g",profile->Pf);
	Edit_Pf->SetText(buffer);
}

// Comes here when user presses OK
void FilterLP::Handle_Ok()
{
	int len = 15;
	char p[16];
	// Put data back into ctrl
	Edit_Pp->GetText(p,len);
	profile->Pp = atof(p);
	Edit_Qp->GetText(p,len);
	profile->Qp = atof(p);
	Edit_Pf->GetText(p,len);
	profile->Pf = atof(p);
	CmOk();
}

void FilterLP::Handle_Help()
{
  //WinHelp("AS1.HLP", HELP_CONTEXT, Filter_Help);
  myHtmlHelp(Filter_Help);
}

//
// Low Pass Filter
//
DEFINE_RESPONSE_TABLE2(FilterLowPass, TDialog,TWindow)
	EV_COMMAND(IDOK,Handle_Ok),
	EV_COMMAND(IDHELP,Handle_Help),
	EV_COMMAND(IDC_LOW_PASS_S,CmSec),
	EV_COMMAND(IDC_LOW_PASS_HZ,CmHz),
	EV_COMMAND(IDC_LOW_PASS_UPDATE,CmUpdate),
	//EV_COMMAND(IDHELP,Handle_Help),
END_RESPONSE_TABLE;


FilterLowPass::FilterLowPass(TWindow* parent, TResId resId, TModule* module)
:
    TDialog(parent, resId, module)
{
  // INSERT>> Your constructor code here.
  Edit_Period = new TEdit(this,IDC_LOW_PASS_PERIOD);
  Edit_Frequency = new TEdit(this,IDC_LOW_PASS_FREQ);
  sec = new TRadioButton(this,IDC_LOW_PASS_S);
  hz = new TRadioButton(this,IDC_LOW_PASS_HZ);
	enable = new TCheckBox(this,IDC_ENABLE_LOW_PASS);
}


FilterLowPass::~FilterLowPass()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.
  delete Edit_Period ;
  delete Edit_Frequency;
  delete sec;
  delete hz;
  delete enable;
}

void FilterLowPass::SetupWindow()
{
	TWindow::SetupWindow();
  char buffer[24];
	sprintf(buffer,"%.3g",profile->LowPassPeriod);
	Edit_Period->SetText(buffer);
	sprintf(buffer,"%.3g",1/profile->LowPassPeriod);
  Edit_Frequency->SetText(buffer);
  if(profile->EnableLowPass)
  	enable->SetCheck(BF_CHECKED);
  Process();
}

void FilterLowPass::PostProcess()
{
	int len = 15;
	char p[16];
  if(sec->GetCheck() == BF_CHECKED) {
		Edit_Period->GetText(p,len);
		profile->LowPassPeriod = atof(p);
		sprintf(p,"%.3g",1/profile->LowPassPeriod);
  	Edit_Frequency->SetText(p);
  } else {
		Edit_Frequency->GetText(p,len);
    float f = atof(p);
    if(f != 0)
    	f = 1/f;
    else
    	f = 0;
		profile->LowPassPeriod = f;
		sprintf(p,"%.3g",profile->LowPassPeriod);
		Edit_Period->SetText(p);
  }
}

void FilterLowPass::CmUpdate()
{
	PostProcess();
}
void FilterLowPass::Process()
{
  if(profile->LowPassSec) {
  	Edit_Frequency->SetReadOnly(true);
  	Edit_Period->SetReadOnly(false);
  	sec->SetCheck(BF_CHECKED);
  } else {
  	Edit_Period->SetReadOnly(true);
  	Edit_Frequency->SetReadOnly(false);
  	hz->SetCheck(BF_CHECKED);
  }
}

// Comes here when user presses OK
void FilterLowPass::Handle_Ok()
{
	PostProcess();
	if(enable->GetCheck() == BF_CHECKED)
  	profile->EnableLowPass;
	CmOk();
}

void FilterLowPass::CmSec()
{
	profile->LowPassSec = true;
  Process();
  Invalidate();
}

void FilterLowPass::CmHz()
{
	profile->LowPassSec = false;
  Process();
  Invalidate();
}

void FilterLowPass::Handle_Help()
{
  //WinHelp("AS1.HLP", HELP_CONTEXT, Filter_Help);
  myHtmlHelp(Filter_Help);

}

/*********************************/
//
// Bob McClure's High Pass Filter
//
DEFINE_RESPONSE_TABLE2(FilterHighPass, TDialog,TWindow)
	EV_COMMAND(IDOK,Handle_Ok),
	EV_COMMAND(IDHELP,Handle_Help),
	//EV_COMMAND(IDHELP,Handle_Help),
END_RESPONSE_TABLE;


FilterHighPass::FilterHighPass(TWindow* parent, TResId resId, TModule* module)
:
    TDialog(parent, resId, module)
{
  // INSERT>> Your constructor code here.
  Edit_Period = new TEdit(this,IDC_LOW_PASS_PERIOD);
}


FilterHighPass::~FilterHighPass()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.
  delete Edit_Period ;

}

void FilterHighPass::SetupWindow()
{
	char buffer[24];
	TWindow::SetupWindow();
	sprintf(buffer,"%.3g",profile->HighPassPeriod);
	Edit_Period->SetText(buffer);
}

// Comes here when user presses OK
void FilterHighPass::Handle_Ok()
{
	int len = 15;
	char p[16];
	// Put data back into ctrl
	Edit_Period->GetText(p,len);
	profile->HighPassPeriod = atof(p);
	CmOk();
}

void FilterHighPass::Handle_Help()
{
  //WinHelp("AS1.HLP", HELP_CONTEXT, Filter_Help);
  myHtmlHelp(Filter_Help);

}

