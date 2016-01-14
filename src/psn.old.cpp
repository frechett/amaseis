// Class for Public Seismic Network (PSN) file.
// Based on code from Larry Cochrane's readpsn.c program
// Alan Jones, SUNY Binghamton, May 2000
#include "as1.h"

//void PSNException::exception()
//{
//	MdiClient->MessageBox("PSN file error");
//}
PSNException pe;
// PSN member functions
// PSN constructor for constructing internally from a SAC file
PSN::PSN(KSAC *ksac) {
	PHdr = new PsnHeader;
  Initiate();
  //PPSNData = ksac->GetData();
  strcpy(filename,"untitled");
  npts = ksac->GetSACHV(NPTS);
  // Move number of points into bsave
  memcpy(bsave+1,&npts,6);
  // Move amount of data.  (Should this include the header or not?)
  // If more than 64KB, this won't be correct.  But it doesn't look
  // like it is used anyway.
  short n = (short)(npts*sizeof(short)+sizeof(PsnHeader));
  memcpy(bsave+5,&n,2);
  float delta = (float)ksac->GetSACHV(DELTA);
  PHdr->sYear = (short)ksac->GetSACHV(NZYEAR);
  short jday = (short)ksac->GetSACHV(NZJDAY);
  int month, day;
  month_day(jday,PHdr->sYear,
  	month,day);
  // Starting time:
  PHdr->sMonth = (short)month;
  PHdr->sDay = (short)day;
  PHdr->sHour = (short)ksac->GetSACHV(NZHOUR);
  PHdr->sMin  = (short)ksac->GetSACHV(NZMIN);
  PHdr->sSec  = (short)ksac->GetSACHV(NZSEC);
	PHdr->s100th = (short)(ksac->GetSACHV(NZMSEC)/10);
  // We now have to compute the start time in 10ths of seconds
  long endTime = ((((long)PHdr->sHour * 3600L) + ((long)PHdr->sMin * 60L) +
			(long)PHdr->sSec) * 10) + (long)PHdr->s100th;
  endTime += (long)(10*delta*npts);
  // Compute components of time
  PHdr->f100th = (short)(endTime%10);
  endTime /= 10;
  PHdr->fSec = (short)(endTime%60);
  endTime /= 60;
  PHdr->fMin = (short)(endTime%60);
  endTime /= 60;
  PHdr->fHour = (short)(endTime%24);

  // Scale data if necessary
  float depmin = ksac->GetSACHV(DEPMIN);
  float depmax = ksac->GetSACHV(DEPMAX);
  float scale_factor = 1;
  if(depmin < -2048 || depmax > 2047) {
  	if(depmax > -depmin)
    	scale_factor = 2047/depmax;
    else
    	scale_factor = -2048/depmin;
    depmax *= scale_factor;
    depmin *= scale_factor;
  }
  float *data = ksac->GetData();
 	PPSNData=new short[npts];
  for(int i=0;i<npts;i++) {
  	PPSNData[i] = scale_factor*data[i];
  }

  PHdr->dataMin = (short)depmin;
  PHdr->dataMax = (short)depmax;

 	PHdr->latInt = (short)ksac->GetSACHV(STLA);
  PHdr->latDec = (short)(((int)(0.5+100*ksac->GetSACHV(STLA)))%100);
  PHdr->longInt = (short)ksac->GetSACHV(STLO);
  PHdr->longDec =(short)(((int)(0.5+100*ksac->GetSACHV(STLO)))%100);

}
// Destructor
PSN::~PSN(){
  delete PPSNData;
  delete PHdr;
}
//- initialize new sac header values
//
void PSN::Initiate(char *PPSNFile, short iflag)
{
	// Construct the first 7 bytes of bsave
  bsave[0] = 0xfd;					// Flag indicating BSAVE format
	PHdr->dataCount = 0xffff;	// Indicating that number of data points is
  													// in bsave[1-4]
  PHdr->format = 3;					// New format
  if(PPSNFile != NULL) {
  	strcpy(filename,PPSNFile);
  } else {
  	strcpy(filename,"untitled");
  }
  if(iflag != 0)
    if(SFlag & 0x01)
      delete(PPSNData);
  PPSNData = 0;


  SFlag = 0x00;
  if(PPSNFile)
  {
    ReadPSN(PPSNFile);
  }
}

//- ReadPSN : read PSN file
//
short PSN::ReadPSN(char *PPSNFile) throw(PSNException)
{
	ifstream PSNStrm(PPSNFile,ios::binary);
 	PSNStrm.read((unsigned char *) bsave,7);
 	PSNStrm.read((unsigned char *) PHdr,200);
	if(!goodPSNQuakeFile())  {
    throw(pe);
	}
  if(npts < 0 || npts > 1e6)
  	throw(pe);
  PPSNData = new short[npts];

  PSNStrm.read((unsigned char *) PPSNData,npts*sizeof(short));

  SFlag |= 0x01;
  PSNStrm.close();
  return 0;
}

// Convert from int array string to normal string - Old format
void PSN::getIntString(char *to, short *from, int cnt)
{
	char *tmp = to;
	while(cnt--)  {
		*to = (char)*from;
		++to; ++from;
	}
	*to = 0;
	fixstr(tmp);
}

// Convert from int array string to normal string - New format
void PSN::getIntStringNew(char *to, short *from, int cnt)
{
	short *ip = (short *)to, d, tmp = (short)cnt;
	char *ptr = (char *)&d, c;
	cnt /= 2;
	while(cnt--)  {
		d = *from++;
		c = ptr[0];
		ptr[0] = ptr[1];
		ptr[1] = c;
		*ip++ = d;
	}
	to[tmp] = 0;
	fixstr(to);
}

/* This function strips any trailing spaces */
void PSN::fixstr(char *str)
{
	char *ptr = str;
	if(!strlen(str))
		return;
	while(*ptr)
		++ptr;
	--ptr;
	while(ptr != str)  {
		if(*ptr == ' ')
			*ptr = 0;
		else
			break;
		--ptr;
	}
}

/* convert PSN format latitude or longitude to a double */
double PSN::getLatLong(short num, short dec)
{
	double f = (double)num;
	if(num < 0 && dec > 0)
		dec = (short)-dec;
	f += ((double)dec / 1000.0);
	return(f);
}

// Convert from a char string array to a int string
// used to pack the Location field
void PSN::saveIntString(short *to, char *from, int cnt)
{
	int i;
	for(i = 0; i != cnt; i++)
		to[i] = 0;
	while(*from && cnt)  {
		*to++ = *from++;
		--cnt;
	}
}

// Used to pack the Comment field
void PSN::saveIntStringNew(short *to, char *from, int cnt)
{
	char d[2];
	short *ip = (short *)&d[0];
	int i, len = strlen(from), num;

	for(i = 0; i != (cnt/2); i++)
		to[i] = 0;

	if(len >= cnt)
		num = cnt;
	else
		num = len;
	while(num)  {
		*ip = 0;
		if(num)  {
			d[1] = *from++;
			--num;
		}
		if(num)  {
			d[0] = *from++;
			--num;
		}
		*to++ = *ip;
	}
}

// Used to pack the event or station latitude or longitude
// iptr points to the integer portion of one of the lat/long header fields and
// dptr points to the decimal portion.
// example of a input string: 37.453N
void PSN::makeLatLong(short *iptr, short *dptr, const char *str)
{
	char tmp[40];
	int neg = 0;
	char *ptr;

	strcpy(tmp, str);
	if((0 != (ptr = strchr(tmp, 'N'))) || (0 != (ptr = strchr(tmp, 'E'))))
		neg = FALSE;
	else if((0 != (ptr = strchr(tmp, 'S'))) || (0 !=(ptr = strchr(tmp, 'W'))))
		neg = TRUE;
	if(ptr)  {
		--ptr;
		if(*ptr != ' ')
			++ptr;
		*ptr = 0;
	}
	*iptr = (short)atoi(tmp);
	if(neg)
		*iptr = (short)-*iptr;
	*dptr = 0;
	if(0 != (ptr = strchr(tmp, '.')))  {
		int len = strlen(&ptr[1]);
		if(!len)
			return;
		if(len > 3)
			ptr[4] = 0;
		if(len == 3 && ptr[3] == '0')  {
			ptr[3] = 0;
		}
		double d = atof(ptr) * 1000.0;
		*dptr = (short)(d+0.0005);
		if(neg)
			*dptr = (short)-*dptr;
	}
}


/* checks for a proper PSN event file. Also sets the number of samples */
bool PSN::goodPSNQuakeFile()
{
	long lcnt, *ptr, cnt;

	if(bsave[0] != 0xfd || (PHdr->format != 0x02 && PHdr->format != 0x03))
		return(0);
	if(PHdr->sYear < 1960 || PHdr->sYear > 2010)
		return(0);
	if(PHdr->sMonth < 1 || PHdr->sMonth > 12)
		return(0);
	if(PHdr->sDay < 1 || PHdr->sDay > 31)
		return(0);
	if(PHdr->sHour > 24 || PHdr->sMin > 60 || PHdr->sSec > 60)
		return(0);
	if(PHdr->fHour > 24 || PHdr->fMin > 60 || PHdr->fSec > 60)
		return(0);
	cnt = PHdr->dataCount;
	if(cnt == 65535L)  {
		ptr = (long *)&bsave[1];
		lcnt = *ptr;
	}
	else
		lcnt = cnt - 100L;
	if(lcnt < 100L || lcnt > 250000L)
		return(0);
	npts = lcnt;
	return true;
}

/* calculates the samples per second rate */
double PSN::calcSPS()
{
	long startTime, endTime, totalSeconds;
	int fHour;

	startTime = ((((long)PHdr->sHour * 3600L) + ((long)PHdr->sMin * 60L) +
			(long)PHdr->sSec) * 10) + (long)PHdr->s100th;
	if(PHdr->fHour < PHdr->sHour)
		fHour = PHdr->fHour + 24;
	else
		fHour = PHdr->fHour;
	endTime = ((((long)fHour * 3600L) + ((long)PHdr->fMin * 60) +
			(long)PHdr->fSec) * 10L) + (long)PHdr->f100th;

	totalSeconds = (long)((endTime - startTime) / 10L);
	return((double)npts / (double)totalSeconds);
}


//-Write PSN file
//
short PSN::WritePSN(char* PPSNFile)
{
	ofstream PSNStrm(PPSNFile,ios::binary);
  PSNStrm.write((unsigned char *) bsave,7);
  PSNStrm.write((unsigned char *) PHdr,sizeof(PsnHeader));
  // Convert to short
  PSNStrm.write((unsigned char *)PPSNData,npts * sizeof(short));
  return 0;
}

// set the data array of PSN file -- only dependent variables
// This is for the data being in short form
short PSN::SetData(short* sdata,long npts)
{

   try{PPSNData=new short[npts];}
   catch(xalloc){ kerror(1,"PSN::SetData(short)"); abort();}

   for(long i=0;i<npts;i++)
      PPSNData[i]=sdata[i];

   return 0;
}
// set the data array of PSN file -- only dependent variables
// This is for the data being in float form
short PSN::SetData(float* sdata,long npts)
{

	try{PPSNData=new short[npts];}
  catch(xalloc){ kerror(1,"PSN::SetData(short)"); abort();}

  for(long i=0;i<npts;i++) {
    float d = sdata[i];
    if(d > 2047) d = 2047;
    else if(d <= -2048) d = - 2048;
      PPSNData[i]=(short)d;
  }

  return 0;
}

//-------------- friend member function ----------------------
//
istream& operator>>(istream& is,PSN& PSN)
{
	#if 0
 	is.read((unsigned char *) bsave,7);
 	is.read((unsigned char *) PHdr,200);
	if(!goodPSNQuakeFile())  {
    throw(pe);
	}
  #endif
  PSN.SFlag |= 0x01;
  return is;
}

//
ostream& operator<<(ostream& os,PSN& /*PSN*/)
{

// write SAC header value
//
   //os.write((unsigned char *) PSN.PHdr,PSNHLen);

// output SAC data values
//
   //os.write((unsigned char *)PSN.PPSNData,
   //              PSN.PHdr->npts * sizeof(float));
   //os.write((unsigned char *)datablockend,sizeof(long));
   return os;
}

