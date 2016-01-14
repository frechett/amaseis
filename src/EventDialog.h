//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         eventdialog.h
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for EventDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(eventdialog_h)              // Sentry, use file only if it's not already included.
#define eventdialog_h
#include "as1.h"            // Definition of all resources.

//{{TDialog = EventDialog}}

class EventDialog : public TDialog {
  public:
    EventDialog(TWindow* parent, EVENT_DATA &ed, TResId resId = IDD_EVENTDIALOG, TModule* module = 0);
    virtual ~EventDialog();

//{{EventDialogRSP_TBL_BEGIN}}
  protected:
    void Handle_Ok();
    void Handle_Help();
    void Handle_Evt();
    void Process();
    EVENT_DATA *ped;
    virtual void SetupWindow();
//{{EventDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(EventDialog);

//{{EventDialogXFER_DEF}}
  protected:
    TEdit* event_name;
    TEdit* event_depth;
    TEdit* event_latitude;
    TEdit* event_longitude;
    TEdit* event_mb;				// Magnitude Mb
    TEdit* event_ms;				// Magnitude Ms
    TEdit* event_ml;				// Magnitude Ml
    TEdit* event_mw;				// Magnitude Mw
    TEdit* event_year;
    TEdit* event_month;
    TEdit* event_day;
    TEdit* event_hour;
    TEdit* event_minute;
    TEdit* event_second;
    TStatic* event_origin;
    TEdit* origin_year;
    TEdit* origin_month;
    TEdit* origin_day;
    TEdit* origin_hour;
    TEdit* origin_minute;
    TEdit* origin_second;
    TButton* create_evt;
    TEdit* seismic_waves;
    int year, month, day, hour, minute;
    float second;

//{{EventDialogXFER_DEF_END}}
};    //{{EventDialog}}


#endif  // eventdialog_h sentry.

