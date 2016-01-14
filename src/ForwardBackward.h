//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         forwardbackward.h
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for ForwardBackward (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(forwardbackward_h)              // Sentry, use file only if it's not already included.
#define forwardbackward_h

#include <owl/dialog.h>


#include <owl/commctrl.h>
#include "as1.rh"            // Definition of all resources.


//{{TDialog = ForwardBackward}}
class ForwardBackward : public TDialog {
  public:
    ForwardBackward(TWindow* parent, TResId resId = IDD_FORWARD_BACKWARD, TModule* module = 0);
    virtual ~ForwardBackward();

//{{ForwardBackwardVIRTUAL_BEGIN}}
  public:
   // virtual bool CanClose();
//{{ForwardBackwardVIRTUAL_END}}

//{{ForwardBackwardRSP_TBL_BEGIN}}
  protected:
    void CmDayBack();
    void CmDayForward();
    void CmMonthBack();
    void CmMonthForward();
    void CmWeekBack();
    void CmWeekForward();
    void CmYearBack();
    void CmYearForward();
    void CmNow();
//{{ForwardBackwardRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(ForwardBackward);
};    //{{ForwardBackward}}


#endif  // forwardbackward_h sentry.

