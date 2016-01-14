//----------------------------------------------------------------------------
//  Project Stations
//  RunTime Software
//  Copyright © 2007 All Rights Reserved.
//
//  SUBSYSTEM:    Stations Application
//  FILE:         stationsapp.cpp
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TStationsApp (TApplication).
//
//----------------------------------------------------------------------------

#include <owl/pch.h>

#include <owl/buttonga.h>
#include <owl/statusba.h>
#include <classlib/cmdline.h>
#include <winsys/registry.h>
#include <stdio.h>
#include "plot.h"
#include "stationsapp.h"
#include "stationswindow.h"                        // Definition of client class.
#include "stationsaboutdlg.h"                        // Definition of about dialog.
#include "ftp_stations.h"


//{{TStationsApp Implementation}}


//
// Build a response table for all messages/commands handled
// by the application.
//
DEFINE_RESPONSE_TABLE2(TStationsApp, TRecentFiles, TApplication)
//{{TStationsAppRSP_TBL_BEGIN}}
  EV_COMMAND(CM_HELPABOUT, CmHelpAbout),
  //EV_REGISTERED(MruFileMessage, CmFileSelected),
//{{TStationsAppRSP_TBL_END}}
END_RESPONSE_TABLE;


//--------------------------------------------------------
// TStationsApp
//
TStationsApp::TStationsApp() : TApplication("Stations"), TRecentFiles(".\\Stations.ini", 4)
{

  // Common file file flags and filters for Open/Save As dialogs.  Filename and directory are
  // computed in the member functions CmFileOpen, and CmFileSaveAs.
  //
  FileData.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
  FileData.SetFilter("All Files (*.*)|*.*|");
  FileData.DefExt = "txt";

  // INSERT>> Your constructor code here.
}


TStationsApp::~TStationsApp()
{
  // INSERT>> Your destructor code here.
}


void TStationsApp::SetupSpeedBar(TDecoratedFrame* frame)
{
  ApxHarbor = new THarbor(*frame);

  // Create default toolbar New and associate toolbar buttons with commands.
  //
  #if 0
  TDockableControlBar* cb = new TDockableControlBar(frame);
  cb->Insert(*new TButtonGadget(CM_FILENEW, CM_FILENEW));
  cb->Insert(*new TButtonGadget(CM_FILEOPEN, CM_FILEOPEN));
  cb->Insert(*new TButtonGadget(CM_FILESAVE, CM_FILESAVE));
  cb->Insert(*new TSeparatorGadget(6));
  cb->Insert(*new TButtonGadget(CM_EDITCUT, CM_EDITCUT));
  cb->Insert(*new TButtonGadget(CM_EDITCOPY, CM_EDITCOPY));
  cb->Insert(*new TButtonGadget(CM_EDITPASTE, CM_EDITPASTE));
  cb->Insert(*new TSeparatorGadget(6));
  cb->Insert(*new TButtonGadget(CM_EDITUNDO, CM_EDITUNDO));
  cb->Insert(*new TSeparatorGadget(6));
  cb->Insert(*new TButtonGadget(CM_EDITFIND, CM_EDITFIND));
  cb->Insert(*new TButtonGadget(CM_EDITFINDNEXT, CM_EDITFINDNEXT));

  // Add caption and fly-over help hints.
  //
  cb->SetCaption("Toolbar");
  cb->SetHintMode(TGadgetWindow::EnterHints);

  ApxHarbor->Insert(*cb, alTop);
  #endif
}


//--------------------------------------------------------
// TStationsApp
// ~~~~~
// Application intialization.
//
TSDIDecFrame* frame;
void TStationsApp::InitMainWindow()
{
  if (nCmdShow != SW_HIDE)
    nCmdShow = (nCmdShow != SW_SHOWMINNOACTIVE) ? SW_SHOWMAXIMIZED : nCmdShow;

  frame = new TSDIDecFrame(0, GetName(), 0, true);

  frame->Attr.Style |= WS_MAXIMIZE;
  //frame->Attr.Style &= (~WS_MAXIMIZEBOX);
  //	 	(~WS_OVERLAPPED)&
	//	(~WS_BORDER);
  // Assign icons for this application.
  //
  frame->SetIcon(this, IDI_STATIONS);
  frame->SetIconSm(this, IDI_STATIONS);

  // Menu associated with window and accelerator table associated with table.
  //
  frame->AssignMenu(IDM_SDI);


  // Associate with the accelerator table.
  //
  frame->Attr.AccelTable = IDM_SDI;

  TStatusBar* sb = new TStatusBar(frame, TGadget::Recessed,
                                  TStatusBar::CapsLock        |
                                  TStatusBar::NumLock         |
                                  TStatusBar::ScrollLock);
  frame->Insert(*sb, TDecoratedFrame::Bottom);

  SetupSpeedBar(frame);

  SetMainWindow(frame);

  frame->SetMenuDescr(TMenuDescr(IDM_SDI));
}


//--------------------------------------------------------
// TStationsApp
// ~~~~~
// Application instance initialization.
//
void TStationsApp::InitInstance()
{
  TApplication::InitInstance();

  ProcessCmdLine(lpCmdLine);
}


//--------------------------------------------------------
// TStationsApp
// ~~~~~
// Process command line parameters.
//
void TStationsApp::ProcessCmdLine(char * CmdLine)
{
  TCmdLine cmd(CmdLine);

  while (cmd.Kind != TCmdLine::Done) {
    if (cmd.Kind == TCmdLine::Option) {
      if (strnicmp(cmd.Token, "unregister", cmd.TokenLen) == 0) {
        UnRegisterInfo();
        return;
      }
    }
    cmd.NextToken();
  }

  RegisterInfo();
}


//--------------------------------------------------------
// TStationsApp
// ~~~~~
// Register application info.
//
void TStationsApp::RegisterInfo()
{
  TAPointer<char> buffer = new char[_MAX_PATH];

  GetModuleFileName(buffer, _MAX_PATH);

  TRegKey(TRegKey::ClassesRoot, "Stations.Application\\DefaultIcon").SetDefValue(0, REG_SZ, buffer, strlen(buffer));
  strcat(buffer, ",1");
  TRegKey(TRegKey::ClassesRoot, "Stations.Document.1\\DefaultIcon").SetDefValue(0, REG_SZ, buffer, strlen(buffer));
  strcpy(buffer, "Stations.Document.1");
  TRegKey(TRegKey::ClassesRoot, ".txt").SetDefValue(0, REG_SZ, buffer, strlen(buffer));
}


//--------------------------------------------------------
// TStationsApp
// ~~~~~
// Unregister application info.
//
void TStationsApp::UnRegisterInfo()
{
  TAPointer<char> buffer = new char[_MAX_PATH];

  GetModuleFileName(buffer, _MAX_PATH);

  TRegKey(TRegKey::ClassesRoot, "Stations.Application").DeleteKey("DefaultIcon");
  TRegKey(TRegKey::ClassesRoot, "Stations.Document.1").DeleteKey("DefaultIcon");

  TRegKey::ClassesRoot.DeleteKey("Stations.Application");
  TRegKey::ClassesRoot.DeleteKey("Stations.Document.1");
  TRegKey::ClassesRoot.DeleteKey(".txt");
}



//--------------------------------------------------------
// TStationsApp
// ~~~~~~~~~~~
// Menu File New command
//
void TStationsApp::CmFileNew()
{
}


//--------------------------------------------------------
// TStationsApp
// ~~~~~~~~~~~
// Menu File Open command
//
void TStationsApp::CmFileOpen()
{
  // Display standard Open dialog box to select a file name.
  //
  *FileData.FileName = 0;

	TStationsWindow * client = TYPESAFE_DOWNCAST(GetMainWindow()->GetClientWindow(), TStationsWindow);     // Client window for the frame.
  if (client->CanClose())
    if (TFileOpenDialog(GetMainWindow(), FileData).Execute() == IDOK)
      OpenFile();
}


void TStationsApp::OpenFile(const char* fileName)
{
  if (fileName)
    strcpy(FileData.FileName, fileName);


  // Add the file to the MRU list.
  //
  SaveMenuChoice(FileData.FileName);
}


//{{TSDIDecFrame Implementation}}


TSDIDecFrame::TSDIDecFrame(TWindow* parent, const char far* title, TWindow* clientWnd, bool trackMenuSelection, TModule* module)
:
  TDecoratedFrame(parent, title, !clientWnd ? new TStationsWindow(0, "") : clientWnd, trackMenuSelection, module)
{
  // INSERT>> Your constructor code here.

}


TSDIDecFrame::~TSDIDecFrame()
{
  // INSERT>> Your destructor code here.

}


//--------------------------------------------------------
// TStationsApp
// ~~~~~~~~~~~
// Menu Help About Stations command
//
void TStationsApp::CmHelpAbout()
{
  // Show the modal dialog.
  //
  TStationsAboutDlg(MainWindow).Execute();
}


int32 TStationsApp::CmFileSelected(uint wp, int32)
{
  TAPointer<char> text = new char[_MAX_PATH];

  GetMenuText(wp, text, _MAX_PATH);

  OpenFile(text);
  return 0;
}



int OwlMain(int , char* [])
{
  TStationsApp   app;
  return app.Run();
}

//
// Utility functions
//
static char * regKey = "Software\\RunTime Software\\Stations";
// Registry functions
static HKEY subKey;
bool OpenKey() {
	bool ret = false;
  DWORD disposition;
	if(ERROR_SUCCESS == RegCreateKeyEx(
    HKEY_CURRENT_USER,	// handle of an open key
    regKey,	// address of subkey name
    0,	// reserved
    "LOCATE",	// address of class string
    REG_OPTION_NON_VOLATILE	,	// data saved on re-start
    KEY_ALL_ACCESS	,	// desired security access
    NULL,	// address of key security structure
    &subKey,	// address of buffer for opened handle
    &disposition 	// address of disposition value buffer
   ))
   	ret = true;
	return ret;
}
bool SetKeyInt(char *name,CONST int value)
{
	bool ret = false;
  LPBYTE addr = (LPBYTE)&value;
	if(ERROR_SUCCESS == RegSetValueEx(
    subKey,				// handle of key to set value for
    name,					// address of value to set
    0,						// reserved
    REG_DWORD,		// DWORD
    addr,				// address of value data
    sizeof(int) 	// size of value data
    ))
    ret = true;
  return ret;
}
bool GetKeyInt(char *name,int &value)
{
	bool ret = false;
  LPBYTE addr = (LPBYTE)&value;
  DWORD size = sizeof(DWORD);
	if(ERROR_SUCCESS == RegQueryValueEx(
    subKey,		// handle of key to query
    name,			// address of name of value to query
    0,				// reserved
    0,				// address of buffer for value type
    addr,			// address of data buffer
    &size 		// address of data buffer size
   )) {
   	ret = true;

	}
	return ret;
}
bool SetKeyBool(char *name,CONST bool value)
{
	bool ret = false;
  LPBYTE addr = (LPBYTE)&value;
	if(ERROR_SUCCESS == RegSetValueEx(
    subKey,				// handle of key to set value for
    name,					// address of value to set
    0,						// reserved
    REG_BINARY,		// flag for value type
    addr,					// address of value data
    sizeof(bool) 	// size of value data
    ))
    ret = true;
  return ret;
}
bool GetKeyBool(char *name,bool &value)
{
	bool ret = false;
  LPBYTE addr = (LPBYTE)&value;
  DWORD size = sizeof(bool);
	if(ERROR_SUCCESS == RegQueryValueEx(
    subKey,	// handle of key to query
    name,		// address of name of value to query
    0,			// reserved
    0,			// address of buffer for value type
    addr,		// address of data buffer
    &size 	// address of data buffer size
   )) {
   	ret = true;

	}
	return ret;
}

bool CloseKey()
{
	if(ERROR_SUCCESS == RegCloseKey(subKey))
  	return true;
  else
  	return false;
}

