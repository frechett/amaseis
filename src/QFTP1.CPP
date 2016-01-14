#include <stdlib.h>								// Required for atoi()
#include	"..\winsock.h"					// Winsock header file

#define PROG_NAME "Quick FTP Version 1"

#define HOST_NAME "NIC.DDN.MIL"		// FTP server host
#define PASSWORD "PASS guest\r\n"	// Password for FTP server host
#define WINSOCK_VERSION 0x0101		// Program requires Winsock version 1.1
#define DEFAULT_PROTOCOL 0				// No protocol specified, use default
#define NO_FLAGS 0								// No special flags specified

char szCommandBuffer[100];				// Buffer for FTP commands
LPSTR lpszFunctionName;						// Pointer for function names

UINT GetReplyCode(LPSTR lpszServerReply)
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

UINT ReadFTPServerReply(SOCKET hControlChannel)
	{
		char sReceiveBuffer[1024];	// Data-storage buffer for FTP server reply
		int iLength;								// Length of data received from FTP server
		
		lpszFunctionName = "ReadFTPServerReply";

    if ((iLength = recv(hControlChannel, (LPSTR)sReceiveBuffer, 
    			sizeof(sReceiveBuffer), NO_FLAGS)) == SOCKET_ERROR)
  		{
				int iWinsockErr = WSAGetLastError();
				wsprintf(szCommandBuffer, "Error %d from the recv() function!!", 
							iWinsockErr);   
				MessageBeep(MB_ICONHAND);
				MessageBox(NULL, szCommandBuffer, lpszFunctionName, 
							MB_OK|MB_ICONSTOP);
			  // Return 999 to indicate an error has occurred
				return(999);
			} 
					
		sReceiveBuffer[iLength] = '\0';
		MessageBeep(MB_ICONASTERISK);
    MessageBox(NULL, (LPSTR)sReceiveBuffer, lpszFunctionName,
          MB_OK|MB_ICONINFORMATION);
    
    // Extract the reply code from the server reply and return as an integer      
		return(GetReplyCode(sReceiveBuffer));	            
	}
	
UINT SendFTPCommand(SOCKET hControlChannel, LPSTR szCommandBuffer)
	{
		lpszFunctionName = "SendFTPCommand";

		// Send the FTP command
		if ((send(hControlChannel, (LPSTR)szCommandBuffer, 
					lstrlen(szCommandBuffer), NO_FLAGS)) == SOCKET_ERROR)
			{
				int iWinsockErr = WSAGetLastError();
				wsprintf(szCommandBuffer, "Error %d from the send() function!!", 
							iWinsockErr);   
				MessageBeep(MB_ICONHAND);
				MessageBox(NULL, szCommandBuffer, lpszFunctionName, 
							MB_OK|MB_ICONSTOP);
			  // Return 999 to indicate an error has occurred
				return(999);
			} 
		
		// Read the server's reply and return the reply code as an integer
		return(ReadFTPServerReply(hControlChannel));	            
	}
	
UINT AnonymousFTPLogIn(SOCKET hControlSocket)
	{
		int nReplyCode;									// FTP server reply code
    int iMsg = 0;										// Index subscript for FTP commands
    
		lpszFunctionName = "AnonymousFTPLogIn";
		
    char	*LoginCommand[]	=	
    		{
    			"USER anonymous\r\n",
    			PASSWORD,
			    NULL
			  };
			  
    do
    	{
    		nReplyCode = SendFTPCommand(hControlSocket, 
    					(LPSTR)LoginCommand[iMsg++]);
			} 
		while (LoginCommand[iMsg] && nReplyCode < 400);
		
		return(nReplyCode);
	}

SOCKET ConnectFTPControlSocket(LPSTR lpszHost)
	{
		LPHOSTENT lpHostEnt;					// Internet host information structure
		SOCKADDR_IN sockAddr;					// Socket address structure
		LPSERVENT lpServEnt;					// Service information structure
		short nProtocolPort;					// Protocol port
		int nConnect;									// Socket connection results
		SOCKET hControlSocket;				// Control socket handle
		
		lpszFunctionName = "ConnectFTPControlSocket";

		if (!(lpHostEnt = gethostbyname(lpszHost)))
			{
				int iWinsockErr = WSAGetLastError();
				wsprintf(szCommandBuffer,
							"Error #%d while resolving address for %s", 
							iWinsockErr, lpszHost);
				MessageBeep(MB_ICONHAND);
				MessageBox(NULL, szCommandBuffer, lpszFunctionName, 
							MB_OK|MB_ICONSTOP);
				return(INVALID_SOCKET);
			}
			
		if ((hControlSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP))
					== INVALID_SOCKET)
			{
				int iWinsockErr = WSAGetLastError();
				wsprintf(szCommandBuffer,
							"Error #%d occurred while creating socket!!", 
							iWinsockErr);
				MessageBeep(MB_ICONHAND);
				MessageBox(NULL, szCommandBuffer, lpszFunctionName, 
							MB_OK|MB_ICONSTOP);
				return(INVALID_SOCKET);
			}
						
		lpServEnt = getservbyname("ftp", DEFAULT_PROTOCOL);
						
		if (lpServEnt == NULL)
			nProtocolPort = htons(IPPORT_FTP);	
		else
			nProtocolPort = lpServEnt->s_port;

		// Define the socket address	
		sockAddr.sin_family = AF_INET;		
		sockAddr.sin_port = nProtocolPort;
		sockAddr.sin_addr = *((LPIN_ADDR)*lpHostEnt->h_addr_list);
						
		// Connect the socket
		if( nConnect = connect(hControlSocket, (LPSOCKADDR)&sockAddr, 
					sizeof(sockAddr)))
			{
				int iWinsockErr = WSAGetLastError();
				wsprintf(szCommandBuffer,
							"Error #%d occurred while connecting socket!!", 
							iWinsockErr);
				MessageBeep(MB_ICONHAND);
				MessageBox(NULL, szCommandBuffer, lpszFunctionName, 
							MB_OK|MB_ICONSTOP);
				return(INVALID_SOCKET);
			}
			
		if (ReadFTPServerReply(hControlSocket) >= 400)
			return(INVALID_SOCKET);
		else
			return(hControlSocket);
	}

int PASCAL WinMain(HANDLE hInstance, HANDLE hPrevInstance, 
			LPSTR lpszCmdParam, int nCmdShow)
	{
		WSADATA wsaData;							// Winsock implementation details
		SOCKET hControlChannel;				// Socket handle for the control channel
		UINT nReplyCode;							// FTP reply code

		lpszFunctionName = "WinMain";
		
		if (WSAStartup(WINSOCK_VERSION, &wsaData))
			{
				MessageBeep(MB_ICONHAND);
				MessageBox(NULL, "Could not load Windows Sockets DLL.", 
							lpszFunctionName, MB_OK|MB_ICONSTOP);
				return(NULL);
			}
						
		hControlChannel = ConnectFTPControlSocket((LPSTR)HOST_NAME);
		// Note that from a DLL, here we would return 
		// our control channel handle (if valid) for 
		// use by other program modules.

		if (hControlChannel != INVALID_SOCKET)
			{
				// If we have a control channel, then login.
				nReplyCode = AnonymousFTPLogIn(hControlChannel);
				             
				if (nReplyCode == 230)	// User logged in; we can proceed.
					{
						SendFTPCommand(hControlChannel, "QUIT\r\n");
						closesocket(hControlChannel);
					}
			}
		
		WSACleanup();
		MessageBeep(MB_ICONEXCLAMATION);
		MessageBox(NULL, "THE END!!", PROG_NAME, MB_OK|MB_ICONEXCLAMATION);
		return(NULL); 
	}
