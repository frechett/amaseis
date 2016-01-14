//----------------------------------------------------------------------------
//  Project as1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    as1.apx Application
//  FILE:         showdata.h
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for ShowData (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(showdata_h)              // Sentry, use file only if it's not already included.
#define showdata_h


class ShowData : public TDialog {
  public:
    ShowData(TWindow* parent, TResId resId = IDD_SHOWDATA, TModule* module = 0);
    virtual ~ShowData();

  protected:
//{{SetZeroXFER_DEF_END}}
		void CmOk();
    TStatic *level;
    bool visible;
//{{SetZeroVIRTUAL_BEGIN}}
  public:
    void Show(int);			// Display number
  	DECLARE_RESPONSE_TABLE(ShowData);

    virtual bool ShowWindow(int cmdShow);
//{{SetZeroVIRTUAL_END}}

//{{SetZeroRSP_TBL_BEGIN}}
  protected:
//{{SetZeroRSP_TBL_END}}
};    //{{ShowData}}


#endif  // showdata_h sentry.

