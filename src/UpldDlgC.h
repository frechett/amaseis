//----------------------------------------------------------------------------
//  Project Upload
//  
//  Copyright © 1997. All Rights Reserved.
//
//  SUBSYSTEM:    Upload Application
//  FILE:         uplddlgc.h
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for TUploadDlgClient (TDialog).
//
//----------------------------------------------------------------------------
#if !defined(uplddlgc_h)              // Sentry, use file only if it's not already included.
#define uplddlgc_h

#include "uplodapp.rh"                  // Definition of all resources.

#include <owl/listbox.h>

#define IDC_STATICTEXT1 104

//class myListBox : public TListBox {
//	public:
//		myListBox(TWindow *parent, int id);
  	//void SetText(const char *format,...);
//};

//{{TDialog = TUploadDlgClient}}
//struct TUploadDlgClientXfer {
//{{TUploadDlgClientXFER_DATA}}
//    TListBoxData  activity;
//{{TUploadDlgClientXFER_DATA_END}}
//};

class TUploadDlgClient : public TDialog {
  public:
    TUploadDlgClient(TWindow* parent, TResId resId = IDD_CLIENT, TModule* module = 0);
    virtual ~TUploadDlgClient();
    FtpUpload *ftpupload;
//{{TUploadDlgClientVIRTUAL_BEGIN}}
  public:
    virtual void SetupWindow();
    void SetText(const char *format,...);
//{{TUploadDlgClientVIRTUAL_END}}

//{{TUploadDlgClientXFER_DEF}}
  protected:
		TListBox *activity;
//{{TUploadDlgClientXFER_DEF_END}}

//{{TUploadDlgClientRSP_TBL_BEGIN}}
  protected:
    void EvTimer(uint timerId);
//{{TUploadDlgClientRSP_TBL_END}}
DECLARE_RESPONSE_TABLE(TUploadDlgClient);
};    //{{TUploadDlgClient}}


#endif  // uplddlgc_h sentry.
