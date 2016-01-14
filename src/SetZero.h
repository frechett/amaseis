//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         setzero.h
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for SetZero (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(setzero_h)              // Sentry, use file only if it's not already included.
#define setzero_h


class SetZero : public TDialog {
  public:
    SetZero(TWindow* parent, TResId resId = IDD_SET_ZERO, TModule* module = 0);
    virtual ~SetZero();

  protected:
//{{SetZeroXFER_DEF_END}}
		void CmOk();
    void SetupWindow();
    TEdit *zerolevel;
    bool visible;
//{{SetZeroVIRTUAL_BEGIN}}
  public:
    void Show(int);			// Display number
  	DECLARE_RESPONSE_TABLE(SetZero);

    virtual bool ShowWindow(int cmdShow);
//{{SetZeroVIRTUAL_END}}

//{{SetZeroRSP_TBL_BEGIN}}
  protected:
//{{SetZeroRSP_TBL_END}}
};    //{{SetZero}}


#endif  // setzero_h sentry.

