//----------------------------------------------------------------------------
//  Project rsw
//  
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    rsw.apx Application
//  FILE:         progress.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for Progress (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(progress_h)              // Sentry, use file only if it's not already included.
#define progress_h

//{{TDialog = Progress}}
#define SDBuffSize 255

class Progress : public TDialog {
  public:
    Progress(TWindow* parent, TResId resId = IDD_PROGRESS, TModule* module = 0);
    virtual ~Progress();
    TStatic* msg;

//{{ProgressXFER_DEF}}
  public:
    void SetText(const char *format,...);

//{{ProgressXFER_DEF_END}}
};    //{{Progress}}


#endif  // progress_h sentry.

