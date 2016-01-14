       //======================================
// Class TCPThread
//
// Thread which waits for interrupts from com port
//
//========================================
// TCPThreadException
//class TCPThreadException {
//};
class TCPThread : public TThread, WComm {
	public:
  	TCPThread(SOCKET s);
    ~TCPThread();
    void disconnect();
    bool isRunning();		// Checks status. Returns true if running
    SOCKET socket;
    char host[16];			// Host name
    char host_addr[48];	// IP address
  private:
  	int Run();
  public:
    int operator == ( const TCPThread & tcpthread) const {
    	return socket == tcpthread.socket;
    }
};
typedef TIArrayAsVector<TCPThread> TCPArray;
typedef TIArrayAsVectorIterator<TCPThread> TCPIterator;

// TCPClient used by a client. Doesn't run in a thread
class TCPClient : public WComm {
	public:
  	TCPClient(char *addr, int port);
  	TCPClient(char *addr, int port, SOCKET socket);	// Constructor if we already have a socket
    ~TCPClient();
    void disconnect();
    char host[16];			// Host name
    char host_addr[48];	// IP address
    bool Hello();				// Initial handshake
    bool Goodbye();
    bool getTime();
    bool getSPS();
    int getFile(char *fspec);
    int fileSize(char *fspec);
    bool connected;
};

//======================================
// Class ListenThread
//
// Thread which waits for interrupts from com port
//
//========================================
// TCPThreadException
//class TCPThreadException {
//};
class ListenThread : public TThread, WComm {
	public:
  	ListenThread(int port);
    ~ListenThread();
    SOCKET listen_socket;
    int getPort() {return port;}
    void disconnect();
    // Array and Iterator for keeping track of the
    // connections
    TCPArray *tcpArray;
    int next();
  private:
  	int which_tcp;	// Which of the tcpArray should we use
  	int Run();
    int port;
};
