//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         magnitude.h
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for Magnitude (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(magnitude_h)              // Sentry, use file only if it's not already included.
#define magnitude_h

#include <owl/dialog.h>


#include <owl/commctrl.h>
#include "as1.rh"            // Definition of all resources.
#include <owl/static.h>
#include <owl/edit.h>


//{{TDialog = Magnitude}}
//struct MagnitudeXfer {
//{{MagnitudeXFER_DATA}}
//    char    raw_amplitude[16 ];
//    char    period[16 ];
//    char    inst_response[16 ];
 //   //char    amplitude_microns[16 ];
//{{MagnitudeXFER_DATA_END}}
//};

extern char * DisplayResponseTable(char *filename);
extern float InterpolateResponse(char *cal_file, float per);

class Magnitude : public TDialog {
  public:
    Magnitude(TWindow* parent, TResId resId = IDD_MAGNITUDE, TModule* module = 0);
    virtual ~Magnitude();

//{{MagnitudeXFER_DEF}}
  protected:
    TEdit* period;
    TEdit* calibration_file;
    TStatic* inst_response;
    TStatic* amplitude_microns;
    TEdit* gca;
    TEdit* depth;
    TStatic* magnitude;
    TStatic* raw_amplitude;
    TStatic* q_field;
    TStatic* log_field;
    TStatic* response_table;

//{{MagnitudeXFER_DEF_END}}

//{{MagnitudeVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
//{{MagnitudeVIRTUAL_END}}

//{{MagnitudeRSP_TBL_BEGIN}}
  protected:
    void HandleOK();
    void HandleHelp();
    void HandleCancel();
    void Process();			// Process when a change is made
    void AmplitudeChange();
    void PeriodChange();
    void IRChange();
//{{MagnitudeRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(Magnitude);
};    //{{Magnitude}}


#endif  // magnitude_h sentry.

