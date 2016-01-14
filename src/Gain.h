//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         gain.h
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for Gain (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(gain_h)              // Sentry, use file only if it's not already included.
#define gain_h

//{{TDialog = Gain}}
class Gain : public TDialog {
  public:
    Gain(TWindow* parent, TResId resId = IDD_GAIN_TIME_X, TModule* module = 0);
    virtual ~Gain();
    float GetGain();
    float GetExpansion();
    void SetGain(float g);
    void SetX(float fx);

  protected:
    void SetupWindow();
    void Handle_Ok();
    TWindow *parent;
  	TEdit *gain;
    TEdit *time_expansion;
    TUpDown *spin_gain, *spin_x;
    bool SpinGain(TNmUpDown& not);
    bool SpinX(TNmUpDown& not);
    float g;			// Gain
 DECLARE_RESPONSE_TABLE(Gain);

};    //{{Gain}}


#endif  // gain_h sentry.

