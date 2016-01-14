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
#if !defined(Input_h)              // Sentry, use file only if it's not already included.
#define Input_h


class Input : public TDialog {
  public:
    Input(TWindow* parent, char * caption, char *prompt_text, char *buf, int length,
    	TResId resId = IDD_INPUT, TModule* module = 0);
    virtual ~Input();

  protected:
		void CmOk();
    void SetupWindow();
    TEdit *input;
    TStatic *prompt;
    char *prompt_text;
    char *caption;
    bool visible;
    char *buf;
    int length;
  public:
  	DECLARE_RESPONSE_TABLE(Input);
};    //{{Input}}


#endif  // Input_h sentry.

