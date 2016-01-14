// TCP functions
#include "as1.h"
//
// TCPThread constructor
//
TCPThread::TCPThread(SOCKET s) : WComm(s)
{
  socket = s;
}
TCPThread::~TCPThread()
{
}

void TCPThread::disconnect()
{
	closeConnection();
}
//
// Return true if thread is running
bool TCPThread::isRunning()
{
	TThread::TStatus status = GetStatus();
  return (status == TThread::Running);
}
// Run() function for TCP Thread which is spun off
// by TCPListen
// This thread waits for messages from the Client and
// responds to each message.
int TCPThread::Run()
{
  char msg[128];
  char query[64];
  Progress *pro;
  int nc;
  // Following statement blocks until client sends a message.
  // First message should be "PING$"
  bool done = false;
  pro = new Progress(MdiClient);
  pro->Create();
  pro->SetText("TCP thread starting");
  Debug("TCP thread starting");
  Sleep(100);
  SYSTEMTIME utc;
  delete pro;
  while(!done) {
  	// Receive commands and handle them
  	nc = recvData(msg,sizeof(msg));
    //Debug("Received: %s",msg);
  	if(nc < 0)
  		break;
    msg[nc] = 0;
  	if(0 == strncmp(msg,"PING",4)) {
    	Debug("Get PING, send PONG");
  		sendData("PONG");
    } else if(0 == strncmpi("GETTIME",msg,7)) {
    	Debug("Get GETTIME");
      GetSystemTime(&utc);
      sprintf(msg,"%04d-%02d-%02d %02d:%02d:%02d.%03d",
      	utc.wYear,utc.wMonth,utc.wDay,
      	utc.wHour,utc.wMinute,utc.wSecond,utc.wMilliseconds);
      sendData(msg);
      Debug(msg);
    } else if(0 == strncmpi("GETSPS",msg,6)) {
    	Debug("Get GETSPS");
    	sprintf(msg,"%f",samprate);
      sendData(msg);
      Debug(msg);
    } else if(0 == strncmpi("GETLAT",msg,6)) {
    	Debug("Get GETLAT");
    	sprintf(msg,"%lf",profile->latitude);
      sendData(msg);
      Debug(msg);
    } else if(0 == strncmpi("GETLON",msg,6)) {
    	Debug("Get GETLON");
    	sprintf(msg,"%lf",profile->longitude);
      sendData(msg);
      Debug(msg);
    } else if(0 == strncmpi("GETELE",msg,6)) {
    	Debug("Get GETELE");
    	sprintf(msg,"%f",profile->elevation);
      sendData(msg);
      Debug(msg);
    } else if(0 == strncmpi("GETCODE",msg,7)) {
    	Debug("Get GETCODE");
    	sprintf(msg,"%s",profile->station_code);
      sendData(msg);
      Debug(msg);
    } else if(0 == strncmpi("GETNAME",msg,7)) {
    	Debug("Get GETNAME");
    	sprintf(msg,"%s",profile->station_name);
      sendData(msg);
      Debug(msg);
    } else if(0 == strncmpi("GETCOMP",msg,7)) {
    	Debug("Get GETCOMP");
    	sprintf(msg,"%c",profile->component);
      sendData(msg);
      Debug(msg);
    } else if(sscanf(msg,"GET %s",query)) {
    	Debug("Get %s",query);
    	// Send a file to this client.
      fileSend(query);
    } else if(0 == strcmpi(msg,"BYE")) {
    	Debug("BYE: Sending back OKAY. Shuting down",query);
      sendData("OKAY");
      break;
    } else if(sscanf(msg,"FETCHCURRENT %s",query)) {
    	// query returns the number of points to begin fetch from
      int npts = atoi(query);
      // Get data from current hour
      HourRecord *hr = MdiClient->hourrecord;
      // How much data is requested?
      // Number of points is 1/2 of the number of bytes
      int n_data = hr->npts - npts;
      if(n_data <= 0) {
        sendData("0");
      } else {
      	sprintf(msg,"%d",n_data);
        sendData(msg);
        // Should receive "OK"
  			nc = recvData(msg,sizeof(msg));
      	// Point to beginning of data requested
      	short *d = hr->data+npts;
        //send(m_socket,(char *)d,n_data*sizeof(short),0);
        sendBinary((char *)d,n_data*sizeof(short));
      }
  	} else {
    	Debug("Received an unknown command: %s",msg);
    }
  }
  closeConnection();
  pro = new Progress(MdiClient);
  pro->Create();
  pro->SetText("TCP thread terminating");
  Debug("TCP thread terminating");
  Sleep(1000);
  delete pro;
  return true;
}
//
// ListenThread constructor
//
ListenThread::ListenThread(int port)
{
  this->port = port;
 	// Set up an empty array of connections
  // Not sure we need this
  tcpArray = new TCPArray(1,0,1);
  which_tcp = 0;
}
ListenThread::~ListenThread()
{
}

// next()
// Returns which is the next member of the array to use
int ListenThread::next()
{
	int i = -1;
	if(tcpArray) {
  	int n = tcpArray->GetItemsInContainer();
    if(n == 0)
    	return i;
		i = which_tcp;
  	which_tcp++;
  	which_tcp = which_tcp % n;
  }
  return i;
}

void ListenThread::disconnect()
{
	// Close down all the attached connections
  //tcpArray->ForEach(Disc,0);
  for(unsigned i=0;i<tcpArray->GetItemsInContainer();i++) {
  	TCPThread *t = (*tcpArray)[i];
    t->disconnect();
  }
  // Close down the "wait for connection" thread;
  closeConnection();

}
// Do the listening
int ListenThread::Run()
{
	SOCKET s;
  Progress *pro = new Progress(MdiClient);
  pro->Create();
  //::Sleep(1000);
  pro->SetText("TCP Listening on port %d",port);
  Debug("TCP Listening on port %d",port);
  Sleep(1000);
  delete pro;
	startServer(port);
  char address[48];
  do {
  	Debug("Waiting for connection from client");
  	s = waitForClient(address);
    if(s == SOCKET_ERROR) {
    	break;
    }
		pro = new Progress(MdiClient);
  	pro->Create();
  	pro->SetText("Got connection. Socket %d. addr %s",s,address);
  	Debug("Got connection. Socket %d. addr %s",s,address);
  	Sleep(1000);
    delete pro;

    // Spin off a new connection thread
    TCPThread *tcpthread = new TCPThread(s);
    strcpy(tcpthread->host_addr,address);
    tcpArray->Add(tcpthread);
    tcpthread->Start();
  } while (s != SOCKET_ERROR);
  Debug("Listen thread terminating");
  return 0;
}

// Constructor for TCPClient
// Connects to a server using that server's address and well known port
TCPClient::TCPClient(char *addr, int port) : WComm()
{
	connected = !connectServer(addr, port);
  Debug("Client connected to %s on port %d",addr,port);
}
TCPClient::TCPClient(char *addr, int port, SOCKET s) : WComm(s)
{
	connected = !connectServer(addr, port);
}
TCPClient::~TCPClient()
{
	closeConnection();
  connected = false;
}
// Initial handshake
// Returns false if everything okay
// Fetch latitude, longitude, elevation, station name, and station code
bool TCPClient::Hello()
{
	char buffer[48];
  int size = 47;
  int n;
  Debug("Send PING");
	sendData("PING");
  recvData(buffer,size);
  Debug("Receive %s",buffer);
  if(strncmpi(buffer,"PONG",4)) return true;
  Debug("Request Latitude");
	sendData("GETLAT");
  recvData(buffer,size);
  if(0 == sscanf(buffer,"%lf",&profile->latitude)) return true;
  Debug("Get %lf",profile->latitude);
  Debug("Request Longitude");
	sendData("GETLON");
  recvData(buffer,size);
  if(0 == sscanf(buffer,"%lf",&profile->longitude)) return true;
  Debug("Get %lf",profile->longitude);
  Debug("Request elevation");
	sendData("GETELE");
  recvData(buffer,size);
  if(0 == sscanf(buffer,"%f",&profile->elevation)) return true;
  Debug("Get %f",profile->elevation);
  Debug("Request component");
	sendData("GETCOMP");
  recvData(buffer,size);
  if(0 == sscanf(buffer,"%c",&profile->component)) return true;
  Debug("Get %c",profile->component);
  Debug("Request station code");
	sendData("GETCODE");
  recvData(buffer,size);
  if(0 == sscanf(buffer,"%s",profile->station_code)) return true;
  Debug("Get %s",profile->station_code);
  Debug("Request station name");
	sendData("GETNAME");
  recvData(buffer,size);
  if(0 == sscanf(buffer,"%s",profile->station_name)) return true;
  Debug("Get %s",profile->station_name);
  return false;
}
// Signing off
bool TCPClient::Goodbye()
{
	char buffer[48];
  int size = 47;
 	Debug("Send BYE");
	sendData("BYE");
  recvData(buffer,size);
  if(strstr(buffer,"OKAY"))
  	return false;
  else
  	return true;
}

//
// fileSize checks to see if the server has a file
// if not, a zero is returned.
// If it has the file, a number is returned giving
// the size of the file.
int TCPClient::fileSize(char *fspec)
{
	char rec[24];
  sprintf(rec,"GET %s$\n",fspec);
  sendData(rec);
  int n = recvData(rec,16);
  if(n > 0) {
  	n = atoi(rec);
  } else n = 0;
  return n;
}
//
// getTime()
// Fetch time from server to synchronize our clocks
// Returns false on success
bool TCPClient::getTime()
{
	char rec[36];
  SYSTEMTIME utc;
	sendData("GETTIME");
  int n = recvData(rec,36);
  if(n < 0) return true;
  rec[n] = 0;
  if(7 == sscanf(rec,"%d-%d-%d %d:%d:%d.%d",
  	&utc.wYear,&utc.wMonth,&utc.wDay,
    &utc.wHour,&utc.wMinute,&utc.wSecond,&utc.wMilliseconds)) {
    SetSystemTime(&utc);
    return false;
  }
  else return true;
}
// getSPS()
// Get samples per second so our client is running at same rate as
// server
bool TCPClient::getSPS()
{
	char rec[36];
	sendData("GETSPS");
  int n = recvData(rec,36);
  if(n < 0) return true;
  rec[n] = 0;
  samprate = atof(rec);
  return false;
 }
#if 0
int TCPClient::getFile(char *fspec)
{
	char buffer[64];
  // Send command to server that we want this file.
  fileReceive(fspec);
  return false;
}

#endif
