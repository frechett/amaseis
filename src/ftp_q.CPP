#include <owl/owlpch.h>
#include <owl/listbox.h>
#include <classlib/filename.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <fstream.h>
#include "ftp_u.h"
#include "profile.h"
#include "uplodapp.h"
#include "uplddlgc.h"                        // Definition of client class.

extern Profile *profile;
extern TSDIDecFrame* frame;

//FtpThread::FtpThread(char *url, char *username, char *password,
//	char *path, char *filename)
FtpUpload::FtpUpload(TUploadDlgClient *activity) : ftp()
{
	this->activity = activity;
  URL = strdup(profile->ftp_addr);
  Username = strdup(profile->ftp_user);
  Password = strdup(profile->ftp_pw);
  Path = strdup(profile->ftp_path);
}
FtpUpload::~FtpUpload()
{
	free(URL);
  free(Username);
  free(Password);
  free(Path);
}

// Log into the FTP site
bool FtpUpload::Login()
{
	int ret;
  char buf[64];
  // Open up ftp
  ret = DoOpen(URL);
  if(ret) {
  	activity->SetText("FTP Problem connection to %s",URL);
   	//MdiClient->MessageBox(buf);
  	DoClose();
    return true;
  }
  ret = DoLogin(Username,Password);
  Debug("FTP opened. Logging in with name: %s",Username);
  if(ret) {
  	Debug("FTP problem logging in as user \"%s\" and pw \"%s\"",
      Username,Password);
  	//MdiClient->MessageBox(buf);
  	DoClose();
    return true;
  }
  if(DoCD(Path)) {
  	// Problem. Couldn't change path.
    Debug("Could not move to path: %s",Path);
    return true;
  }
  return false;
}
// Convert a SYSTEMTIME to the number of hours since 1601
static long st2hour(SYSTEMTIME st)
{
	FILETIME ft;
  LARGE_INTEGER li;
	SystemTimeToFileTime(&st,&ft);
  memcpy(&li,&ft,sizeof(li));
  // Convert to seconds
  LONGLONG longint= li.QuadPart/10000000L;
  // Convert to hours
  longint = longint/3600;
  long hour = (long)longint;
  return hour;
}
// Convert a long hour to a SYSTEMTIME
static SYSTEMTIME hour2st(long hour)
{
	FILETIME ft;
  LARGE_INTEGER li;
  SYSTEMTIME st;
  // Set low 4 bytes
  memcpy(&ft,&hour,sizeof(hour));
  // Zero out the high bytes
  ft.dwHighDateTime=0;
  // Copy into a large integer
  memcpy(&li,&ft,sizeof(li));
  // Convert to seconds
  li.QuadPart = li.QuadPart*3600;
  // Covert to 100 ns
  li.QuadPart = li.QuadPart*10000000L;
  // copy back to a file time
  memcpy(&ft,&li,sizeof(li));
  FileTimeToSystemTime(&ft,&st);
  return st;
}



// Dummy Run() function. We don't use it since we don't call Start()
// to spin off a thread.
//int FtpThread::myRun()
//{
//	return 1;
//}
// Upload on hour file
bool FtpUpload::Upload(char *filespec)
{
	char buf[64];
  Debug("Upload: %s",filespec);
  int year, month, day, hour;
  sscanf(filespec,"%d\\%d\\%d\\%d.%*c",&year,&month,&day,&hour);
	bool ret = true;
 	// Create file name

 	sprintf(buf,"%s.%04d.%02d.%02d.%02d.%c",
  	profile->station_code,year,month,day,hour,profile->component);
 	char *b = buf+3;
  activity->SetText("Uploading hour file: %s",buf);
  DoBinary();
  FILE *fp = fopen(filespec,"rb");
  int size = filelength(fileno(fp));
  if(size == 0) {
   	return true;
  }
  char *data = new char[size];
  fread(data,size,1,fp);
  fclose(fp);
	if(DoPutData(b,data,size)) {
   	activity->SetText("FTP problem sending data to upload hour file");
    delete data;
    return ret;
  }
  return false;
}

// Put station information on the FTP site
// Also clean out all minutes files
// Returns true if there is a problem
bool FtpUpload::PutStationInfo()
{
	int ret = true;
	char buf[1024];
  // Move to directory for this station
	DoCD(Path);
  Debug("Upload station information");
  // Create data to upload:
  DoAscii();
  sprintf(buf,"Latitude=%lg\r\n",profile->latitude);
  sprintf(buf+strlen(buf),"Longitude=%lg\n",profile->longitude);
  sprintf(buf+strlen(buf),"Elevation=%g\n",profile->elevation);
  sprintf(buf+strlen(buf),"Component=%c\n",profile->component);
  sprintf(buf+strlen(buf),"StationName=%s\n",profile->station_name);
  sprintf(buf+strlen(buf),"StationCode=%s\n",profile->station_code);
  sprintf(buf+strlen(buf),"Gain=%g\n",profile->gain);
  char name[24];
  sprintf(name,"%s.Station.ini",profile->station_code);
  if(DoPutData(name,buf,strlen(buf))) {
  	activity->SetText("Problem sending station information");
  	DoClose();
  	return ret;
  }
  ret = false;
  // Clean out minute files
  //for(int i = 1;i<=59;i++) {
  //	sprintf(buf,"%02d.minute",i);
  //  DoDelete(buf);
  //}
  return false;
}
// Put sample rate on FTP server
// Returns true if there is a problem
bool FtpUpload::PutSPS(float sps)
{
	char buf[64];
	bool ret = true;
  // Move to directory for this station
	DoCD("/");
	DoCD(Path);
  Debug("Upload SPS information");
  // Create data to upload:
  DoAscii();
  sprintf(buf,"SampleRate=%g\r\n",sps);
  activity->SetText("FTP upload Sample rate: %g",sps);
  char name[24];
  sprintf(name,"%s.SampleRate.ini",profile->station_code);
 	if(DoPutData(name,buf,strlen(buf))) {
  	activity->SetText("Problem sending sample rate information");
  	DoClose();
  	return ret;
  }
  return false;
}

ftp::ftp() {
	Connected = 0;
	hListenSocket = INVALID_SOCKET;
	hControlSocket = INVALID_SOCKET;
	hDataSocket = INVALID_SOCKET;
	bSendPort      = 1;
	ReadCommand = 0;
  passive = true;
	bMode=ASCII;

	InitWinsock();

};

ftp::~ftp() {

};


/*
 * DoOpen
 * this function is called when the o,open command is issued.
 * it connects to the requested host, and logs the user in
 *
 * Returns: true if a problem
 */
bool ftp::DoOpen( char *command)
{
	bool ret = false;
   /*
    * do not do anything if we are already connected.
    */
   if( Connected ) {
       //printf("Already connected.  Close connection first.\n");
       //fflush(stdout);
       return true;
   }

   hControlSocket = ConnectToServer(command,"21");
   Sleep(1);

   if( hControlSocket > 0)  {
     //printf("Connected to %s\n",szHost);
/*#if (defined(WIN32) || defined(_WIN32) )

	 sprintf(command,"dftp: Connected to %s ", szHost);

	 SetConsoleTitle(command);  // set console window title
#endif*/

     Connected = 1;         /* we are now connected */
     GetReply();            /* get reply (welcome message) from server */
     //DoLogin((char *)NULL); /* prompt for username and password */
     //DoBinary();            /* default binary mode */
   }
   else
   	ret = true;
	return ret;
}



/*
 * DoLogin
 * Syntax: DoLogin(user,password)
 */
bool ftp::DoLogin( char *User, char *Password)
{
	bool ret = false;
  if( !Connected )
		return true;
  /*
  * send user name & password to server  & get reply message
  */
  sprintf(szBuffer,"USER %s\r\n",User);
  SendControlMsg(szBuffer,strlen(szBuffer));
  GetReply();
  if(Password) {
    sprintf(szBuffer,"PASS %s\r\n",Password);
    SendControlMsg(szBuffer,strlen(szBuffer));
    GetReply();
    //GetLine();
    if(strncmp(szBuffer,"530",3) == 0) {
     	return true;
    }
  }
  else
  	ret = true;

   return ret;
}

/*
 * DoClose
 * closes connection to the ftp server
 */
void ftp::DoClose( void )
{
	Debug("Close FTP");
   if( !Connected  ) {
     printf("Not Connected.\n");
   }
   else {
	   SendControlMsg("quit\r\n",6);
	   GetReply();
	   CloseControlConnection();
	   hControlSocket = -1;
	   SetConsoleTitle("dftp: Connection closed");
	   Connected = 0;
   }
}

/*
 * DoList
 * perform directory listing i.e: ls
 * Returns pointer to first line returned
 */
char * ftp::DoList( char *command)
{
	char *ret;
   if( !Connected ) {
      //printf("Not Connected.\n");
      return 0;
   }


   if( SetupDataConnection()) {
       return 0;
   }

   /*
    * parse command
    */
   if( !strcmp(command,"ls") )  {
       sprintf(szBuffer,"NLST\r\n");
   }
   else if( !strcmp(command,"dir") )
       sprintf(szBuffer,"LIST\r\n");

   else if( !strncmp(command, "ls ",3)) {
       while( *command == ' ') command++;
       sprintf(szBuffer,"LIST %s\r\n",&command[3]);
   }
   /*
    * send command to server and get response
    */
   SendControlMsg(szBuffer,strlen(szBuffer));
   memset(szBuffer,0,FTP_BUFFER_SIZE);
   GetReply();

   /*
    * accept server's connection
    */
   if(!passive) {
   	if(AcceptConnection() < 0) {
      printf("Cannot accept connection.\n");
      return 0;
   	}
   }
   CloseListenSocket();       /* close listening socket */

   /*
    * display directory listing.
    */
   while( ReadDataMsg(szBuffer,FTP_BUFFER_SIZE) > 0) {
       //fflush(stdout);
       //printf(szBuffer);
       //memset(szBuffer,0,FTP_BUFFER_SIZE);
   }
   ret = strdup(szBuffer);
   /*
    * read response
    */
   (void)GetReply();

   return ret;
}

/*
 * DoCD
 * change to another directory on the remote system
 * If fails, returns true;
 */
bool ftp::DoCD( char *command)
{
  if( !Connected ) {
   	//printf("Not Connected.\n");
    return true;
  }


  /*
   * send command to server and read response
   */
  sprintf(szBuffer, "CWD %s\r\n",command);
  SendControlMsg(szBuffer,strlen(szBuffer));
  return (GetReply() != 250);
}
/*
 * DoMKD
 * Make a new directory on remote system
 * If fails, returns true;
 */
bool ftp::DoMKD( char *command)
{
  if( !Connected ) {
   	//printf("Not Connected.\n");
    return true;
  }


  /*
   * send command to server and read response
   */
  sprintf(szBuffer, "MKD %s\r\n",command);
  SendControlMsg(szBuffer,strlen(szBuffer));
  return (GetReply() != 257);
}


/*
 * DoLCD
 * change directory on the local system
 */
void ftp::DoLCD( char *command)
{

   char *dir = &command[3];

   while(*dir && (*dir == ' ' || *dir == '\t') ) dir++;

   /*
    * if dir is not specified, then print the current dir
    */
   if( ! *dir ) {
      dir = getcwd((char *)NULL,256);
      if( !dir)
	perror("getcwd");
      else
	printf("Current directory is: %s\n",dir);
   }
   else {
      if( chdir(dir) < 0)
	perror("chdir");
      else {
	dir = getcwd((char *)NULL,256);
	if( !dir)
	    perror("getcwd");
	else
	printf("Current directory is: %s\n",dir);
      }
   }
}

/*
 * DoLLS
 * perform local directory listing.  winqvt implements this, but
 * this is not supported by many systems.  it's not really needed
 * since we already have '!' command.  you can just do !ls or !dir.
 */
void ftp::DoLLS( char * /*command */ )
{
#if ( !defined(_WIN32) || !defined(WIN32) )
    system("ls");
#else
    system("dir");
#endif
}

/*
 * function to pass commands to the system, ie: dir.
 * this function gets called when '!' is encountered
 */
void DoShellCommand( char *command )
{
  command++;  /* ignore '!' */
#if ( !defined(_WIN32) || !defined(WIN32) )

    system(command);
#else
	if( !command || !*command)
		system("cmd");   /* have to fix this for win95 */
	else                     /* maybe i should use 'start' */
		system(command); /* programatically determine which */
#endif                           /* system we are on, then make the  */
				 /* appropriate call */
}


/*
 * DoBinary
 * set file transfer mode to binary
 */
void ftp::DoBinary()
{
  if( !Connected ) {
      printf("Not Connected.\n");
      return;
  }
   sprintf(szBuffer, "TYPE I\r\n");
   SendControlMsg(szBuffer,strlen(szBuffer));
   GetReply();
   //printf("File transfer modes set to binary.\n");
   bMode = BINARY;
}

/*
 * DoAscii
 * set file transfer mode to ascii text
 */
void ftp::DoAscii()
{
  if( !Connected ) {
      printf("Not Connected.\n");
      return;
  }
   sprintf(szBuffer, "TYPE A\r\n");
   SendControlMsg(szBuffer,strlen(szBuffer));
   GetReply();
   //printf("File transfer modes set to ascii.\n");
   bMode = ASCII;

}

/*
 * DoGet
 * retrieve a file from the remote host.  calls GetFile(..)
 */
void ftp::DoGet( char *command)
{

  if( !Connected ) {
      printf("Not Connected.\n");
      return;
  }
  //(void)strtok(command," ");
  //GetFile(strtok((char *)NULL, " "));
  GetFile(command);
}

/*
 * DoPut
 * send a file to the remote host.  calls PutFile(..)
 */
#if 0
bool ftp::DoPut( char *command )
{
  if( !Connected ) {
      printf("Not Connected.\n");
      return false;
  }
  //(void)strtok(command," ");
  //PutFile(strtok((char *)NULL, " "));
  return PutFile(command);
}
#endif

/*
 * DoRhelp
 * sends a help command to the server.
 */
void ftp::DoRhelp( char *command )
{
  char *szCommand;

  if( !Connected ) {
      printf("Not Connected.\n");
      return;
  }
  (void)strtok(command," ");
  szCommand=strtok((char *)NULL, " ");

  if( szCommand && *szCommand )
     sprintf(szBuffer,"HELP %s\r\n",szCommand);
  else
     sprintf(szBuffer, "HELP\r\n");

  SendControlMsg(szBuffer, strlen(szBuffer));
  GetReply();
}
/*
 * DoDelete
 * Delete file
 */
bool ftp::DoDelete( char *command )
{
  if( !Connected ) {
      printf("Not Connected.\n");
      return true;
  }
  sprintf(szBuffer,"DELE %s\r\n",command);
  SendControlMsg(szBuffer, strlen(szBuffer));
  return (GetReply() != 250);
}

/*
 * DoRMD
 * Remove directory
 * If fails, returns true;
 */
bool ftp::DoRMD( char *command)
{
  if( !Connected ) {
   	//printf("Not Connected.\n");
    return true;
  }


  /*
   * send command to server and read response
   */
  sprintf(szBuffer, "RMD %s\r\n",command);
  SendControlMsg(szBuffer,strlen(szBuffer));
  return (GetReply() != 250);
}

/*
 * retrieves the current directory on the remote host
 */
void ftp::DoPWD()
{

  if( !Connected ) {
      printf("Not Connected.\n");
      return;
  }
  sprintf(szBuffer, "PWD\r\n");
  SendControlMsg(szBuffer,strlen(szBuffer));
  GetReply();
}

/*
 * Sets passive mode
 */
void ftp::DoPASV()
{

  if( !Connected ) {
      printf("Not Connected.\n");
      return;
  }
  sprintf(szBuffer, "PASV\r\n");
  SendControlMsg(szBuffer,strlen(szBuffer));
  GetReply();
  //GetLine();
  //puts(szBuffer);
  // Extract server host address and port
  int h1, h2, h3, h4, p1, p2;
  char *p = strchr(szBuffer,'(');
  if(p) {
  	if(6 == sscanf(p,"(%d,%d,%d,%d,%d,%d)",&h1,&h2,&h3,&h4,&p1,&p2)) {
    	sprintf(szServerAddr,"%d.%d.%d.%d",h1,h2,h3,h4);
      sprintf(szServerPort,"%d",p1*256+p2);
  		//printf("%d %d %d %d %d %d \n",h1,h2,h3,h4,p1,p2);
    	printf("Addr: %s, Port: %s\n",szServerAddr,szServerPort);
  		passive = true;
    }
  }
}

int ftp::GetListenSocket()
{
    int sockfd, flag=1,len;
    struct sockaddr_in  serv_addr, TempAddr;
    char *port,*ipaddr;
    char szBuffer[64]={0};

    /*
     * Open a TCP socket (an Internet stream socket).
     */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		return INVALID_SOCKET;
    }


    /*
     * Fill in structure fields for binding
     */
   if( bSendPort ) {
       bzero((char *) &serv_addr, sizeof(serv_addr));
       serv_addr.sin_family      = AF_INET;
       serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
       serv_addr.sin_port        = htons(0); /* let system choose */
   }
   else {
     /* reuse the control socket then */
      if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,
         (char *)&flag,sizeof(flag)) < 0) {
	 	perror("setsockopt");
     	closesocket(sockfd);
	 	return INVALID_SOCKET;
      }
   }

    /*
     * bind the address to the socket
     */
    if (bind(sockfd,(struct sockaddr *)&serv_addr,
	     sizeof(serv_addr)) < 0) {
	perror("bind");
    closesocket(sockfd);
	return INVALID_SOCKET;
    }

    len = sizeof(serv_addr);
    if(getsockname(sockfd,
		   (struct sockaddr *)&serv_addr,
		   &len)<0) {
       perror("getsockname");
       closesocket(sockfd);
       return INVALID_SOCKET;
    }
    len = sizeof(TempAddr);
    if(getsockname(hControlSocket,
		   (struct sockaddr *)&TempAddr,
		   &len)<0) {
       perror("getsockname");
       closesocket(sockfd);
       return INVALID_SOCKET;

    }
    ipaddr = (char *)&TempAddr.sin_addr;
    port  = (char *)&serv_addr.sin_port;
#define  UC(b)  (((int)b)&0xff)
    sprintf(szBuffer,"PORT %d,%d,%d,%d,%d,%d\r\n",
          UC(ipaddr[0]), UC(ipaddr[1]), UC(ipaddr[2]), UC(ipaddr[3]),
          UC(port[0]), UC(port[1]));
    /*
     * allow ftp server to connect
     * allow only one server
     */
    if( listen(sockfd, 1) < 0) {
	perror("listen");
    closesocket(sockfd);
	return INVALID_SOCKET;
    }
    SendControlMsg(szBuffer,strlen(szBuffer));
    GetReply();
    hListenSocket = sockfd;
    return sockfd;
}



int ftp::AcceptConnection()

{
    struct sockaddr_in cli_addr;
    int clilen = sizeof(cli_addr);
    int sockfd;

    sockfd = accept(hListenSocket, (struct sockaddr *) &cli_addr,
			   &clilen);
    if (sockfd < 0) {
        perror("accept");
	return INVALID_SOCKET;
    }

    hDataSocket = sockfd;
    closesocket(hListenSocket);
    return sockfd;
}


int ftp::ConnectToServer(char *name, char *port)
{
  int s;
  unsigned int portnum;
  struct sockaddr_in server;
  struct hostent *hp;
  while( name && *name == ' ') name++;
  if( !name || ! (*name) )
      return INVALID_SOCKET;
  portnum = atoi(port);
  bzero((char *) &server, sizeof(server));
  if( isdigit(name[0])) {
   server.sin_family      = AF_INET;
   server.sin_addr.s_addr = inet_addr(name);
   server.sin_port        = htons(portnum);
  }
  else{
   if ( (hp = gethostbyname(name)) == NULL)
    {
	  printf("gethostbyname: error code: %d\r\n", WSAGetLastError());
	  return INVALID_SOCKET;

    }
    bcopy(hp->h_addr,(char *) &server.sin_addr,hp->h_length);
    server.sin_family = hp->h_addrtype;
    server.sin_port = htons(portnum);
   }/* else */

  /* create socket */
  if( (s = socket(AF_INET, SOCK_STREAM, 0)) < 1) {
  	printf("socket: error code: %d\r\n", WSAGetLastError());
    return INVALID_SOCKET;

  }

  if (connect(s,(struct sockaddr *)&server, sizeof(server))< 0) {
  	printf("connect: error code: %d\r\n", WSAGetLastError());
    return INVALID_SOCKET;

  }

  setsockopt(s,SOL_SOCKET,SO_LINGER,0,0);
  setsockopt(s,SOL_SOCKET,SO_REUSEADDR,0,0);
  setsockopt(s,SOL_SOCKET,SO_KEEPALIVE,0,0);
  hDataSocket = s;
  return s;
}

void ftp::GetError()
{
	int ret = WSAGetLastError();
  char error[64];
  switch(ret) {
  case WSAECONNREFUSED:
		sprintf(error,"Connection refused");
    break;
  case WSAENOTCONN:
  	sprintf(error,"Not connected");
   	break;
  case WSAETIMEDOUT:
  	sprintf(error,"Timed out"); break;
  case WSAEISCONN:
  	sprintf(error,"Already connected"); break;
  case WSAECONNRESET:
  	sprintf(error,"Connection reset"); break;
  default:
  	sprintf(error,"Receive error: %d",ret);
  }
  //activity->SetText(error);
  MessageBeep(MB_ICONEXCLAMATION);
}
int ftp::ReadControlMsg( char *szBuffer, int len)
{
  int ret = 0;
  if(IsReadyToRead(hControlSocket)) {
  	if( (ret=recv(hControlSocket,szBuffer,len,0)) < 0) {
    	//Debug("Socket problem in recv/ReadControlMsg: %d",ret);
      GetError();
    }
  } else {
  	//Debug("Not ready to read in ReadControlMsg");
  }
  return ret;
}

int ftp::SendControlMsg( char *szBuffer, int len)
{
   if( send(hControlSocket,szBuffer,len,0) <= 0)
       return 0;
   return 1;
}

int ftp::ReadDataMsg( char *szBuffer, int len)
{
  int ret = 0;
  if(IsReadyToRead(hDataSocket)) {
   	if( (ret=recv(hDataSocket,szBuffer,len,0)) < 0) {
  		//Debug("Socket problem in recv/ReadDataMsg: %d",ret);
      GetError();
    }
  } else {
  	//Debug("Not ready to read in ReadDataMsg");
  }
  return ret;
}



int ftp::SendDataMsg( char *szBuffer, int len)

{
   if( send(hDataSocket,szBuffer,len,0) <= 0)
       return 0;
   return 1;
}

void ftp::CloseDataConnection( int hDataSocket )
{
   closesocket(hDataSocket);
   //hDataSocket = INVALID_SOCKET;
}

void ftp::CloseControlConnection( void )
{
	 closesocket(hControlSocket);
     hControlSocket = INVALID_SOCKET;
}


void ftp::CloseListenSocket( void )
{
      closesocket(hListenSocket);
      hListenSocket = INVALID_SOCKET;
}


int ftp::CheckControlMsg( char *szPtr, int len)
{
    return recv(hControlSocket,szPtr,len,MSG_PEEK);
}

int ftp::GetLine()
{
  //int done;
  int iLen, iBuffLen=0;
  char *szPtr = szBuffer, nCode[4]={0},ch=0;
  //while( (iBuffLen < FTP_BUFFER_SIZE) && (CheckControlMsg(&ch,1)  > 0) ){
  while(iBuffLen < FTP_BUFFER_SIZE){
 		//iLen = recv(hControlSocket,&ch,1,0);
    iLen = ReadControlMsg(&ch,1);
		iBuffLen += iLen;
		*szPtr = ch;
		szPtr += iLen;
		if( ch == '\n' )
	    break;    // we have a line: return
  }
  *(szPtr+1) = (char)0;
  strncpy(nCode, szBuffer, 3);
  return atoi(nCode);
}
#if 0
#define MULTILINE_REPLY '-'				// FTP flag for multiline replies
#define EOL_MARKER "\r\n"					// FTP end-of-line marker

int ftp::GetLine()
{
		LPSTR lpEOL;										// End-of-line
		UINT nLimitReplyBytes;					// Length of first reply in the buffer
		PSTR pLastLineCode = "123\0";		// Storage buffer for reply code
		LPSTR lpLastLine;								// Pointer to the last line
		int i;													// General purpose index

 		// lpszFunctionName = "ReadReplyLine";

		nLimitReplyBytes = 0;

		if (*(szBuffer+3) == MULTILINE_REPLY)
			{
				// Get the code from the reply buffer
				for (i = 0; i <3; i++ )
					*(pLastLineCode+i) = *(szBuffer+i);

        // Use a trailing space to look for the last line
				*(pLastLineCode+i) = ' ';

				// Search the buffer for the last line
				if ((lpLastLine = _fstrstr(szBuffer, pLastLineCode)))
					{
						// Okay, be cautious and make sure a CRLF exists
						lpEOL = (LPSTR)_fstrstr(lpLastLine, EOL_MARKER);
						// Note length to read if more than one reply is in the buffer
						nLimitReplyBytes = lpEOL ? (UINT)((lpEOL - szBuffer)+2) : 0;
					}
				else
					nLimitReplyBytes = 0;
			}
		else
			{
				// If the reply is not multiline then find the end of the line.
				lpEOL = (LPSTR)_fstrstr(szBuffer, EOL_MARKER);

				// If an end-of-line marker was not found, read everything
				// (i.e. don't limit the reply size). Otherwise, only read to the
				// end of line marker.
				nLimitReplyBytes = lpEOL ? (UINT)((lpEOL - szBuffer)+2) : 0;
			}
		//return(nLimitReplyBytes);
    return atoi(pLastLineCode);
	}

#endif
int ftp::GetReply()
{
return ReadFTPServerReply(hControlSocket);
#if 0
   int done = 0, iRetCode = 0;
   memset(szBuffer,0,FTP_BUFFER_SIZE);
   while(!done ) {
   		iRetCode = GetLine();
			(void)strtok(szBuffer,"\r\n");
			//puts(szBuffer);
			if( szBuffer[3] != '-' && iRetCode > 0 )
		    done = 1;
			//memset(szBuffer,0,FTP_BUFFER_SIZE);
   }
   return iRetCode;
#endif
}
int ftp::ReadFTPServerReply(SOCKET hControlSocket)
	{
		int iBytesRead;						// Bytes read from the control channel
		int iBufferLength;				// Length of the server reply buffer
		int iEnd;									// Index into the server reply buffer
		int iSpaceRemaining;			// Space remaining in the buffer
		int iReplySize;						// Length of first reply in the buffer

 		//lpszFunctionName = "ReadFTPServerReply";

		// Be cautious and zero fill the buffer
		memset(szBuffer, 0, sizeof(szBuffer));

		// Be tricky and peek ahead to see what's in the buffer.
		if ((iBytesRead = recv(hControlSocket,
					(LPSTR)(szBuffer), sizeof(szBuffer), MSG_PEEK)) > 0)
			{
				if ((iReplySize = ReadReplyLine(szBuffer)) == 0)
					iBufferLength = iSpaceRemaining = sizeof(gsServerReplyText);
				else
					iBufferLength = iSpaceRemaining = iReplySize;
			}
		else
			{
				//MessageBeep(MB_ICONINFORMATION);
				//MessageBox(NULL,
				//			(LPSTR)"Nothing on the control channel to read!",
				//	  	lpszFunctionName, MB_OK|MB_ICONINFORMATION);
				return(999);
			}

		iEnd = 0;	// Start with zero bytes.
		do
			{
				iSpaceRemaining -= iEnd;
				iBytesRead = recv(hControlSocket,
							(LPSTR)(gsServerReplyText+iEnd), iSpaceRemaining, NO_FLAGS);

    		iEnd+=iBytesRead;

    		// Make sure CRLF was not the the last byte pair received.
    		// Otherwise, recv() will wait forever for the next packet.
     		if (*(gsServerReplyText+(iEnd-1)) == LINEFEED)
    			{
						LPSTR lpLineFeed;
						LPSTR lpLastLine;

						// Temporarily replace the real last line marker (LF)
						// with a NULL so _fstrstr doesn't go crazy.
						*(gsServerReplyText+(iEnd-1)) = '\0';

						// See if any other line-feeds exist in the buffer
						// (which means the buffer contains multiple lines).
						lpLineFeed = _fstrrchr(gsServerReplyText, LINEFEED);

            // Restore the line-feed character.
						*(gsServerReplyText+(iEnd-1)) = LINEFEED;

						if (lpLineFeed)
							// If a line-feed was found then the next character is
							// the start of the last line.
							lpLastLine = lpLineFeed+1;

						else
							// Otherwise, the buffer contains only have one line
							lpLastLine = gsServerReplyText;

						// Be cautious and make sure the last line includes a reply code
						if (GetReplyCode(lpLastLine))
							{
								// Test the reply code to see if it indicates more lines
		    				if (*(lpLastLine+3) != MULTILINE_REPLY)
		    					break;	// If not, break out of the loop.
							}
						else
							;	// Whoops, somebody's server is not playing by the rules.
					}
			}
		while (iBytesRead > 0 && iEnd < iBufferLength);

    if (iBytesRead == SOCKET_ERROR)
  		{
				int iWinsockErr = WSAGetLastError();
				wsprintf(szBuffer, "Error %d from the recv() function!!",
							iWinsockErr);
				MessageBeep(MB_ICONSTOP);
				//MessageBox(NULL, szBuffer, lpszFunctionName,
				//			MB_OK|MB_ICONSTOP);
			  // Return 999 to indicate an error has occurred
				return(999);
			}

		gsServerReplyText[iEnd] = '\0';
#ifdef _DEBUG
		MessageBeep(MB_ICONINFORMATION);
	  MessageBox(NULL, (LPSTR)gsServerReplyText, lpszFunctionName,
	        MB_OK|MB_ICONINFORMATION);
#endif
    // Extract the reply code from the server reply and return as an integer
		return(GetReplyCode(gsServerReplyText));
	}

int ftp::GetReplyCode(LPSTR lpszServerReply)
	{
		UINT nCode;																		// Reply code as a number
		char c;																				// Temporary storage

		// lpszFunctionName = "GetReplyCode";

		c = *(lpszServerReply+3);											// Save the character
		*(lpszServerReply+3) = '\0';									// Terminate the code

		nCode = atoi((const char *)lpszServerReply);	// Convert code to number
		*(lpszServerReply+3) = c;											// Restore the character

		return(nCode);																// Return the reply code
	}

int ftp::ReadReplyLine(LPSTR lpszReplyBuffer)
	{
		LPSTR lpEOL;										// End-of-line
		UINT nLimitReplyBytes;					// Length of first reply in the buffer
		PSTR pLastLineCode = "123?";		// Storage buffer for reply code
		LPSTR lpLastLine;								// Pointer to the last line
		int i;													// General purpose index

 		// lpszFunctionName = "ReadReplyLine";

		//nLimitReplyBytes = 0;

		if (*(lpszReplyBuffer+3) == MULTILINE_REPLY)
			{
				// Get the code from the reply buffer
				for (i = 0; i <3; i++ )
					*(pLastLineCode+i) = *(lpszReplyBuffer+i);

        // Use a trailing space to look for the last line
				*(pLastLineCode+i) = ' ';

				// Search the buffer for the last line
				if (0 != (lpLastLine = _fstrstr(lpszReplyBuffer, pLastLineCode)))
					{
						// Okay, be cautious and make sure a CRLF exists
						lpEOL = (LPSTR)_fstrstr(lpLastLine, EOL_MARKER);
						// Note length to read if more than one reply is in the buffer
						nLimitReplyBytes = lpEOL ? (UINT)((lpEOL - lpszReplyBuffer)+2) : 0;
					}
				else
					nLimitReplyBytes = 0;
			}
		else
			{
				// If the reply is not multiline then find the end of the line.
				lpEOL = (LPSTR)_fstrstr(lpszReplyBuffer, EOL_MARKER);

				// If an end-of-line marker was not found, read everything
				// (i.e. don't limit the reply size). Otherwise, only read to the
				// end of line marker.
				nLimitReplyBytes = lpEOL ? (UINT)((lpEOL - lpszReplyBuffer)+2) : 0;
			}
		return(nLimitReplyBytes);
	}


LPSTR ftp::GetFTPServerReplyText(VOID)
	{
		return((LPSTR)gsServerReplyText);	// Return full text of server's reply
	}

int ftp::SendFTPCommand(SOCKET hControlChannel, char * gszCommandBuffer, int len)
	{
		//lpszFunctionName = "SendFTPCommand";

		// Send the FTP command
		if ((send(hControlChannel, (LPSTR)gszCommandBuffer,
					//lstrlen(gszCommandBuffer), NO_FLAGS)) == SOCKET_ERROR)
					len, NO_FLAGS)) == SOCKET_ERROR)
			{
				int iWinsockErr = WSAGetLastError();
				wsprintf(gszCommandBuffer, "Error %d from the send() function!!",
							iWinsockErr);
				MessageBeep(MB_ICONSTOP);
				//MessageBox(NULL, gszCommandBuffer, lpszFunctionName,
				//			MB_OK|MB_ICONSTOP);
			  // Return 999 to indicate an error has occurred
				return(999);
			}

		// Read the server's reply and return the reply code as an integer
		return(ReadFTPServerReply(hControlChannel));
	}


#if 0
int ftp::CheckInput()
{
  //int rval;
  fd_set readfds, writefds;

#if (!defined(WIN32) || !defined(_WIN32) )
  struct timeval timeout;
  FD_ZERO(&readfds);
  FD_ZERO(&writefds);
  FD_ZERO(&exceptfds);
  FD_CLR (fileno(stdin),&readfds);

  timeout.tv_sec = 0 ;                /* 0-second timeout. */
  timeout.tv_usec = 0 ;               /* 0 microseconds.  */
  FD_SET(fileno(stdin),&readfds);

  select ( fileno(stdin)+1,
	         &readfds,
			 &writefds,
			 &exceptfds,
			 &timeout);
 /* SELECT interrupted by signal - try again.  */

  if (errno == EINTR && i ==-1)  {
 	  return 0;

  }
  return ( FD_ISSET(fileno(stdin),&readfds) );
#else
	   //return (kbhit() );
     return 1;
#endif

}
#endif

#if 0
int  ftp::CheckFds( char *command)
{
 int rval, i;
 fd_set readfds, writefds, exceptfds;
 struct timeval timeout;
 //char *szInput=command;


/*  memset(command,0,FTP_BUFFER_SIZE); */
  memset(szBuffer,0,FTP_BUFFER_SIZE);
  FD_ZERO(&readfds);
  FD_ZERO(&writefds);
  FD_ZERO(&exceptfds);
#if (!defined(WIN32) || !defined(_WIN32) )
  FD_CLR (fileno(stdin),&readfds);
#endif
  if( hControlSocket > 0)
      FD_CLR (hControlSocket,&readfds);
  timeout.tv_sec = 0 ;                /* 1-second timeout. */
  timeout.tv_usec = 0 ;               /* 0 microseconds.  */

#if (!defined( _WIN32 ) || !defined( WIN32 ) )
  FD_SET(fileno(stdin),&readfds);
#endif

  if( hControlSocket > 0)
      FD_SET(hControlSocket,&readfds);

  i=select ((hControlSocket > 0) ? (hControlSocket+1) : 1,
	         &readfds,
			 &writefds,
			 &exceptfds,
			 &timeout);
 /* SELECT interrupted by signal - try again.  */
if (errno == EINTR && i ==-1)  {
   /*memset(command,0,FTP_BUFFER_SIZE);*/
   return 0;
}


if( (hControlSocket > 0) && FD_ISSET(hControlSocket, &readfds) )
{
  if ( ( rval = ReadControlMsg(szBuffer,FTP_BUFFER_SIZE))  > 0)
	  {
	  //printf(szBuffer);
	  //printf("ftp>");
	  //fflush(stdout);
	  return 0;
	  }
  else {
	 //printf("\r\nConnection closed by server\r\n");
	 //SetConsoleTitle("dftp: Connection closed");
	 CloseControlConnection();
	 hControlSocket = -1;
	 return 0;
  	}
 }
   return (ReadCommand = GetWin32Input(command));
}
#endif


int ftp::InitWinsock()
{
	WSADATA WsaData;
	if( !WSAStartup(0x0101,&WsaData) ) {
		//SetConsoleTitle("dftp: Connection closed");
		return 1;
	}
	else {
	 //printf("Winsock cannot be started\r\n");
	 return 0;
	}
}

void ftp::CleanUp()
{
	WSACleanup();
}



/*
 * GetPassword
 * read in the user's password.  turn off echoing before reading.
 * then turn echoing back on.
 */
#if 0
void ftp::GetPassword( char *szPass )
{


#if (!defined( _WIN32 ) || !defined( WIN32 ))
     EchoOff();
     (void)printf("Password:");
     (void)fgets(szPass,PASSWORD_LENGTH,stdin);
     (void)strtok(szPass,"\n");
     EchoOn(stdin,1);
#else
     /*
      * for WIN32 we just use getch() instead of getche()
      */
     int i=0;
     char ch=0;

	 (void)printf("Password:");

     while( (i < PASSWORD_LENGTH) && ( ch != '\r') ) {

	  ch = getch();
	  if( ch != '\r' && ch != '\n')
	     szPass[i++] = ch;
     }
#endif
     (void)printf("\r\n");
}
#endif

/*
 * GetUnixInput
 * function called to get user input on the UNIX side.
 */
#if 0
int ftp::GetUnixInput(char *command)
{
#if (!defined( _WIN32 ) || !defined( WIN32 ))
	char ch;
	int i;

	/*
	 * ignore leading whitespace
	 */
	while( (ch=getchar()) == ' ' || ch == '\t') ;

	if( ch != '\n') {
	    command[0] = ch;
	    fgets(&command[1],FTP_BUFFER_SIZE,stdin);
	    strtok(command,"\n");
	    i = strlen(command) - 1;

	    while( i>0 && isspace(command[i]))
	       i--;
	    if( i>= 0)
	       command[i+1] = 0;

	}
	return 1;
#else
       return 0;
#endif
}
#endif

/*
 * GetWin32Input
 * called to get input on the WIN32 side.
 */
#if 0
int ftp::GetWin32Input( char *command)
{
#if (defined( _WIN32 ) || defined( WIN32 ))
   char ch;
   static int i=0;

   /*
    * i, above, is static because this function can be called
    * many times before '\r' is pressed.  we set i=0 when
    * '\r' is encountered.
    */


   while( kbhit() ) {
      if( (ch=getch()) == 0)
	  getch();          /* ignore */
      else {
	 if( ch == '\r') {
	     command[i] = 0;
	     i = 0;
	     printf("\r\n");
	     i = strlen(command) - 1;

	     /*
	      * ignore trailing whitespace
	      */
	     while( i>0 && isspace(command[i]))
		i--;
		 if( i>= 0)
			 command[i+1] = 0;
		 i = 0;

	     return 1;
	 }
	 else if( ch == (char)8 && i > 0) {   /* process backspace */
	     printf("%c %c",8,8);
	     command[i] = 0;
	     i--;
	 }
	 else if( ( ch >= 32) && ( ch <= 126) ) {
	     printf("%c", ch);
	     command[i++] = ch;
	 }
      }
   }
   return 0;
#else
   return 0;
#endif
}
#endif


/*
 * GetFile
 * called to retrive a file from remote host
 */
void ftp::GetFile( char *fname)
{
   FILE *fp;
   int fd;
   int nBytesRead;
   // , nTotal, nBytesRead;
   // retval, aborted;
   //char *abortstr = "ABOR\r\n", ch;
   if( !fname || ! (*fname)) {
      return;
   }

   /*
    * open the file with current mode
    */
   if(0 == (fp=fopen(fname,(bMode==ASCII) ? "wt" : "wb"))) {
      perror("file open");
      return;
   }

   if(SetupDataConnection()) {
       fclose(fp);
       return;
   }

   /*
    * send command to server and read response
    */
   sprintf(szBuffer,"RETR %s\r\n",fname);
   if(!SendControlMsg(szBuffer,strlen(szBuffer))) {
      fclose(fp);
      return;
   }
   GetReply();

   /*
    * accept server connection
    */
   if( AcceptConnection() <= 0) {
       fclose(fp);
       return;
   }
   /*
    * now get file and store
    */
   fd = fileno(fp);
   while( (nBytesRead=ReadDataMsg(szBuffer,FTP_BUFFER_SIZE)) > 0) {


	   write(fd,szBuffer,nBytesRead);
	   //nTotal+=nBytesRead;
	   GetReply();
   }
   closesocket(fd);
   CloseDataConnection(hDataSocket);
 /*/  ControlCHit = 0; */
   GetReply();
}
/*
 * GetData
 * called to retrieve data from remote host
 * Returned data is in allocated space. Calling program must free it
 */
char * ftp::DoGetData( char *fname, int &npts)
{
  //int retval, aborted=0;
  int nBytesRead;
  npts = 0;
  int nTotal = 0;
  //char *abortstr = "ABOR\r\n", ch;
  if( !fname || ! (*fname)) {
     return 0;
  }

  if(SetupDataConnection()) {
      return 0;
  }

  /*
   * send command to server and read response
   */
  sprintf(szBuffer,"RETR %s\r\n",fname);
  if(!SendControlMsg(szBuffer,strlen(szBuffer))) {
     return 0;
  }
  GetReply();
  if(0 == strncmp(szBuffer,"550",3)) {
  	// File not found
    npts = 0;
    return 0;
  }

  /*
   * accept server connection if not passive
   */
  if(!passive) {
  	if( AcceptConnection() <= 0) {
    	return 0;
  	}
  }
  /*
   * now get file and put in memory buffer
   */
  char *data = NULL;
  while( (nBytesRead=ReadDataMsg(szBuffer,FTP_BUFFER_SIZE)) > 0) {
    data = (char *)realloc(data,nTotal+nBytesRead);
    memcpy(data+nTotal,szBuffer,nBytesRead);
	  nTotal+=nBytesRead;
  }
  GetReply();
  npts = nTotal;
  CloseDataConnection(hDataSocket);
  return data;
}

bool ftp::SetupDataConnection()
{
	bool ret = false;
  // Connect DataSocket according to whether this is an active or passive connection
  // With passive, we connect to the address and port given us from the PASV command.
  // If active, we set up a ListenSocket and let the server connect to us.
  if(passive) {
  	// Find we have to issue the PASV command each time to get a new port
    // to connect to.
    DoPASV();
    hDataSocket = ConnectToServer(szServerAddr,szServerPort);
    if(hControlSocket == NULL) {
      ret = true;
    }
  }
  else
  if( GetListenSocket() < 0) {
    ret = true;
  }
  return ret;
}

/*
 * PutFile
 * called to transfer a file to the remote host using the current
 * file transfer mode.  it's just like GetFile.
 *
 * i have commented out lines that would have helped with trapping
 * ctrl-c because longjmp did not work :((
 * if you figure it out, then uncomment them
 */
#if 0
bool ftp::PutFile( char *fname)
{
	bool ret = true;
   FILE *fp;
   int fd, nTotal=0, nBytesRead, aborted=0;
   char *abortstr = "ABOR\r\n";

  /* void (*OldHandler)(int); */

	if( !fname || ! (*fname)) {
  	//printf("No file specified.\n");
    return ret;
  }
  if(0 == (fp=fopen(fname,(bMode==ASCII) ? "rt" : "rb"))) {
      //perror("file open");
      return ret;
  }

  if(SetupDataConnection()) {
  	fclose(fp);
    return ret;
  }

   /*
    * send command to server & read reply
    */
   // Fetch the filename from the full path.
   //strrev(fname);
  char *p;
  char *pp = fname;
  while((p = strchr(pp,'\\')) != NULL)
  	pp = ++p;
  sprintf(szBuffer,"STOR %s\r\n",pp);
  if(!SendControlMsg(szBuffer,strlen(szBuffer))) {
      fclose(fp);
      return ret;
   }
   GetReply();

  /*
   * accept server connection if not passive connection
  */
  if(!passive) {
  	if( AcceptConnection() <= 0) {
  		fclose(fp);
    	return ret;
  	}
  }
   /*
    * now send file
    */

   fd = fileno(fp);
/*
   OldHandler = signal(SIGINT,SigHandler);
   retval = setjmp(abortfile);

   if( retval != 0 ) {
      printf("Aborting\r\n");
      fflush(stdout);
      close(fd);
      CloseDataConnection(hDataSocket);
      if( OldHandler)
	  (void)signal(SIGINT,OldHandler);
      ControlCHit = 0;
      GetReply();
      return;
   }
  */
   printf("Press ESC to abort\r\n");
   while( (nBytesRead=read(fd,szBuffer,FTP_BUFFER_SIZE)) > 0) {
      SendDataMsg(szBuffer,nBytesRead);
      nTotal+=nBytesRead;
      printf("%s : %d sent\r",fname,nTotal);
      #if 0
	   if( CheckInput() ) {

			if( !(ch = getch()) ) getch();
			if( ch == 27 )
				aborted = 1;
	   }
     #endif

	   /*
	    * send an abort command to server if we aborted.
	    */
	   if( aborted ) {

		   //printf("\r\nAbort: Waiting for server to finish.");
		   SendControlMsg(abortstr,strlen(abortstr));
		   break;
	   }
   }

   /*(void)signal(SIGINT,OldHandler); */

   //printf("\r\n");
   /*
    * close data connection
    */
   CloseDataConnection(hDataSocket);
   closesocket(fd);
   GetReply();
   return false;
}

#endif
/*
 * DoPutData
 * called to transfer data in membory to a file to the remote host.
 * using the binary file transfer mode.
 */
bool ftp::DoPutData( char *fname, char *data, int len)
{
	bool ret = true;
  if( !Connected ) {
      return ret;
  }
  //int fd, nTotal=0, nBytesRead=0, retval, aborted=0;
  //char *abortstr = "ABOR\r\n";

  if(SetupDataConnection()) {
    return ret;
  }

  /*
   * send command to server & read reply
   */
  sprintf(szBuffer,"STOR %s\r\n",fname);
  if(!SendControlMsg(szBuffer,strlen(szBuffer))) {
      return ret;
  }
  GetReply();

  /*
   * accept server connection if not passive connection
  */
  if(!passive) {
  	if( AcceptConnection() <= 0) {
    	return ret;
  	}
  }
  /*
   * now send data
  */
  SendDataMsg(data,len);
  CloseDataConnection(hDataSocket);
  //closesocket(fd);
  GetReply();
  if(strncmp(szBuffer,"226",3)) {
  	//Debug("Error in DoPutData: %s",szBuffer);
    return true;
  }
  ret = false;
  return ret;
}
//extern "C" BOOL FAR PASCAL IsReadyToRead(SOCKET hSocket)
static char *lpszFunctionName;
bool ftp::IsReadyToRead(SOCKET hSocket)
	{
		FD_SET setReadyToRead;			// Socket set to test ready-to-read status
		TIMEVAL timeTimeOut;				// Amount of time to wait for status change
		int nReady;									// Ready-to-read flag

		lpszFunctionName = "IsReadyToRead";

		timerclear(&timeTimeOut);
    timeTimeOut.tv_sec = 5;	// Timeout after 5 seconds
		FD_ZERO(&setReadyToRead);
		FD_SET(hSocket, &setReadyToRead);

		if ((nReady = select(NULL, (LPFD_SET)&setReadyToRead, NULL, NULL,
					&timeTimeOut)) == SOCKET_ERROR)
			{
				int iWinsockErr = WSAGetLastError();
				wsprintf(szBuffer, "Error %d from the select() function!!",iWinsockErr);
				MessageBeep(MB_ICONSTOP);
				//MessageBox(NULL, szBuffer, lpszFunctionName,MB_OK|MB_ICONSTOP);
				//Debug("%s Function: %s",szBuffer, lpszFunctionName);
				return(FALSE);
			}

		return(nReady ? TRUE : FALSE);
	}


