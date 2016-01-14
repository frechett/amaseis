#include <stdlib.h>								// Required for atoi()
#include <string.h>								// Required for _fstrstr()
#include "..\winsock.h"						// Winsock header file

#define PROG_NAME "Quick FTP.DLL"

#define WINSOCK_VERSION 0x0101		// Program requires Winsock version 1.1
#define DEFAULT_PROTOCOL 0				// No protocol specified, use default
#define NO_FLAGS 0								// No special flags specified
#define QUEUE_SIZE 1							// Size of connection queue for listen()
#define LINEFEED 0x0A							// Line-feed character
#define MULTILINE_REPLY '-'				// FTP flag for multiline replies
#define EOL_MARKER "\r\n"					// FTP end-of-line marker

char gsServerReplyText[2048];			// FTP server reply buffer
char gszCommandBuffer[100];				// Buffer used to format FTP commands
LPSTR lpszFunctionName;						// Pointer to function names
char gsReplyBuffer[2048];					// Buffer for peeking at FTP server replies
char gsDataBuffer[4096];					// Storage buffer for data channel

extern "C" UINT FAR PASCAL GetReplyCode(LPSTR lpszServerReply)
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

extern "C" BOOL FAR PASCAL IsReadyToRead(SOCKET hSocket)
	{
		FD_SET setReadyToRead;			// Socket set to test ready-to-read status
		TIMEVAL timeTimeOut;				// Amount of time to wait for status change
		int nReady;									// Ready-to-read flag
		
		lpszFunctionName = "IsReadyToRead";

		timerclear(&timeTimeOut);
		FD_ZERO(&setReadyToRead);
		FD_SET(hSocket, &setReadyToRead);
		
		if ((nReady = select(NULL, (LPFD_SET)&setReadyToRead, NULL, NULL, 
					&timeTimeOut)) == SOCKET_ERROR)
			{
				int iWinsockErr = WSAGetLastError();
				wsprintf(gszCommandBuffer, "Error %d from the select() function!!", 
							iWinsockErr);   
				MessageBeep(MB_ICONSTOP);
				MessageBox(NULL, gszCommandBuffer, lpszFunctionName, 
							MB_OK|MB_ICONSTOP);
				return(FALSE);
			}
			
		return(nReady ? TRUE : FALSE);
	}

extern "C" UINT FAR PASCAL ReadReplyLine(LPSTR lpszReplyBuffer)
	{
		LPSTR lpEOL;										// End-of-line
		UINT nLimitReplyBytes;					// Length of first reply in the buffer
		PSTR pLastLineCode = "123?";		// Storage buffer for reply code
		LPSTR lpLastLine;								// Pointer to the last line
		int i;													// General purpose index

 		// lpszFunctionName = "ReadReplyLine";

		nLimitReplyBytes = 0;
		
		if (*(lpszReplyBuffer+3) == MULTILINE_REPLY)		
			{
				// Get the code from the reply buffer
				for (i = 0; i <3; i++ )
					*(pLastLineCode+i) = *(lpszReplyBuffer+i);
        
        // Use a trailing space to look for the last line
				*(pLastLineCode+i) = ' ';
				
				// Search the buffer for the last line
				if ((lpLastLine = _fstrstr(lpszReplyBuffer, pLastLineCode)))
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


extern "C" LPSTR _export FAR PASCAL  GetFTPServerReplyText(VOID)
	{
		return((LPSTR)gsServerReplyText);	// Return full text of server's reply
	}

extern "C" UINT FAR PASCAL  ReadFTPServerReply(SOCKET hControlChannel)
	{
		int iBytesRead;						// Bytes read from the control channel
		int iBufferLength;				// Length of the server reply buffer
		int iEnd;									// Index into the server reply buffer
		int iSpaceRemaining;			// Space remaining in the buffer
		int iReplySize;						// Length of first reply in the buffer
		
 		lpszFunctionName = "ReadFTPServerReply";
			
		// Be cautious and zero fill the buffer
		_fmemset(gsReplyBuffer, 0, sizeof(gsReplyBuffer));
		
		// Be tricky and peek ahead to see what's in the buffer.
		if ((iBytesRead = recv(hControlChannel, 
					(LPSTR)(gsReplyBuffer), sizeof(gsReplyBuffer), MSG_PEEK)) > 0)
			{
				if ((iReplySize = ReadReplyLine(gsReplyBuffer)) == 0)
					iBufferLength = iSpaceRemaining = sizeof(gsServerReplyText);
				else 
					iBufferLength = iSpaceRemaining = iReplySize;
			}
		else
			{
				MessageBeep(MB_ICONINFORMATION);
				MessageBox(NULL, 
							(LPSTR)"Nothing on the control channel to read!",
					  	lpszFunctionName, MB_OK|MB_ICONINFORMATION);
				return(999);
			}

		iEnd = 0;	// Start with zero bytes.
		do
			{
				iSpaceRemaining -= iEnd;
				iBytesRead = recv(hControlChannel, 
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
				wsprintf(gszCommandBuffer, "Error %d from the recv() function!!", 
							iWinsockErr);   
				MessageBeep(MB_ICONSTOP);
				MessageBox(NULL, gszCommandBuffer, lpszFunctionName, 
							MB_OK|MB_ICONSTOP);
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
	
extern "C" UINT _export FAR PASCAL  SendFTPCommand(SOCKET hControlChannel, LPSTR gszCommandBuffer)
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
	
extern "C" UINT _export FAR PASCAL  ReadDataChannel(SOCKET hControlSocket, 
			SOCKET hDataSocket, LPSTR lpszFileName)
	{
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
    		nBytesRecv = recv(hDataSocket, (LPSTR)&gsDataBuffer, 
    					sizeof(gsDataBuffer), NO_FLAGS);
    					
    		lData += nBytesRecv;
    		if (nBytesRecv > 0 )
    			{
    				if (HFILE_ERROR == _lwrite (hFile, gsDataBuffer, nBytesRecv))
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
		else if (lData == 0)
			{
				MessageBeep(MB_ICONINFORMATION);
				MessageBox(NULL, 
							(LPSTR)"Nothing on the data channel to read!",
					  	lpszFunctionName, MB_OK|MB_ICONINFORMATION);
				return(FALSE);
			}
#ifdef _DEBUG			
		else
			{
				wsprintf(gszCommandBuffer,"%lu bytes written to %s\n", lData, 
							lpszFileName);
				MessageBeep(MB_ICONINFORMATION);
				MessageBox(NULL, gszCommandBuffer, lpszFunctionName, 
							MB_OK|MB_ICONINFORMATION);
			}
#endif		
		// Read the control channel to see what the server thought about it.
		return(ReadFTPServerReply(hControlSocket));
	}

extern "C" UINT _export FAR PASCAL TransferFile(SOCKET hControlSocket, 
			SOCKET hDataSocket, HFILE hFile)
	{
		char szFileData[1024];	 			// Buffer to hold file data
		int nCharRecv;								// Number of characters received
		char szMsg[100];							// General purpose buffer for messages
				
		nCharRecv = recv(hDataSocket, (LPSTR)&szFileData, sizeof(szFileData), NO_FLAGS);
		if (nCharRecv > 0 )
			{
				if (HFILE_ERROR == _lwrite (hFile, szFileData, nCharRecv))
					{
						_lclose(hFile);
						wsprintf(szMsg,"%d Error occurred during recv()!", nCharRecv);
						MessageBox(NULL, szMsg, PROG_NAME, MB_OK|MB_ICONSTOP);
						return(HFILE_ERROR);
					}
    	}
    else if (nCharRecv == SOCKET_ERROR)
			{
				_lclose(hFile);
				nCharRecv = WSAGetLastError();
				wsprintf(szMsg,"%d Error occurred during recv()!", nCharRecv);
				MessageBox(NULL, szMsg, PROG_NAME, MB_OK|MB_ICONSTOP);
				return(SOCKET_ERROR);
			}

    if (nCharRecv == 0)
    	{
    		_lclose(hFile);
				ReadFTPServerReply(hControlSocket);
    	}
 		
		return(nCharRecv);
	}

extern "C" void _export FAR PASCAL ExtractFileName(LPSTR lpPathString, 
			LPSTR lpszFileName)
	{
		LPSTR lp;									// General purpose pointer
		UINT i;										// General purpose index
		UINT iExtLength = 0;			// Length of file extension (following the dash)

		// Find the last forward slash (assume UNIX/NT directory conventions)		
		if (lp = _fstrrchr(lpPathString, '/'))
			lp++;
		else
			lp = lpPathString;
			
		// Loop while the length of the name is less than 8 characters 
		// (a valid DOS length), the character is not a dot and the 
		// pointer is valid
		for (i = 0; i < 8 && *lp != '.' && lp; i++)
			*(lpszFileName+i) = *(lp++);
				    
		// Add a dot to mark the start of the file extension
		*(lpszFileName+i) = '.';
		i++;
				
		// Find the last dot and use up to the next three characters for
		// the new file name.
		if (lp = _fstrrchr(lpPathString, '.'))
				for (iExtLength = 0; iExtLength < 3 && (*(lp)); iExtLength++)
						*(lpszFileName+(i++)) = *(++lp);
						
		// Null-terminate the new file name
		*(lpszFileName+i) = '\0';

		return;
	}

extern "C" HFILE _export FAR PASCAL  CreateTransferFile(LPSTR lpszFileName)
	{
		HFILE hFile;							// File handle for data file
    OFSTRUCT openFileBuff;		// The Windows open file data structure

		lpszFunctionName = "CreateTransferFile";
		
		if ((hFile = OpenFile(lpszFileName, (OFSTRUCT far *)&openFileBuff, 
					OF_CREATE)) == HFILE_ERROR)
			{
				_lclose(hFile);
				wsprintf(gszCommandBuffer,"Error creating file: %s\n", lpszFileName);
				MessageBeep(MB_ICONSTOP);
				MessageBox(NULL, gszCommandBuffer, lpszFunctionName, MB_OK|MB_ICONSTOP);
			}
		return(hFile);
	}
	
extern "C" SOCKET _export FAR PASCAL  ConnectFTPControlSocket(LPSTR lpszHost)
	{
		LPHOSTENT lpHostEnt;					// Internet host information structure
		SOCKADDR_IN sockAddr;					// Socket address structure
		LPSERVENT lpServEnt;					// Service information structure
		short nProtocolPort;					// Protocol port
		int nConnect;									// Socket connection results

		SOCKET hControlSocket = INVALID_SOCKET;	
		
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

		// Make the control socket non-blocking before returning.
		IsReadyToRead(hControlSocket);
			
		if (ReadFTPServerReply(hControlSocket) >= 400)
			return(INVALID_SOCKET);
		else
			return(hControlSocket);
	}

extern "C" SOCKET FAR PASCAL RequestDataConnection(SOCKET hControlSocket, SOCKET hListenSocket)
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
	
extern "C" SOCKET _export FAR PASCAL  CreateListenSocket(SOCKET hControlSocket)
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
		
extern "C" SOCKET _export FAR PASCAL  AcceptDataConnection(SOCKET hListenSocket)
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
			{
				// Make the data channel non-blocking before returning
				IsReadyToRead(hDataSocket);
				return(hDataSocket);
			}
	}

extern "C" BOOL _export FAR PASCAL  LoadWinsock(VOID)
	{
		WSADATA wsaData;							// Winsock implementation details

		if (WSAStartup(WINSOCK_VERSION, &wsaData))
			{
				MessageBeep(MB_ICONSTOP);
				MessageBox(NULL, "Could not load Windows Sockets DLL.", 
							PROG_NAME, MB_OK|MB_ICONSTOP);
				return(FALSE);
			}
		else
			return(TRUE);
	}
	
extern "C" BOOL _export FAR PASCAL  CloseWinsock(VOID)
	{
		if (WSACleanup())
			{
				int iWinsockErr = WSAGetLastError();
				wsprintf(gszCommandBuffer, 
							"WSACleanup() caused error# %d", 
							iWinsockErr);   
				MessageBeep(MB_ICONSTOP);
				MessageBox(NULL, gszCommandBuffer, PROG_NAME, 
							MB_OK|MB_ICONSTOP);
				return(FALSE);
			}
		else
			return(TRUE);
	}
	
int _export FAR PASCAL LibMain(HANDLE hInstance, WORD wDataSeg,
			WORD wHeapSize, LPSTR lpszCmdParam)
	{
		if (wHeapSize > 0)	// Heap size defined in the DEF file. 
			UnlockData(0);		// Unlocks the current data segment. 
			
		return(1);
	}

int _export FAR PASCAL WEP(int nParam)
	{
		return(1);
	}
