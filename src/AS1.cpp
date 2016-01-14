//----------------------------------------------------------------------------
//  Project AS1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    AS1 Application
//  FILE:         as1.cpp
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Source file for implementation of TAS1 (TApplication).
//
//----------------------------------------------------------------------------
#include "as1.h"

HINSTANCE appl_inst;
TDecoratedMDIFrame* frame;

//{{TAS1 Implementation}}


//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE2(TAS1, TRecentFiles, TApplication)
//{{TAS1RSP_TBL_BEGIN}}
  EV_WM_WININICHANGE,
  EV_REGISTERED(MruFileMessage, CmFileSelected),
//{{TAS1RSP_TBL_END}}
END_RESPONSE_TABLE;


//--------------------------------------------------------
// TAS1
// ~~~~~
//
// Use file AmaSeis.ini to save the most recently used (MRU) SAC files
// since we want all our parameters in as1.ini.
TAS1::TAS1() : TApplication("AS1"), TRecentFiles(".\\AmaSeis.ini", 4)
{

  Printer = 0;
  Printing = 0;

  // INSERT>> Your constructor code here.
}


TAS1::~TAS1()
{
  delete Printer;

  // INSERT>> Your destructor code here.
}


void TAS1::SetupSpeedBar(TDecoratedMDIFrame* frame)
{
  ApxHarbor = new THarbor(*frame);

  // Create default toolbar New and associate toolbar buttons with commands.
  //

  TDockableControlBar* cb = new TDockableControlBar(frame);
  //cb->Insert(*new TButtonGadget(CM_MDIFILENEW, CM_MDIFILENEW));
  cb->Insert(*new TButtonGadget(CM_MDIFILEOPEN, CM_MDIFILEOPEN));
  cb->Insert(*new TButtonGadget(CM_FILESAVE, CM_FILESAVE));
  cb->Insert(*new TButtonGadget(CM_EXTRACT_EQ, CM_EXTRACT_EQ));
  cb->Insert(*new TButtonGadget(CM_PICK, CM_PICK));
  cb->Insert(*new TButtonGadget(CM_TRAVEL_TIME, CM_TRAVEL_TIME));
  cb->Insert(*new TButtonGadget(CM_FFT, CM_FFT));
  cb->Insert(*new TButtonGadget(CM_NOW, CM_NOW));
  cb->Insert(*new TButtonGadget(CM_FORWARD_BACKWARD, CM_FORWARD_BACKWARD));
  cb->Insert(*new TSeparatorGadget(6));
  cb->Insert(*new TButtonGadget(CM_FILEPRINT, CM_FILEPRINT));
  //cb->Insert(*new TTextGadget(0,0,0,13,GetResString(IDS_VERSION)));

  // Add caption and fly-over help hints.
  //
  cb->SetCaption("Toolbar");
  cb->SetHintMode(TGadgetWindow::EnterHints);

  ApxHarbor->Insert(*cb, alTop);
}

Client*  MdiClient;


//--------------------------------------------------------
// TAS1
// ~~~~~
// Application intialization.
//
void TAS1::InitMainWindow()
{
  if (nCmdShow != SW_HIDE)
    nCmdShow = (nCmdShow != SW_SHOWMINNOACTIVE) ? SW_SHOWMAXIMIZED : nCmdShow;
	//nCmdShow = SW_SHOWMAXIMIZED;
  MdiClient = new Client;
  frame = new TDecoratedMDIFrame(Name, IDM_MDI, *MdiClient, false);
  // 2009.11.24: Turn on maximize box so people can resize.
  //frame->Attr.Style &= (~WS_THICKFRAME) &
	// (~WS_OVERLAPPED)&
	// 	//(~WS_MAXIMIZEBOX)&
	// 	(~WS_BORDER);
	//frame->Attr.Style |= WS_MAXIMIZE;
  //frame->Attr.ExStyle |= WS_EX_TOPMOST;
  // Assign icons for this application.
  //
  frame->SetIcon(this, IDI_MDIAPPLICATION);
  frame->SetIconSm(this, IDI_MDIAPPLICATION);
  frame->SetCaption("AS1 - Caption");
  // Associate with the accelerator table.
  //
  frame->Attr.AccelTable = IDM_MDI;


  SetupSpeedBar(frame);

  SetMainWindow(frame);

  frame->SetMenuDescr(TMenuDescr(IDM_MDI));

}


//--------------------------------------------------------
// TAS1
// ~~~~~
// Application instance initialization.
//
void TAS1::InitInstance()
{
  TApplication::InitInstance();

  ProcessCmdLine(lpCmdLine);
}


//--------------------------------------------------------
// TAS1
// ~~~~~
// Process command line parameters.
//
void TAS1::ProcessCmdLine(char * CmdLine)
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
// TAS1
// ~~~~~
// Register application info.
//
void TAS1::RegisterInfo()
{
  TAPointer<char> buffer = new char[_MAX_PATH];

  GetModuleFileName(buffer, _MAX_PATH);

  TRegKey(TRegKey::ClassesRoot, "AS1.Application\\DefaultIcon").SetDefValue(0, REG_SZ, buffer, strlen(buffer));
  strcat(buffer, ",1");
  TRegKey(TRegKey::ClassesRoot, "AS1.Document.1\\DefaultIcon").SetDefValue(0, REG_SZ, buffer, strlen(buffer));
  strcpy(buffer, "AS1.Document.1");
  TRegKey(TRegKey::ClassesRoot, ".as1").SetDefValue(0, REG_SZ, buffer, strlen(buffer));
}


//--------------------------------------------------------
// TAS1
// ~~~~~
// Unregister application info.
//
void TAS1::UnRegisterInfo()
{
  TAPointer<char> buffer = new char[_MAX_PATH];

  GetModuleFileName(buffer, _MAX_PATH);

  TRegKey(TRegKey::ClassesRoot, "AS1.Application").DeleteKey("DefaultIcon");
  TRegKey(TRegKey::ClassesRoot, "AS1.Document.1").DeleteKey("DefaultIcon");

  TRegKey::ClassesRoot.DeleteKey("AS1.Application");
  TRegKey::ClassesRoot.DeleteKey("AS1.Document.1");
  TRegKey::ClassesRoot.DeleteKey(".as1");
}





//--------------------------------------------------------
// TAS1
// ~~~~~~~~~~~
// Menu Help About AS1 command
//
void TAS1::CmHelpAbout()
{
  // Show the modal dialog.
  //
  TAS1AboutDlg(MainWindow).Execute();
}


int32 TAS1::CmFileSelected(uint wp, int32)
{
  TAPointer<char> text = new char[_MAX_PATH];

  GetMenuText(wp, text, _MAX_PATH);

  //Client* theClient = TYPESAFE_DOWNCAST(GetMainWindow()->GetClientWindow(), Client);

  //if (theClient)
  //  theClient->CmFileOpen();
  MdiClient->OpenMRUEvent(text);
  return 0;
}

void TAS1::EvWinIniChange(char far* section)
{
  appl_inst = GetInstance();

  if (strcmp(section, "windows") == 0) {
    // If the device changed in the WIN.INI file then the printer
    // might have changed.  If we have a TPrinter(Printer) then
    // check and make sure it's identical to the current device
    // entry in WIN.INI.
    //
    if (Printer) {
      const int bufferSize = 255;
      char printDBuffer[bufferSize];
      LPSTR printDevice = printDBuffer;
      LPSTR devName;
      LPSTR driverName = 0;
      LPSTR outputName = 0;
      if (::GetProfileString("windows", "device", "", printDevice, bufferSize)) {
        // The string which should come back is something like:
        //
        //      HP LaserJet III,hppcl5a,LPT1:
        //
        // Where the format is:
        //
        //      devName,driverName,outputName
        //
        devName = printDevice;
        while (*printDevice) {
          if (*printDevice == ',') {
            *printDevice++ = 0;
            if (!driverName)
              driverName = printDevice;
            else
              outputName = printDevice;
          }
          else
            printDevice = ::AnsiNext(printDevice);
        }

        if (Printer->GetSetup().Error != 0                ||
            strcmp(devName, Printer->GetSetup().GetDeviceName()) != 0  ||
            strcmp(driverName, Printer->GetSetup().GetDriverName()) != 0 ||
            strcmp(outputName, Printer->GetSetup().GetOutputName()) != 0 ) {
          // New printer installed so get the new printer device now.
          //
          delete Printer;
          Printer = new TPrinter(this);
        }
      }
      else {
        // No printer installed(GetProfileString failed).
        //
        delete Printer;
        Printer = new TPrinter(this);
      }
    }
  }
}
int ver_res, hor_res;
TAS1 *papp;
char working_dir[32];
int TCPHostPort;
char TCPHostAddress[64];
char FTPAddr[48];
char FTPUser[24];
char FTPPW[32];
char FTPPath[32];
// arguments for invoking:
// as1.exe working_dir  TCPHostPort TCPHostAddress FTPURL FTPUser FTPPW
// Note: working_dir is the same as the station code
int OwlMain(int argc, char* argv[])
{
	TAS1   app;
 	TScreenDC dc;
  if(argc >= 2)
  	sprintf(working_dir,"%s\\",argv[1]);
  else
  	*working_dir = 0;
  if(argc >= 3)
  	TCPHostPort = atoi(argv[2]);
  if(argc >= 4)
  	strcpy(TCPHostAddress,argv[3]);
  if(argc >= 5)
  	strcpy(FTPAddr,argv[4]);
  if(argc >= 6)
  	strcpy(FTPUser,argv[5]);
  if(argc >= 7)
  	strcpy(FTPPW,argv[6]);
  if(argc >= 8)
  	strcpy(FTPPath,argv[7]);
	ver_res = dc.GetDeviceCaps(VERTRES);
	hor_res = dc.GetDeviceCaps(HORZRES);
  papp = &app;
  //Debug("argc:%d,argv[1]:%s,working_dir:%s,port:%d,url:%s",argc,argv[1],working_dir,TCPHostPort,TCPHostAddress);
  return app.Run();
}

