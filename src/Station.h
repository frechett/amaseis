//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         station.h
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for Station (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(station_h)              // Sentry, use file only if it's not already included.
#define station_h

#include <owl/dialog.h>

#include "as1.rh"            // Definition of all resources.
#include <owl/button.h>
#include <owl/radiobut.h>
#include <owl/edit.h>


//{{TDialog = Station}}

class Station : public TDialog {
  public:
    Station(TWindow* parent, TResId resId = IDD_STATION, TModule* module = 0);
    virtual ~Station();

//{{StationVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
//{{StationVIRTUAL_END}}

//{{StationRSP_TBL_BEGIN}}
  protected:
    void Handle_Ok();
    void Handle_Help();
//{{StationRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(Station);

//{{StationXFER_DEF}}
  protected:
    TEdit* elevation;
    TRadioButton* north;
    TEdit* station_code;
    TRadioButton* vertical;
    TButton* help;
    TButton* okay;
    TRadioButton* east;
    TButton* cancel;
    TEdit* latitude;
    TEdit* longitude;
    TEdit* station_name;
    TEdit* network_id;

//{{StationXFER_DEF_END}}
};    //{{Station}}


#endif  // station_h sentry.

