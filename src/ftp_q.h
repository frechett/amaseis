//#ifndef __ftp_q__
//#define __ftp_q__

#include <io.h>
#define DEFAULT_PORT_NUM 21
#define PASSWORD_LENGTH 256
#define EINTR WSAEINTR
#define bzero(x,y) memset(x,0,y)
#define bcopy(x,y,z) memcpy(y,x,z)
//#define close closesocket

enum {
 LS = 0,
 BINARY,
 ASCII,
 PWD,
 CD,
 OPEN,
 CLOSE,
 QUIT,
 LCD,
 LLS,
 LDIR,
 USER,
 SHELL,
 IGNORE_COMMAND,
 GET,
 PUT,
 HELP,
 RHELP,

 FTP_COMPLETE=1, /* these will be used later */
 FTP_CONTINUE,
 FTP_ERROR
};

#define FTP_BUFFER_SIZE 4096
class ftp_q {
public:
	ftp_q();
	~ftp_q();
	bool DoOpen(char *);
	char * DoList(char *);
	bool DoCD(char *);
  bool DoMKD(char *);
	void DoShellCommand(char *);
	bool DoLogin(char *User, char *Password);
	void DoClose(void);
	void DoLCD( char *);
	bool DoPut( char *);
  bool DoPutData(char *filename,char *data,int len);
  char *DoGetData(char *filename,int &npts);
	void DoGet( char *);
	void DoLLS(char * );
	void DoBinary();
	void DoRhelp( char *);
	void DoAscii();
	void DoPWD();
	void DoPASV();
  bool DoDelete(char *);
	int  CheckFds(char *);

private:
	char szBuffer[FTP_BUFFER_SIZE];  /* buffer used to read/write */
	char szUser[20];          /* stores username */
	char szPass[256];         /* stores user password */
  char szServerAddr[20];	// e.g. "123.456.78.9"
  char szServerPort[8];		// e.g. "65898"
	int Connected;     /* flag for connect status */
  bool passive;			// In passive mode
	char gsServerReplyText[2048];			// FTP server reply buffer
	char gszCommandBuffer[100];				// Buffer used to format FTP commands
	LPSTR lpszFunctionName;						// Pointer to function names
	char gsReplyBuffer[2048];					// Buffer for peeking at FTP server replies
	char gsDataBuffer[4096];					// Storage buffer for data channel

	SOCKET hListenSocket;
	SOCKET hControlSocket;
	SOCKET hDataSocket;
	int bSendPort;
	int ReadCommand;
	int bMode;

  int GetReplyCode(LPSTR lpszServerReply);
  int ReadReplyLine(LPSTR lpszReplyBuffer);
  char * GetFTPServerReplyText();
  int ReadFTPServerReply(SOCKET hControlChannel);
  int SendFTPCommand(SOCKET hControlChannel, LPSTR gszCommandBuffer, int len);
  int ReadDataChannel(SOCKET hControlSocket,SOCKET hDataSocket, LPSTR lpszFileName);
  //int TransferFile(SOCKET hControlSocket,SOCKET hDataSocket, HFILE hFile);
  //void ExtractFileName(LPSTR lpPathString,LPSTR lpszFileName);
	SOCKET ConnectFTPControlSocket(LPSTR lpszHost);
	SOCKET RequestDataConnection(SOCKET hControlSocket, SOCKET hListenSocket);
  SOCKET CreateListenSocket(SOCKET hControlSocket);
  SOCKET AcceptDataConnection(SOCKET hListenSocket);
  bool InitWinsock();
 	bool CloseWinsock();


  int GetLine();
	int GetReply();
  void GetError();
	void CleanUp();
	int SendControlMsg(char *, int);
	int SendDataMsg( char *szBuffer, int len);
	int ConnectToServer(char *name, char *port);
	int GetListenSocket();
	//int InitWinsock();
	int AcceptConnection();
	void CloseControlConnection( void );
	void CloseDataConnection( int hDataSocket );
	void CloseListenSocket();
	int ReadDataMsg( char *szBuffer, int len);
	void GetPassword( char *szPass );
	int GetUnixInput(char *command);
	int GetWin32Input( char *command);
	void GetFile( char *fname);
	bool PutFile( char *fname);
	int ReadControlMsg( char *szBuffer, int len);
	int CheckControlMsg( char *szPtr, int len);
	int CheckInput();
  bool SetupDataConnection();
	bool IsReadyToRead(SOCKET hSocket);
};

enum FtpCommand {
  FTP_NOOP,
  FTP_CONNECT,
  FTP_USER,
  FTP_PASSWORD,
  FTP_CD,
  FTP_LCD,
  FTP_PUT,
  FTP_CLOSE
};                            
class TUploadDlgClient;
class FtpUpload : public ftp_q {
  public:
		FtpUpload(TUploadDlgClient *activity);
    //FtpThread(FtpActionArray *array);
    ~FtpUpload();
    bool PutStationInfo();	// Upload station information
    bool PutSPS(float sps);					// Upload sample rate (samples/sec)
    bool Login();
    TUploadDlgClient *activity;
    bool Upload(char *filespec);
  protected:
    char *Path;
  private:
    int days_to_save;
    char *URL;
    char *Username;
    char *Password;
};


//#endif