#include <stdlib.h>								// Required for atoi()
#include "..\winsock.h"						// Winsock header file

#define PROG_NAME "Quick FTP Version 2"

#define HOST_NAME "NIC.DDN.MIL"		// FTP server host
#define PASSWORD "PASS guest\r\n"	// Password for FTP server host
#define WINSOCK_VERSION 0x0101		// Program requires Winsock version 1.1
#define DEFAULT_PROTOCOL 0				// No protocol specified, use default
#define NO_FLAGS 0								// No special flags specified
#define QUEUE_SIZE 1							// Size of connection queue for listen()
#define LINEFEED 0x0A							// Line-feed character

char gsServerReplyBuffer[2048];		// FTP server reply buffer
char gszCommandBuffer[100];				// Buffer used to format FTP commands
LPSTR lpszFunctionName;						// Pointer to function names

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
		// Note that the function now uses a global receive buffer so 
		// in the future other program modules can access the full
		// text of the server's reply rather than just the
		// server's reply code (which this function returns.)
		
		int iBytesRead;						// Bytes read from the control channel
		int iBufferLength;				// Length of the server reply buffer
		int iEnd;									// Index into the server reply buffer
		int iSpaceRemaining;			// Space remaining in the buffer
		
		lpszFunctionName = "ReadFTPServerReply";

		iEnd = 0;
		iBufferLength = iSpaceRemaining = sizeof(gsServerReplyBuffer);
		do
			{
				iSpaceRemaining -= iEnd;
				iBytesRead = recv(hControlChannel, 
							(LPSTR)(gsServerReplyBuffer+iEnd), iSpaceRemaining, NO_FLAGS);
    			
    		iEnd+=iBytesRead;
    		
    		// Make sure CRLF was not the the last byte pair received. 
    		// Otherwise, recv() will wait forever for the next packet.
    		if (*(gsServerReplyBuffer+(iEnd-2)) == '\r'  && 
    					*(gsServerReplyBuffer+(iEnd-1)) == '\n')
    			break;
			}
		while (iBytesRead > 0 && iEnd < iBufferLength); 
		
    if (iBytesRead == SOCKET_ERROR)
  		{
				int iWinsockErr = WSAGetLastError();
				wsprintf(gszCommandBuffer, "Error %d from the recv() function!!", 
							iWinsockErr);   
				MessageBeep(MB_ICONSTOP);
				MessageBox(NULL, gszCommandBuffer, lpszFunctionName, 
							MB_OK|MB_ICONSTOP);
			  // Return 999 to indicate an error has occurred
				return(999);
			} 
					
		gsServerReplyBuffer[iEnd] = '\0';
		MessageBeep(MB_ICONINFORMATION);
    MessageBox(NULL, (LPSTR)gsServerReplyBuffer, lpszFunctionName,
          MB_OK|MB_ICONINFORMATION);
          
    // Extract the reply code from the server reply and return as an integer
		return(GetReplyCode(gsServerReplyBuffer));	            
	}	
	
UINT SendFTPCommand(SOCKET hControlChannel, LPSTR gszCommandBuffer)
	{
		lpszFunctionName = "SendFTPCommand";

		// Send the FTP command
		if ((send(hControlChannel, (LPSTR)gszCommandBuffer, 
					lstrlen(gszCommandBuffer), NO_FLAGS)) == SOCKET_ERROR)
			{
				int iWinsockErr = WSAGetLastError();
				wsprintf(gszCommandBuffer, "Error %d from the send() function!!", 
							iWinsockErr);   
				MessageBeep(MB_ICONSTOP);
				MessageBox(NULL, gszCommandBuffer, lpszFunctionName, 
							MB_OK|MB_ICONSTOP);
			  // Return 999 to indicate an error has occurred
				return(999);
			} 
		
		// Read the server's reply and return the reply code as an integer
		return(ReadFTPServerReply(hControlChannel));	            
	}	
	
BOOL ReadDataChannel(SOCKET hDataSocket, LPSTR lpszFileName)
	{
		char sDataBuffer[4096];		// Data-storage buffer for the data channel
		int nBytesRecv;						// Bytes received from the data channel
		HFILE hFile;							// File handle for data file
    OFSTRUCT openFileBuff;		// The Windows open file data structure 
		LONG lData = 0L;					// Bytes received and written to the data file

		lpszFunctionName = "ReadDataChannel";

		if ((hFile = OpenFile(lpszFileName, (OFSTRUCT far *)&openFileBuff, 
					OF_CREATE)) == HFILE_ERROR)
			{
				_lclose(hFile);
				wsprintf(gszCommandBuffer,"Error creating file: %s\n", lpszFileName);
				MessageBeep(MB_ICONSTOP);
				MessageBox(NULL, gszCommandBuffer, lpszFunctionName, MB_OK|MB_ICONSTOP);
				return(FALSE);
			}
			
    do	// Ready read and write
    	{
    		nBytesRecv = recv(hDataSocket, (LPSTR)&sDataBuffer, 
    					sizeof(sDataBuffer), NO_FLAGS);
    					
    		lData += nBytesRecv;
    		if (nBytesRecv > 0 )
    			{
    				if (HFILE_ERROR == _lwrite (hFile, sDataBuffer, nBytesRecv))
    					{
								wsprintf(gszCommandBuffer,"Error writing file: %s\n", lpszFileName);
								MessageBeep(MB_ICONSTOP);
								MessageBox(NULL, gszCommandBuffer, lpszFunctionName, 
											MB_OK|MB_ICONSTOP);
	    					break;
    					}
		    	}
			}
		while (nBytesRecv > 0);
								
		// Close the file and check for error returns.
		_lclose(hFile);
		if (nBytesRecv == SOCKET_ERROR)
			{ 
				int iWinsockErr = WSAGetLastError();
				wsprintf(gszCommandBuffer,
							"Error #%d occurred receiving: %s\n", 
							iWinsockErr, lpszFileName);
				MessageBeep(MB_ICONSTOP);
				MessageBox(NULL, gszCommandBuffer, lpszFunctionName, 
							MB_OK|MB_ICONSTOP);
				return(FALSE);
			}
		else
			{
				wsprintf(gszCommandBuffer,"%lu bytes written to %s\n", lData, 
							lpszFileName);
				MessageBeep(MB_ICONINFORMATION);
				MessageBox(NULL, gszCommandBuffer, lpszFunctionName, 
							MB_OK|MB_ICONINFORMATION);
			}

		return(TRUE);
	}	

UINT AnonymousFTPLogIn(SOCKET hControlSocket)
	{
		int nReplyCode;									// FTP server reply code
    int iMsg;												// Index subscript for FTP commands
    
		lpszFunctionName = "AnonymousFTPLogIn";
		
    char	*LoginCommand[]	=	
    		{
    			"USER anonymous\r\n",
    			PASSWORD,
			    0
			  };
			  
    iMsg = 0;
    do
    	{
    		nReplyCode = SendFTPCommand(hControlSocket, 
    					(LPSTR)LoginCommand[iMsg]);
				iMsg++;
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
				wsprintf(gszCommandBuffer,
							"Error #%d while resolving address for %s", 
							iWinsockErr, lpszHost);
				MessageBeep(MB_ICONSTOP);
				MessageBox(NULL, gszCommandBuffer, lpszFunctionName, 
							MB_OK|MB_ICONSTOP);
				return(INVALID_SOCKET);
			}
			
		if ((hControlSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP))
					== INVALID_SOCKET)
			{
				int iWinsockErr = WSAGetLastError();
				wsprintf(gszCommandBuffer,
							"Error #%d occurred while creating socket!!", 
							iWinsockErr);
				MessageBeep(MB_ICONSTOP);
				MessageBox(NULL, gszCommandBuffer, lpszFunctionName, 
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
				wsprintf(gszCommandBuffer,
							"Error #%d occurred while connecting socket!!", 
							iWinsockErr);
				MessageBeep(MB_ICONSTOP);
				MessageBox(NULL, gszCommandBuffer, lpszFunctionName, 
							MB_OK|MB_ICONSTOP);
				return(INVALID_SOCKET);
			}
			
		if (ReadFTPServerReply(hControlSocket) >= 400)
			return(INVALID_SOCKET);
		else
			return(hControlSocket);
	}	

SOCKET RequestDataConnection(SOCKET hControlSocket, SOCKET hListenSocket)
	{
		SOCKADDR_IN sockAddr;					// Socket address structure
    int iLength;									// Length of the address structure
    UINT nLocalPort;							// Local port for listening
		UINT nReplyCode;							// FTP server reply code

		lpszFunctionName = "RequestDataConnection";

    // Get the address for the hListenSocket
		iLength = sizeof(sockAddr);
		if (getsockname(hListenSocket, (LPSOCKADDR)&sockAddr, 
					&iLength) == SOCKET_ERROR)
			{
				int iWinsockErr = WSAGetLastError();
				wsprintf(gszCommandBuffer,
							"Error #%d occurred while getting listen socket name!!", 
							iWinsockErr);
				MessageBeep(MB_ICONSTOP);
				MessageBox(NULL, gszCommandBuffer, lpszFunctionName, 
							MB_OK|MB_ICONSTOP);
				return(INVALID_SOCKET);
			}

		// Extract the local port from the hListenSocket
		nLocalPort = sockAddr.sin_port;
								
		// Now, reuse the socket address structure to 
		// get the IP address from the control socket.
		if (getsockname(hControlSocket, (LPSOCKADDR)&sockAddr, 
					&iLength) == SOCKET_ERROR)
			{
				int iWinsockErr = WSAGetLastError();
				wsprintf(gszCommandBuffer,
							"Error #%d occurred while getting control socket name!!", 
							iWinsockErr);
				MessageBeep(MB_ICONSTOP);
				MessageBox(NULL, gszCommandBuffer, lpszFunctionName, 
							MB_OK|MB_ICONSTOP);
				return(INVALID_SOCKET);
			}
					
		// Format the PORT command with the correct numbers.
		wsprintf(gszCommandBuffer, "PORT %d,%d,%d,%d,%d,%d\r\n", 
					sockAddr.sin_addr.S_un.S_un_b.s_b1, 
					sockAddr.sin_addr.S_un.S_un_b.s_b2,
					sockAddr.sin_addr.S_un.S_un_b.s_b3,
					sockAddr.sin_addr.S_un.S_un_b.s_b4,
					// Remember, the port is in network byte order. The FTP server
					// expects to see the high-order byte from the port (in terms of 
					// the network byte order) first. As such, from a PC, (which uses
					// little endian byte order) the program must transmit the low-order 
					// byte first and then the high-order. (If this is confusing, see the
					// "Internet Programming" chapter entitled "Time and the Network
					// Byte Order".
					nLocalPort & 0xFF,	
					nLocalPort >> 8);
																
		// Tell the server which port to use for data.
		if (nReplyCode = SendFTPCommand(hControlSocket, gszCommandBuffer)
					!= 200)
			{
				wsprintf(gszCommandBuffer, 
							"Error %d from PORT command to server!", 
							nReplyCode);
				MessageBeep(MB_ICONSTOP);
				MessageBox(NULL, gszCommandBuffer, lpszFunctionName, 
							MB_OK|MB_ICONSTOP);
				return(INVALID_SOCKET);
			}
		else
			return(hListenSocket);
	}	
	
SOCKET CreateListenSocket(SOCKET hControlSocket)
	{
		SOCKADDR_IN sockAddr;					// Socket address structure
		SOCKET hListenSocket;					// Handle for the listener socket
		
		lpszFunctionName = "CreateListenSocket";

		if ((hListenSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) 
					== INVALID_SOCKET)
			{
				int iWinsockErr = WSAGetLastError();
				wsprintf(gszCommandBuffer,
							"Error #%d occurred while creating socket!!", 
							iWinsockErr);
				MessageBeep(MB_ICONSTOP);
				MessageBox(NULL, gszCommandBuffer, lpszFunctionName, 
							MB_OK|MB_ICONSTOP);
				return(INVALID_SOCKET);
			}
		// Let the system assign a socket address	
		sockAddr.sin_family = AF_INET;		
		sockAddr.sin_port = htons(0);		// htons() is just a reminder.
		sockAddr.sin_addr.s_addr = INADDR_ANY;
						
		// Bind the socket
		if (bind(hListenSocket, (LPSOCKADDR)&sockAddr, sizeof(sockAddr)))
			{
				int iWinsockErr = WSAGetLastError();
				wsprintf(gszCommandBuffer,
							"Error #%d occurred while binding socket!!", 
							iWinsockErr);
				MessageBeep(MB_ICONSTOP);
				MessageBox(NULL, gszCommandBuffer, lpszFunctionName, 
							MB_OK|MB_ICONSTOP);
				return(INVALID_SOCKET);
			}

		// Listen for the FTP server connection
		if (listen(hListenSocket, QUEUE_SIZE))	
			{
				int iWinsockErr = WSAGetLastError();
				wsprintf(gszCommandBuffer, 
							"Error %d on listen socket!!", 
							iWinsockErr);   
				MessageBeep(MB_ICONSTOP);
				MessageBox(NULL, gszCommandBuffer, lpszFunctionName, 
							MB_OK|MB_ICONSTOP);
				return(INVALID_SOCKET);
			}
		
		// Ask the server to connect to the port monitored by the listener socket
		return(RequestDataConnection(hControlSocket, hListenSocket));
	}	
		
SOCKET AcceptDataConnection(SOCKET hListenSocket)
	{
		SOCKET hDataSocket;				// Handle for socket that receives data
		SOCKADDR_IN sockAddr;			// Socket address structure
		int iAddrLength;					// The length of the socket address
	
		lpszFunctionName = "AcceptDataConnection";

		hDataSocket = accept(hListenSocket, (LPSOCKADDR)&sockAddr, 
				&iAddrLength); 
				
		// Close the listener socket since it is no longer needed
		closesocket(hListenSocket);
		
		if (hDataSocket == INVALID_SOCKET)
			{
				int iWinsockErr = WSAGetLastError();
				wsprintf(gszCommandBuffer, 
							"Error %d while accepting a data socket connection!!", 
							iWinsockErr);   
				MessageBeep(MB_ICONSTOP);
				MessageBox(NULL, gszCommandBuffer, lpszFunctionName, 
							MB_OK|MB_ICONSTOP);
				return(INVALID_SOCKET);
			}
		else
			return(hDataSocket);
	}	

VOID DemonstrateCommand(SOCKET hControlChannel)
	{
		SOCKET hDataChannel;		// Handle for socket to receive server data
		SOCKET hListenSocket;		// Handle for the listener socket
		UINT nReplyCode;				// FTP server reply code

		lpszFunctionName = "DemonstrateCommand";
			  
		if ((hListenSocket = CreateListenSocket(hControlChannel))
			 	== INVALID_SOCKET)
			return;
					
		// Hard code the NLST command for testing.
		if (nReplyCode = SendFTPCommand(hControlChannel, 
					"NLST\r\n") >= 400)
			return;
			
		// Accept the data connection from the server. Note that
		// AcceptDataConnection() closes the hListenSocket.
		if ((hDataChannel = AcceptDataConnection(hListenSocket))
					== INVALID_SOCKET)
			return;
			
		ReadDataChannel(hDataChannel, "NLST.TXT");
		closesocket(hDataChannel);
		return;
	} 

int PASCAL WinMain(HANDLE hInstance, HANDLE hPrevInstance, 
			LPSTR lpszCmdParam, int nCmdShow)
	{
		WSADATA wsaData;							// Winsock implementation details
		SOCKET hControlChannel;				// Socket handle for the control channel
		UINT nReplyCode;							// FTP server reply code

		lpszFunctionName = "WinMain";
		
		if (WSAStartup(WINSOCK_VERSION, &wsaData))
			{
				MessageBeep(MB_ICONSTOP);
				MessageBox(NULL, "Could not load Windows Sockets DLL.", 
							lpszFunctionName, MB_OK|MB_ICONSTOP);
				return(NULL);
			}
						
		hControlChannel = ConnectFTPControlSocket((LPSTR)HOST_NAME);

		if (hControlChannel != INVALID_SOCKET)
			{
				// If the control socket is valid, perform an anonymous FTP login.
				nReplyCode = AnonymousFTPLogIn(hControlChannel);
				             
				if (nReplyCode == 230)	// User logged in; okay to proceed.
					{
						DemonstrateCommand(hControlChannel);
						SendFTPCommand(hControlChannel, "QUIT\r\n");
						closesocket(hControlChannel);
					}
			}
		
		WSACleanup();
		MessageBeep(MB_ICONEXCLAMATION);
		MessageBox(NULL, "THE END!!", PROG_NAME, MB_OK|MB_ICONEXCLAMATION);
		return(NULL); 
	}	
