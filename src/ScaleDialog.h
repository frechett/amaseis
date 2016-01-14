//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         scaledialog.h
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for ScaleDialog (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(scaledialog_h)              // Sentry, use file only if it's not already included.
#define scaledialog_h

//#include <owl/dialog.h>


//#include <owl/checkbox.h>
//#include <owl/edit.h>
//#include <owl/commctrl.h>
//#include "as1.rh"            // Definition of all resources.


//{{TDialog = ScaleDialog}}
struct ScaleDialogXfer {
//{{ScaleDialogXFER_DATA}}
    uint    c_auto_scaling;
    char    e_max_amplitude[ 255 ];
//{{ScaleDialogXFER_DATA_END}}
};

class ScaleDialog : public TDialog {
  public:
    ScaleDialog(TWindow* parent, bool &auto_scale, float &max_amplitude, TResId resId = IDD_SCALE_DIALOG, TModule* module = 0);
    virtual ~ScaleDialog();

//{{ScaleDialogVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
//{{ScaleDialogVIRTUAL_END}}

//{{ScaleDialogRSP_TBL_BEGIN}}
  protected:
  	bool *auto_scale;
    float *max_amplitude;
    void OKClicked();
//{{ScaleDialogRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(ScaleDialog);

//{{ScaleDialogXFER_DEF}}
  protected:
    TCheckBox* c_auto_scaling;
    TEdit * e_max_amplitude;

//{{ScaleDialogXFER_DEF_END}}
};    //{{ScaleDialog}}


#endif  // scaledialog_h sentry.

