//----------------------------------------------------------------------------
//  Project Upload
//  
//  Copyright © 1997. All Rights Reserved.
//
//  SUBSYSTEM:    Upload Application
//  FILE:         uplodapp.cpp
//  AUTHOR:       
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TUploadApp (TApplication).
//
//----------------------------------------------------------------------------

#include <owl/pch.h>

#include <stdio.h>
#include "ftp_q.h"
#include "uplodapp.h"
#include "uplddlgc.h"                        // Definition of client class.


//{{TUploadApp Implementation}}


//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE1(TUploadApp, TApplication)
//{{TUploadAppRSP_TBL_BEGIN}}
  EV_COMMAND(CM_HELPABOUT, CmHelpAbout),
//{{TUploadAppRSP_TBL_END}}
END_RESPONSE_TABLE;


//--------------------------------------------------------
// TUploadApp
//
TUploadApp::TUploadApp() : TApplication("Upload")
{

  // INSERT>> Your constructor code here.
}


TUploadApp::~TUploadApp()
{
  // INSERT>> Your destructor code here.
}


//--------------------------------------------------------
// TUploadApp
// ~~~~~
// Application intialization.
//
TSDIDecFrame* frame;
void TUploadApp::InitMainWindow()
{
  if (nCmdShow != SW_HIDE)
    nCmdShow = (nCmdShow != SW_SHOWMINNOACTIVE) ? SW_SHOWNORMAL : nCmdShow;

  frame = new TSDIDecFrame(0, GetName(), 0, false);
  frame->SetFlag(wfShrinkToClient);

  // Assign icons for this application.
  //
  frame->SetIcon(this, IDI_UPLOAD);
  frame->SetIconSm(this, IDI_UPLOAD);

  SetMainWindow(frame);

}



//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(TSDIDecFrame, TDecoratedFrame)
//{{TSDIDecFrameRSP_TBL_BEGIN}}
//{{TSDIDecFrameRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{TSDIDecFrame Implementation}}
TSDIDecFrame::TSDIDecFrame(TWindow* parent, const char far* title, TWindow* clientWnd, bool trackMenuSelection, TModule* module)
:
  TDecoratedFrame(parent, title, !clientWnd ? new TUploadDlgClient(0) : clientWnd, trackMenuSelection, module)
{

}


TSDIDecFrame::~TSDIDecFrame()
{
  // INSERT>> Your destructor code here.

}


//--------------------------------------------------------
// TUploadApp
// ~~~~~~~~~~~
// Menu Help About Upload command
//
void TUploadApp::CmHelpAbout()
{
}



int OwlMain(int , char* [])
{
  TUploadApp   app;
  return app.Run();
}

void TSDIDecFrame::SetupWindow()
{
  TDecoratedFrame::SetupWindow();


}



