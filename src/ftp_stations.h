#ifndef __ftp__
#define __ftp__

#include <io.h>
#include "progress.h"
#define DEFAULT_PORT_NUM 21
#define PASSWORD_LENGTH 256
#define EINTR WSAEINTR
#define bzero(x,y) memset(x,0,y)
#define bcopy(x,y,z) memcpy(y,x,z)
#define WINSOCK_VERSION 0x0101		// Program requires Winsock version 1.1
#define DEFAULT_PROTOCOL 0				// No protocol specified, use default
#define NO_FLAGS 0								// No special flags specified
#define QUEUE_SIZE 1							// Size of connection queue for listen()
#define LINEFEED 0x0A							// Line-feed character
#define MULTILINE_REPLY '-'				// FTP flag for multiline replies
#define EOL_MARKER "\r\n"					// FTP end-of-line marker
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
class ftp {
public:
	ftp();
	~ftp();
	bool DoOpen(char *);
	char * DoList(char *);
	bool DoCD(char *);
  bool DoMKD(char *);
  bool DoRMD(char *);				// Remove directory
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
	char gsServerReplyText[2048];			// FTP server reply buffer
	char szUser[20];          /* stores username */
	char szPass[256];         /* stores user password */
  char szServerAddr[20];	// e.g. "123.456.78.9"
  char szServerPort[8];		// e.g. "65898"
	int Connected;     /* flag for connect status */
  bool passive;			// In passive mode

	int hListenSocket;
	int hControlSocket;
	int hDataSocket;
	int bSendPort;
	int ReadCommand;
	int bMode;

	int GetReply();
	int ReadFTPServerReply(SOCKET hControlSocket);
	int GetReplyCode(LPSTR lpszServerReply);
  int GetLine();
  void GetError();
	void CleanUp();
	int SendControlMsg(char *, int);
	int SendFTPCommand(SOCKET hControlChannel, char * gszCommandBuffer, int len);
	int SendDataMsg( char *szBuffer, int len);
	int ConnectToServer(char *name, char *port);
	int GetListenSocket();
	int InitWinsock();
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
	int ReadReplyLine(LPSTR lpszReplyBuffer);
	bool IsReadyToRead(SOCKET hSocket);
  char *GetFTPServerReplyText();

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
class FtpStations : public ftp {
  public:
		FtpStations(Progress *pro);
    ~FtpStations();
    bool GetStationXML();			// Get IRIS stations info
    bool Login();
  protected:
    char *Path;
  private:
  	Progress *pro;
    char *URL;
    char *Username;
    char *Password;
};


#endif