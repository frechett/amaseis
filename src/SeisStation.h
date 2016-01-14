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
#if !defined(seisstation_h)              // Sentry, use file only if it's not already included.
#define seisstation_h

#include <owl/dialog.h>

#include "as1.rh"            // Definition of all resources.
#include <owl/button.h>
#include <owl/radiobut.h>
#include <owl/edit.h>


//{{TDialog = Station}}

class SeisStation : public TDialog {
  public:
    SeisStation(TWindow* parent, KSAC *sac, TResId resId = IDD_STATION, TModule* module = 0);
    virtual ~SeisStation();

//{{StationVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
//{{StationVIRTUAL_END}}

//{{StationRSP_TBL_BEGIN}}
  protected:
    void Handle_Ok();
    void Handle_Help();
    KSAC *sac;
//{{StationRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(SeisStation);

//{{StationXFER_DEF}}
  protected:
    TEdit* elevation;
    TEdit* station_name;
    TRadioButton* north;
    TEdit* station_code;
    TRadioButton* vertical;
    TButton* help;
    TButton* okay;
    TRadioButton* east;
    TButton* cancel;
    TEdit* latitude;
    TEdit* longitude;

//{{StationXFER_DEF_END}}
};    //{{Station}}


#endif  // station_h sentry.

