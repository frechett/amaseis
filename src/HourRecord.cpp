/********************
** Class HourRecord
**
** Stores one hour of seismogram data
********************/
#include "as1.h"

#define LF 0x0a
#define CR 0x0d
extern Profile *profile;
// 2003.12.13: The cc counter has to be global.  I had it as a class variable
// but when a new hour azrrives, it does not get initialized.  It should
// never get initialized except at the beginning since it is counting
// characters in the data stream.
static int cc;
static short last_dp;	// Last datapoint
float samprate;   	// "Global" sample rate, samples per second
int samples_per_hour;
int block_size;			// Number of DATAQ samples in a chunk
//static int jul[] = {0,0,31,59,90,120,151,181,212,243,273,304,334,365};
//static int jul_ly[] = {0,0,31,60,91,121,152,182,213,244,274,305,335,366};
//char *months[13] = {"",
//    "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"
//};

float HourRecord::c0;
float HourRecord::c1;
float HourRecord::d1;
FtpThread *HourRecord::ftp_instance = 0;
unsigned char HourRecord::tmp_buf[];
// Constructor based on time of day
HourRecord::HourRecord()
{
	SYSTEMTIME utc;
  // From time of day, figure out where we are in the record
  GetSystemTime(&utc);		// This yields UTC
  hour0 = st2hour(utc);
	hour = utc.wHour;
 	day = utc.wDay;
 	month = utc.wMonth;
 	year = utc.wYear;
  data = 0;
  data2 = 0;
  floatdata = 0;
  butterworthLP = 0;
  butterworthHP = 0;
  last_npts = 0;
  sec0 = (utc.wSecond +60*utc.wMinute) + 0.001*(double)utc.wMilliseconds;
  // New record.  Use global sample rate
  sps = samprate;
  // We have to see if there is already a record out there for this hour.
  HourRecord *hr2 = new HourRecord(hour0, true);
  block_ctr = block_size;
  average = 0;
  if(hr2->npts != 0) {
    size = sps*4000;
  	//sample0 = 0;
    npts = sec0*sps;
    sec0 = 0;
    data = new short[size];
    if(profile->glitch_removal)
    	data2 = new short[size];
    floatdata = new float[size];
    int i;
    for(i = 0;i<hr2->npts && i<size;i++) {
    	data[i] = hr2->data[i];
      if(profile->glitch_removal)
      	data2[i] = data[i];
  		//IIR(i);
      floatdata[i] = data[i];
    }
    // Flush out rest of arrays
    for(;i<size;i++) {
    	data[i] = 0;
      floatdata[i] = 0;
    }
    year = hr2->year;
    month = hr2->month;
    day = hr2->day;
    hour = hr2->hour;

  } else {
		// Create place for an hour's worth of data
  	size = (int)(sps*4000);		// Use 4000 seconds as a safety amount.
  	data = new short[size];
    if(profile->glitch_removal)
    	data2 = new short[size];
  	floatdata = new float[size];
  	for(int i=0;i<size;i++) {
  		data[i] = 0;
      if(profile->glitch_removal)
      	data2[i] = 0;
    	floatdata[i] = 0;
  	}
    if(profile->ftp_download)
    	npts = 0;
    else
   		npts = sps*sec0;
  }
  filter();
  delete hr2;
	last_drawn = npts;
  // If this is a client, set npts to zero so it will get its data
  // for the current hour from the server
  if(profile->tcp_client)
    	npts = 0;
}

// Constructor based on hour from 1601
HourRecord::HourRecord(long h0, bool current)
{
	SYSTEMTIME st = hour2st(h0);
  year = st.wYear;
  month = st.wMonth;
  day = st.wDay;
  hour = st.wHour;
  hour0 = h0;
  sps = samprate;
  last_npts = 0;
  butterworthLP = 0;
  butterworthHP = 0;
 	filterCoeff();
  //depmen = 0;
  char *path = getPath();
  //size = samprate*4000;
  last_drawn = 0;
  block_ctr = block_size;
  last_minute_downloaded = -1;
  last_minute_checked = -1;
  average = 0;
  data = 0;
  data2 = 0;
  floatdata = 0;
  size = 0;
  npts = 0;
  int glitches_found = 0;
  // See if we can find the file
  HANDLE handle = CreateFile(
    path,
    DWORD GENERIC_READ,	// access (read-write) mode
    0,	// share mode
    0,	// pointer to security attributes
    OPEN_EXISTING,	// how to create
    FILE_ATTRIBUTE_NORMAL,	// file attributes
    0 	// handle to file with attributes to copy
   );
  if(INVALID_HANDLE_VALUE == handle) {
    // Before fetching file, we have to make sure there is a
    // directory to receive it.
    //Debug("Couldn't find path: %s",path);
    //Debug("Creating path");
    createPath();
  	// If this is current hour, create a full record
    if(current) {
      npts = 0;
      return;
    } else
  	// File doesn't exist and this isn't the current hour.
    // see if we are a client or an ftp download instance.
    // If we are, a client maybe we can fetch it from the server.
		if(profile->tcp_client) {
    	if(MdiClient->tcpclient) {
        // Before fetching file, we have to make sure there is a
        // directory to receive it.
        //Debug("Looking for file: %s",path);
        //Debug("Creating path");
        //createPath();
        npts = 0;
        char *remote_path = getRemotePath();
    		data = (short *) MdiClient->tcpclient->fileReceive(remote_path,size);
      	// if size == 0, file does not exist
        if(data) {
        	Debug("Found file on server. Size: %d",size);
        	npts = size/sizeof(short);
          size = npts;
          save();
        } else {
        	Debug("File not on server. Try one more time");
    			data = (short *) MdiClient->tcpclient->fileReceive(remote_path,size);
        	if(data) {
        		Debug("Found file on server. Size: %d",size);
        		npts = size/sizeof(short);
          	size = npts;
          } else {
        		Debug("File STILL not on server.");
          	size = 0;
        		npts = 0;
          }
        }
        save();
      } else
      	npts = 0;
      if(npts == 0) {
      	// If this is not the current hour, create a null file
      	size = 0;
      	save();
      }
    }
    else if(profile->ftp_download) {
      npts = 0;
    	// Fetch from FTP server
      // Create an instance of FtpThread but don't run as a thread
      // Call member function download
      //Debug("Fetch via FTP");
      Progress *pro = new Progress(MdiClient);
      pro->Create();
      pro->SetText("Look for %s via FTP",path);
      Debug("Look for %s via FTP",path);
      if(ftp_instance == 0) {
      	ftp_instance = new FtpThread(0);
        if(ftp_instance->Login())
        	return;
      }
			if(ftp_instance->Download(this)) {
      	pro->SetText("Couldn't find it. Set to zero");
      	Debug("Couldn't find it. Set to zero");
      	npts = 0;
    		//size = samprate*4000;
  			//data = new short[size];
    		//memset(data,0,size*sizeof(short));
        size = 0;
      } else {
        pro->SetText("Found it. Size: %d",npts);
        Debug("Found it. Size: %d",npts);
        size = npts;
      }
      pro->SetText("Save");
      save();
      delete pro;
    }
  } else {
  	// File exists
    // See how big is it
    int filelength = GetFileSize(handle,0);
    // Subtract off header and convert to samples
    size =  (filelength - 4)/sizeof(short);
    // If we are a client and the file is only 4-bytes long, check again
    // for the file.
    // 2007.04.12: Take out so it doesn't keep trying
    if(size == 0) {
    	npts = -1; 	// Set as flag to say we have a short file.
      CloseHandle(handle);
      return;
    }
    #if 0
    if(size == 0) {
    	if(profile->tcp_client) {
    		if(MdiClient->tcpclient) {
        	CloseHandle(handle);
        	Debug("4-byte file. Looking for file: %s",path);
        	npts = 0;
    			data = (short *) MdiClient->tcpclient->fileReceive(getRemotePath(),size);
      		// if size == 0, file does not exist
        	if(data) {
          	Debug("Found file on server. Size: %d",size);
        		npts = size/sizeof(short);
          	size = npts;
        	} else {
        		Debug("File not on server. Create.");
          	size = 0;
        		npts = 0;
        	}
        	save();
      	} else
      		npts = 0;
      }
    }
    #endif
    // Assume an hour record
    sps = size/3600.0;
    // Set size larger for some breathing room
    //size = samprate*4000;
    // 2005.01.08: Change from samprate to sps which is actually
    // the "local" sample rate
    //size = sps*4000;
   	// Read the data but make enough room, and then some, for an entire hour
    // 2003.09.25: Change.  Read first 4 bytes to get size and then allocate
    // space for all the data
  	unsigned long read;
    //size = samprate*4000;
    // Read first 4 bytes which contain the length in samples
		ReadFile(handle,&npts,sizeof(npts),&read,0);
    // Roger Sparks suggested the following change in case npts is
    // ever larger than size.
    //if(npts > size)
    //	npts = size;
    if(read == sizeof(npts)) {
    	// Convert to points
      // At this point, size and npts should be the same.
    	// Read the data
      // Following just for safety
      size = min(npts,size);
      // If this is the current hour and we are doing ftp_download,
      // set npts to zero. Otherwise, downloaded minute files will overwrite
      if(current && profile->ftp_download)
      	npts = 0;
      else
      	npts = size;
			data = new short[size];
    	memset(data,0,size*sizeof(short));
    	ReadFile(handle,data,npts*sizeof(short),&read,0);
    } else {
    	size = npts = 0;
      data = new short[size];
    }
    CloseHandle(handle);
  }
	floatdata = new float[size];
  // Glitch removal
  if(profile->glitch_removal) {
  	if(npts) {
    	// Take median of first five points to get starting point
      // rather than first point.
			int *ss = new int[5];
      for(int i=0;i<5;i++)
        ss[i] = data[i];
      sortlongint(ss,5,0);
    	short s;
      short last_dp = ss[2];
      delete ss;
  		for(int i=0;i<npts;i++) {
      	s = data[i];
        #if 0
      	if(abs(last_dp - s) > 500) {
        	glitches_found++;
          data[i] = last_dp;
        } else
      		last_dp = s;
        #endif
      }
    }
  }
  if(glitches_found) {
  	Debug("%u glitches found in %s",glitches_found, path);
  }
  filter();
}
//Destructor
HourRecord::~HourRecord()
{
 	delete [] data;
 	delete [] floatdata;
 	delete [] data2;
  delete butterworthLP;
  delete butterworthHP;
}

static FtpThreadUploadOneMinute *uploadoneminute;
//
// UploadOneMinute()
// Sends one minute's worth of data to host.
// But, more correctly, sends npts-last_npts amount of data
void HourRecord::UploadOneMinute(int minute)
{
	int n = npts - last_npts;
	//FtpThread *ft = new FtpThread(0);
  // npts and last_npts are in units of shorts.
  // data is being sent as bytes
  delete uploadoneminute;
  uploadoneminute = new FtpThreadUploadOneMinute(data+last_npts,n,minute);
  uploadoneminute->Start();
  last_npts = npts;
}

//
// Filter coefficients
void HourRecord::filterCoeff()
{
  delete butterworthLP;
  delete butterworthHP;
  butterworthLP = 0;
  butterworthHP = 0;
  sps = samprate;
  if(profile->helicorder_filter) {
  	if(profile->EnableLowPass && profile->LowPassPeriod)
  		butterworthLP = new ButterworthLP(sps,1.0/profile->LowPassPeriod,2);
    else
    	butterworthLP = 0;
    if(profile->EnableHighPass && profile->HighPassPeriod)
      butterworthHP = new ButterworthHP(sps,1.0/profile->HighPassPeriod,2);
    else
    	butterworthHP = 0;
  }
}

void HourRecord::filter()
{
  filterCoeff();
  if(data) {
  	for(int i = 0;i<npts;i++)
  		floatdata[i] = data[i];
  	if(butterworthLP)
			butterworthLP->Filter(floatdata,npts);
  	if(butterworthHP)
  		butterworthHP->Filter(floatdata,npts);
  } else
  	npts = 0;
}

// add()
// returns: true if added okay
bool HourRecord::add(short d)
{
	bool ret = true;
	if(npts < size) {
		data[npts] = d;
    float df = d;
    //IIR(npts);
    if(butterworthLP)
    	df = butterworthLP->NextElement(df);
    if(butterworthHP)
    	df = butterworthHP->NextElement(df);
    floatdata[npts] = df;
    npts++;
  }
  if(npts >= size) {
  	ret = false;
  }
  return ret;
}

// Process a buffer of data
// Samples are in form nnnn<CR><LF>nnnn<CR><LF>.. where nnnn is
// a number between 0 and 4095.  Leading zeros are truncated
// so each record can have from 3 to 6 characters.
// 2000.10.09: Extend for more general processing
// 2007.02.15: Add TCP processing
//static int save_s[96];
bool HourRecord::Process(unsigned char *p, int size)
{
	unsigned char c;
  bool ret = true;
  int s;
  int i;
  int n;
  int a2;
  float a1;
  short *sbuf;
  // 2008.06.03: Had put in the following to keep from crashing if
  // not device is attached. Forgot that it kept TCP/IP realtime data from
  // coming in. Moved the if(p == NULL) down.
  //if(p == NULL)
  //	return false;
  // If this instance is a tcp client, get data from the server
  // instead of the device
  #if 1
  if(profile->tcp_client) {
  	if(MdiClient->tcpclient) {
			sbuf = MdiClient->tcpclient->fetchCurrent(npts,n);
      if(n && sbuf) {
      	for(i = 0;i<n;i++) {
        	if(add(sbuf[i]) == false) {
          	ret = false;
            break;
          }
        }
        delete sbuf;	// Delete space acquired in fetchCurrent()
      } else
      	ret = false;
    } else
    	ret = false;
    return ret;
  } else if(p == NULL) {
    return false;
  } else if(profile->ftp_download) {
  	size = 0;
    // Comment out FTP minute stuff
    #if 0
    SYSTEMTIME utc;
    GetSystemTime(&utc);
    if(utc.wMinute != last_minute_downloaded) {
    	if(utc.wMinute == 0) {
      	last_minute_downloaded = 0;
        last_minute_checked = 0;
      }
    	if(utc.wMinute != last_minute_checked) {
    		FtpThread *ft = new FtpThread(0);
      	ft->DownloadMinuteFiles(this,last_minute_downloaded+1,utc.wMinute);
      	delete ft;
        last_minute_checked = utc.wMinute;
      }
    }
    #endif
  }
  #endif
  if(size == 0) {
  	return false;
  }
  // Not sure how much area we need.  By making it equal to the
  // number of characters, we should have plenty.
  switch(profile->device) {
  case Profile::AS1:
  case Profile::INFILTECQM1:
  case Profile::EQ1:
  case Profile::SEPUK1:
  case Profile::RHS3:
  case Profile::RHV1:
  	sbuf = new short[size];
		// Pull out characters one at a time
    n = 0;
    if(size < 0 || size > 10000)
    	break;
  	for(i=0;i<size;i++) {
    	if(cc > 20 || cc < 0) { 	// Got a problem
      	cc = 0;
    		break;
      }
    	c = p[i];
    	if(c == CR) {
      	tmp_buf[cc] = 0;
      	cc = 0;
      	if(strlen((char *)tmp_buf) == 0)
      		break;
    		// Got a carriage return. Process
        if(profile->device == Profile::RHS3) {
          sscanf(tmp_buf,"%f %f",&a1, &profile->beam_off_centre_value);
          sbuf[n++] = (short) (10*a1);
        } else if(profile->device == Profile::RHV1) {
          sscanf(tmp_buf,"%f %f",&a1, &profile->beam_off_centre_value);
          sbuf[n++] = (short) (a1);
        } else
          sbuf[n++] = (short) atoi((char *)tmp_buf);
    	} else if(c == LF) {
    		// Don't do anything.  Discard
      	cc = 0;
    	} else {
  			tmp_buf[cc++] = c;
    	}
  	}
    // Now process, looking for glitches, if called for
    for(i=0;i<n;i++) {
      // Glitch removal.  If last point is more than 500 different
      // from this point, don't use it.  Use last point instead.
    	// Data is from 0 to 4095.  Re-zero it
    	// 2000.09.10: Extend to any level
      s = sbuf[i];
      // Put in second buffer after subtracting off zero level.
      if(data2)
      		data2[npts] = s -(short)profile->zero_level;

      if(profile->glitch_removal) {
       	if(last_dp != 0) {
         	if(abs(last_dp - s) > 500) {
            SYSTEMTIME utc;
  					GetSystemTime(&utc);		// This yields UTC
            Debug("Glitch: %u replaced by %u",s,last_dp);
           	s = last_dp;
          } else
           	last_dp = s;
        } else
         	last_dp = s;				// This only gets called on the first
      }     								 	// pass to set last_dp to something other
      												// than zero.
      else
      	last_dp = s;
      // Subtract off zero level
      s -= (short)profile->zero_level;
      if(add(s) == false) {
				// At end.  Terminate
       	ret = false;
       	break;
      }
    }
    delete sbuf;
    break;
  case Profile::DATAQ151:
  case Profile::DATAQ154:
  case Profile::DATAQ158:
  case Profile::DATAQ194:
  case Profile::DATAQ145:
  case Profile::DATAQ155:
  	// Data comes in in format:
  	//         B7  B6  B5  B4  B3  B2  B1  B0
  	// Char 1  A4  A3  A2  A1  A0   1  Di   0
  	// Char 2 A11  A10 A9  A8  A7  A6  A5   1
  	//
  	// So we key on bit 0 and look for a 0 then a 1
		// Pull out characters one at a time
    //static int alan1, alan2;
  	for(i=0;i<size;i++) {
    	if(cc > 2) { 	// Got a problem
      	cc = 0;
    		break;
      }
    	c = p[i];
    	if((c & 0x01) == 0) {	// First character?
      	tmp_buf[0] = c;
    	} else if (c & 0x01) {	// Second character.  Process
      	//alan1++;
    		tmp_buf[1] = c;
      	cc = 0;
    		// process
        unsigned short lb = tmp_buf[0];
        unsigned short hb = tmp_buf[1] & 0xfe;
        lb >>= 3;
        hb <<= 4;
        hb = hb+lb;
        s = (short)hb;
      	//s = (short)((tmp_buf[0] >> 3) + ((tmp_buf[1] & 0xfe) << 4));
      	// Data is from 0 to 4095.  Re-zero it
      	// 2000.09.10: Extend to any level
        //Debug("Raw:%d",s);
        //save_s[block_ctr] = s;
        average += s;
        block_ctr--;
        if(block_ctr <= 0) {
        	block_ctr = block_size;
         	// The max resolution is 12 bits. Extend to 16 by multiplying by 16
          // BEFORE dividing by the block size.
       		int stotal = (16*average)/block_size;
       		//s = average/block_size;
      		stotal -= profile->zero_level;
          average = 0;
          if(stotal > 32767)
          	s = 32767;
          else if(stotal < -32768)
          	s = -32768;
          else
          	s = (short)stotal;
      		if(add(s) == false) {
						// At end.  Terminate
        		ret = false;
        		break;
          }
      	}
    	}
  	}
    break;
#if 0
  case Profile::DATAQ155:
  	// Data comes in in format:
  	//         B7  B6  B5  B4  B3  B2  B1  B0
  	// Char 1  A4  A3  A2  A1  A0   1  Di   0
  	// Char 2 A11  A10 A9  A8  A7  A6  A5   1
  	//
  	// So we key on bit 0 and look for a 0 then a 1
		// Pull out characters one at a time
    //static int alan1, alan2;
  	for(i=0;i<size;i++) {
    	if(cc > 2) { 	// Got a problem
      	cc = 0;
    		break;
      }
    	c = p[i];
    	if((c & 0x01) == 0) {	// First character?
      	tmp_buf[0] = c;
    	} else if (c & 0x01) {	// Second character.  Process
      	//alan1++;
    		tmp_buf[1] = c;
      	cc = 0;
    		// process
        unsigned short lb = tmp_buf[0];
        unsigned short hb = tmp_buf[1] & 0xfe;
        lb >>= 3;
        hb <<= 4;
        hb = hb+lb;
        s = (short)hb;
      	s -= profile->zero_level;
     		if(add(s) == false) {
					// At end.  Terminate
       		ret = false;
       		break;
      	}
    	}
  	}
    break;
#endif
  }
  return ret;
}

// Write n short zeros in a file
// If a problem, return false
bool HourRecord::WriteZeros(HANDLE h,int n)
{
	unsigned long written;
  bool ret = true;
	short *samp = new short[n];
  for(int i=0;i<n;i++ )
  	samp[i] = 0;
  n *= sizeof(short);	// Converts number of samples to number of bytes
	WriteFile(h,samp,n,&written,0);
  delete samp;
  if(n != written) {
  	ret = false;
  }
  return ret;
}

//
//-----------------------------------
// Save data in a file in a sub-directory:
//
// year -
//      Month -
//             Day
//
// e.g. 2000\02\23
// File name is hour, e.g. "04"
// Format of file is length(4 bytes), data (shorts)
// 2004.11.21: Also save an hour's worth of data
// If more == true, have to pad out file to an hour
// if more == false (default) just write data with no padding
bool HourRecord::save(bool more)
{

	bool ret = true;
  if(profile->passive)
  	return ret;
  char *filespec = getPath();
  // Try to open the file
  HANDLE hfile = CreateFile(
  	filespec,
  	GENERIC_WRITE,
    0,					// no sharing
    0,					// no security
    OPEN_ALWAYS,
    FILE_ATTRIBUTE_NORMAL,
    0);					// No template
  if(hfile == INVALID_HANDLE_VALUE) {
  	// Probably the path doesn't exist.  Try to create it.
  	ret = false;
  } else {
    // 2005.01.15: Write entire hour always
    //unsigned long n = npts*sizeof(short);
    unsigned long n = samprate*3600;
    if(n > size)
     	n = size;
  	//unsigned long nn = npts;
    // Write 4-byte header
    unsigned long written = 0;
  	WriteFile(hfile,&n,sizeof(n),&written,0);
    if(written != sizeof(n)) {
    	ret = false;
    } else {
    	// Convert from number of samples to number of bytes
    	n *= sizeof(short);
      // Write data that has not had glitches removed but only if onAS1
      if(data2 && profile->device == Profile::AS1)
      	WriteFile(hfile,data2,n,&written,0);
      else
      	WriteFile(hfile,data,n,&written,0);
      if(n != written) {
       	ret = false;
      }
      #if 0
      if(more) {
      	// If we haven't written a full hour, pad out to an hour
        // See how much of the hour is left.
        SYSTEMTIME utc;
  			GetSystemTime(&utc);		// This yields UTC
				float t = 0.001*utc.wMilliseconds + utc.wSecond + 60*utc.wMinute;
        t = 3600.0 - t;
      	nn = sps*t;
      	if(nn > 0) {
      		// Have some more zeros to write
        	ret = WriteZeros(hfile,nn);
        }
      }
      #endif
    }
    CloseHandle(hfile);
    // Write a file to tell FTP what to upload:
    FILE *fp = fopen("upload.txt","wt");
    fprintf(fp,"Upload:%s\n",filespec);
    fprintf(fp,"Samples/Second:%g\n",samprate);
    fclose(fp) ;
  }
  return ret;
}

//
// CreatePath
// This is weird.  I find that the FindFirstFile often finds a directory
// even if it isn't there.  So, whether it is or not, I attempt to create
// the directory.  So, I'm commenting out the FindFirstFile calls.  Seems
// to work fine.
char *HourRecord::createPath() {
	//WIN32_FIND_DATA finddata;
  static char temp[24];
  sprintf(temp,"%s%04d",profile->data_directory,year);
  //FindFirstFile(
  //  temp,				// pointer to name of file to search for
  //  &finddata);	// pointer to returned information
  //if((finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
  //	if(CreateDirectory(temp,0) == 0)
  //  	return 0;
  //}
  CreateDirectory(temp,0);
  sprintf(temp,"%s%04d\\%02d",profile->data_directory,year,month);
  //FindFirstFile(
  //	temp,				// pointer to name of file to search for
  //  &finddata);	// pointer to returned information
  //if((finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
  //	if(CreateDirectory(temp,0) == 0)
  //  	return 0;
  //}
  CreateDirectory(temp,0);
  sprintf(temp,"%s%04d\\%02d\\%02d",profile->data_directory,year,month,day);
  //FindFirstFile(
  // 	temp,				// pointer to name of file to search for
  //  &finddata);	// pointer to returned information
  //if((finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
  //	if(CreateDirectory(temp,0) == 0)
  //  	return 0;
  //}
  CreateDirectory(temp,0);
  return temp;
}

//
// Convert a long hour to a SYSTEMTIME
SYSTEMTIME HourRecord::hour2st(long hour)
{
	FILETIME ft;
  LARGE_INTEGER li;
  SYSTEMTIME st;
  // Set low 4 bytes
  memcpy(&ft,&hour,sizeof(hour));
  // Zero out the high bytes
  ft.dwHighDateTime=0;
  // Copy into a large integer
  memcpy(&li,&ft,sizeof(li));
  // Convert to seconds
  li.QuadPart = li.QuadPart*3600;
  // Covert to 100 ns
  li.QuadPart = li.QuadPart*10000000L;
  // copy back to a file time
  memcpy(&ft,&li,sizeof(li));
  FileTimeToSystemTime(&ft,&st);
  return st;
}

//
// Convert a long hour:seconds to a SYSTEMTIME
SYSTEMTIME HourRecord::hourSec2st(long hour, double sec)
{
	FILETIME ft;
  LARGE_INTEGER li;
  SYSTEMTIME st;
  // Set low 4 bytes
  memcpy(&ft,&hour,sizeof(hour));
  // Zero out the high bytes
  ft.dwHighDateTime=0;
  // Copy into a large integer
  memcpy(&li,&ft,sizeof(li));
  // Convert to seconds
  long s = (long)sec;
  li.QuadPart = li.QuadPart*3600 + s;
  // Get fractional part of seconds
  sec = sec - (double)s;
  // Covert to 100 ns
  li.QuadPart = li.QuadPart*10000000L + (long)(sec*1e7);
  // copy back to a file time
  memcpy(&ft,&li,sizeof(li));
  FileTimeToSystemTime(&ft,&st);
  return st;
}

// Convert a SYSTEMTIME to the number of hours since 1601
long HourRecord::st2hour(SYSTEMTIME st)
{
	FILETIME ft;
  LARGE_INTEGER li;
	SystemTimeToFileTime(&st,&ft);
  memcpy(&li,&ft,sizeof(li));
  // Convert to seconds
  LONGLONG longint= li.QuadPart/10000000L;
  // Convert to hours
  longint = longint/3600;
  long hour = (long)longint;
  return hour;
}

//
//---------------------------
// getPath()
//
// Create a path to this record's file
char *HourRecord::getPath()
{
	static char path[24];
  sprintf(path,"%s%04d\\%02d\\%02d\\%02d.%c",
  	profile->data_directory,year,month,day,hour,profile->component);
  return path;
}
//---------------------------
// getRemotePath()
//
// Create a path to this record's remote file
char *HourRecord::getRemotePath()
{
	static char path[24];
  sprintf(path,"%04d\\%02d\\%02d\\%02d.%c",
  	year,month,day,hour,profile->component);
  return path;
}
//
// IIR (Infinite Impulse Response) filter (recursive filter)
// Forumula, from Press:
// y[n] = c0*x[n] + c1*x[n-1] + d1*y[n-1]
#if 0
void HourRecord::IIR(int i)
{
	float x;
	// If cutoff is larger than sampling frequency, don't do anything
  if(profile->lp_cutoff > samprate)
  	x = data[i];
  else {
		x = c0*data[i];
    if(i >0) {
      x += c1*data[i-1] + d1*floatdata[i-1];
    } else {
    	x = data[0];
    }
  }
  floatdata[i] = x;
}
#endif
// Following change by Roger Sparks
void HourRecord::IIR(int i)            //Original code
{
	//float x;  //Original code
  int x;      //Roger changed to type int

  #if 0
  //If cutoff is larger than sampling frequency, don't do anything
  if(profile->lp_cutoff >samprate)
  	x  = data[i];
  else {
    //x = c0*data[i];     //Original code
    if (i > 0) {
  	  //x += c1*data[i-1] + d1*floatdata[i-1];      //Original code
      x = c0*data[i] + c1*data[i-1] + d1*floatdata[i-1]; //Roger changed to this

    } else {
      x = data[0];
    }
  }
  #endif
  floatdata[i] = data[i];
}
//zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz



//
// ZeroOut()
// Create a record of zeros
void HourRecord::ZeroOut()
{
	size = 4000*samprate;
  data = new short[size];
  memset(data,0,size*sizeof(short));
  npts = size;
  sps = samprate;
}
