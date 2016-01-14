/******************************
Class Serial IO
Alan L. Jones, February 20, 2000
******************************/
#include "as1.h"

extern Profile *profile;
// Default constructor
SerialIO::SerialIO(int buff_size) {
	buffer = new unsigned char[buff_size];
  this->buff_size = buff_size;
  memset(buffer,0,buff_size);
  opened = cominit();
}

// Destructor
// 2002.03.21:
SerialIO::~SerialIO() {
	com_port_off();
  delete [] buffer;
}
// ***************************************************
// cominit()
// initialize communications port for interrupt input

// Returns
// 1: Can't initialize
// 0: Initialized okay
//
// **************************************************
bool SerialIO::cominit(void)
{
  bool ret;
  DCB CommData;
  COMMTIMEOUTS CommTimeouts;
  block_size = 1;
  char com[16];
  // 2006.10.25: Found was not working for COM10 and above
  // Found this on Microsoft web site:
  // To specify a COM port number greater than 9,
  // use the following syntax: "\\\\.\\COM10".
  // This syntax works for all port numbers and hardware
  // that allows COM port numbers to be specified.
  //sprintf(com,"COM%d",profile->comport);
	sprintf(com,"\\\\.\\COM%d",profile->comport);
	comhandle = CreateFile(com,GENERIC_READ | GENERIC_WRITE,
		0,					// Comm devices must be opened with exclusive access
		NULL,				// no security attributes
		OPEN_EXISTING,	// Needed for comm devices
		FILE_ATTRIBUTE_NORMAL,			// Normal
		NULL				// Template must be NULL for comm devices
		);
	if(comhandle == INVALID_HANDLE_VALUE)
		return false;
	//Retrieve default Port settings...
	GetCommState(comhandle, &CommData);
	//Adjust for app specific parameters....
	CommData.BaudRate = profile->baud;
	CommData.ByteSize = 8;
	CommData.Parity = 0;
	CommData.StopBits = ONESTOPBIT;
  CommData.fAbortOnError = 1;	// In order to catch errors
  ret = SetCommState(comhandle,&CommData);
  if(ret == false)
  	return ret;
  // Following makes chars_avail() return right away whether there is
  // data or not.
  CommTimeouts.ReadIntervalTimeout = MAXDWORD;
  // 2005.11.01: Changes suggested by Roger Sparks
  CommTimeouts.ReadTotalTimeoutConstant = 5;	// Used to be 0.  Roger suggested change
  CommTimeouts.ReadTotalTimeoutMultiplier = MAXDWORD; 	// Used to be 0
  CommTimeouts.ReadIntervalTimeout = MAXDWORD;
  CommTimeouts.ReadIntervalTimeout = MAXDWORD;
  ret = SetCommTimeouts(comhandle,&CommTimeouts);
  return ret;
}

// Initialize a DATAQ151, DATAQ154, or DATAQ194
void SerialIO::initDATAQ()
{
	static char *avail = 0;
	// Initialize DATAQ151, 154, 145, or 194
  char s[120];
  // Before sending any command, have to start with a null.
  // To do this, we put a 0 into s[0].
  s[0] = 0; s[1] = 'R'; s[2] = 0;
  Sleep(1000);
  int n;
  switch(profile->device) {
  case Profile::DATAQ151:
  	// Reset is just 'R'
    // Reset command
    Send(s,2);
    break;
  case Profile::DATAQ154:
  case Profile::DATAQ145:
  case Profile::DATAQ194:
  	// Reset is "RZ"
    s[2] = 'Z';
    // Reset command
    Send(s,3);
    break;
  case Profile::DATAQ155:
  	// Reset is "R1"
    s[2] = '1';
    // Reset command
    Send(s,3);
    break;
  }
  if(profile->device == Profile::DATAQ155) {
  	// ASCII commands for 155
    // Use channel 0. max gain: 7
  	strcpy(s,"asc\rslist 0 x0700\r");
    Send(s,strlen(s));
    Sleep(100);
  	avail = chars_avail(n,false);
    // The 155 can only sample down to 11.44 sps
    // We will use a block size of 4 to extend the
    // data from its 14-bit value to 16-bit.
    // This will also extend the sample rate to about 3 Hx
    block_size = 4;
    int srate = 750000/(block_size*profile->samprate);
		sprintf(s,"srate %d\r",srate);
    Send(s,strlen(s));
    Sleep(100);
  	avail = chars_avail(n,false);
  	samprate = profile->samprate;
    Send("bin\r",4);
    Sleep(100);
    avail = chars_avail(n,false);
    Send("start\r",6);
    Sleep(10);
    avail = chars_avail(n,false);
    return;
  } else
  if(profile->device == Profile::DATAQ158) {
  	// Initial 158 command. DATAQ responds with "DATAQ"
    // but we don't bother listening
    memcpy(s,"\A0\0",4);
    Send(s,4);
  	Sleep(100);
  	avail = chars_avail(n,false);
    // C01 means one channel
    memcpy(s,"\0C01\0",4);
    Send(s,4);
    Sleep(100);
  	avail = chars_avail(n,false);
    //Set channel 1 to a gain of 8
    //Input list. 00: no offset into list
    // E300: I don't know where this came from
    // 2010.12.12. Note from Australia that gain isn't big enough. I'm wondering if:
    // Cmd  gain
    // E30:    8
    // E40:   16
    // E50:   32
    // ...
    // E80:  256
    // E90:  512
    //memcpy(s,"\0L00E900\0",9);
    // 2010.12.17: By looking at the DATAQ SDK, have discovered that the assumption
    // above is correct. But I also found out that there are two models of the 158:
    // 158U and 158UP where P means "programmable" The 158U has gains from 1 to 8
    // whereas the 158U can go to 512. By doing testing, I found that using '9'
    // with the 158U still gives a gain of 8. So I will leave it set to 9 (gain of 512).
    memcpy(s,"\0L00E900\0",9);
    // The commented out code below sets all 4 channels to a gain of 8
    //memcpy(s,"\0L00E900E301E302E303\0",21);
    Send(s,9);
  	Sleep(100);
  	avail = chars_avail(n,false);
    // 2008.05.12: We had never set the sampling rate. I must have discovered
    // that it was 480 s/s and used that. Let us set to 600 using the M
    // command. Where sample rate = 14,745,600/divisor.
    // We want 600 so 14745600/600 = 24576 = 0x6000.
   	// 2010.12.17: switch from 600 s/s to 1200
    // We want 1200 so 14745600/1200 = 12288 = 0x3000.
    //memcpy(s,"\0M6000\0",7);
    memcpy(s,"\0M3000\0",7);
    Send(s,7);
    // Start scanning
    memcpy(s,"\0S1\0",4);	// Start scanning
    Send(s,4);
  	Sleep(100);
  	avail = chars_avail(n,false);
  	//int count = profile->baud/((int)(0.5+profile->samprate));
  	//block_size = count/40;
    // 2008.05.10: Seems to be sampling at one-half of specified rate.
    // Try halving the block_size
    //block_size = count/80;
    // block_size = 600/our sample rate
   	// 2010.12.17: switch from 600 s/s to 1200
    //block_size = 600/((int)(0.5+profile->samprate));
    block_size = 1200/((int)(0.5+profile->samprate));
  	samprate = profile->samprate;
    Sleep(100);
  	avail = chars_avail(n,false);
    return;
  }
  Sleep(100);
  avail = chars_avail(n,false);
  // Set 151RS to be single-ended, s[2] = 0;
  // For differential, s[2] = 1;
  // Set channel number = 1
  // Default is channel 1.  By not specifying the channel,
  // this works for both 151 and 154
  //s[1] = 'C'; s[2] = '1';
  //serialio->Send(s,3);
  //Sleep(100);
  Debug("chars_avail: %d:%s",n,avail);
  s[1] = 'D'; s[2] = '1';
  Send(s,3);
  Sleep(100);
  avail = chars_avail(n,false);
  //Debug("chars_avail: %d:%s",n,avail);
  // Send sample rate = Baud/counter
  // Round to nearest integer
  int count = profile->baud/((int)(0.5+profile->samprate));
  samprate = profile->samprate;
  //Debug("samprate: %f, count:%d",samprate,count);
  // Throughput (sample rate) is baud/counter.
  // baud is set to 4800, sample rate = 240, therefore, count = 20;
  // 2004.11.04: Nope.  I tested.  It seems that a count of 20
  // gives a sample rate of 120.  I think when DATAQ says "throughput"
  // they mean chars/sec, not datapoints per second.  Therefore,
  // we are changing the count to 10, then the block size IS baud/(sps*20);
  // We set the sample rate to be the fastest the device
  // can go and then use averaging in HourRecord to average
  // over one time (1/samprate) sample period.
  // The number of samples is obtained by dividing count by
  // 20.  Why 20?  Because there are two bytes per sample and
  // 10 bits per byte.
  //block_size = count/20;
  // 20 doesn't seem to work.  Set to 40.
  block_size = count/40;
  // However we have report from 151 user that it samples twice as fast.
  // Set counter to 40 in this case
  if(profile->device == Profile::DATAQ151)
  	count = 40;
  else
  	count = 20;
  // 2011.06.20: This is strange. I bought a DI-145 this week. They
  // claim the programming is the same as for the 194. However, have had
  // two reports that it samples much too fast. Found, by experimentation
  // that if I set the block_count to 240, at 10 sps it works.
  // So, generalizing, block_size = 2400/samprate.
  // 2011.08.29: On further experimentation, I find it sampling too slow.
  if(profile->device == Profile::DATAQ145)
  	//block_size = profile->baud/samprate;
  	block_size = profile->baud/(samprate*2);
  // Send low byte
  s[1] = 'L'; s[2] = (unsigned char)(count % 256);
  Send(s,3);
  Sleep(100);
  avail = chars_avail(n,false);
  //Debug("Low byte: chars_avail:%d,%s",n,avail);

  // Send high byte
  s[1] = 'M'; s[2] = (unsigned char)(count/256);
  Send(s,3);
  Sleep(100);
  avail = chars_avail(n,false);
  //Debug("High byte: chars_avail:%d,%s",n,avail);

	// Send command to start digitizing
	s[1] = 'S'; s[2] = '1';
	n = Send(s,3);

	// Delay 1.0 seconds to gobble up first few characters
	// which includes the first 2 bytes which are just the
	// echo of "S1"
	Sleep(1000);
	avail = chars_avail(n,false);
  //Debug("Done. chars_avail:%d,%s",n,avail);
}

// *************************************************
// com_port_off()   Program to turn off RS-232 port
// *************************************************
void SerialIO::com_port_off(void)
{
	CloseHandle(comhandle);
}

// ********************
// chars_avail()
//
// Reads characters from serial port.
// Returns number of characters in buffer
// Also puts zero at end so user can dump as a string
// *********************
unsigned char * SerialIO::chars_avail(
	int &size,          // Returns number of characters in buffer)
  bool parity)				// If true, knock out parity bit
{
	static bool bResult;
	static  DWORD dwError;
	DWORD ret_size;
  if(profile->tcp_client || profile->ftp_download) {
  	size = 0;
    return 0;
  }
  memset(buffer,0,10);
	bResult = ReadFile(comhandle, buffer,buff_size,&ret_size,NULL);
  // if there was a problem, or the async. operation's still pending ...
	if (!bResult)
	{
  	dwError = GetLastError();

    // deal with the error code
    switch (dwError)
    {

    	case ERROR_HANDLE_EOF:
				//bResult = ReadFile(comhandle, buffer,BUFF_SIZE,&ret_size,NULL);
        break;
      default:
      	// Get and clear current errors on the port.
        COMSTAT comStat;
    		DWORD   dwErrors;
        //SYSTEMTIME utc;
  			//GetSystemTime(&utc);		// This yields UTC
        char *er;;
    		if (!ClearCommError(comhandle, &dwErrors, &comStat)) {
        	// Write to debug file error.
          if(dwErrors & CE_FRAME)
          	er = "Frame";
          else if(dwErrors & CE_RXOVER)
          	er = "Input buffer over flow";
          else if(dwErrors & CE_OVERRUN)
          	er = "Buffer overrun";
          else
          	er = "Unknown";
          Debug("%s error in chars_avail",er);
        }
        break;
  	} // end switch

	}
  size = (int)ret_size;
  //size_read += size;
	if(parity) {
    int i = 0;
		for(unsigned char *p=buffer;i<size;p++,i++)
			*p = (char)(0x7f & *p);
	}
	buffer[size] = 0;
	return buffer;
}


// Send data out port
// s: string to send
// ns: number of characters in string
// (can't use strlen() since s might have nulls
DWORD SerialIO::Send(char *s, int ns)
{
  DWORD n = 0;			// returns number of bytes written
	WriteFile(comhandle,s,ns,&n,0);
  return n;
}
