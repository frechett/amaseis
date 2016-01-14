//----------------------------------------------------------------------------
//  Project AS1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    AS1 Application
//  FILE:         client.cpp
//  AUTHOR:       Alan Jones
//
//  OVERVIEW          
//  ~~~~~~~~
//  Source file for implementation of Client (TMDIClient).
//
//----------------------------------------------------------------------------

#include "as1.h"
//
// Global variable profile
Profile *profile;
static OPENFILENAME ofn;			// Holds OpenFileName information
static char filenames[256];
static ShowData *showdata;
static ForwardBackward *forward_backward;
extern TDecoratedMDIFrame* frame;
LOGPALETTE *loadLogPalette();

// Arrays of hour records
typedef TIArrayAsVector<HourRecord> RecordArray;
typedef TIArrayAsVectorIterator<HourRecord> RecordArrayIterator;

//#define lines_per_screen 24
static RecordArray records(24,0,8);
PleaseWait::PleaseWait(TWindow* parent, TResId resId, TModule* module)
:
    TDialog(parent, resId, module)
{
}


PleaseWait::~PleaseWait()
{
  Destroy(IDCANCEL);

}
void Debug(const char *format,...)
{
  if(strlen(profile->log_file) == 0)
  	return;
  va_list argptr;
  va_start(argptr,format);
  char buf[512];
  SYSTEMTIME utc;
  GetSystemTime(&utc);
	sprintf(buf,"%02d:%02d:%02d.%03d: ",utc.wHour,utc.wMinute,utc.wSecond,utc.wMilliseconds);
	//strcpy(buf,p+11);
	//strcat(buf,": ");
	vsprintf(buf+13,format,argptr);
  FILE *fp = fopen(profile->log_file,"a+t");
  //FILE *fp = fopen("As1Debug.txt","a+t");
  if(fp) {
  	fputs(buf,fp);
   	fputs("\n",fp);
   	fclose(fp);
  }
	va_end(argptr);
}

// This Debug routine only for special things such as someone
// connecting in to the server. It goes into the fixed-name file
// AS1_TCPDebug.txt
void Debug2(const char *format,...)
{
  va_list argptr;
  va_start(argptr,format);
  char buf[512];
  SYSTEMTIME utc;
  GetSystemTime(&utc);
	sprintf(buf,"%02d:%02d:%02d.%03d: ",utc.wHour,utc.wMinute,utc.wSecond,utc.wMilliseconds);
	//strcpy(buf,p+11);
	//strcat(buf,": ");
	vsprintf(buf+13,format,argptr);
  FILE *fp = fopen("AS1_TCPDebug.txt","a+t");
  if(fp) {
  	fputs(buf,fp);
   	fputs("\n",fp);
   	fclose(fp);
  }
	va_end(argptr);
}

static int last_p;


//
// Build a response table for all messages/commands handled by the application.
//
DEFINE_RESPONSE_TABLE1(Client, TMDIClient)
//{{ClientRSP_TBL_BEGIN}}
  EV_WM_VSCROLL,
  EV_COMMAND(CM_EXTRACT_EQ,				CmExtract),
  EV_COMMAND(CM_DELETE,						CmDelete),
	EV_COMMAND_ENABLE(CM_DELETE, 		CeDeleteEnable),
  EV_COMMAND(CM_MDIFILEOPEN,			CmFileOpen),
  EV_COMMAND(CM_DATE_TIME,  			CmDateTime),
  //EV_COMMAND(CM_FILTER,	  				CmFilter),
  EV_COMMAND(CM_GAIN,		  				CmGain),
  EV_COMMAND(CM_FILEPRINT,   			CmFilePrint),
  EV_COMMAND(CM_BITMAP, 					CmWriteBitMap),
  EV_COMMAND(CM_SETTINGSSTATION1,	CmStation),
  EV_COMMAND(CM_SHOW_DATA,				CmShowData),
  EV_COMMAND(CM_SET_ZERO,					CmSetZero),
  EV_COMMAND(CM_CONTINUOUS, 			CmContinuous),
  EV_COMMAND(CM_FILEPRINTERSETUP, CmFilePrintSetup),
  EV_COMMAND(CM_HELP_TOPICS,  	 	CmHelpTopics),
  EV_COMMAND(CM_HELPABOUT,  		 	CmHelpAbout),
  EV_COMMAND(CM_DEVICE,  		 	 		CmDevice),
  EV_COMMAND(CM_COM_PORT,  		 		CmComPort),
  EV_COMMAND(CM_CALIBRATION,  	 	CmCalibration),
  EV_MESSAGE(WM_MOUSEWHEEL, 			EvMouseWheel),
  //EV_COMMAND(CM_UPLOAD_24,  	 		CmUpload24Hours),
  EV_COMMAND(CM_FORWARD_BACKWARD, CmForwardBackward),
  EV_COMMAND_ENABLE(CM_FILEPRINT, CmPrintEnable),
  EV_COMMAND_ENABLE(CM_FILEPRINTERSETUP, 	CmPrintEnable),
  EV_COMMAND(CM_NOW,  	 				  CmNow),
  EV_WM_LBUTTONDOWN,
  EV_WM_MOUSEMOVE,
  EV_WM_LBUTTONUP,
  EV_WM_TIMER,
  EV_WM_SIZE,
//{{ClientRSP_TBL_END}}
END_RESPONSE_TABLE;


//{{Client Implementation}}

//--------------------------------------------------------
// Client
// ~~~~~~~~~~~
// Construction/Destruction handling.
//
Client::Client(TModule* module)
:
  TMDIClient(module)
{
  // INSERT>> Your constructor code here.
  //Attr.Style &= (~WS_MAXIMIZEBOX);
  serialio = 0;
  digitizing = false;
  samprate = 0;
  scrollbar = 0;
  h_offset = 0;
  x1 = x2 = 0;
  //getSR = false;
  pleasewait = 0;
	//scrollbar = new TScrollBar(this,CM_SCROLLBAR_VERT,hor_res-20,0,0,ver_res-80,false);
  scrollbar = 0;
  canclose = true;
  jump_mouse = true;
  hourrecord = 0;
  iris_bmap = 0;
  this_minute = 2;
  painting = false;
  memory_dc = 0;
  memory_bm = 0;
}


Client::~Client()
{
	Debug("Shutting Down");
  Destroy();
  //if(fp_debug)
  //	fclose(fp_debug);
  // INSERT>> Your destructor code here.
  // Turn off digitizer if a DATAQ151
  if(serialio) {
  	if((profile->device == Profile::DATAQ151) ||
     	(profile->device == Profile::DATAQ145) ||
     	(profile->device == Profile::DATAQ154) ||
     	(profile->device == Profile::DATAQ194) ||
     	(profile->device == Profile::DATAQ158)) {
      char s[3];
    	s[0] = 0;
    	s[1] = 'S';
    	s[2] = 0;
    	// Stop digitizing
    	serialio->Send(s,3);
    	Sleep(10);
    	s[1]= 'R';
    	// Reset
    	serialio->Send(s,3);
    } else if(profile->device == Profile::DATAQ155) {
    	serialio->Send("stop\r",5);
    }
  }
  profile->write();
 	delete scrollbar;
 	delete showdata;
 	delete serialio;
 	delete iris_bmap;
 	delete pleasewait;
  delete memory_dc;
  delete memory_bm;
}
//--------------------------------------------------------
// Client
// ~~~~~~~~~~~
// MDIClient site initialization.
//
void Client::SetupWindow()
{
	//static unsigned char *p;
  // Default SetUpWindow processing.
  //
  TMDIClient::SetupWindow();

  // Initialize profile
  if(profile == 0)
  	profile = new Profile();
  Debug("%s %d %s %s %s %s %s",working_dir,TCPHostPort,TCPHostAddress,FTPAddr,
  	FTPUser,FTPPW);

  Debug("%s:%d:%s",working_dir,TCPHostPort,TCPHostAddress);
  Debug("tcp_client:%d",profile->tcp_client);
  //MdiClient->MessageBox(temp);
	lines_per_screen = profile->lines_per_screen;		// Number of lines on the screen
  timer_rate = 200;
  if(profile->tcp_client) {
  	refresh_interval = profile->tcp_refresh_seconds/(timer_rate* 0.001);
  } else
  	refresh_interval = 1;
  ctr = 0;
  minute_ctr = 2;
 	Debug("Starting Amaseis. %s",GetResString(IDS_VERSION));
  // Set up caption to indicate this station
  char buf[128];
  sprintf(buf,"Station: %s/%s Lat: %.5g Lon: %.5g",profile->station_code,
  profile->station_name,profile->latitude,profile->longitude);
  frame->SetCaption(buf);
  // Initialize serial IO
  if(profile->passive)
  	serialio = 0;
  else if(serialio == 0)
  	serialio = new SerialIO(10240);
  //
  // ******2009.03.19***********
  // Put in pop-up if can't open COM port
  if(serialio->opened == false) {
  	char temp[32];
    sprintf(temp, "Can't open COM%d", profile->comport);
  	MessageBox(temp);
  }

  pleasewait = new PleaseWait(this);
	//Child *child = new Child(*this, "Please Wait", new PleaseWait(0));

  SetFileFilter(); 	// Not for filtering wave form. For filtering file name

  // Set waveform filter up for helicorder low-pass Butterworth 2nd order
  if(profile->LowPassPeriod)
  	Xfm::HelicorderFilterLP = ButterworthLP(profile->samprate,1.0/profile->LowPassPeriod,2);
  if(profile->HighPassPeriod)
  	Xfm::HelicorderFilterHP = ButterworthHP(profile->samprate,1.0/profile->HighPassPeriod,2);

  // Startup timer
  int size;
  #if 0
 	max_scroll = lines_per_screen*(profile->DaysToRetainRecords-1);
	scrollbar->Create();
 	scrollbar->SetRange(0,max_scroll,true);
  scrollbar->GetRange(size,max_scroll);
	scrollbar->SetPosition(max_scroll);
  scrollbar->SetPageMagnitude(4);
  //scrollbar->ShowWindow(SW_HIDE);
  // Scrollbar doesn't always "take" my max position so reset to
  // what it thinks max position is
  max_scroll = scrollbar->GetPosition();
  #endif
  if(serialio) {
  	//
    // *******2009.03.19*****
    // Allow timer to start even if com port not initialized so user
    // can see past data.
  	//if(serialio->opened) {
  		// Call chars_avail to flush buffer
    	serialio->chars_avail(size,true);
   		// Go to GET SAMPLE RATE but we know what it is but we still
    	// have to go there to get things going.
 			SetTimer(TIMER_GET_SAMPRATE,40);

    	csr = START;
  	//}
  }
  else {
    char temp[24];
    //MessageBeep(MB_ICONASTERISK);
    if(!profile->tcp_client && !profile->ftp_download && !profile->passive) {
    	sprintf(temp,"Problem opening COM%d",profile->comport);
  		MessageBox(temp,"");
    }
 		SetTimer(TIMER_GET_SAMPRATE,40);
  }
 	// Fetch small IRIS logo and put in bitmap
  TDib *dib;
	try {
 		dib = new TDib("IRIS-3.BMP");
 	}
 	catch (...){
 		dib = 0;
 	}

 	if(dib) {
 		iris_bmap = new TBitmap(*dib);
 		delete dib;
 	}
  else
  	iris_bmap = 0;
  TClientDC dc(*this);
  dc.SetMapMode(MM_ANISOTROPIC);
	showdata = new ShowData(this);
	showdata->Create();
  showdata->ShowWindow(SW_HIDE);
}


void Client::CmHelpTopics()
{
	//WinHelp("as1.hlp",HELP_FINDER,NULL);
  myHtmlHelp(NULL);
}


void Client::CmHelpAbout()
{
  // Show the modal dialog.
  //
  TAS1AboutDlg(this).Execute();
}

void Client::EvSize(uint sizeType, TSize& size)
{
	TMDIClient::EvSize(sizeType, size);
  delete scrollbar;
  scrollbar = 0;
	Invalidate();
}

void Client::SetFileFilter()
{
	static char curr_directory[64];
  // Common file file flags and filters for Open/Save As dialogs.  Filename and directory are
  // computed in the member functions CmFileOpen, and CmFileSaveAs.

  // Display Explorer-like standard Open dialog box to select a file name.
  // This dialog allows more than one file to be selected.
  ofn.lStructSize = sizeof(OPENFILENAME);
  ofn.hwndOwner = NULL;
  ofn.hInstance = NULL;
  char *filter =
  	"SAC files (*.sac)|*.sac|"
    "PEPP files (*.set)|*.set|"
    "PSN files (*.psn)|*.psn|";
  char *p = filter;
  char *pp;
  // The filter must consist of pairs of comments and filters
  // separated by nulls and end with two nulls
  while(0 != (pp = strchr(p,'|'))) {
  	*pp++ = 0;
    p = pp;
  }
  ofn.lpstrFilter = filter;
  ofn.lpstrCustomFilter = NULL;
  ofn.nMaxCustFilter = 0;
  ofn.nFilterIndex = 1;
  *filenames = 0;					// No initial file
  ofn.lpstrFile = filenames;
  ofn.nMaxFile = 256;
  ofn.lpstrFileTitle = NULL;
  ofn.nMaxFileTitle = 256;
  GetCurrentDirectory(64,curr_directory);
  ofn.lpstrInitialDir = curr_directory;
  ofn.lpstrTitle = NULL;
  ofn.Flags = OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_FILEMUSTEXIST |
  	OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
  ofn.lpstrDefExt = NULL;
  ofn.lCustData = 0;
  ofn.lpfnHook = NULL;
  ofn.lpTemplateName = NULL;
}

#if 0
void Client::OpenFile(const char* fileName)
{
  TAS1* theApp = TYPESAFE_DOWNCAST(GetApplication(), TAS1);

  if (fileName)
    strcpy(theApp->FileData.FileName, fileName);

  // See if this is a SAC file
 	KSAC *ksac;
 	PEPP *pepp;
  try {
    //MessageBox("Can we open as a PEPP file?");
  	pepp = new PEPP(theApp->FileData.FileName);
  } catch (...) {
    pepp = 0;
  } if(pepp != 0) {
 		// Everything okay.  Convert to SAC file internally and start
    // up Event screen with this event.
    //MessageBox("Convert PEPP to SAC");
   	ksac = new KSAC(pepp);
    //MessageBox("Converted to SAC.  Delete pepp");
   	delete pepp;
    //MessageBox("Start up Event screen");
  } else {
  	// Problem.  See if a SAC file
    //MessageBox("Problem.  See if this is a SAC file");
   	try {
   		ksac = new KSAC(theApp->FileData.FileName);
    } catch (...) {
      //MessageBox("Problem.  SAC didn't work either");
      ksac = 0;
    }
    if(ksac == 0) {
    	MessageBox("File error","Can't read file");
      return;
    }
  }
	CreateEq(ksac);
  // Add to MRU (Most Recently Used) files list
  theApp->SaveMenuChoice(theApp->FileData.FileName);
}
#endif
//--------------------------------------------------------
// Client
// ~~~~~~~~~~~
// Menu File Open command
//
void Client::CmFileOpen()
{
	int n = 0;
	KSAC ** ksacs = OpenEvent(n);
  if(n) {
		CreateEq(ksacs,n);
  }
}

// Read one or more event files.
// Returns an array of SAC events
// The number of events returned is in 'n'
KSAC ** Client::OpenEvent(int &n)
{
  KSAC **rksac = NULL;

  // We have to get and restore current directory.
  // GetOpenFileName shouldn't change the directory since
  // we specified OFN_NOCHANGEDIR but it does if user
  // selects more than one file.  Strange!
  //char curr_directory[64];
  //GetCurrentDirectory(64,curr_directory);
  // Don't want initial file to pop up since, if the
  // previous selection was multiple files, the directory
  // name pops up.
  *ofn.lpstrFile = 0;
  bool ret = GetOpenFileName(&ofn);
  if(ret) {
    // Process files one by one.  Point to first one.
    n = 0;
    KSAC *temp_ksac = 0;
    //for(char *p = ofn.lpstrFile+ofn.nFileOffset;*p;p += 1+strlen(p),n++) {
    for(char *p = ofn.lpstrFile+ofn.nFileOffset;*p;p += 1+strlen(p),n++) {
     	Debug("Event file opened: %s",p);
    	// This is kinda dumb.  If the user selects just one file, ofn.lpstrFile points
      // to the entire filespec (path+filename).  However, if the user selects more than
      // one, ofn.lpstrFile points to just the path followed by a null followed by the
      // string of names.
      char buf[240];
      // See if it is multiple selection
      if(ofn.lpstrFile[ofn.nFileOffset-1] == 0)
      	// Multiple selection
      	sprintf(buf,"%s\\%s",ofn.lpstrFile,p);
      else
      	// Single selection
        strcpy(buf,ofn.lpstrFile);
  		switch(ofn.nFilterIndex) {
    	default:
      	try {
					temp_ksac = new KSAC(buf);
      	} catch (...) {
        	temp_ksac = 0;
          MessageBox("Can't open file:",buf);
        }
      	if(temp_ksac != 0)
      		break;
      	// If SAC failed, maybe this is really a PEPP file.
      	// Fall through and see if PEPP will read it
    	case 2:	// PEPP file
    		PEPP *pepp;
      	try{pepp = new PEPP(buf);}
      	catch(...) {
        	pepp = 0;
      	}
      	if(pepp != 0) {
      		// Convert PEPP file to a SAC file
      		temp_ksac = new KSAC(pepp);
      		delete pepp;
      	}
      	break;
    	case 3:	// PSN file
    		PSN *psn;
      	try{psn = new PSN(buf);}
      	catch(...) {
        	psn = 0;
      	}
      	if(psn != 0) {
      		// Convert PEPP file to a SAC file
      		temp_ksac = new KSAC(psn);
      		delete psn;
      	}
      	break;
      }
    	if(temp_ksac == 0)
      	break;
    	//SetCaption(p);
    	// Add to MRU (Most Recently Used) files list
      TApplication *tapp = this->GetApplication();
			TAS1* theApp = TYPESAFE_DOWNCAST(tapp, TAS1);
 	  	theApp->SaveMenuChoice(buf);
      // Add to array of SAC instances
      if(rksac == 0) {
      	rksac = (KSAC **) calloc(1,sizeof(KSAC **));
      } else {
      	rksac = (KSAC **) realloc(rksac,(n+1)*sizeof(KSAC **));
      }
      temp_ksac->SACTimeToLongDouble();
      rksac[n] = temp_ksac;
    }
	}
  #if 0
  else {
  	// Error
    r = CommDlgExtendedError();
    sprintf(filenames,"GetOpenFileName error: %d",r);;
    MessageBox(filenames);
  }
  #endif
  //SetCurrentDirectory(curr_directory);
  return rksac;
}

// Open an event file selected from the File/MRU list
void Client::OpenMRUEvent(char *filename)
{
	KSAC ** ksacs = OpenEvent(filename);
  if(ksacs)
		CreateEq(ksacs,1);
}

KSAC ** Client::OpenEvent(char *filename)
{
	KSAC ** rksac = 0;
	KSAC * temp_ksac;
	try {
		temp_ksac = new KSAC(filename);
  } catch (...) {
   	temp_ksac = 0;
  }
  if(temp_ksac) {
  	rksac = (KSAC **) calloc(1,sizeof(KSAC **));
    rksac[0] = temp_ksac;
  }
  return rksac;
}

// Fetch a string from a resource
char *GetResString(UINT Id)
{
  static char temp[256];
  LoadString(appl_inst,Id,temp,256);
  return temp;
}
/*************************
** ErrorBox()
** Generalized error pop up
*************************/
void ErrorBox(TWindow *p,UINT res,char *s)
{
  char temp[48];
  wsprintf(temp,GetResString(res),s);
  MessageBeep(MB_ICONEXCLAMATION);
  p->MessageBox(temp,"Error",MB_ICONEXCLAMATION);
}

void Client::CmGain()
{
}

void Client::EvVScroll(UINT code, UINT pos, HWND wnd)
{
  TWindow::EvVScroll(code,pos,wnd); // perform default WM_HSCROLL processing
  int p = scrollbar->GetPosition();
  if(p != last_p) {
  	h_offset = max_scroll - p;
  	Invalidate();
  }
  last_p = p;
}
void Client::CreateEq(KSAC **ksacs,int n)
{
  // If there is already an event, destroy it
  //if(event != 0) {
  //	event->Destroy();
  //  delete event;
  //}
  //event = new Event(*this,"untitled",ksacs,n,0,false);
  //canclose = false;
  //if(CloseChildren()) {
  Child* curChild = (Child *)GetActiveMDIChild();
  if(curChild) {
  	curChild->Destroy();
    delete curChild;
  }
	Child *event = new Child(*this, "Event", new Event(0,"This event",ksacs,n));
  event->Attr.Style |= WS_MAXIMIZE;
  //child->Attr.W = 0.95*Attr.W;
  //child->Attr.H = 0.95*Attr.H;
  //child->Attr.X = 0;
  //child->Attr.Y = 0;

  event->SetIconSm(GetApplication(), IDI_MDIAPPLICATION);
  if(ksacs) {
  	event->SetCaption(ksacs[0]->GetFileName());
  }
  event->Create();
  //}
  //canclose = true;
}


// Comes here on every timer-tick
void Client::EvTimer(UINT id) {
 	TClientDC dc(*this);
  TColor col;
  static char buffer[1024];
  unsigned char *p;
  int size;
  TSize sz;
  static int cr_count;		// Count CRs
  static int timer_count;	// Count timer ticks
  static DWORD init_CT;		// Initial clock ticks
  static DWORD final_CT;	// Final clock ticks
  static int last_chunk;
  //int chunk;
  bool any;

  //FILE *fp;
 	//TMemoryDC dc_mem(dc);
 	switch(id) {
  case TIMER_GET_SAMPRATE:
    //if(fp_debug == 0)
  	switch(csr) {
    	case START:
        pleasewait->Create();
  			//
  			// Start up a TCP Listen instance
  			if(profile->tcp_server) {
  				if(profile->tcp_port) {
  					listenthread = new ListenThread(profile->tcp_port);
    				listenthread->Start();
    			}
  			}
  			else
  				listenthread = 0;
  			// If not a listener, we might be a client
  			if(listenthread == 0) {
  				if(profile->tcp_client &&
    				profile->tcp_port &&
      			strlen(profile->tcp_host_addr) != 0) {
            char buf[128];
            try {
    					tcpclient = new TCPClient(profile->tcp_host_addr,profile->tcp_port);
            } catch(...) {
            	//Debug("Couldn't establish TCP connection to Server");
              //Debug("Turn off TCP");
              profile->tcp_client = true;
              break;
            }
      			if(tcpclient->connected) {
      				Debug("Sign on: Hello getTime getSPS");
      				tcpclient->Hello();
        			tcpclient->getTime();
        			tcpclient->getSPS();
        			sprintf(buf,"Station: %s/%s Lat: %.5g Lon: %.5g",profile->station_code,
  							profile->station_name,profile->latitude,profile->longitude);
  						frame->SetCaption(buf);
        			Invalidate();
      			} else {
      				delete tcpclient;
        			tcpclient = 0;
      			}
    			}
  			}
  			if(profile->ftp_download) {
  				// Fetch sample rate from the host.
    			FtpThread *ftp = new FtpThread(0);
          // Following debug code.
          //ftp->GetFile("SampleRate.ini");
          float ss = ftp->GetSPS();
          if(ss)
    				profile->samprate = ss;
          else {
            profile->ftp_download = false;
            KillTimer(TIMER_GET_SAMPRATE);
          	sprintf(buffer,
            	"Problem getting sample rate from FTP site %s.\n"
              "Turning off FTP download.",profile->ftp_addr);
            //Debug(buffer);
            MessageBox(buffer,"AmaSeis");
          }
          if(ftp->GetStationInfo()) {
          	Debug("Problem getting station information from FTP site","AmaSeis");
            profile->ftp_download = false;
          }
          else {
            char buf[128];
  					sprintf(buf,"Station: %s/%s Lat: %.5g Lon: %.5g",profile->station_code,
  					profile->station_name,profile->latitude,profile->longitude);
  					frame->SetCaption(buf);
          }
    			delete ftp;
  			} else
  			if(profile->ftp_upload) {
  				Debug("Starting up. Upload any files not already there.");
  				if(profile->ftp_addr && profile->ftp_user && profile->ftp_pw) {
          	if(ftpthread) {
            	ftpthread->Terminate();
              int i = 10;
              for(;ftpthread->GetStatus() == TThread::Running && i;i--) {
              	Sleep(100);
              }
            }
      			delete ftpthread;
  					ftpthread = new FtpThread(24);
      			Progress *pro = new Progress(this);
      			pro->Create();
      			pro->SetText("Uploading Station information");
      			if(ftpthread->PutStationInfo()) {
            	KillTimer(TIMER_GET_SAMPRATE);
              //profile->ftp_upload = false;
              sprintf(buffer,"Problem putting station information on host");
            	pro->SetText(buffer);
              Debug(buffer);
              MessageBeep(MB_ICONEXCLAMATION);
            	MessageBox(buffer,"AmaSeis");
            } else {
            	pro->SetText("Uploading samples/second information");
      				samprate = profile->samprate;
            	if(ftpthread->PutSPS()) {
              	sprintf(buffer,"Problem putting station information on host");
                pro->SetText(buffer);
              	MessageBeep(MB_ICONEXCLAMATION);
            		MessageBox(buffer,"AmaSeis");
              } else {
              	// Used to be:
    						ftpthread->Start();
                // Change to:
                //ftpthread->myRun();
                // so as to not start up a thread. Might have been having
                // some conccurrancy problems.
              }
            }
            delete pro;

    			}
				}
  			if((profile->device == Profile::DATAQ151) ||
  				(profile->device == Profile::DATAQ145) ||
  				(profile->device == Profile::DATAQ154) ||
  				(profile->device == Profile::DATAQ155) ||
  				(profile->device == Profile::DATAQ194) ||
          (profile->device == Profile::DATAQ158)) {
					KillTimer(TIMER_GET_SAMPRATE);
          serialio->initDATAQ();
          // Make sure sample rate is an integer
          //samprate = (int)samprate;

    			samples_per_hour = 3600*samprate;
          //Debug("samprate:%f",samprate);
    			// Initialize an HourRecord starting at this time
    			hourrecord = new HourRecord();

    			digitizing = true;
          if(pleasewait) {
          	delete pleasewait;
            pleasewait = 0;
          }
          //pleasewait->ShowWindow(SW_HIDE);
    			SetTimer(TIMER_SECOND,100);
    			//Invalidate();	// So the screen will get redrawn with digitizing info.
				} else {
      		//fp = fopen("junk.txt","rb");
      		//if(fp) {
      		//	fread(buffer,539,1,fp);
        	//	fclose(fp);
      		//}

    			// flush any characters from buffer
          if(!profile->ftp_download && !profile->passive)
    				serialio->chars_avail(size,true);
      		csr = COMPUTING;
      		cr_count = 0;
      		timer_count = 0;
      		init_CT = GetTickCount();
      }
      break;
    case COMPUTING:
    	// Have we been here for 125 counts (5 seconds?) @
      // 20 ms per tick?
      // 2001.01.25: Set according to init_samp_sec.
      //if(timer_count++ >= 125) {
      if(profile->tcp_client || profile->ftp_download || profile->passive) {
      	csr = DONE;
        break;
      }
      if(timer_count++ >= profile->init_samp_sec*1000/40) {
      //if(timer_count++ >= profile->init_samp_sec) {
        csr = DONE;
        break;
      }

    	// Count new line characters
     	p = serialio->chars_avail(size,true);
      any = false;
    	for(;p != NULL;cr_count++) {
    		p = (unsigned char *)strchr((char *)p,0x0a);
      	if(p == NULL)
      		break;
      	p++;
        any = true;
    	}
      if(any) {
        final_CT = GetTickCount();
      }
      break;
    case DONE:
    	KillTimer(TIMER_GET_SAMPRATE);
    	KillTimer(TIMER_SECOND);
      if(pleasewait) {
      	pleasewait->Destroy();
        delete pleasewait;
        pleasewait = 0;
      }
      if(profile->passive)
      	samprate = profile->samprate;
      else {
    		samprate = (float)cr_count/(final_CT - init_CT);
    		samprate *= 1000;
      }
      if(profile->tcp_client) {
      	if(MdiClient->tcpclient)
      		MdiClient->tcpclient->getSPS();
      }
      if(profile->ftp_upload) {
  			FtpThread *ft = new FtpThread(0);
      	ft->PutSPS();
      	if(ft->PutSPS())
        	//profile->ftp_upload = false;
        delete ft;
      }
      if(profile->ftp_download) {
  			// Fetch sample rate from the host.
    		FtpThread *ftp = new FtpThread(0);
    		float ss = ftp->GetSPS();
        if(ss)
        	profile->samprate = ss;
        else
        	profile->ftp_download = false;
    		delete ftp;
  		}
    	if(samprate <.1) {
        MessageBeep(MB_ICONASTERISK);
        //switch(profile->device) {
        //case Profile::AS1: 			 		samprate = 6; 	break;
        //case Profile::INFILTECQM1:	samprate = 16;  break;
        //}
        samprate = profile->samprate;
        sprintf(buffer,"No data on COM%d.  Device not found.\n"
        	"Setting sampling rate to %g per second\n"
          "You can still use AmaSeis to view events\n"
          "or you can select Settings/Device to select a new device.",
          profile->comport,samprate);
      	if(!profile->tcp_client && !profile->ftp_download && !profile->passive)
        	MessageBox(buffer);
      }

    	samples_per_hour = 3600*samprate;

    	// Initialize an HourRecord starting at this time
    	hourrecord = new HourRecord();
      #if 0
      if(profile->ftp_download) {
    		FtpThread *ftp = new FtpThread(0);
        // Fetch all of the minute files
        //SYSTEMTIME utc;
        //GetSystemTime(&utc);
        //int minute = utc.wMinute;
        //if(minute > 0) {
        	if(ftp->DownloadMinuteFiles(hourrecord,1,59, true)) {
          	Debug("Problem downloading minute files 1 to 59");
            profile->ftp_download = false;
          }
        //}
    		delete ftp;
  		}
      #endif

    	// Now start up the sampling timer.  Short initial time and then slower.
    	SetTimer(TIMER_SECOND,100);
    	digitizing = true;
    	Invalidate();	// So the screen will get redrawn with digitizing info.
  		break;
    }
    break;
  case TIMER_SECOND:
  	bool okay = true;		// Used as a flag to see if we should restart timer
    KillTimer(TIMER_SECOND);
    // See if there is any data in the comport
    // Following is to knock out parity bit on AS1 and INFILTECQM and
    // not knock it out on DATAQ since it is sending 8-bit data
    if(profile->passive) {
    	p = NULL;
      size = 0;
    } else {
    switch(profile->device) {
    	case Profile::AS1:
    	case Profile::RHS3:
    	case Profile::RHV1:
 	 		case Profile::INFILTECQM1:
    	case Profile::EQ1:
    	case Profile::SEPUK1:
			p = serialio->chars_avail(size,true); break;
  		case Profile::DATAQ151:
  		case Profile::DATAQ145:
  		case Profile::DATAQ154:
  		case Profile::DATAQ155:
  		case Profile::DATAQ194:
  		case Profile::DATAQ158:
			p = serialio->chars_avail(size,false); break;
    	}
    }
    //fputs(p,fp_debug);

    /********************* Test code *****************/
    //static char *t =
    	//{"0\r\n10\r\n20\r\n30\r\n40\r\n50\r\n"};
      /****
    	 "10\r\n11\r\n12\r\n13\r\n14\r\n15\r\n16\r\n17\r\n18\r\n19\r\n"
    	 "20\r\n21\r\n22\r\n23\r\n24\r\n25\r\n26\r\n27\r\n28\r\n29\r\n"
    	 "30\r\n31\r\n32\r\n33\r\n34\r\n35\r\n36\r\n37\r\n38\r\n39\r\n"
    	 "40\r\n41\r\n42\r\n43\r\n44\r\n45\r\n46\r\n47\r\n48\r\n49\r\n"
    	 "50\r\n49\r\n48\r\n47\r\n46\r\n45\r\n44\r\n43\r\n42\r\n41\r\n"
    	 "40\r\n39\r\n38\r\n37\r\n36\r\n35\r\n34\r\n33\r\n32\r\n31\r\n"
    	 "30\r\n29\r\n28\r\n27\r\n26\r\n25\r\n24\r\n23\r\n22\r\n21\r\n"
    	 "20\r\n19\r\n18\r\n17\r\n16\r\n15\r\n14\r\n13\r\n12\r\n11\r\n"
    	 "10\r\n09\r\n08\r\n07\r\n06\r\n05\r\n04\r\n03\r\n02\r\n01\r\n"
    	 "-1\r\n-2\r\n-3\r\n-4\r\n-5\r\n-6\r\n-7\r\n-8\r\n-9\r\n-9\r\n"
    	 "-8\r\n-7\r\n-6\r\n-5\r\n-4\r\n-3\r\n-2\r\n-1\r\n00\r\n00\r\n"};
       *****/
    //static char *t = {"2098\r\n2098\r\n2098\r\n1998\r\n"
    //                  "1998\r\n1998\r\n"};
    //****/
    //static char *t = {"7\r\n21\r\n49\r\n24\r\n-52\r\n-57\r\n-23\r\n43\r\n34\r\n-68\r\n"
		//			"-44\r\n6\r\n\r\n20\r\n10\r\n-10\r\n-83\r\n-76\r\n20\r\n95\r\n-16\r\n-30\r\n"};
    //p = t;
    //size = strlen(t);
    //SetTimer(TIMER_SECOND,1000);

    /*************************************************/
   	SYSTEMTIME utc;
  	GetSystemTime(&utc);		// This yields UTC
    // Write time of day in lower-left-hand corner
		if (GetActiveWindow())              //Roger --write only if window is visible
    	writetime(utc,dc);
    // Send up a minute's worth of data every minute
    #if 0
    if(profile->ftp_upload) {
    	if(utc.wMinute != this_minute) {
        // Do some modulo arithmetic so that the minute files run from
        // 01.minute to 60.minute. The following should work but it doesn't:
        //int mmm = 1+(utc.wMinute-1)%60;
        // This is because C doesn't handle negative numbers properly.
        // Instead do it this way:
        //int mmm = utc.wMinute;
        //if(mmm == 0)
         //	mmm = 60;
        // Don't upload minute 60. The client probably won't reach it
        // anyway and it will download an hour file so it is quite useless
        // Minute 0 is really minute 60
        if(utc.wMinute) {
        	hourrecord->UploadOneMinute(utc.wMinute);
        }
      	this_minute = utc.wMinute;
        this_minute %= 60;	// Do modulo 60
      }
    }

    #endif
    // Should we save the screen automatically?
    if(profile->save_screen_minutes) {
    	if(utc.wMinute != this_minute) {
      	this_minute = utc.wMinute;
        this_minute %= 60;	// Do modulo 60
        // See if we should save screen
      	minute_ctr--;
        if(minute_ctr <= 0) {
          minute_ctr = profile->save_screen_minutes;
          // Save image of screen in bitmap
          char *save_screen_dir = (strlen(profile->save_screen_path)) ? profile->save_screen_path : "SaveScreen";
      		//static char *screen_save_dir = "SaveScreen";
          CreateDirectory(save_screen_dir,0);
          char fn[255];
          sprintf(fn,"%s\\%s.png",save_screen_dir,profile->station_code);
          //WriteBitMap(this,fn);
  				TPalette *palette;
 					LOGPALETTE *log = loadLogPalette();
 					if(log)
 					{
						palette = new TPalette(log);
            if(palette) {
            	try {
    						memory_dc->SelectObject(*palette);
    						//static int n = memory_dc->RealizePalette();
  							// Create a DIB
  							TDib *cdib = new TDib(*memory_bm,palette);
    						writepng(cdib,palette,fn);
  							delete cdib;
              } catch (...) {
              }
  						delete palette;
            }
   					delete log;
  				}
        }
      }
    }
    // See if we are at a new hour
    if(utc.wHour != hourrecord->hour) {
    //if(true) {
    	// Don't recompute sample rate first time through
      // Wait until a full hour has gone by
      //if((profile->device != Profile::DATAQ151) &&
      //	(profile->device != Profile::DATAQ154) &&
      //	(profile->device != Profile::DATAQ194)) {
      // We had been counting on the DATAQ having an accurate sample
      // rate.  However Alan Kafka reported that is was not accurate.
      // I did some tests and found out that mine was running at 14.9
      // when set to 16 sps.
      //if(getSR) {
    	if(profile->device ==  Profile::AS1 ||
      	 profile->device ==  Profile::INFILTECQM1 ||
      	 profile->device ==  Profile::SEPUK1 ||
      	 profile->device ==  Profile::RHS3 ||
      	 profile->device ==  Profile::RHV1 ||
      	 profile->device ==  Profile::EQ1)
         {
      		// Compute sample rate based on last full hour unless we are a client
          Debug("New hour");
          if(profile->tcp_client) {
          	// Open and close the connection
            //Debug("Close and reopen TCP connection");
            // Let fetchcurrent accesses finish
            Sleep(2000);
            //tcpclient->Goodbye();

            //tcpclient->closeConnection();
            // Give server a chance to finish up its hour-work
            //Sleep(2000);
            // Reopen connection
            //tcpclient->createSocket();
            //tcpclient->connectServer(profile->tcp_host_addr,profile->tcp_port);
          	//Debug("getTime, getSPS");
            if(tcpclient) {
          		tcpclient->getTime();
        			tcpclient->getSPS();
            }
          } else {
        		samples_per_hour = hourrecord->npts;
        		samprate = (float)samples_per_hour/3600;
          }
          //hourrecord->SetSPS(samprate);
       //	}
      }
      //getSR = true;
    	// We are at a new hour
      // Save this record
      // Don't save current data into a record if we are a client.
      // Leave the record empty. When Paint finds it empty, it will
      // try to fetch it from the server
      if(!profile->tcp_client && !profile->ftp_download && !profile->passive) {
     		if(hourrecord->save() == false) {
       		// Path doesn't exist.  Create it;
        	hourrecord->createPath();
       		Debug("Create path: %s",hourrecord->getPath());
        	if(hourrecord->save() == false) {
         		//KillTimer(TIMER_SECOND);
          	okay = false;
          	MessageBeep(MB_ICONASTERISK);
 						MessageBox("Problem saving record","");
        	}
        }
      }
      if(profile->ftp_upload) {
      	Debug("Just finished an hour. Upload new hour file.");
        // Give the last minute file a chance to upload.
        //Sleep(1000);
  			if(profile->ftp_addr && profile->ftp_user && profile->ftp_pw) {
          delete ftpthread;
  				ftpthread = new FtpThread(1,profile->ftp_days_to_save);
          Debug("Upload SPS %g",samprate);
          if(ftpthread->PutSPS()) {
          	Debug("Problem uploading sample rate.");
            //profile->ftp_upload = false;
          } //else
          //if(ftpthread->DeleteMinuteFiles()) {
          //	Debug("Problem deleting minute files.");
          //  //profile->ftp_upload = false;
          //  delete ftpthread;
          //  ftpthread = 0;
          //} else
    		 	ftpthread->Start();
        }
    	}
      if(profile->ftp_download) {
      	FtpThread *ft = new FtpThread(0);
        float ss = ft->GetSPS();
        if(ss) {
        	samprate = ss;
        	Debug("Just downloaded new sample rate: %f",samprate);
          samples_per_hour = ss*3600;
        } else {
        	Debug("Problem downloading sample rate");
          profile->ftp_download = false;
        }
        delete ft;
      }

     	delete hourrecord;
     	//long currenthour = HourRecord::st2hour(utc);
      //#ifdef DEBUG_FILE
      //Debug("Create new hour: %u",utc.wHour);
      //#endif
     	//hourrecord = new HourRecord(currenthour);
     	hourrecord = new HourRecord();
     	//hourrecord = new HourRecord();
      //fputs("New hour record created: ",fp_debug);
      //fputs(hourrecord->getPath(),fp_debug);
      //fputs("\n",fp_debug);
      //Debug("New hour record created: %s",hourrecord->getPath());
      //Debug("Sample rate: %g",samprate);
      Invalidate();
    }
    // else
    // 2003:03:29. Used to be "else" here which kept the first time
    // segment in an hour from getting recorded
    bool rr = false;
    if(profile->tcp_client) {
    	// Only fetch current once every so often as controled by profile->tcp_refresh_seconds
    	if(0 == (ctr++%refresh_interval)) {
    		// I think things get messed up on the current data if
      	// the screen is painting
        //tcpclient->Goodbye();

        //tcpclient->closeConnection();
        // Give server a chance to finish up its hour-work
        //Sleep(2000);
        // Reopen connection
        //tcpclient->createSocket();
        //tcpclient->connectServer(profile->tcp_host_addr,profile->tcp_port);
        //tcpclient->reconnectServer();
    		if(!painting)
    			rr = hourrecord->Process(p,size);
      }
    } else
    	rr = hourrecord->Process(p,size);
    if(size || rr) {
      // Process into HourRecord
     	//bool rr = hourrecord->Process(p,size);
      if(rr == true) {
      	if(hourrecord->npts) {
        	showdata->Show(hourrecord->data[hourrecord->npts-1]);
        }
      	// Draw new data points to screen
    		//if (GetActiveWindow()) {              //Roger --write only if window is visible
					writetime(utc,dc);
      	//	DrawNewData(dc);
        //}
        // Write into memory bitmap
        if(memory_dc) {
        	DrawNewData(dc,*memory_dc);
					writetime(utc,*memory_dc);
        }
      } else {
      	//DrawNewData(dc);
        //char buf[40];
    		//sprintf(buf,"Overun: size: %d, npts: %d",size,hourrecord->npts);
    		//MessageBeep(MB_ICONASTERISK);
    		//MessageBox(buf);

        #if 0
        #ifdef DEBUG_FILE
        Debug("Ran out of space.  Start new hour record.");
        #endif
    		if (GetActiveWindow())              //Roger --write only if window is visible
					writetime(utc,dc);
      	// Save this record
      	if(hourrecord->save() == false) {
        	// Path doesn't exist.  Create it;
        	#ifdef DEBUG_FILE
        	Debug("Create path: %s",hourrecord->getPath());
        	#endif
          if(0 == hourrecord->createPath()) {
          	//KillTimer(TIMER_SECOND);
            okay = false;
            MessageBeep(MB_ICONASTERISK);
  					MessageBox("Problem creating path to save record","");
          }
          if(hourrecord->save() == false) {
          	//KillTimer(TIMER_SECOND);
            okay = false;
        		MessageBeep(MB_ICONASTERISK);
  					MessageBox("Problem saving record","");
          }
        }
      	delete hourrecord;
      	hourrecord = new HourRecord();
      	//records.AddAt(hourrecord,0);
       	#ifdef DEBUG_FILE
       	Debug("New hour record: %s",hourrecord->getPath());
       	#endif
        #endif
        //Invalidate();
    	}
    }
    else {
    	// No data received.
      switch(profile->device) {
      case Profile::DATAQ145:
      case Profile::DATAQ151:
  		case Profile::DATAQ154:
  		case Profile::DATAQ155:
  		case Profile::DATAQ194:
      case Profile::DATAQ158:

      	//KillTimer(TIMER_SECOND);
        okay = false;
      	sprintf(buffer,"No data received on COM%d from DATAQ device.\n"
          "You can still use AmaSeis to view events\n"
          "or you can select Settings/Device to select a new device.",profile->comport);
      	MdiClient->MessageBox(buffer, "AmaSeis");
        break;
      }
      //CloseWindow();
      //return;

      #if 0
    	// No data received.  Try to reset if on a DATAQ
      if((profile->device == Profile::DATAQ151) ||
      	(profile->device == Profile::DATAQ154) ||
      	(profile->device == Profile::DATAQ145) ||
      	(profile->device == Profile::DATAQ194)) {
        	serialio->initDATAQ();
      }
      #endif
      if(profile->tcp_client)
      	profile->tcp_client = true;
    }
    // See if we are starting new chunk.  If so, invalidate to
    // redraw screen reflecting new chunk
  	int lph = profile->lines_per_hour;
    int spc = samples_per_hour/lph;
 		int chunk = lph-(hourrecord->npts)/spc;
    // If chuck == 0, we are past end of data.  Don't refresh screen
    if((chunk != last_chunk) && (chunk != 0))
    	Invalidate();
    if (chunk == 0)
      chunk = lph;     //Roger added this "if" line so that invalidate
      											// would not be called twice for lph = 1
    else
    	last_chunk = chunk;

    if(okay)
    	SetTimer(TIMER_SECOND,timer_rate);
    break;
  }
}
void Client::EvLButtonDown(UINT, TPoint &p)
{
	if(down == false)
		Mark();
	down = true;
  SetMark(p);
  #if 0
  //Roger ____begin code to give cursor position in minutes and seconds
  char temp1[48];
  //Roger__Get the x postion from the function "SetMark"
  int* xposition = &x1;
  //Roger__xposition is the cursor x location, ranges from 40 to 775
	TRect cr = GetClientRect();
  TClientDC dc(*this);
  TRect vr = GetVR(dc,cr);
  int w = vr.right-vr.left;
  int mkmin = ((*xposition - vr.left)*60)/w;
  int mksec = (((*xposition - vr.left)*3600)/w)%60;
  sprintf(temp1," %02d:%02d Min:Sec ",mkmin,mksec);
  //writexy(dc,vr.left,vr.bottom,0,temp1,RED,WHITE,0,0);
   //Roger end time display code
  #endif
}
void Client::EvMouseMove(UINT, TPoint &p)
{
  //Roger--I ran into the problem that my computer got to the mouse so fast that the variables needed
  //Roger--to run mouse  time were not set up.  Then the program would crash during startup.
  //Roger--Jump_mouse is used to delay mouse time window until all the variables required are available
  //Roger--Jump_mouse is set to false in Paint after DrawTraces has run. Jump.mouse is declared
  //Roger--in Client.h as a private variable, and initially set to true in Client::Client constructor
 	//if(hourrecord == NULL)  //Original code from Alan, Roger replaced with bool jump_mouse 11/11/03
  if(jump_mouse == true)           //Roger tried this , set to false in paint, true at program start
  	return;
  painting = true;
	TClientDC dc(*this);
	float hsf = GetHsf();
	int x = p.x;
  //int line = p.y/hsf + .5;
  // 2009.11.12: Take out the 0.5 since we added the margin at the top. Otherwise
  // times changes on the line rather than half-way between.
  int line = p.y/hsf;
  TRect cr = GetClientRect();
  TRect vr = GetVR(dc,cr);
  int width = vr.Width();
  int lph = profile->lines_per_hour;
  // Compute samples per chunk
  int spc = samples_per_hour/lph;
  // Compute records per page
  int rpp = lines_per_screen/lph;
  int chunk0;
	//chunk0 = (hourrecord->npts)/spc;
  chunk0 = lph - 1;
  // Compute hour from current time
  int h = (line+chunk0)/lph-rpp;   //Original code
  //Roger  original code is breaking here but only when
 	//Roger -- program is running as a stand-alone exe.

  float xsf = GetXsf(dc,cr);
  // Convert x-coordinate from screen to viewport
  x -= vr.Left();
  // Convert from screen to datapoints
  TPoint p1 = invXform(line+chunk0,x,width,xsf);
  // x is sample value.  Divide by samprate to get seconds
  int sec = p1.x/samprate;
  long h0 = hourrecord->hour0;
	h = h0+h-h_offset;
  char temp[48];
  TRect bm = TRect(vr.Left(),vr.Bottom(),cr.Right(),cr.Bottom());
  viewport(dc,bm,1);
  rect(dc,mouse_w,WHITE);
  // if(vr.Contains(p))	// original
  if(vr.Contains(p) && line > 0 )   //Roger Want no time if curser is not in window , works
		sprintf(temp,"%02d:%02d:%02d",h%24,sec/60,sec%60);
  else
  	*temp = 0;
  TFont *font_small = new TFont("MS Sans Serif",7);
  writexy(dc,mouse_w.TopLeft(),font_small,temp,BLACK,-1,0,2);
  delete font_small;
  viewport(dc,vr,1);

	if(down) {
    Mark();
    Mark(p);
  }
  painting = false;
}
void Client::EvLButtonUp(UINT, TPoint &)
{
	down = false;
}

// record where mouse is when it is set down
void Client::SetMark(TPoint p)
{
	float hsf = GetHsf();
	//line1 = line2 = p.y/hsf + .5;
	line1 = line2 = p.y/hsf;
  x1 = x2 = p.x;
}

void Client::Mark()
{
	TClientDC dc(*this);
	Mark(dc);
}

void Client::Mark(TDC &dc, TPoint p)
{
	float hsf = GetHsf();
	x2 = p.x;
  //line2 = p.y/hsf + .5;
  line2 = p.y/hsf;
  Mark(dc);
}

void Client::Mark(TPoint p)
{
	TClientDC dc(*this);
  Mark(dc,p);
}

// Mark the section
void Client::Mark(TDC &dc)
{
	// If line1 is below line1, reverse
	//if(line1 > line2) {
  //	int h = line2;
  //  line2 = line1;
  //  line1 = h;
  //  h = x2;
  //  x2 = x1;
  //  x1 = h;
  //}
  TRect cr = GetClientRect();
  TRect vr = GetVR(dc,cr);
  float hsf = GetHsf();
  viewport(dc,cr,0);
  // 2009.08.29. Geoff Coleson, Scotland and Tammy Bravo pointed out that
  #if 0
	if(line1 == line2) {
  	if(x1 == x2)
    	return;
		dc.PatBlt(TRect(x1,hsf*(line1-.5),x2,hsf*(line1+.5)),DSTINVERT);
  } else {
  	// First line
		dc.PatBlt(TRect(x1,hsf*(line1-.5),vr.Right(),hsf*(line1+.5)),DSTINVERT);
    // Next lines
		if(line2 > line1+1) {
			dc.PatBlt(TRect(vr.Left(),hsf*(line1+.5),vr.Right(),hsf*(line2-.5)),DSTINVERT);
    }
    // Last line
		dc.PatBlt(TRect(vr.Left(),hsf*(line2-.5),x2,hsf*(line2+.5)),DSTINVERT);
  }
  #endif
	if(line1 == line2) {
  	if(x1 == x2)
    	return;
		dc.PatBlt(TRect(x1,hsf*(line1),x2,hsf*(line1+1)),DSTINVERT);
  } else {
  	// First line
		dc.PatBlt(TRect(x1,hsf*(line1),vr.Right(),hsf*(line1+1)),DSTINVERT);
    // Next lines
		if(line2 > line1+1) {
			dc.PatBlt(TRect(vr.Left(),hsf*(line1+1),vr.Right(),hsf*(line2)),DSTINVERT);
    }
    // Last line
		dc.PatBlt(TRect(vr.Left(),hsf*(line2),x2,hsf*(line2+1)),DSTINVERT);
  }
}

#if 0
// Upload 24 hours worth of data to an FTP site.
void Client::CmUpload24Hours()
{
	char buf[48];
	if(IDOK != Input(this,"Event name","Enter event name",buf,sizeof(buf)).Execute())
  	return;
  //MessageBox(buf);
  long h0 = hourrecord->hour0;
  HourRecord *hr;
  FILE *fp = fopen("Upload.txt","at");
  fprintf(fp,"Event name: %s\n",buf);
  if(fp) {
  	for(int h = -24; h < 0;h++) {
			hr = new HourRecord(h0+h-h_offset);
      char *filespec = hr->getPath();
    	if(hr->npts == 0)
    		continue;
      fprintf(fp,"Upload:%s\n",filespec);
    }
    fclose(fp);
  }

}
// Enable CmUpload23Hours
void Client::CeUpload24HoursEnable(TCommandEnabler& tce) {
		if(strlen(profile->ftp_addr) && strlen(profile->ftp_pw) && strlen(profile->ftp_upload_path))
	tce.Enable(true);
  else
  	tce.Enable(false);
}
#endif
// Enable
void Client::CeDeleteEnable(TCommandEnabler& tce) {
	if((profile->ftp_download ||  profile->tcp_client) &&   (x1 != x2))
		tce.Enable(true);
  else
  	tce.Enable(false);
}

// Copy the event
void Client::CmExtract() {
	// See if there is anything to copy.
  int n;
  if(line1 == line2 && x1 == x2) {
  	MessageBox("You must mark with mouse trace to extract","Extract error");
  	return;
  }
  TClientDC dc(*this);
  // If point1 is before point2, reverse
  if(line1 > line2) {
  	int line = line2;
    line2 = line1;
    line1 = line;
  }
  if(line1 == line2) {
  	if(x1 > x2) {
    	int x = x2;
      x2 = x1;
      x1 = x;
    }
  }
  // Must be something there.  Go fetch all the records
  // But first, save record 0 since it is not
  // saved.
  // Roger replaced the following with line with code below
  //hourrecord->save();
  // Following is needed if extraction is done at hour 00 since it
  // needs to have the path created.
  if(hourrecord->save() == false) {
  	// Path doesn't exist.  Create it;
    //Debug("In extract.  Create hourrecord path: %s",hourrecord->getPath());
		hourrecord->createPath();
		if(hourrecord->save() == false) {
			KillTimer(TIMER_SECOND);
			MessageBeep(MB_ICONASTERISK);
			MessageBox("Problem saving record","");
		}
	}

  TRect cr = GetClientRect();
  TRect vr = GetVR(dc,cr);
  int width = vr.Width();
  int lph = profile->lines_per_hour;
  // Compute samples per chunk
  int spc = samples_per_hour/lph;
  // Compute records per page
  int rpp = lines_per_screen/lph;
	//int chunk0 = hourrecord->npts/spc;
  int chunk0 = lph-1;
  int h1 = (line1+chunk0)/lph-rpp;
  float xsf = GetXsf(dc,cr);
  // Convert x-coordinate from screen to viewport
  x1 -= vr.Left();
  x2 -= vr.Left();
  // Convert from screen to datapoints

  TPoint p1 = invXform(line1+chunk0,x1,width,xsf);
  int h2 = (line2+chunk0)/lph-rpp;
  TPoint p2 = invXform(line2+chunk0,x2,width,xsf);
  long h0 = hourrecord->hour0;
 	records.Flush();
  HourRecord *hr;
  for(int h = h1; h <= h2;h++) {
		hr = new HourRecord(h0+h-h_offset);
    if(hr->npts == 0)
    	hr->ZeroOut();
    // Compute x-scaling factor for this hour
    float xsf = (lph*vr.Width())/(3600.0*hr->GetSPS());
    hr->SetXSF(xsf);
    records.Add(hr);
  }
  // Figure out how many data points we have
  long npts;
  // Use sample rate from first hour extracted
  hr = records[0];
  float samprate1 = hr->GetSPS();
  // Use samprate1 in generting SAC file. See below.
	p1 = invXform(line1+chunk0,x1,width,hr->GetXSF());
  if(records.GetItemsInContainer() == 1) {
  	// Only one record
		p2 = invXform(line2+chunk0,x2,width,hr->GetXSF());
  	npts = p2.x - p1.x + 1;
  } else {
    npts = hr->npts - p1.x;
  	for(unsigned i = 1;i<records.GetItemsInContainer()-1;i++) {
  		hr = records[i];
    	npts += hr->npts;
    }
  	p2 = invXform(line2+chunk0,x2,width,hr->GetXSF());
    npts += p2.x;
  }
  float *floatdata = new float[npts];
  if(records.GetItemsInContainer() == 1) {
  	// Only one record
  	hr = records[0];
  	for(int i=0;i<npts;i++) {
    	int j = p1.x+i;
      if(j>=hr->npts)
      	floatdata[i] = 0;
      else
   			floatdata[i] = (float)hr->data[j];
    }
  } else {
  	hr = records[0];
    n = hr->npts;
    int i = 0;
    int k;
  	for(k = p1.x;k<n;k++) {
    	if(i>=npts) break;
    	floatdata[i++] = (float)hr->data[k];
    }
    unsigned j;
  	for(j=1;j<records.GetItemsInContainer()-1;j++) {
    	hr = records[j];
      int n = hr->npts;
    	for(int k=0;k<n;k++) {
      	if(i>=npts) break;
      	floatdata[i++] = (float)hr->data[k];
      }
    }
    // get last record
    hr = records[j];
    if(hr->npts != 0) {
    	for(int k=0;k<p2.x;k++) {
    		if(i>=npts) break;
    		floatdata[i++] = (float)hr->data[k];
      }
    }
  }
  // Have the event in array floatdata with npts points
  // Build a SAC event
  STATION station;
  SYSTEMTIME st = HourRecord::hourSec2st(h0+h1-h_offset,p1.x/samprate1);
  station.stla = profile->latitude;
  station.stlo = profile->longitude;
  station.stel = profile->elevation;
  station.delta = 1/samprate1;
  strncpy(station.name,profile->station_code,15);
  station.name[15] = 0;
  *station.component =profile->component;
  station.component[1] = 0;
  //Debug("Extract event with %d points",npts);
  KSAC *k;
  try {
  	k = new KSAC(npts,floatdata,station,st);
  }
  catch(...) {k = 0;}
  static KSAC *ksacs[1];
  if(k) {
    ksacs[0] = k;
  	CreateEq(ksacs,1);
  }
}
// Delete these records from disk
void Client::CmDelete() {
	// See if there is anything to copy.
  int n;
  if(line1 == line2 && x1 == x2) {
  	MessageBox("You must mark with mouse trace to delete","Delete error");
  	return;
  }
  TClientDC dc(*this);
  // If point1 is before point2, reverse
  if(line1 > line2) {
  	int line = line2;
    line2 = line1;
    line1 = line;
  }
  if(line1 == line2) {
  	if(x1 > x2) {
    	int x = x2;
      x2 = x1;
      x1 = x;
    }
  }
  // Must be something there.  Go fetch all the records
  // But first, save record 0 since it is not
  // saved.
  // Roger replaced the following with line with code below
  //hourrecord->save();
  // Following is needed if extraction is done at hour 00 since it
  // needs to have the path created.
  if(hourrecord->save() == false) {
  	// Path doesn't exist.  Create it;
    //Debug("In extract.  Create hourrecord path: %s",hourrecord->getPath());
		hourrecord->createPath();
		if(hourrecord->save() == false) {
			KillTimer(TIMER_SECOND);
			MessageBeep(MB_ICONASTERISK);
			MessageBox("Problem saving record","");
		}
	}

  TRect cr = GetClientRect();
  TRect vr = GetVR(dc,cr);
  int width = vr.Width();
  int lph = profile->lines_per_hour;
  // Compute samples per chunk
  int spc = samples_per_hour/lph;
  // Compute records per page
  int rpp = lines_per_screen/lph;
	//int chunk0 = hourrecord->npts/spc;
  int chunk0 = lph-1;
  int h1 = (line1+chunk0)/lph-rpp;
  float xsf = GetXsf(dc,cr);
  // Convert x-coordinate from screen to viewport
  x1 -= vr.Left();
  x2 -= vr.Left();
  // Convert from screen to datapoints

  TPoint p1 = invXform(line1+chunk0,x1,width,xsf);
  int h2 = (line2+chunk0)/lph-rpp;
  TPoint p2 = invXform(line2+chunk0,x2,width,xsf);
  long h0 = hourrecord->hour0;
 	records.Flush();
  HourRecord *hr;
  for(int h = h1; h <= h2;h++) {
		hr = new HourRecord(h0+h-h_offset);
    char *path = hr->getPath();
    remove(path);
  }
  Invalidate();
}

// Fetch margins and character height
void Client::GetMargins(TDC &dc,int &lmargin, int &rmargin,
	int &cwidth, int &cheight)
{
	TEXTMETRIC system_tm;
  dc.GetTextMetrics(system_tm);
  cwidth = system_tm.tmAveCharWidth;
  cheight = system_tm.tmHeight;
  lmargin = 6*cwidth;
  rmargin = 3*cwidth;
}

// Get the helicorder viewport rectangle based on the client rectangle (cr)
TRect Client::GetVR(TRect cr,int lmargin, int rmargin, int cheight) {
	//return TRect(cr.Left()+lmargin,0,cr.Right()-rmargin,cr.Bottom()-4*cheight);
	return TRect(cr.Left()+lmargin,cheight,cr.Right()-(rmargin+cheight),cr.Bottom()-4*cheight);
}

TRect Client::GetVR(TDC &dc,TRect cr) {
	int lmargin, rmargin, cwidth, cheight;
  GetMargins(dc,lmargin,rmargin,cwidth,cheight);
	return GetVR(cr,lmargin,rmargin,cheight);
}

float Client::GetHsf()
{
	TRect cr = GetClientRect();
	TClientDC dc(*this);
 	return GetHsf(dc,cr);
}

float Client::GetHsf(TDC &dc,TRect cr)
{
	TRect vr = GetVR(dc,cr);
 	//return (float)vr.Height()/26.0;
 	return (float)vr.Height()/(lines_per_screen+2);
}

float Client::GetXsf(TDC &dc,TRect cr)
{
	TRect vr = GetVR(dc,cr);
  int lph = profile->lines_per_hour;
	return (float)(lph*vr.Width())/(3600.0*samprate);
}

float Client::GetYsf(TDC &dc,TRect cr)
{
	float hsf = GetHsf(dc,cr);
 	return hsf*profile->gain/2047.0;
}


// Transform hours to vertical screen coordinates
// Note: hours are always 0 or negative.  The current hour
// is at the bottom where hour = 0.
float Client::XformHour(int h,float hsf)
{
	return hsf*(lines_per_screen+h);
}
//-------------
// xform
//
// Transform from data coordinates to screen coordinates
// parameter i: Index into data (x-coord)
// parameter d: data (seismogram record)
// parameter hour: Hour of this record
// Offset in screen coordinates
TPoint Client::Xform(int i, float d, int hour,float xsf,float ysf, float hsf)
{
	// Break x-coordinate into two parts, horizontal and vertical
  // depending on how many lines per hour and how far we are
  // into the record
	TPoint p = TPoint(i*xsf,XformHour(hour,hsf)+d*ysf);
  return p;
}

// Inverse transformation.  Convert from screen coordinates
// (in lines, and x) to hour,seconds
TPoint Client::invXform(int h, int x,int width,float xsf)
{
	// Convert from x-screen coordinates to data points
	int dp = x/xsf;
  // Convert from y-screen coordinate to hour
  int lph = profile->lines_per_hour;
  //h += (lph-1);
  int chunk = h%lph;
  h /= lph;
  h -= lines_per_screen/lph;
  dp += (width*chunk)/xsf;
  return TPoint(dp,h);
}
// Same as above but i is in seconds
#if 0
TPoint Client::Xsecform(int i, int d, int hour,float hsf, float xsec_sf,float ysf)
{
	float x = i*xsec_sf;
  int y = 0;
	return TPoint(x,XformHour(hour+y,hsf)+d*ysf);
}
#endif

// 2009.07.10: Instead of painting the screen, we change to paint into
// a memory bitmap and then paint the entire bitmap.
void Client::Paint(TDC&dc, bool, TRect& )
{
  //int lmargin, rmargin;
  int cwidth, cheight;
 	TEXTMETRIC large_tm;
  TColor col;
 	TBitmap *bmap;
  TBitmap *IRIS_bmap;
  TSize sz;
  TDib *dib;
  TFont *font_large;
  TRect cr = GetClientRect();
  if(digitizing) {
  	if(scrollbar == 0) {
  		int size;
			scrollbar = new TScrollBar(this,CM_SCROLLBAR_VERT,cr.Width()-20,0,0,cr.Height()-60,false);
  		max_scroll = lines_per_screen*(profile->DaysToRetainRecords-1);
			scrollbar->Create();
 			scrollbar->SetRange(0,max_scroll,true);
  		scrollbar->GetRange(size,max_scroll);
			scrollbar->SetPosition(max_scroll);
  		scrollbar->SetPageMagnitude(4);
  		//scrollbar->ShowWindow(SW_HIDE);
  		// Scrollbar doesn't always "take" my max position so reset to
  		// what it thinks max position is
  		max_scroll = scrollbar->GetPosition();
   	}
  	if(memory_bm == 0) {
  		memory_dc = new TMemoryDC(dc);
			memory_bm = new TBitmap(dc,cr.Width()-20,cr.Height());
			// Connect it to memory DC
    	memory_dc->SelectObject(*memory_bm);
      memory_dc->SetViewportOrg(TPoint(0,0));
      //memory_dc->TextRect(cr,colors[RED]);
  	}
  	memory_dc->SetMapMode(MM_ANISOTROPIC);
    //TSize oldVExt, oldWExt;
  	//dc.SetWindowExt(TSize(2000,2000),&oldVExt);
  	//dc.SetViewportExt(cr.Size(),&oldWExt);
    // See if we have a high-resolution device such as a printer
    // where the Viewport extent is greater than the Windows extent.
    // If so, make the Window larger
    TSize vExt = memory_dc->GetViewportExt();
    TSize cExt = memory_dc->GetWindowExt();
    if(vExt.cx > cr.Width()) {
  		cr = TRect(TPoint(0,0),vExt);
      //dc.SetWindowExt(vExt);
    } else {
    	// Don't want to do mark/unmark stuff if going to printer
  		Mark();
			SetMark(TPoint(0,0));
    }
  	scrollbar->ShowWindow(SW_SHOW);
    viewport(*memory_dc,cr,1);
   	DrawFrame(*memory_dc,cr);
   	//TRect vr = GetVR(dc,cr);
    //viewport(*memory_dc,vr,1);

    //memory_dc->TextRect(vr,colors[WHITE]);
 		DrawTraces(*memory_dc,cr);
    viewport(*memory_dc,cr,1);
    jump_mouse = false;
    // Copy from memory bitmap to screen
    dc.BitBlt(cr,*memory_dc,TPoint(0,0),SRCCOPY);
  } else {
   	TMemoryDC dc_mem(dc);
  	int y;
  	TEXTMETRIC system_tm;
    dc.GetTextMetrics(system_tm);
    cwidth = system_tm.tmAveCharWidth;
    cheight = system_tm.tmHeight;
  	dc.SetMapMode(MM_TEXT);
  	font_large = new TFont("Arial Bold",60,0,0,0,FW_BOLD);
 		dc.SelectObject(*font_large);
  	dc.GetTextMetrics(large_tm);
   	rect(dc,cr,DARKGRAY);
  	y = 2*cwidth;
		try {
 			dib = new TDib("AS1.BMP");
 		}
 		catch (...){
 			dib = 0;
			dc.SetBkColor(BLACK);
   		writexy(dc,TPoint(cr.right/2,cr.bottom/2),0,"Unable to open file: AS1.BMP",
    		BLACK,-1,1,1);
 		}

 		if(dib) {
 			bmap = new TBitmap(*dib);
 			// Connect it to memory DC
 			dc_mem.SelectObject(*bmap);
 			sz = TSize(dib->Size());
 			if(sz.cx)
 			{
   			// Put dib in upper left
   			TRect r_bm(TPoint(5,y),sz);
   			dc.BitBlt(r_bm,dc_mem,TPoint(0,0),SRCCOPY);
 			}
			dc_mem.RestoreBitmap();
			delete bmap;
 			delete dib;
 		}
    // Put up IRIS logo
		try {
 			dib = new TDib("IRIS-2.BMP");
 		}
 		catch (...){
 			dib = 0;
			dc.SetBkColor(BLACK);
   		writexy(dc,TPoint(cr.right/2,cr.bottom/2),0,"Unable to open file: IRISLogo.BMP",
    		BLACK,-1,1,1);
 		}

 		if(dib) {
 			IRIS_bmap = new TBitmap(*dib);
 			// Connect it to memory DC
 			dc_mem.SelectObject(*IRIS_bmap);
 			sz = TSize(dib->Size());
 			if(sz.cx)
 			{
   			// Put dib in upper right
   			TRect r_bm(TPoint(cr.right-sz.cx-5,y),sz);
   			dc.BitBlt(r_bm,dc_mem,TPoint(0,0),SRCCOPY);
 			}
			dc_mem.RestoreBitmap();
			delete IRIS_bmap;
 			delete dib;
 		}
    #if 0
    // Following stolen from as1aboutdlg
    TProjectRCVersion applVersion(GetModule());

  	// Get the product name and product version strings.
  	//
    char buffer[255];
    *buffer = 0;
   	LPSTR prodName = 0, prodVersion = 0;
    //LPSTR copyright = 0;

  	if (applVersion.GetProductName(prodName) && applVersion.GetProductVersion(prodVersion)) {
    	// IDC_VERSION is the product name and version number, the initial value of IDC_VERSION is
    	// the word Version(in whatever language) product name VERSION product version.
    	//
    	sprintf(buffer,"%s %s", prodName, prodVersion);
  	}

    #endif
    int x = (cr.Width())/2;
  	writexy(dc,x,y,font_large,GetResString(IDS_PROJECT),RED,-1,LEFT,TOP,2);
  	y += large_tm.tmHeight;
    delete font_large;
  	font_large = new TFont("Arial Bold",-30,0,0,0,FW_BOLD);
  	writexy(dc,x,y,font_large,"IRIS",RED,-1,CENTER,TOP,2);
  	y += 30;
  	delete font_large;
  	dc.SelectStockObject(SYSTEM_FONT);

  	writexy(dc,x,y,0,"Incorporated Research",RED,-1,CENTER,TOP);
  	y += cheight;
  	writexy(dc,x,y,0,"Institutions for Seismology",RED,-1,CENTER,TOP);
  	y += cheight;
  	writexy(dc,x,y,0,GetResString(IDS_VERSION),BLACK,-1,CENTER,TOP);
  	//writexy(dc,x,y,0,buffer,BLACK,-1,1,2);
  	y += cheight;

  	writexy(dc,x,y,0,"Written by Alan Jones",BLACK,-1,1,2);
  	y += cheight;
  	writexy(dc,x,y,0,"Alan.Jones@binghamton.edu",BLACK,-1,1,2);
  	y += cheight;
  	writexy(dc,x,y,0,"AlanJones@stny.rr.com",BLACK,-1,1,2);
  	y += 2*cheight;
 		try {
 			dib = new TDib("SEISMOGR.BMP");
 		}
 		catch (...){
 			dib = 0;
			dc.SetBkColor(BLACK);
   		writexy(dc,TPoint(cr.right/2,cr.bottom/2),0,"Unable to open file: SEISMOGR.BMP",
      	BLACK,-1,1,1);
 		}
 		if(dib) {
 			bmap = new TBitmap(*dib);
 			// Connect it to memory DC
 			dc_mem.SelectObject(*bmap);
 			sz = TSize(dib->Size());
 			if(sz.cx)
 			{
   			// Center the bitmap in the screen
   			TRect r_bm(TPoint((cr.Width()-sz.cx)/2,cr.Height()-sz.cy - 3*cheight),
     			sz);
   			dc.BitBlt(r_bm,dc_mem,TPoint(0,0),SRCCOPY);
       	border(dc,r_bm.InflatedBy(5,5),0,true,5,0);
 			}
			dc_mem.RestoreBitmap();
			delete bmap;
 			delete dib;
 		}
  }
  // Test pattern at upper left
  #if 0
    dc.SelectObject(TPen(TColor::White));
    dc.SelectObject(TBrush(TColor::White));
    dc.Rectangle(0,0,5,10);
    dc.SelectObject(TPen(TColor::LtRed));
    dc.SelectObject(TBrush(TColor::LtRed));
    dc.Rectangle(5,0,10,10);
    dc.SelectObject(TPen(TColor::LtGreen));
    dc.SelectObject(TBrush(TColor::LtGreen));
    dc.Rectangle(10,0,15,10);
    dc.SelectObject(TPen(TColor::LtBlue));
    dc.SelectObject(TBrush(TColor::LtBlue));
    dc.Rectangle(15,0,20,10);
  #endif
}

//----------------
// DrawFrame()
//
int Client::DrawFrame(TDC &dc, TRect &cr)
{
	TEXTMETRIC small_tm;
  int lmargin,rmargin,cwidth,cheight;
  if(samprate == 0)
  	return 0;
  char temp[128];
  TColor tmpcol;
  if(hourrecord == NULL)
  	return 0;
  //TRect cr = GetClientRect();
  TRect vr = GetVR(dc,cr);
  GetMargins(dc,lmargin,rmargin,cwidth,cheight);
  TFont *font_small = new TFont("MS Sans Serif",0.7*cheight);
 	dc.SelectObject(*font_small);
 	dc.GetTextMetrics(small_tm);
  dc.RestoreFont();
  TFont *font = new TFont("MS Sans Serif",cheight,0,0,0,FW_BOLD);
  int lph = profile->lines_per_hour;
  // Bottom margin
  TRect bm = TRect(vr.Left(),vr.Bottom(),cr.Right(),cr.Bottom());

  int time_w_y = bm.Height()-2*small_tm.tmHeight;
  int time_w_w = 	10*small_tm.tmAveCharWidth;
  int time_w_h = time_w_y+small_tm.tmHeight+1;
  time_w = TRect(0,time_w_y,time_w_w,time_w_h);
  mouse_w = time_w.OffsetBy(16*small_tm.tmAveCharWidth,0);
  TRect blm = TRect(0,vr.Bottom(),lmargin,cr.Bottom());

  // Compute scaling factors.  X-scale factor. Converts from data points to screen points
	// float xsf = GetXsf(dc,cr);
  // xsec_sf converts from seconds to screen coordinates
	float xsec_sf = (float)(lph*vr.Width())/3600.0;
	// Hour-scale factor.  Break into 25 sections.  There are 24
	// hours/day but we want space above and below the first and
 	// last record
 	float hsf = GetHsf(dc,cr);
  // Draw everything gray
  viewport(dc,cr,1);
  dc.TextRect(cr,colors[DARKGRAY]);
  // Put station name and coordinates at top
  //sprintf(temp,"Station: %s/%s Lat: %g Lon: %g",profile->station_code,
  //	profile->station_name,profile->latitude,profile->longitude);
  sprintf(temp,"Lat: %.5g Lon: %.5g",profile->latitude,profile->longitude);
  writexy(dc,cr.TopLeft()+TSize(cwidth*7,0),font_small,temp,WHITE,-1,LEFT,TOP);

  // Then paint helicorder white
  dc.TextRect(vr,colors[WHITE]);
 	tmpcol = TColor(colors[DARKGRAY]);
 	viewport(dc,bm,0);
  // Put borders on the time window and the mouse window
 	border(dc,time_w.InflatedBy(3,3),0,true,3,0);
 	border(dc,mouse_w.InflatedBy(3,3),0,true,3,0);
  int yyy = 3*bm.Height()/4;
  if(listenthread) {
  	sprintf(temp,"Server listening on port %d",profile->tcp_port);
  	writexy(dc,bm.Width()/4,yyy,font_small,temp,WHITE,-1,LEFT,TOP);
  } else if (tcpclient) {
  	sprintf(temp,"Client active on %s port %d",profile->tcp_host_addr,profile->tcp_port);
  	writexy(dc,bm.Width()/4,yyy,font_small,temp,WHITE,-1,LEFT,TOP);
  } else if(profile->ftp_download) {
  	sprintf(temp,"Downloading from FTP site: %s",profile->ftp_addr);
  	writexy(dc,bm.Width()/4,yyy,font_small,temp,WHITE,-1,LEFT,TOP);
  }
  if(profile->device == Profile::RHS3 || profile->device == Profile::RHV1) {
  	sprintf(temp,"Beam off-centre value: %g",profile->beam_off_centre_value);
  	writexy(dc,bm.Width()/2,yyy,font_small,temp,WHITE,-1,LEFT,TOP);
  }

  // Draw IRIS logo
  if(iris_bmap) {
 		TMemoryDC dc_mem(dc);
  	dc_mem.SelectObject(*iris_bmap);
  	TSize sz = iris_bmap->Size();
 		if(sz.cx)
 		{
  		TSize new_size;
    	int div = (sz.cy*2)/bm.Height();
      if(div == 0)
      	div = 2;

    	new_size.cx = sz.cx/div;
    	new_size.cy = sz.cy/div;
  		TRect r_bm(TPoint(bm.right-3*new_size.cx,bm.Height()-new_size.cy-3),new_size);
    	dc.StretchBlt(r_bm,dc_mem,TRect(TPoint(0,0),sz),SRCCOPY);
  		// Write station code to left of IRIS logo
      TPoint station_code = r_bm.TopLeft()+TPoint(-30,0);
  		if(strlen(profile->station_code)) {
  			TFont *bigfont = new TFont("Arial",2*cheight,0,0,0,FW_BOLD);
  			r_bm = writexy(dc,station_code,bigfont,profile->station_code,BLUE,WHITE,RIGHT,TOP,0,WHITE);
  			delete bigfont;
  		}
  		writexy(dc,r_bm.BottomLeft()-TSize(5,0),font_small,GetResString(IDS_VERSION),WHITE,-1,RIGHT,BOTTOM);
  	}
		dc_mem.RestoreBitmap();
  }

  viewport(dc,vr,1);
  // Draw vertical tick marks for each 5 minutes if the screen is one-hour wide
  int xx = vr.Height();
  if(lph <= 1) {
  	// Draw a time vertical line every 5 minutes
  	for(int i=300;i<3600;i += 300) {
    	int x = xsec_sf*i;
  		plot(dc,x,0);
    	plot(dc,x,cwidth,BLACK,1);
  		plot(dc,x,xx);
    	plot(dc,x,xx-cwidth,BLACK,1);
  	}
  } else {
  	// Draw a vertical line every minute for faster sweeps
  	for(int i=60;i<3600/lph;i += 60) {
    	int x = xsec_sf*i;
  		plot(dc,x,0);
    	plot(dc,x,cwidth,BLACK,1);
  		plot(dc,x,xx);
    	plot(dc,x,xx-cwidth,BLACK,1);
  	}
  }
  //Draw horizontal tick marks for each hour
  // See what chunk we are on.
  //int spc = samples_per_hour/lph;
 	//int chunk0 = lph-hourrecord->npts/spc;
 	int chunk0 = 1;
  //int chunk0 = lph;
  // In this loop, h = 0 corresponds to the current hour and is displayed at the
  // bottom of the page.
  for(int h=0;h>-(lines_per_screen+1/lph);h--) {
		int yy = XformHour(lph*(h-1)+chunk0,hsf);
  	//plot(dc,xx,yy);
    //plot(dc,xx+cwidth,yy,BLUE,1);
    // Write date at midnight
    if(((h+hourrecord->hour-h_offset)%24) == 0) {
    	long h1 = hourrecord->hour0+h-h_offset;
      SYSTEMTIME st = HourRecord::hour2st(h1);
      sprintf(temp,"%04d/%02d/%02d",
      	st.wYear,
      	st.wMonth,
        st.wDay);
      TFont *bigfont = new TFont("Arial",2*cheight,0,0,0,FW_BOLD);
    	writexy(dc,cwidth*2,yy,bigfont,temp,BLUE,-1,LEFT,CENTER,1,BLACK);
      delete bigfont;
    }
    xx = vr.Width();
  	plot(dc,0,yy);
    plot(dc,xx,yy,LIGHTBLUE,1);
  }

  xx = cwidth/2;
  viewport(dc,cr,1);
  border(dc,vr.InflatedBy(xx,xx),0,true,xx,0);
  // Write the hours down the left-hand side
  viewport(dc,vr.OffsetBy(-lmargin,0),0);
  xx = lmargin - cwidth;
  //Debug("h_offset:%d",h_offset);
  for(int h=0;h> -(lines_per_screen+1/lph);h--) {
  	int j = h+hourrecord->hour-h_offset;
    // Do modulo lines_per_screen
    while(j<0)
    	j += lines_per_screen;
		sprintf(temp,"%02d",j);
    int yy = XformHour(lph*(h-1)+chunk0,hsf);
    writexy(dc,xx,yy,font,temp,WHITE,-1,RIGHT,CENTER,2,BLACK);
    // Write minutes
    for(int i = 1;i<lph;i++) {
    	sprintf(temp,":%02d",i*60/lph);
      yy = XformHour(lph*(h-1)+i+chunk0,hsf);
      if(yy > vr.bottom)
      	break;
    	writexy(dc,xx,yy,font_small,temp,BLACK,-1,RIGHT,CENTER);
    }
  }
  delete font_small;
  //
  // Create a rotated font
  // Get size from system font
  TFont *rt = new TFont("Arial",cheight,0,900,0,FW_BOLD);
  writexy(dc,cwidth,XformHour(-12,hsf),rt,"Hours",WHITE,-1,0,1,2,BLACK);
  delete rt;
  // Write the minutes at the bottom
  viewport(dc,bm,0);
  //
  // Put a label every 5 minutes if running at 1 line per hour
  if(lph <= 1) {
  	for(int i=0;i<=3600/lph;i += 300) {
  		sprintf(temp,"%d",i/60);
    	writexy(dc,xsec_sf*i,cwidth,font,temp,WHITE,-1,CENTER,TOP,2,BLACK);
  	}
  }
  // Put a label every minute if running faster
  else if(lph < 30){
  	for(int i=0;i<=3600/lph;i += 60) {
  		sprintf(temp,"%d",i/60);
    	writexy(dc,xsec_sf*i,cheight/4,0,temp,WHITE,-1,CENTER,TOP,2,BLACK);
   	}
  }
  // Add labels for seconds if running even faster
  else {
  	for(int i=0;i<=3600/lph;i += 10) {
  		sprintf(temp,"%02d:%02d",i/60,i%60);
    	writexy(dc,xsec_sf*i,cheight/4,0,temp,WHITE,-1,CENTER,TOP,2,BLACK);
   	}
  }
  char *label = (lph < 30) ? "Minutes" : "Minutes:Seconds";
  writexy(dc,xsec_sf*1800/lph,cheight*1.25,font,label,WHITE,-1,CENTER,TOP,2,BLACK);
  viewport(dc,vr,0);
  if(profile->helicorder_filter) {
  	if(profile->EnableHighPass && profile->EnableLowPass) {
    	sprintf(temp,"Sample rate: %0.4g  Decimate factor: %d  Gain: %g  Band pass cutoff periods: %g s %g s",
  			samprate,profile->decimate,profile->gain,profile->LowPassPeriod,profile->HighPassPeriod);
    }
    else if(profile->EnableLowPass) {
    	sprintf(temp,"Sample rate: %0.4g  Decimate factor: %d  Gain: %g  Low pass cutoff period: %g s",
  			samprate,profile->decimate,profile->gain,profile->LowPassPeriod);
    }
    else if(profile->EnableHighPass) {
    	sprintf(temp,"Sample rate: %0.4g  Decimate factor: %d  Gain: %g  High pass cutoff period: %g s",
  			samprate,profile->decimate,profile->gain,profile->HighPassPeriod);
    }
  } else {
  		sprintf(temp,"Sample rate: %0.4g  Decimate factor: %d  Gain: %g  No filter",
  			samprate,profile->decimate,profile->gain);
  }
  writexy(dc,6*cwidth,vr.Height()-cheight,font,temp,RED,-1,LEFT,CENTER);
  //sprintf(temp,"Station: %s/%s Latitude: %g  Longitude: %g  Elevation: %g",
  //	profile->station_code,profile->station_name,profile->latitude,
  //  profile->longitude,profile->elevation);
  //writexy(dc,6*cwidth,2*cheight,font,temp,RED,-1,0,1);
  delete font;
  viewport(dc,cr,0);
  return lmargin;
}


//---------------------------
// DrawTraces
// Drawing is done in a rectangle which is 24 hours high and 1 hour wide
//
// Have found out that polyline can't handle more than 4000 or so
// points so do this with plot(x,y) or dc.LineTo(x,y).
// 2004.11.22: Assume each hour record is full and compute a "local" sample
// rate based on this.
void Client::DrawTraces(TDC &dc,TRect &cr)
{
	int last_chunk = 0;
  int chunk0 = 0;
  if(samprate == 0)
  	return;
  if(hourrecord == 0)
  	return;
  int lph = profile->lines_per_hour;
  // How many samples per chunk?
  int chunk;
	// RecordArrayIterator nextRecord(records);
  int h;	//  keeps track of hour
  long h0 = hourrecord->hour0;

  TRect vr = GetVR(dc,cr);
  viewport(dc,cr,1);
  viewport(dc,vr,0);
  //float xsf = GetXsf(dc,cr);
  float ysf = GetYsf(dc,cr);
  float hsf = GetHsf(dc,cr);
  HourRecord *hr;
  // See what chunk we are on
  int spc = samples_per_hour/lph;
 	//chunk0 = lph-hourrecord->npts/spc;
  // It seems chunk0 should always be equal to one!
  chunk0 = 1;
  //for(h=0;h>-(lines_per_screen-1)/lph;h--) {
  for(h=0;h>-lines_per_screen-1/lph;h--) {
  //for(h=0;h>-lines_per_screen-1/lph;h--) {
  	dc.BeginPath();
  	int j = h-h_offset;
  	// For current record, it isn't saved yet so use it out of memory
    //Take out at recommendation of Roger
  	if(j == 0) {
      // Current record isn't filtered.  Do it now.
    	hr = hourrecord;
      hr->filter();
    }
    else
    if(j > 0) {
    	continue;
    }
    else
    	hr = new HourRecord(h0+j);
  	int n = hr->npts;
    if(n == -1)
    	continue;
    float d;
   	viewport(dc,vr,1);
    float  xsf = (float)(vr.Width())/((float)spc);
  	if(n) {
      //samples_per_hour = 3600*samprate;
      //int samples_per_hour = 3600*hr->GetSPS();
      // Set samples/hour based on number of points assuming
      // there is exactly one hour of data. But don't do this for
      // hour zero. We are still collecting data.
      if(j == 0)
      	spc = samples_per_hour/lph;
      else
      	spc = n/lph;
      chunk0 = 1;
			// Set xsf for this hour
      xsf = (float)(vr.Width())/((float)spc);
      hr->SetXSF(xsf);
    	chunk = 0;
      d = hr->floatdata[0];
      // Compute a "local" sample rate for this one trace based on the
      // fact that there should be one hour of data except for the first trace
      float lxsf = xsf;
      //float lxsf = hr->GetXSF();
      //if(j != 0)
      //	float lxsf = (float)(lph*vr.Width())/((float)hr->npts);
 			TPoint p = Xform(0,-d,lph*(h-1)+chunk+chunk0,lxsf,ysf,hsf);
    	TPen pen(colors[BLACK],1);
    	dc.SelectObject(pen);
      dc.MoveTo(p);
      int dec = profile->decimate/lph;
      if(dec < 1)
      	dec = 1;
      //float depmen = hr->depmen;
			for(int i=0;i<hr->npts;i += dec) {
      	if(i >= hr->npts) break;
 				d = hr->floatdata[i] /* - depmen */;
      	// What chunk are we working on
      	chunk = i/spc;
 				TPoint p = Xform((i%spc),-d,lph*(h-1)+chunk+chunk0,lxsf,ysf,hsf);
      	if(chunk != last_chunk) {
      		last_chunk = chunk;
        	dc.MoveTo(p);
      	}
   			dc.LineTo(p);
   			//dc.SetPixel(p,0);
      }
		}
    else
    {
    	TPen pen(colors[RED],1);
    	dc.SelectObject(pen);
      for(chunk = 0;chunk < lph;chunk++) {
      	TPoint p = Xform(0,0,lph*(h-1)+chunk+chunk0,xsf,ysf,hsf);
        dc.MoveTo(p);
      	p = Xform(spc,0,lph*(h-1)+chunk+chunk0,xsf,ysf,hsf);
        dc.LineTo(p);
        //dc.SetPixel(p,0);
      }
    }
    dc.EndPath();
    dc.StrokePath();
    if(j != 0)
   		delete hr;
  }
  if(HourRecord::ftp_instance) {
    delete HourRecord::ftp_instance;
    HourRecord::ftp_instance = 0;
  }
}

//---------------------------
// DrawNew Data which is stored in an HourRecord
// Drawing is done in a rectangle which is 24 hours high and 1 hour wide
//
// 2009.07.21: Change from input of dc to input of dc and memory_dc so
// that we can write into the memory dc at the same time as writing to the
// screen
void Client::DrawNewData(TDC &dc,TMemoryDC &memory_dc)
{
  if(samprate == 0)
  	return;
	// The first record tells us where we are within an hour so we
  // know how to paint the records.
  static int last_chunk;		// Keeps track of which chunk we are in so
  													// we know when to start new chunk
  static TPoint last_point = TPoint(0,0);
	//  hour coordinate is current hour
  int lph = profile->lines_per_hour;
  // Current time in current record
  HourRecord *hr = hourrecord;
  TRect cr = GetClientRect();
  float xsf = GetXsf(dc,cr);
  float ysf = GetYsf(dc,cr);
  float hsf = GetHsf(dc,cr);
  // How many samples per chunk?
  int spc = samples_per_hour/lph;
  int chunk;

  int n = hr->npts - hr->last_drawn;
  if(n < 0)
  	n = hr->npts;
  float d;
  TRect vr = GetVR(dc,cr);
  TPoint p;
 	//int chunk0 = lph-hourrecord->npts/spc;
  int chunk0 = 1;
  chunk = hr->last_drawn/spc;
  if(chunk0 <= 0) {
  	// chunk0 cannot be 0 or negative.  If it is, it means we are past the end.
    // This is okay as far as data is concerned since our sample rate is an estimate
    // However, we can't let it display like this
    //MessageBeep(-1);
		last_chunk = chunk0;
    return;
  }

  if(n) {
  	//Debug("n:%d",n);
  	int j = 0;
    viewport(dc,vr,1);
    viewport(memory_dc,vr,1);
    //chunk = (hr->sample0)/spc;
    TPen pen(colors[BLACK],1);
    dc.SelectObject(pen);
    memory_dc.SelectObject(pen);
    //if(last_point == TPoint(0,0)) {
    d = hr->floatdata[hr->last_drawn];
 		last_point = Xform(((hr->last_drawn)%spc),-d,-lph*(1-h_offset)+chunk+chunk0,xsf,ysf,hsf);
    //}
    dc.MoveTo(last_point);
    memory_dc.MoveTo(last_point);

		for(int i=hr->last_drawn;i<hr->npts;i++,j++) {
			float d = hr->floatdata[i];
 			p = Xform((i%spc),-d,-lph*(1-h_offset)+chunk+chunk0,xsf,ysf,hsf);
      // What chunk are we working on
      chunk = i/spc;
      if(chunk != last_chunk) {
      	last_chunk = chunk;
        dc.MoveTo(p);
        memory_dc.MoveTo(p);
      }
      if(i == hr->last_drawn) {
        dc.MoveTo(p);
        memory_dc.MoveTo(p);
      }
   		dc.LineTo(p);
   		memory_dc.LineTo(p);
      //dc.SetPixel(p,0);
		}
  	hr->last_drawn = hr->npts-1;
    //last_point = p;
  }
  last_chunk = chunk;
}
//---------------------------
// DrawNew Data which is stored in an HourRecord
// Drawing is done in a rectangle which is 24 hours high and 1 hour wide
//
#if 0
void Client::DrawNewData(TDC &dc)
{
  if(samprate == 0)
  	return;
	// The first record tells us where we are within an hour so we
  // know how to paint the records.
  static int last_chunk;		// Keeps track of which chunk we are in so
  													// we know when to start new chunk
  static TPoint last_point = TPoint(0,0);
	//  hour coordinate is current hour
  int lph = profile->lines_per_hour;
  // Current time in current record
  HourRecord *hr = hourrecord;
  TRect cr = GetClientRect();
  float xsf = GetXsf(dc,cr);
  float ysf = GetYsf(dc,cr);
  float hsf = GetHsf(dc,cr);
  // How many samples per chunk?
  int spc = samples_per_hour/lph;
  int chunk;

  int n = hr->npts - hr->last_drawn;
  if(n < 0)
  	n = hr->npts;
  float d;
  TRect vr = GetVR(dc,cr);
  TPoint p;
 	//int chunk0 = lph-hourrecord->npts/spc;
  int chunk0 = 1;
  chunk = hr->last_drawn/spc;
  if(chunk0 <= 0) {
  	// chunk0 cannot be 0 or negative.  If it is, it means we are past the end.
    // This is okay as far as data is concerned since our sample rate is an estimate
    // However, we can't let it display like this
    //MessageBeep(-1);
		last_chunk = chunk0;
    return;
  }

  if(n) {
  	int j = 0;
    viewport(dc,vr,1);
    //chunk = (hr->sample0)/spc;
    TPen pen(colors[BLACK],1);
    dc.SelectObject(pen);
    //if(last_point == TPoint(0,0)) {
    d = hr->floatdata[hr->last_drawn];
 		last_point = Xform(((hr->last_drawn)%spc),-d,-lph*(1-h_offset)+chunk+chunk0,xsf,ysf,hsf);
    //}
    dc.MoveTo(last_point);

		for(int i=hr->last_drawn;i<hr->npts;i++,j++) {
			float d = hr->floatdata[i];
 			p = Xform((i%spc),-d,-lph*(1-h_offset)+chunk+chunk0,xsf,ysf,hsf);
      // What chunk are we working on
      chunk = i/spc;
      if(chunk != last_chunk) {
      	last_chunk = chunk;
        dc.MoveTo(p);
      }
      if(i == hr->last_drawn) {
        dc.MoveTo(p);
      }
   		dc.LineTo(p);
      //dc.SetPixel(p,0);
		}
  	hr->last_drawn = hr->npts-1;
    //last_point = p;
  }
  last_chunk = chunk;
}

#endif
void Client::writetime(SYSTEMTIME utc, TDC &dc)
{
	TRect cr = GetClientRect();
  TRect vr = GetVR(dc,cr);
  TRect bm = TRect(vr.Left(),vr.Bottom(),cr.Right(),cr.Bottom());
  viewport(dc,bm,1);
  char temp[32];
  rect(dc,time_w,WHITE);
  sprintf(temp,"%02d:%02d:%02d",utc.wHour,utc.wMinute,utc.wSecond);
  TFont *font_small = new TFont("MS Sans Serif",7);
  writexy(dc,time_w.TopLeft(),font_small,temp,BLACK,-1,0,2);
  delete font_small;
  viewport(dc,vr,1);
}

bool Client::CanClose()
{
	if(hourrecord) {
  	Debug("CanClose");
  	// Don't save if running as a client. Instead, fetch data from
    // server next time we start.
  	if(!profile->tcp_client && !profile->ftp_download && !profile->passive) {
 			if(hourrecord->save(true) == false) {
   			// Path doesn't exist.  Create it;
      	hourrecord->createPath();
      	hourrecord->save(true);
      }
    }
  }
  Child* curChild = (Child *)GetActiveMDIChild();
  if(curChild) {
  	curChild->Destroy();
    delete curChild;
  }
  return canclose;
}
// *****************
// loadLogPalette()
//
// Load palette based on hue_file
// Returns pointer to logpalette
// *****************
LOGPALETTE *loadLogPalette()
{
  // Default logical palette
  static WORD dlp[16][3] = {
  {0,0,0},       // Black
  {0,0,192},     // Blue
  {0,192,0},     // Green
  {0,128,128},   // Cyan
  {128,0,0},     // Red
  {128,0,128},   // Magenta
  {128,128,0},   // Brown
  //{192,192,192}, // Light gray
  // 2009.07.12: Switch light gray and dark gray to make PNG come
  // out okay. I don't understand this.
  {128,128,128}, // Light gray
  //{128,128,128}, // Dark Gray
  {192,192,192}, // Dark Gray
  {0,0,255},     // Light Blue
  {0,255,0},     // Light Green
  {0,255,255},   // Light Cyan
  {255,0,0},     // Light Red
  {255,0,255},   // Light Magenta
  {255,255,0},   // Yellow
  {255,255,255} // White
  };

  WORD n = 16;	// Size of default logical palette
  									// It is really 20 but we just use 16
  //WORD NumDLP = 0;
  // 1999.10.07: Change from 14 to 15 to completely fill palette with
  // 256 colors
	LOGPALETTE *MyLogPalette = (LOGPALETTE*)new char[sizeof(LOGPALETTE) +
			sizeof(PALETTEENTRY) * n];
	MyLogPalette->palVersion = 0x300;
	MyLogPalette->palNumEntries = n;
  int j;
  for(int i = 0; i<n;i++) {
  	j = (WORD)(i);
  	MyLogPalette->palPalEntry[j].peRed    = dlp[i][0];
    MyLogPalette->palPalEntry[j].peGreen  = dlp[i][1];
    MyLogPalette->palPalEntry[j].peBlue   = dlp[i][2];
    MyLogPalette->palPalEntry[j].peFlags  = PC_RESERVED;
	}
	return MyLogPalette;
}

void Client::CmWriteBitMap() {
  TAS1* theApp = TYPESAFE_DOWNCAST(GetApplication(), TAS1);

  theApp->FileData.Flags = OFN_NOCHANGEDIR | OFN_EXPLORER | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
  theApp->FileData.SetFilter("PNG files (*.png)|*.png");
  theApp->FileData.DefExt = "png";
  if (TFileOpenDialog(this,theApp->FileData).Execute() == IDOK) {
  	UpdateWindow();
    WriteBitMap(this,theApp->FileData.FileName);
  }
}



void WriteBitMap(TWindow *parent, char *filename)
{
  // Use this to capture screen image
  TRect cr = parent->GetClientRect();

	TDC *cdc = new TClientDC(*parent);
  TMemoryDC *cdc_mem = new TMemoryDC(*cdc);
	// Create device dependent bitmap
	// cr might have negative origin
	//cr.left = 0;
	//cr.top = 0;
	// Round right-hand since it might be too big
	//cr.right = (int)(10.0*((float)cr.right/10.0));
  TRect cr1 = cr.InflatedBy(-10,0);
	TBitmap *cbm_mem = new TBitmap(*cdc,cr1.Width(),cr1.Height());
  // Connect it to memory DC
  cdc_mem->SelectObject(*cbm_mem);
  // Copy bits from DC to memory DC
  cdc_mem->BitBlt(cr,*cdc,TPoint(0,0),SRCCOPY);
  TPalette *palette;
 	LOGPALETTE *log = loadLogPalette();
  //int cc = bits_pixel;
 	if(log)
 	{
		palette = new TPalette(log);
   	delete log;
    cdc->SelectObject(*palette);
    cdc->RealizePalette();
  	// Create a DIB
  	TDib *cdib = new TDib(*cbm_mem,palette);
  	static int bits = cdib->BitsPixel();
  	static TSize size = cdib->Size();
  	//cdib->WriteFile(filename);
    writepng(cdib,palette,filename);
    cdc->RestoreObjects();           //Roger tried
   	cdc_mem->RestoreObjects();      //Roger tried
  	delete palette;
  	delete cdib;
  }
  delete cbm_mem;
  delete cdc_mem;
  delete cdc;
}


//--------------------------------------------------------
// Client
// ~~~~~~~~~~~
// Menu File Close command
//
void Client::CmFileClose()
{
  Child* curChild = (Child *)GetActiveMDIChild();

  if (curChild) {
    curChild->CloseWindow();
  }
}

//--------------------------------------------------------
// Client
// ~~~~~~~~~~
// Menu File Print command
//
void Client::CmFilePrint()
{
  // Create Printer object if not already created.
  //
  TAS1* theApp = TYPESAFE_DOWNCAST(GetApplication(), TAS1);
  if (theApp) {
    if (!theApp->Printer)
      theApp->Printer = new TPrinter(theApp);
    //TAPointer<char> docName = new char[_MAX_PATH];

    //GetActiveMDIChild()->GetWindowText(docName, _MAX_PATH);
    //this->GetWindowText(docName, _MAX_PATH);
		/********************/
  	if (theApp->Printer) {
    	TWindowPrintout printout("AmaSeis Helicorder", this);
    	//
    	// Using OWL's banding support under Win95 _when_ the EXE's marked 4.0
    	// (under Options|Project|Linker|General) will cause the print to fail.
    	//
    	if(!TSystem::IsWin95())
      	printout.SetBanding(false);
    	theApp->Printer->Print(this, printout, true);
  	}
  }
}

//--------------------------------------------------------
// Client
// ~~~~~~~~~~
// Menu File Print Setup command
//
void Client::CmFilePrintSetup()
{
  TAS1* theApp = TYPESAFE_DOWNCAST(GetApplication(), TAS1);
  if (theApp) {
    if (!theApp->Printer)
      //theApp->Printer = new TPrinter(theApp);
      theApp->Printer = new TPrinter(GetApplication());

    // Bring up the Print Setup dialog.
    //
    // The following line produces a:
    // "Member identifier expected" message at compile time.
    // Don't get this error in a clean new AppExpert
    // application.  Don't know what I did recently
    // to produce it (2000.04.28).  In my RunScore application, I have
    // exactly the same thing and it doesn't produce a compile-time error
    // 2004.12.30: Roger sparks found how to solve the problem.  Took
    // include for WinHelp out of as1.h and just put in files that needed it.
    // 2007.02.19: Found the REAL problem. One identifier was called Setup.
    // Got rid of it. Changed it to "Getting Startee" and problem went away
    theApp->Printer->Setup(this);
  }
}


//--------------------------------------------------------
// Client
// ~~~~~~~~~~
// Menu enabler used by Print, Print Setup and Print Preview.
//
void Client::CmPrintEnable(TCommandEnabler& tce)
{
  //if (GetActiveMDIChild()) {
  {
    TAS1* theApp = TYPESAFE_DOWNCAST(GetApplication(), TAS1);
    if (theApp) {
      // If we have a Printer already created just test if all is okay.
      // Otherwise create a Printer object and make sure the printer really
      // exists and then delete the Printer object.
      //
      if (!theApp->Printer) {
        theApp->Printer = new TPrinter(theApp);
        tce.Enable(!theApp->Printer->GetSetup().Error);
      }
      else
        tce.Enable(!theApp->Printer->GetSetup().Error);
    }
  }
  //else
  //  tce.Enable(false);
}



void Client::CmStation()
{
  // INSERT>> Your code here.
  if (Station(this).Execute() == IDOK) {
  	profile->write();
    Invalidate();
  }
}

void Client::CmShowData()
{
	showdata->ShowWindow(SW_SHOW);
}
void Client::CmSetZero()
{
	SetZero(this).Execute();
}

void Client::CmNow()
{
	h_offset = 0;
	scrollbar->SetPosition(max_scroll);
  Invalidate();
}

void Client::CmContinuous()
{
  // INSERT>> Your code here.
  int size;
  if (ContDialog(this).Execute() == IDOK) {
  	profile->write();
    // update scrollbar

   	max_scroll = lines_per_screen*(profile->DaysToRetainRecords-1);
 		scrollbar->SetRange(0,max_scroll,true);
  	scrollbar->GetRange(size,max_scroll);
		scrollbar->SetPosition(max_scroll);
  	scrollbar->SetPageMagnitude(4);
    max_scroll = scrollbar->GetPosition();
    scrollbar->ShowWindow(SW_HIDE);
    Invalidate();
 }
}


void Client::SetOffset(int offset) {
	h_offset = offset;
  if(h_offset < 0)
 		h_offset = 0;
	if(h_offset > profile->DaysToRetainRecords*24) {
    profile->DaysToRetainRecords = h_offset/24;
 		max_scroll = lines_per_screen*(profile->DaysToRetainRecords-1);
    int size;
 		scrollbar->SetRange(0,max_scroll,true);
  	scrollbar->GetRange(size,max_scroll);
		scrollbar->SetPosition(0);
   	//h_offset = (profile->DaysToRetainRecords-1)*24;
  } else
   	scrollbar->SetPosition(max_scroll-h_offset);
  Invalidate();
}

void Client::CmDateTime()
{
  // Get number of hours and convert to a SYSTEMTIME
  int h0 = hourrecord->hour0;
	SYSTEMTIME st = HourRecord::hourSec2st(h0-h_offset,0);
  if (DateDialog(this,st).Execute() == IDOK) {
    long h = HourRecord::st2hour(st);
    SetOffset(h0-h);
    #if 0
    h_offset = h0-h;
    if(h_offset < 0)
    	h_offset = 0;
		if(h_offset > profile->DaysToRetainRecords*24) {
      profile->DaysToRetainRecords = h_offset/24;
 			max_scroll = lines_per_screen*(profile->DaysToRetainRecords-1);
      int size;
 			scrollbar->SetRange(0,max_scroll,true);
  		scrollbar->GetRange(size,max_scroll);
			scrollbar->SetPosition(0);
    	//h_offset = (profile->DaysToRetainRecords-1)*24;
    } else
    	scrollbar->SetPosition(max_scroll-h_offset);
    Invalidate();
    #endif
  }
}
void Client::CmDevice()
{
  DeviceDialog *dd = new DeviceDialog(this);
  if(dd) {
  	int d = dd->Execute();
    if(d == IDOK)
    	profile->write();
    delete dd;
  }
}
void Client::CmComPort()
{
  ComPortDialog(this).Execute();
}
void Client::CmCalibration()
{
	CalibrationDialog(this).Execute();
}
long Client::EvMouseWheel(UINT wParam, LONG lParam)
{
	//DWORD fwKeys = LOWORD(wParam);    // key flags
	short zDelta = (short) HIWORD(wParam);    // wheel rotation
  zDelta /= 120;		// zDelta is in multiples of 120
  if(zDelta > 0)
  	h_offset--;
  else if (zDelta <0)
  	h_offset++;
  if(h_offset < 0 )
  	h_offset = 0;
  else {
  	last_p = max_scroll - h_offset;
 		scrollbar->SetPosition(last_p);
  	Invalidate();
  }
}

void Client::CmForwardBackward()
{
	delete forward_backward;
 	forward_backward = new ForwardBackward(this);
 	forward_backward->Create();
 	forward_backward->ShowWindow(SW_SHOW);
}

