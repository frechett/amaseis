//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         arrivaldialog.h
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for ArrivalDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(arrivaldialog_h)              // Sentry, use file only if it's not already included.
#define arrivaldialog_h

#include <owl/dialog.h>

#include "as1.rh"            // Definition of all resources.


//{{TDialog = ArrivalDialog}}
class ArrivalDialog : public TDialog {
  public:
    ArrivalDialog(TWindow* parent, TResId resId = IDD_ARRIVALDIALOG, TModule* module = 0);
    virtual ~ArrivalDialog();
};    //{{ArrivalDialog}}


#endif  // arrivaldialog_h sentry.

