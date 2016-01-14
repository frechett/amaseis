//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         forwardbackward.cpp
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of ForwardBackward (TDialog).
//
//----------------------------------------------------------------------------
#include "as1.h"


//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(ForwardBackward, TDialog)
//{{ForwardBackwardRSP_TBL_BEGIN}}
  EV_BN_CLICKED(IDC_DAY_BACK, CmDayBack),
  EV_BN_CLICKED(IDC_DAY_FORWARD, CmDayForward),
  EV_BN_CLICKED(IDC_WEEK_BACK, CmWeekBack),
  EV_BN_CLICKED(IDC_WEEK_FORWARD, CmWeekForward),
  EV_BN_CLICKED(IDC_MONTH_BACK, CmMonthBack),
  EV_BN_CLICKED(IDC_MONTH_FORWARD, CmMonthForward),
  EV_BN_CLICKED(IDC_YEAR_BACK, CmYearBack),
  EV_BN_CLICKED(IDC_YEAR_FORWARD, CmYearForward),
  EV_BN_CLICKED(IDC_FB_NOW, CmNow),
//{{ForwardBackwardRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{ForwardBackward Implementation}}


ForwardBackward::ForwardBackward(TWindow* parent, TResId resId, TModule* module)
:
    TDialog(parent, resId, module)
{
  // INSERT>> Your constructor code here.

}


ForwardBackward::~ForwardBackward()
{
  Destroy(IDCANCEL);

  // INSERT>> Your destructor code here.

}


void ForwardBackward::CmDayBack()
{
  int h_offset = MdiClient->GetOffset();
  h_offset += 24;
  MdiClient->SetOffset(h_offset);
  MdiClient->Invalidate();
}



void ForwardBackward::CmDayForward()
{
  int h_offset = MdiClient->GetOffset();
  h_offset -= 24;
  if(h_offset < 0)
  	h_offset = 0;
  MdiClient->SetOffset(h_offset);
  MdiClient->Invalidate();
}

void ForwardBackward::CmWeekBack()
{
  int h_offset = MdiClient->GetOffset();
  h_offset += 7*24;
  MdiClient->SetOffset(h_offset);
  MdiClient->Invalidate();
}


void ForwardBackward::CmWeekForward()
{
  int h_offset = MdiClient->GetOffset();
  h_offset -= 7*24;
  if(h_offset < 0)
  	h_offset = 0;
  MdiClient->SetOffset(h_offset);
  MdiClient->Invalidate();
}

void ForwardBackward::CmMonthBack()
{
  int h_offset = MdiClient->GetOffset();
  h_offset += 31*24;
  MdiClient->SetOffset(h_offset);
  MdiClient->Invalidate();
}


void ForwardBackward::CmMonthForward()
{
  int h_offset = MdiClient->GetOffset();
  h_offset -= 31*24;
  if(h_offset < 0)
  	h_offset = 0;
  MdiClient->SetOffset(h_offset);
  MdiClient->Invalidate();
}


void ForwardBackward::CmYearBack()
{
  int h_offset = MdiClient->GetOffset();
  h_offset += 365*24;
  MdiClient->SetOffset(h_offset);
  MdiClient->Invalidate();
}


void ForwardBackward::CmYearForward()
{
  int h_offset = MdiClient->GetOffset();
  h_offset -= 365*24;
  if(h_offset < 0)
  	h_offset = 0;
  MdiClient->SetOffset(h_offset);
  MdiClient->Invalidate();
}

void ForwardBackward::CmNow()
{
  MdiClient->SetOffset(0);
  MdiClient->Invalidate();
}
//void ForwardBackward::CmClose()
//{
//  CloseWindow();
//}




//bool ForwardBackward::CanClose()
//{
  //bool result;

  //result = TDialog::CanClose();

  // INSERT>> Your code here.

//  return true;
//}

