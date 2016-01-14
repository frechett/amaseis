//#include "winsock2.h"
//#include <stdio.h>
//#include <conio.h>
//#include "iostream.h"
//#include "fstream.h"

#if 0
class WCException
{
	public:
  	WCException(char *msg,char *s);
    // Copy constructor
    //EEException(const EEException&);
    void Error();
  private:
  	char msg2[100];
    char s2[100];
};
#endif


class WComm
{
	public:
  	// Constructor which starts a listener
		WComm() ;
  	// Constructor which handles a connection
    // given a socket
  	WComm(SOCKET s);
  	~WComm();

    SOCKET getSocket() {return m_socket;}
		bool connectServer(char*,int);
		bool reconnectServer();		// Reconnect
    bool createSocket();
		int sendData(char*);
		int sendBinary(char*, int);
		int recvData(char*,int);
    int recvBinary(char *,int);
		void startServer(int);
		int waitForClient(char *);
		void closeConnection();
    char *fileReceive(char *fpath, int &size);
    short *fetchCurrent(int npts, int &size);	// Requests data from current hour
    															// Send everything beyond npts
    void sendCurrent();						// Sends data from current hour
		bool IsReadyToRead(SOCKET hSocket);
    bool connected;
  protected:
		WSADATA wsaData;
		SOCKET m_socket;
		SOCKET m_backup;
		sockaddr_in con;
		SOCKET AcceptSocket;
    struct sockaddr addr;
    void fileSend(char *fpath);
    bool GetError();
    int port;
    char *ip;
};