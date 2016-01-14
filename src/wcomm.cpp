#include "as1.h"

// Move following variables into the class as class variables
//WSADATA wsaData;
//SOCKET m_socket;
//SOCKET m_backup;
//sockaddr_in con;
//SOCKET AcceptSocket;
// Couldn't get exceptions to work within a thread.  Remove.
#if 0
void WCException::Error()
{
	//rsmsgok("%s %s\n in file: %s",msg2,s2,make_fs("INI"));
}
WCException::WCException(char *msg,char *s)
{
	strcpy(msg2,msg);
  strcpy(s2,s);
}
#endif

static char msg[128];
WComm::WComm()
{
	ip = 0;
  port = 0;
    // Initialize Winsock.
    int iResult = WSAStartup( MAKEWORD(2,2), &wsaData );
    if ( iResult != NO_ERROR ) {
    	strcpy(msg,"Error at WSAStartup()");
    	//throw msg;
    }
    // Create a socket.
    m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

    if ( m_socket == INVALID_SOCKET ) {
        sprintf(msg, "Error at socket(): %ld\n", WSAGetLastError() );
        WSACleanup();
        //throw msg;
    }
		connected = false;
}

// Constructor if we already have a socket
WComm::WComm(SOCKET s) {
	m_socket = s;
}
// Good Programmer don't comment this
//WComm::~WComm(){WSACleanup();}
WComm::~WComm() {
	WSACleanup();
  if(ip)
  	free(ip);
}

bool WComm::createSocket()
{
    // Create a socket.
    if(m_socket)
    	closesocket(m_socket);
    m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

    if ( m_socket == INVALID_SOCKET ) {
        sprintf(msg, "Error at socket(): %ld\n", WSAGetLastError() );
        WSACleanup();
        return true;
    }
		connected = false;
    return false;
}
bool WComm::reconnectServer()
{
	bool ret = false;
    // Connect to a server.
    char *ip2 = 0;
    struct hostent *hp = gethostbyname(ip);
    if(hp != NULL)
    	ip2 =  inet_ntoa( *( struct in_addr*)( hp -> h_addr_list[0]));
    con.sin_family = AF_INET;
    con.sin_addr.s_addr = inet_addr( ip2 );
    con.sin_port = htons( port );
    Progress *pro = new Progress(MdiClient);
    pro->Create();
    pro->SetText("Attempt to connect to %s, port %d",ip,port);
    connected = true;
    int r = connect( m_socket, (SOCKADDR*) &con, sizeof(con) );
    if(r == SOCKET_ERROR) {
        //printf( "Failed to connect.\n" );
        r = WSAGetLastError();
        switch(r) {
        	case WSAEADDRNOTAVAIL:
        		sprintf(msg,"Cannot connect to address: %s",ip);
            break;
          default:
          	sprintf(msg,"Cannot connect to address: %s. Error: %d",ip,ret);
        }
        pro->SetText(msg);
        //WSACleanup();
        Sleep(1000);
        connected = false;
        ret = true;
    }
    delete pro;
    return ret;
}
// Returns true if can't connect.
bool WComm::connectServer(char *ip,int port)
{
	this->ip = strdup(ip);
	this->port = port;
  reconnectServer();
}


void WComm::startServer(int port)
{
    // Connect to a server.
    con.sin_family = AF_INET;
    con.sin_addr.s_addr = inet_addr( "0.0.0.0" );
    con.sin_port = htons( port );
    if ( bind( m_socket, (SOCKADDR*) &con, sizeof(con) ) == SOCKET_ERROR) {
        MessageBeep(MB_ICONEXCLAMATION);
        //strcpy(msg,"Failed to bind socket");
        Progress *pro = new Progress(MdiClient);
        pro->Create();
        pro->SetText("Failed to bind socket on port %d.",port);
        Sleep(1000);
        //printf( "Failed to connect.\n" );
        WSACleanup();
        //throw msg;
        delete pro;

    }

    // Listen on the socket.
    if ( listen( m_socket, 1 ) == SOCKET_ERROR ) {
    	strcpy(msg,"Error listening on socket.");
      //throw msg;
    }

}

int WComm::waitForClient(char *h_name)
{
  int len;
  AcceptSocket = SOCKET_ERROR;
  //while ( AcceptSocket == SOCKET_ERROR ) {
  	AcceptSocket = accept( m_socket, NULL, NULL );
  //}
  if(AcceptSocket == SOCKET_ERROR)
  	return AcceptSocket;
	//getrname(int fd, struct sockaddr *addr, int *addrlen)
  struct hostent *hp;
  struct sockaddr_in *so = (struct sockaddr_in *)&addr;
  len = sizeof(addr);
  int i = getpeername (AcceptSocket, &addr, &len);
  if(i == SOCKET_ERROR) {
  	*h_name = 0;
  	return AcceptSocket;
  }
  hp = gethostbyaddr ((char *)&so->sin_addr.s_addr,
                        sizeof so->sin_addr.s_addr, AF_INET);
  char *hh = (hp ? hp->h_name : inet_ntoa(so->sin_addr));
  strcpy(h_name,hh);
  return AcceptSocket;
}

int WComm::sendData(char *sendbuf)
{
	if(!connected) {
    createSocket();
    if(reconnectServer())
    	return 0;
  }
	return send( m_socket, sendbuf, strlen(sendbuf), 0 );
}

// Send binary data
int WComm::sendBinary(char *sendbuf, int len)
{
	if(!connected) {
    createSocket();
    if(reconnectServer())
    	return 0;
  }

	return send( m_socket, sendbuf, len, 0 );
}

bool WComm::GetError()
{
	int ret = WSAGetLastError();
  char error[64];
  bool shutdown = false;
  Progress *pro = new Progress(MdiClient);
  pro->Create();
  switch(ret) {
  	case WSAECONNREFUSED:
			sprintf(error,"Connection refused");
      shutdown = true;
      break;
    case WSAENOTCONN:
    	sprintf(error,"Not connected");
      connected = false;
      shutdown = true;
      break;
    case WSAETIMEDOUT:
     	sprintf(error,"Timed out"); break;
    case WSAEISCONN:
     	sprintf(error,"Already connected"); break;
    case WSAECONNRESET:
     	sprintf(error,"Connection reset"); 
      connected = false;
      shutdown = true;
      break;
    default:
     	sprintf(error,"Receive error: %d",ret);
  }
  pro->SetText(error);
  Debug(error);
  MessageBeep(MB_ICONEXCLAMATION);
  if(shutdown) {
   	Sleep(1000);
   	pro->SetText("Disconnecting");
    Debug("Disconnecting");
   	Sleep(1000);
   	closeConnection();
  }
  delete pro;
}

// Receive ASCII data.
int WComm::recvData(char *recvbuf,int size)
{
	if(!connected) {
  	return SOCKET_ERROR;
  }
	int sz = 0;
  //if(IsReadyToRead(m_socket)) {
		sz = recv( m_socket, recvbuf, size, 0 );
  	if(sz == SOCKET_ERROR) {
    	GetError();
      return sz;
    }
  //}
  if(sz >= 0)
  	recvbuf[sz] = 0;
	return sz;
}
// Receive binary data.  This is only used when you know exactly
// how much data is coming and the data is binary.
// Returns the number of bytes received
// 2007.04.22: Transfer 1024 bytes at a chunk
int WComm::recvBinary(char *recvbuf,int size)
{
	if(!connected) {
  	return SOCKET_ERROR;
  }
  int totalReceived = 0;
  int blksize = 1024;
  int sz = 0;
  //while(IsReadyToRead(m_socket) && (totalReceived < size)) {
  while(totalReceived < size) {
		sz = recv( m_socket, recvbuf+totalReceived, blksize, 0 );
  	if(sz == SOCKET_ERROR) {
    	GetError();
      return sz;
    }
    totalReceived += sz;
  }
	return totalReceived;
}


void WComm::closeConnection()
{
	closesocket(m_socket);
  connected = false;
}

//
// fileReceive is used by the Client. See fileSend below
// for details of protocol
//
char * WComm::fileReceive(char *fspec, int &size)
{
	char rec[128];
  char *ok = "OK";
  char *nak = "NAK";

  if(!connected) {
  	Debug("Looking for %s but not connected");
  	size = 0;
    return 0;
  }

  // Send request for file
  sprintf(rec,"GET %s\n",fspec);
  Debug(rec);
  sendData(rec);

  int n = recvData(rec,128);
  if(n <= 0)
  	return 0;
  rec[n] = 0;
  size = atoi(rec);
  	// Reply "OK"
  Debug("Size of file to be fetched: %d. Send \"OK\"",size);
	sendData("OK");
  if(size == 0)
  	return 0;
  // Fetch data
  // Get memory to receive data
  char *buffer = new char[size];
  Debug("Wait for binary data");
  n = recvBinary(buffer,size);
  Debug("Got it. Size: %d",n);
  if(n == size )
  	sendData(ok);
  else {
  	// Problem
    Debug("Didn't get proper size. Send NAK");
    sendData(nak);
    delete buffer;
    buffer = 0;
  }
  return buffer;
}

//
// fileSend is used by the Server
// ===============================
// 2007:09:12: ALJ - Changed from fopen() to Windows CreateFile
// This allows us to use Windows functions to get filesize.
// Also, we will retain entire path for file.
// Protocol:
// Server                          Client
//                                 GET filespec
// filesend(filespec)
//   send filesize
//                                 Receive filesize
//                                 if(filesize == 0) then no file, return
//																 Create space in memory for entire file
//																 Create path to file if doesn't exist
//                                 Send OK
//   send data
//                                 receive data
//                                 write data
//                                 send OK
//   return
//
// NOTE: The AmaSeis files use the first four bytes to contain the size
// of the file. (I wish I had never done that.) We want fileSend to
// return only the data so we throw away the first four.
void WComm::fileSend(char *fpath)
{
  char rec[32] = "";
  Debug("fileSend: %s",fpath);
	////////////////////////////////////////
  // Try 4 times to send the file.
  HANDLE handle;
  int i;
 	handle = CreateFile(
  		fpath,
    	DWORD GENERIC_READ,	// access (read-write) mode
    	0,	// share mode
    	0,	// pointer to security attributes
    	OPEN_EXISTING,	// how to create
    	FILE_ATTRIBUTE_NORMAL,	// file attributes
    	0 	// handle to file with attributes to copy
 	);
  if(INVALID_HANDLE_VALUE == handle) {
    // Send zero length
    Debug("File %s doesn't exist. Send back 0",fpath);
		sendBinary("0", 2);
    recvData(rec, 32);
  	return;
  }
 	// File exists
  // See how big is it
  int size = GetFileSize(handle,0);
  // Reduce size by 4 to take care of header
  size -= 4;
  // If file is 4 bytes long, don't send
  if(size == 0) {
    Debug("File %s has only 4 bytes. Send back 0",fpath);
    // Send zero length
		sendBinary("0", 2);
		recvData(rec, 32);
  	return;
  }
  Debug("File size: %d",size);
  // Convert size to ASCII size
	char filesize[10];
  itoa(size,filesize,10);

  // Read entire file into memory and send in one whole swoop.
  // If this doesn't work, do it in chunks.
  unsigned long read;
  char *buffer = new char[size];
  Debug("Read 4-byte header");
  ReadFile(handle,buffer,4,&read,0);
  Debug("Read the rest");
  ReadFile(handle,buffer,size,&read,0);
  if(read == size) {
  	Debug("Send size of file: %s. Size: %d",fpath,size);
		sendData(filesize);
    Debug("Wait for reply");
  	// Should receive "OK" back.
		recvData(rec, 32);
    Debug("Got %s. Send data. %d bytes",rec,read);
  	sendBinary(buffer,read);
    Debug("Wait for reply");
		recvData(rec, 32);
    Debug("Reply: %s",rec);
  }
  else {
  	// Had problem. Tell client we don't have this file
  	strcpy(filesize,"0");
		sendData(filesize);
  	// Should receive "OK" back.
		recvData(rec, 32);
  }
  delete buffer;
  CloseHandle(handle);
}
// Client function
// fetchCurrent(int npts)
// Request all data in current hour beyond npts
// Server responds with size
// Client then asks for data
// Server responds with data
short * WComm::fetchCurrent(int npts, int &size)
{
	if(!connected) {
  	size = 0;
    createSocket();
    if(reconnectServer())
    	return 0;
  }
  char rec[32] = "";
  sprintf(rec,"FETCHCURRENT %d",npts);
  sendData(rec);
  // Server responds with size of data to be sent
  // Size is number of shorts
  int n =	recvData(rec,sizeof(rec));
  if(n >= 0)
  	size = atoi(rec);
  else
  	size = 0;
  if(size == 0)
  	return 0;
  // Acquire space. Freed in calling program.
  short * buffer = new short[size];
  sendData("OK");
	n = recvBinary((char *)buffer,size*2);
  if(n <= 0) {
  	Debug("Got reply of %d from recv",n);
  	return 0;
  }
 	n /= 2;
 	if(n != size) {
  	Debug("Got bad size back from recv: %d. Should be %d.",n,size);
  	// Problem
   	return 0;
  }
	return buffer;
}
void WComm::sendCurrent()
{
}
//static char *lpszFunctionName;
bool WComm::IsReadyToRead(SOCKET hSocket)
{
	return true;
}
#if 0
		FD_SET setReadyToRead;			// Socket set to test ready-to-read status
		TIMEVAL timeTimeOut;				// Amount of time to wait for status change
		int nReady;									// Ready-to-read flag

		lpszFunctionName = "IsReadyToRead";

		timerclear(&timeTimeOut);
    //timeTimeOut.tv_sec = 60;	// Timeout after 60 seconds
		FD_ZERO(&setReadyToRead);
		FD_SET(hSocket, &setReadyToRead);

		if ((nReady = select(NULL, (LPFD_SET)&setReadyToRead, NULL, NULL,
					&timeTimeOut)) == SOCKET_ERROR)
			{
				int iWinsockErr = WSAGetLastError();
        char szBuffer[48];
				wsprintf(szBuffer, "Error %d from the select() function!!",iWinsockErr);
				MessageBeep(MB_ICONSTOP);
				//MessageBox(NULL, szBuffer, lpszFunctionName,MB_OK|MB_ICONSTOP);
				Debug("%s Function: %s",szBuffer, lpszFunctionName);
				return(FALSE);
			}

		return(nReady ? TRUE : FALSE);
}

#endif


