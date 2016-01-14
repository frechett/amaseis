#include "as1.h"

// undefine value in SAC format and header length
static short          SACHDLen=632;           // bytes
static float          SACFUD=-12345.;
static long           SACLUD=-12345;
static char*          SACCUD="-12345";
// Following array is from 0 to 180 degrees.  Used in interpolation routine.
// extern float degrees[];


// Instance of KSACException to be thrown if an error occurs
KSACException ke;

// KSAC member functions
// KSAC constructor for constructing internally
KSAC::KSAC(long npts, float *data, STATION station, SYSTEMTIME st) {
	PSACHD = new KSACHD;
  filetype = SACFILE;
  Initiate();
  PSACData = data;
  PSACDataOriginal = new float[npts*sizeof(float)];
  memcpy(PSACDataOriginal,data,npts*sizeof(float));
  strcpy(filename,"untitled");
  PSACHD->Npts = npts;
  GetMinMax();
  SystemTimeToSACTime(st);
  SetSACHV(STLA,station.stla);
  SetSACHV(STLO,station.stlo);
  SetSACHV(STEL,station.stel);
  SetSACHV(DELTA,station.delta);
  SetSACHV(KSTNM,station.name);
  SetSACHV(KCMPNM,station.component);
  // Set azimuth and inclination
  switch(station.component[0]) {
  case 'Z': case 'z':
  	SetSACHV(CMPAZ,(float)0); SetSACHV(CMPINC,(float)0); break;
  case 'N': case 'n':
  	SetSACHV(CMPAZ,(float)0); SetSACHV(CMPINC,(float)90); break;
  case 'E': case 'e':
  	SetSACHV(CMPAZ,(float)90); SetSACHV(CMPINC,(float)90); break;
  }
  SetSACHV(O,(float)0);
  SetSACHV(B,(float)0);
  SetSACHV(E,(float)(station.delta*(npts-1)));
}

// Constructor to build KSAC from a PEPP instance
KSAC::KSAC(PEPP *pepp) {
	PSACHD = new KSACHD;
  filetype = PEPPFILE;
 	Initiate();
  strcpy(filename,pepp->filename);
  Header24 *phdr = pepp->PHdr;

  PSACHD->Npts = phdr->npts;
  SetSACHV(DEPMIN,phdr->depmin);
  SetSACHV(DEPMAX,phdr->depmax);
  SetSACHV(DEPMEN,phdr->depmen);
  SetSACHV(STLA,phdr->stla);
  SetSACHV(STLO,phdr->stlo);
  SetSACHV(STEL,phdr->stel);
  SetSACHV(DELTA,1.0/phdr->samplingrate);
  SetSACHV(KSTNM,phdr->kstnm);		// Station name
  char temp[2];
  temp[0] = phdr->cmpname;
  temp[1] = 0;
  SetSACHV(KCMPNM,temp);
  SetSACHV(NZYEAR,(long)phdr->stime.year);
  SetSACHV(NZJDAY,(long)phdr->stime.jday);
  SetSACHV(NZHOUR,(long)phdr->stime.hour);
  SetSACHV(NZMIN,(long)phdr->stime.min);
  SetSACHV(NZSEC,(long)phdr->stime.sec);
  SetSACHV(NZMSEC,(long)((int)(1000*phdr->stime.sec))%1000);
  SetSACHV(B,(float)phdr->begin_time);
  SetSACHV(E,(float)phdr->end_time);
  SetSACHV(EVLA,(float)phdr->evla);
  SetSACHV(EVLO,(float)phdr->evlo);
  // Depth:
  // NOTE: SAC uses meters for depth; PEPP uses km
  SetSACHV(EVDP,(float)(phdr->evdp*1000));
  SetSACHV(DIST,(float)phdr->evdistkm);
  SetSACHV(GCARC,(float)phdr->evdistgc);
  SetSACHV(AZ,(float)phdr->az);
  SetSACHV(BAZ,(float)phdr->baz);
  SetSACHV(EV_MS,(float)phdr->ev_ms);
  SetSACHV(EV_MB,(float)phdr->ev_mb);
  SetSACHV(EV_MW,(float)phdr->ev_mw);
  SetSACHV(EV_ML,(float)phdr->ev_ml);
  SetSACHV(EV_MO,(float)phdr->ev_mo);
  long npts = GetSACHV(NPTS);
  PSACData = new float[npts];
  PSACDataOriginal = new float[npts];
  float *pepdata = pepp->GetData();
  memcpy(PSACData,pepdata,npts*sizeof(float));
  memcpy(PSACDataOriginal,pepdata,npts*sizeof(float));
}

// Constructor to build KSAC from a PSN instance
KSAC::KSAC(PSN *psn) {
	PSACHD = new KSACHD;
  filetype = PSNFILE;
 	Initiate();
  strcpy(filename,psn->filename);
  PsnHeader *phdr = psn->PHdr;

  // See if this is old (2) or new (3) format
	short fmt = phdr->format;

  PSACHD->Npts = psn->npts;
  SetSACHV(DEPMIN,phdr->dataMin);
  SetSACHV(DEPMAX,phdr->dataMax);
  SetSACHV(STLA,psn->getLatLong(phdr->latInt,phdr->longDec));
  SetSACHV(STLO,psn->getLatLong(phdr->longInt,phdr->longDec));
  SetSACHV(DELTA,1.0/psn->calcSPS());
  SetSACHV(NZYEAR,(long)phdr->sYear);
  SetSACHV(NZJDAY,(long)julian(phdr->sYear,
  	phdr->sMonth-1,phdr->sDay));
  SetSACHV(NZHOUR,(long)phdr->sHour);
  SetSACHV(NZMIN,(long)phdr->sMin);
  SetSACHV(NZSEC,(long)phdr->sSec);
  SetSACHV(NZMSEC,(long)(10*phdr->s100th));
  SetSACHV(B,(float)0);
  if(fmt > 2)
  SetSACHV(EVLA,(float)psn->getLatLong(phdr->u.newfmt.quakeLatInt,
    phdr->u.newfmt.quakeLatDec));
  if(fmt > 2)
  SetSACHV(EVLO,(float)psn->getLatLong(phdr->u.newfmt.quakeLongInt,
  	phdr->u.newfmt.quakeLongDec));

  char temp[2];
  temp[0] = phdr->orientation;
  temp[1] = 0;
  SetSACHV(KCMPNM,temp);
  // Depth:
  // NOTE: SAC uses meters for depth; PSN uses 0.1 km
  if(fmt > 2)
  SetSACHV(EVDP,(float)(100*phdr->u.newfmt.depth));
  //SetSACHV(DIST,(float)phdr->evdistkm);
  //SetSACHV(GCARC,(float)phdr->evdistgc);
  //SetSACHV(AZ,(float)phdr->az);
  //SetSACHV(BAZ,(float)phdr->baz);
  //SetSACHV(EV_MS,(float)phdr->ev_ms);
  //SetSACHV(EV_MB,(float)phdr->ev_mb);
  //SetSACHV(EV_MW,(float)phdr->ev_mw);
  //SetSACHV(EV_ML,(float)phdr->ev_ml);
  //SetSACHV(EV_MO,(float)phdr->ev_mo);
  long npts = GetSACHV(NPTS);
  PSACData = new float[npts];
  PSACDataOriginal = new float[npts];
  short *psndata = psn->GetData();
  for(int i=0;i<npts;i++)
  	PSACData[i] = psndata[i];
  memcpy(PSACDataOriginal,PSACData,npts*sizeof(float));
}

// Destructor
KSAC::~KSAC(){
  delete [] PSACData;
  delete PSACHD;
  delete [] PSACDataOriginal;
}

void KSAC::SystemTimeToSACTime(SYSTEMTIME st)
{
  SetSACHV(NZYEAR,(long)st.wYear);
  SetSACHV(NZJDAY,(long)julian(st.wYear,st.wMonth-1,st.wDay));
  SetSACHV(NZHOUR,(long)st.wHour);
  SetSACHV(NZMIN,(long)st.wMinute);
  SetSACHV(NZSEC,(long)st.wSecond);
  SetSACHV(NZMSEC,(long)st.wMilliseconds);
}
SYSTEMTIME KSAC::SACTimeToSystemTime()
{
	SYSTEMTIME st;
  st.wYear = GetSACHV(NZYEAR);
  int j = GetSACHV(NZJDAY);
  int month, day;
  month_day(j,st.wYear,month,day);
  st.wMonth = month;
  st.wDay = day;
  st.wHour = GetSACHV(NZHOUR);
  st.wMinute = GetSACHV(NZMIN);
  st.wSecond = GetSACHV(NZSEC);
  st.wMilliseconds = GetSACHV(NZMSEC);
  return st;
}

// Converts from SYSTEMTIME structure to class variable zTime
// Indicates time in seconds from 1601.
void KSAC::SACTimeToLongDouble()
{
	SYSTEMTIME st = SACTimeToSystemTime();
 	FILETIME ft;
  LARGE_INTEGER li;
  // Copy to a FILETIME structure
  SystemTimeToFileTime(&st,&ft);
  memcpy(&li,&ft,sizeof(LARGE_INTEGER));
  // Time in LARGE_INTEGER is stored in units of 100 nano-seconds
  zTime = li.QuadPart/1000000L;
  // Convert to seconds
  zTime /= 10.0;
}
// Convert ith pick time to a system time
SYSTEMTIME KSAC::PickTimeToSystemTime(int i)
{
	// Adjust starting time
  // Raw pick time
  float t = GetTn(i);
  SYSTEMTIME st = SACTimeToSystemTime();
  FILETIME ft;
  LARGE_INTEGER li, li2;
  // Copy to a FILETIME structure
  SystemTimeToFileTime(&st,&ft);
  memcpy(&li,&ft,sizeof(LARGE_INTEGER));
  // Put adjustment time in a LARGE_INTEGER rounded to 0.1 s
  li2.QuadPart = 0.5 + 10*t;	// Time is now in 0.1 second units
  LONGLONG l2 = li2.QuadPart;
  // Time in LARGE_INTEGER is stored in units of 100 nano-seconds
  //li2.QuadPart = li2.QuadPart*1000000L;
  l2 = l2*1000000L;
  // Add them
  li.QuadPart = li.QuadPart+l2;
  //li.QuadPart = li.QuadPart+li2.QuadPart;
  memcpy(&ft,&li,sizeof(LARGE_INTEGER));
  // Now convert back to a System time
  FileTimeToSystemTime(&ft,&st);
  return st;
}
// Convert ith pick time to LONGLONG Time
LONGLONG KSAC::PickTimeToLLTime(int i)
{
	// Adjust starting time
  // Raw pick time
  float t = GetTn(i);
  SYSTEMTIME st = SACTimeToSystemTime();
  FILETIME ft;
  LARGE_INTEGER li;
  // Copy to a FILETIME structure
  SystemTimeToFileTime(&st,&ft);
  memcpy(&li,&ft,sizeof(LARGE_INTEGER));
  // Put adjustment time in a LARGE_INTEGER rounded to 0.1 s
  LONGLONG l2 = 0.5 + 10*t;	// Time is now in 0.1 second units
  // Time in LARGE_INTEGER is stored in units of 100 nano-seconds
  //li2.QuadPart = li2.QuadPart*1000000L;
  l2 = l2*1000000L;
  // Add them
  li.QuadPart = li.QuadPart+l2;
  return li.QuadPart;
}
// Convert ith pick time to double
double KSAC::PickTimeToDouble(int i)
{
	// Adjust starting time
  // Raw pick time
  float t = GetTn(i);
  SYSTEMTIME st = SACTimeToSystemTime();
  FILETIME ft;
  LARGE_INTEGER li;
  // Copy to a FILETIME structure
  SystemTimeToFileTime(&st,&ft);
  memcpy(&li,&ft,sizeof(LARGE_INTEGER));
  // Convert to 0.1 seconds
  double tt = li.QuadPart/1000000L;
  // Convert to seconds
  tt /= 10.0;
  tt += t;
  return tt;
}



void KSAC::AdjustZTime(float t)
{
	// Adjust starting time
  SYSTEMTIME st = SACTimeToSystemTime();
  FILETIME ft;
  LARGE_INTEGER li, li2;
  // Copy to a FILETIME structure
  SystemTimeToFileTime(&st,&ft);
  memcpy(&li,&ft,sizeof(LARGE_INTEGER));
  // Put adjustment time in a LARGE_INTEGER rounded to 0.1 s
  li2.QuadPart = 0.5 + 10*t;	// Time is now in 0.1 second units
  // Time in LARGE_INTEGER is stored in units of 100 nano-seconds
  li2.QuadPart = li2.QuadPart*1000000L;
  // Add them
  li.QuadPart = li.QuadPart+li2.QuadPart;
  memcpy(&ft,&li,sizeof(LARGE_INTEGER));
  // Now convert back to a System time
  FileTimeToSystemTime(&ft,&st);
  // And to SAC time
  SystemTimeToSACTime(st);
}
//- initialize new sac header values
//
void KSAC::Initiate(char *PSACFile, short iflag)
{
  // set the floating value
  //
  int i;
  float *pft;
  long *pl;
  char *pc;

  zTime = 0;
  if(PSACFile != NULL) {
  	strcpy(filename,PSACFile);
  } else {
  	strcpy(filename,"untitled");
  }
  //TFileName fn(filename);
  //if(!fn.Exists()) {
  //	throw ke;
  //  return;
  //}
  if(iflag != 0)
    if(SFlag & 0x01)
      delete(PSACData);
  PSACData = 0;
  PSACDataOriginal = 0;
  pft=(float *) PSACHD;
  pl=(long *) PSACHD + 70;
  pc=(char *) PSACHD + 440;

  for(i=0;i < 70;i++) *(pft+i)=SACFUD;   // there are 70 float hv
  for(i=0;i < 40;i++) *(pl+i)=SACLUD;    // there are 40 long hv

  strncpy(pc,SACCUD,8);               // the rests are char* hv
  strncpy(pc+8,SACCUD,16);
  for(i=24;i<192;i+=8)
  	strncpy((pc+i),SACCUD,8);
  SFlag = 0x00;
  PSACHD->Npts = 0;
  // 2000.03.29 (ALJ): Even after I put in all the required SAC
  // variables, the file would not read into Unix SAC.  I added
  // the following variables and it then worked.  I don't know
  // which one made the difference and right now, I don't care.
  SetSACHV(IFTYPE,(long)ITIME);		// Time series
  SetSACHV(LEVEN,(long)1);		// Evenly spaced data
  SetSACHV(IDEP,(long)7);			// Velocity?
  SetSACHV(IZTYPE,(long)IT4);	// Reference time?
  SetSACHV(IEVTYP,(long)IQUAKE);   // IQUAKE Earthquake
  SetSACHV(IQUAL,(long)45);		// Quality?
  SetSACHV(LPSPOL,(long)0);		// negative polarity
  SetSACHV(LOVROK,(long)1);		// Okay to overwrite on disk
  SetSACHV(NVHDR,(long)6);		// Current header version
  if(PSACFile)
  {
    ReadSAC(PSACFile);
  	long npts = GetSACHV(NPTS);
    PSACDataOriginal = new float[npts];
  	memcpy(PSACDataOriginal,PSACData,npts*sizeof(float));
  }
}

// Compute depmin, depmax, depmen
void KSAC::GetMinMax()
{
  // Compute maximum, minium, and mean dependent variable
  float depmin = 1e20;
  float depmax = -1e20;
  float depmen = 0;		// Mean
  long npts = GetSACHV(NPTS);
  float *data = GetData();
  for(int i=0;i<npts;i++) {
  	float d=data[i];
    depmen += d;
    if(d < depmin)
    	depmin = d;
    if(d > depmax)
    	depmax = d;
  }
  if(npts != 0)
  	depmen /= npts;
  SetSACHV(DEPMIN,depmin);
  SetSACHV(DEPMAX,depmax);
  SetSACHV(DEPMEN,depmen);
}

// Clear pick times
void KSAC::ClearPickTimes()
{
	float *pft = &PSACHD->T00;
  for(int i=0;i<10;i++) {
  	*pft++ = 0;
  }
}

// Put a pick time in
void KSAC::PutTn(int i,float time)
{
	float *pft = &PSACHD->T00;
  *(pft + i) = time;
  // 2006.06.24: Take out following
  // code which clears out higher picks
  //i++;
  //for(;i<10;i++)
  //  *(pft + i) = 0;
}

// Fetch a pick time
float KSAC::GetTn(int i)
{
	float *pft = &PSACHD->T00;
  float time = *(pft + i);
  if(time == -12345.0)
  	time = 0;
 	return time;
}

//- ReadSac : read sac file
//
short KSAC::ReadSAC(char *PSACFile) throw(KSACException)
{
	ifstream SACStrm(PSACFile,ios::binary);
// read SAC header information
	SACStrm.read((unsigned char *) PSACHD,SACHDLen);
  SACHVSwab();
  filetype = SACFILE;
// read SAC data
//
	if(PSACData != 0)
   	delete PSACData;
  long npts = GetSACHV(NPTS);
  if(npts < 0 || npts > 2e6)
  	throw(ke);
  PSACData = new float[npts];

  SACStrm.read((unsigned char *) PSACData,npts*sizeof(float));
   //long ret = SACStrm.gcount();
  //ret = ReadFile(h,(unsigned char *) PSACData,npts*sizeof(float),&bytesread,0);
  //if(bytesread != npts*sizeof(float))
  //	return 1;
  SACDataSwab();
  GetMinMax();

  SFlag |= 0x01;
   return 0;
}

// Restore original data
void KSAC::Restore()
{
 	long npts = GetSACHV(NPTS);
	//memcpy(PSACData,PSACDataOriginal,npts*sizeof(float));
  SetData(PSACDataOriginal,npts);
}

// Reset original data
void KSAC::ResetOriginal()
{
	delete PSACDataOriginal;
 	long npts = GetSACHV(NPTS);
  PSACDataOriginal = new float[npts*sizeof(float)];
  memcpy(PSACDataOriginal,GetData(),npts*sizeof(float));
}

//-WriteSac
//
short KSAC::WriteSAC(char* fn)
{
   long npts = GetSACHV(NPTS);
   if(npts == SACFUD)
      return kerror(2001,"KSAC::WriteSAC");
   if(fn != 0)
   	 strcpy(filename,fn);
   ofstream SACStrm(filename,ios::binary);
   SACHVSwab();
   SACStrm.write((unsigned char *) PSACHD,SACHDLen);
   // Have to swap bytes back
   SACHVSwab();

// output SAC data values
//
   //char junk[32];
   //sprintf(junk,"npts: %d",npts);
   //MdiClient->MessageBox(junk);
   SACDataSwab();
   SACStrm.write((unsigned char *)PSACData,npts * sizeof(float));
   SACDataSwab();
   return 0;
}

//- Swab SAC header value
//
void KSAC::SACHVSwab()
{
// Swab the float values
//
   Swab(PSACHD->Delta);                        //  1-  4
   Swab(PSACHD->DepMin);                       //  5-  8
   Swab(PSACHD->DepMax);                       //  9- 12
   Swab(PSACHD->Scale);                        // 13- 16
   Swab(PSACHD->ODelta);                       // 17- 20
   Swab(PSACHD->Beg);                          // 21- 24
   Swab(PSACHD->End);                          // 25- 28
   Swab(PSACHD->Org);                          // 29- 32
   Swab(PSACHD->Arr);                          // 33- 36
   Swab(PSACHD->Internal1);                    // 37- 40
   Swab(PSACHD->T00);                          // 41- 44
   Swab(PSACHD->T01);                          // 45- 48
   Swab(PSACHD->T02);                          // 49- 52
   Swab(PSACHD->T03);                          // 53- 56
   Swab(PSACHD->T04);                          // 57- 60
   Swab(PSACHD->T05);                          // 61- 64
   Swab(PSACHD->T06);                          // 65- 68
   Swab(PSACHD->T07);                          // 69- 72
   Swab(PSACHD->T08);                          // 73- 76
   Swab(PSACHD->T09);                          // 77- 80
   Swab(PSACHD->Fini);                         // 81- 84
   Swab(PSACHD->Resp0);                        // 85- 88
   Swab(PSACHD->Resp1);                        // 89- 92
   Swab(PSACHD->Resp2);                        // 93- 96
   Swab(PSACHD->Resp3);                        // 97-100
   Swab(PSACHD->Resp4);                        //101-104
   Swab(PSACHD->Resp5);                        //105-108
   Swab(PSACHD->Resp6);                        //109-112
   Swab(PSACHD->Resp7);                        //113-116
   Swab(PSACHD->Resp8);                        //117-120
   Swab(PSACHD->Resp9);                        //121-124
   Swab(PSACHD->StLa);                         //125-128
   Swab(PSACHD->StLo);                         //129-132
   Swab(PSACHD->StEl);                         //133-136
   Swab(PSACHD->StDp);                         //137-140
   Swab(PSACHD->EvLa);                         //141-144
   Swab(PSACHD->EvLo);                         //145-148
   Swab(PSACHD->EvEl);                         //149-152
   Swab(PSACHD->EvDp);                         //153-156
   Swab(PSACHD->Unused1);                      //157-160
   Swab(PSACHD->Ev_ms);                        //161-164
   Swab(PSACHD->Ev_mb);                        //165-168
   Swab(PSACHD->Ev_mw);                        //169-172
   Swab(PSACHD->Ev_ml);                        //173-176
   Swab(PSACHD->Ev_mo);                        //177-180
   Swab(PSACHD->User5);                        //181-184
   Swab(PSACHD->User6);                        //185-188
   Swab(PSACHD->User7);                        //189-192
   Swab(PSACHD->User8);                        //193-196
   Swab(PSACHD->User9);                        //197-200
   Swab(PSACHD->Dist);                         //201-204
   Swab(PSACHD->Az);                           //205-208
   Swab(PSACHD->BAz);                          //209-212
   Swab(PSACHD->GCArc);                        //213-216
   Swab(PSACHD->Internal2);                    //217-220
   Swab(PSACHD->Internal3);                    //221-224
   Swab(PSACHD->DepMen);                       //225-228
   Swab(PSACHD->CmpAz);                        //229-232
   Swab(PSACHD->CmpInc);                       //233-236
   Swab(PSACHD->XMinmum);                      //237-240
   Swab(PSACHD->XMaxmum);                      //241-244
   Swab(PSACHD->YMinmum);                      //245-248
   Swab(PSACHD->YMaxmum);                      //249-252
   Swab(PSACHD->Unused2);                      //253-256
   Swab(PSACHD->Unused3);                      //257-260
   Swab(PSACHD->Unused4);                      //261-264
   Swab(PSACHD->Unused5);                      //265-268
   Swab(PSACHD->Unused6);                      //269-272
   Swab(PSACHD->Unused7);                      //273-276
   Swab(PSACHD->Unused8);                      //277-280
// Swab the long values
//
   Swab(PSACHD->NZYear);                       //281-284
   Swab(PSACHD->NZJDay);                       //285-288
   Swab(PSACHD->NZHour);                       //289-292
   Swab(PSACHD->NZMin);                        //293-296
   Swab(PSACHD->NZSec);                        //297-300
   Swab(PSACHD->NZMSec);                       //301-304
   Swab(PSACHD->NVHdr);                        //305-308
   Swab(PSACHD->Internal4);                    //309-312
   Swab(PSACHD->Internal5);                    //313-316
   Swab(PSACHD->Npts);                         //317-320
   Swab(PSACHD->Internal6);                    //321-324
   Swab(PSACHD->Internal7);                    //325-328
   Swab(PSACHD->Unused9);                      //329-332
   Swab(PSACHD->Unused10);                     //333-336
   Swab(PSACHD->Unused11);                     //337-340
   Swab(PSACHD->IFType);                       //341-344
   Swab(PSACHD->IDep);                         //345-348
   Swab(PSACHD->IZType);                       //349-352
   Swab(PSACHD->Unused12);                     //353-356
   Swab(PSACHD->IInst);                        //357-360
   Swab(PSACHD->ISTReg);                       //361-364
   Swab(PSACHD->IEVReg);                       //365-368
   Swab(PSACHD->IEVTyp);                       //369-372
   Swab(PSACHD->IQual);                        //373-376
   Swab(PSACHD->ISynth);                       //377-380
   Swab(PSACHD->Unused13);                     //381-384
   Swab(PSACHD->Unused14);                     //385-388
   Swab(PSACHD->Unused15);                     //389-392
   Swab(PSACHD->Unused16);                     //393-396
   Swab(PSACHD->Unused17);                     //397-400
   Swab(PSACHD->Unused18);                     //401-404
   Swab(PSACHD->Unused19);                     //405-408
   Swab(PSACHD->Unused20);                     //409-412
   Swab(PSACHD->Unused21);                     //413-416
   Swab(PSACHD->Unused22);                     //417-420
   Swab(PSACHD->LEven);                        //421-424
   Swab(PSACHD->LPspol);                       //425-428
   Swab(PSACHD->LOvrok);                       //429-432
   Swab(PSACHD->LCalda);                       //433-436
   Swab(PSACHD->Unused23);                     //437-440
}

//-Swab data bytes for PC version
//
void KSAC::SACDataSwab()
{
   ArraySwab(PSACData,GetSACHV(NPTS));
}


//- get Set Sac header value (float)
//
void KSAC::SetSACHV(SACFHV fhv,float fvalue)
{
   switch(fhv){
      case DELTA:
         PSACHD->Delta=fvalue;        break;    //  1-  4
      case DEPMIN:
         PSACHD->DepMin=fvalue;       break;    //  5-  8
      case DEPMAX:
         PSACHD->DepMax=fvalue;       break;    //  9- 12
      case SCALE:
         PSACHD->Scale=fvalue;        break;    // 13- 16
      case ODELTA:
         PSACHD->ODelta=fvalue;       break;    // 17- 20
      case B:
         PSACHD->Beg=fvalue;          break;    // 21- 24
      case E:
         PSACHD->End=fvalue;          break;    // 25- 28
      case O:
         PSACHD->Org=fvalue;          break;    // 29- 32
      case A:
         PSACHD->Arr=fvalue;          break;    // 33- 36
      case INTERNAL1:
         PSACHD->Internal1=fvalue;    break;    // 37- 40
      case T0:
         PSACHD->T00=fvalue;          break;    // 41- 44
      case T1:
         PSACHD->T01=fvalue;          break;    // 45- 48
      case T2:
         PSACHD->T02=fvalue;          break;    // 49- 52
      case T3:
         PSACHD->T03=fvalue;          break;    // 53- 56
      case T4:
         PSACHD->T04=fvalue;          break;    // 57- 60
      case T5:
         PSACHD->T05=fvalue;          break;    // 61- 64
      case T6:
         PSACHD->T06=fvalue;          break;    // 65- 68
      case T7:
         PSACHD->T07=fvalue;          break;    // 69- 72
      case T8:
         PSACHD->T08=fvalue;          break;    // 73- 76
      case T9:
         PSACHD->T09=fvalue;          break;    // 77- 80
      case F:
         PSACHD->Fini=fvalue;         break;    // 81- 84
      case RESP0:
         PSACHD->Resp0=fvalue;        break;    // 85- 88
      case RESP1:
         PSACHD->Resp1=fvalue;        break;    // 89- 92
      case RESP2:
         PSACHD->Resp2=fvalue;        break;    // 93- 96
      case RESP3:
         PSACHD->Resp3=fvalue;        break;    // 97-100
      case RESP4:
         PSACHD->Resp4=fvalue;        break;    //101-104
      case RESP5:
         PSACHD->Resp5=fvalue;        break;    //105-108
      case RESP6:
         PSACHD->Resp6=fvalue;        break;    //109-112
      case RESP7:
         PSACHD->Resp7=fvalue;        break;    //113-116
      case RESP8:
         PSACHD->Resp8=fvalue;        break;    //117-120
      case RESP9:
         PSACHD->Resp9=fvalue;        break;    //121-124
      case STLA:
         PSACHD->StLa=fvalue;         break;    //125-128
      case STLO:
         PSACHD->StLo=fvalue;         break;    //129-132
      case STEL:
         PSACHD->StEl=fvalue;         break;    //133-136
      case STDP:
         PSACHD->StDp=fvalue;         break;    //137-140
      case EVLA:
         PSACHD->EvLa=fvalue;         break;    //141-144
      case EVLO:
         PSACHD->EvLo=fvalue;         break;    //145-148
      case EVEL:
         PSACHD->EvEl=fvalue;         break;    //149-152
      case EVDP:
         PSACHD->EvDp=fvalue;         break;    //153-156
      case UNUSED1:
         PSACHD->Unused1=fvalue;      break;    //157-160
      case EV_MS:
         PSACHD->Ev_ms=fvalue;        break;    //161-164
      case EV_MB:
         PSACHD->Ev_mb=fvalue;        break;    //165-168
      case EV_MW:
         PSACHD->Ev_mw=fvalue;        break;    //169-172
      case EV_ML:
         PSACHD->Ev_ml=fvalue;        break;    //173-176
      case EV_MO:
         PSACHD->Ev_mo=fvalue;        break;    //177-180
      case USER5:
         PSACHD->User5=fvalue;        break;    //181-184
      case USER6:
         PSACHD->User6=fvalue;        break;    //185-188
      case USER7:
         PSACHD->User7=fvalue;        break;    //189-192
      case USER8:
         PSACHD->User8=fvalue;        break;    //193-196
      case USER9:
         PSACHD->User9=fvalue;        break;    //197-200
      case DIST:
         PSACHD->Dist=fvalue;         break;    //201-204
      case AZ:
         PSACHD->Az=fvalue;           break;    //205-208
      case BAZ:
         PSACHD->BAz=fvalue;          break;    //209-212
      case GCARC:
         PSACHD->GCArc=fvalue;        break;    //213-216
      case INTERNAL2:
         PSACHD->Internal2=fvalue;    break;    //217-220
      case INTERNAL3:
         PSACHD->Internal3=fvalue;    break;    //221-224
      case DEPMEN:
         PSACHD->DepMen=fvalue;       break;    //225-228
      case CMPAZ:
         PSACHD->CmpAz=fvalue;        break;    //229-232
      case CMPINC:
         PSACHD->CmpInc=fvalue;       break;    //233-236
      case XMINMUM:
         PSACHD->XMinmum=fvalue;      break;    //237-240
      case XMAXMUM:
         PSACHD->XMaxmum=fvalue;      break;    //241-244
      case YMINMUM:
         PSACHD->YMinmum=fvalue;      break;    //245-248
      case YMAXMUM:
         PSACHD->YMaxmum=fvalue;      break;    //249-252
      case UNUSED2:
         PSACHD->Unused2=fvalue;      break;    //253-256
      case UNUSED3:
         PSACHD->Unused3=fvalue;      break;    //257-260
      case UNUSED4:
         PSACHD->Unused4=fvalue;      break;    //261-264
      case UNUSED5:
         PSACHD->Unused5=fvalue;      break;    //265-268
      case UNUSED6:
         PSACHD->Unused6=fvalue;      break;    //269-272
      case UNUSED7:
         PSACHD->Unused7=fvalue;      break;    //273-276
      case UNUSED8:
         PSACHD->Unused8=fvalue;      break;    //277-280
   }
}


//
//
void KSAC::SetSACHV(SACLHV ,KIFTYPE ftype)
{
   PSACHD->IFType = (long)ftype;
}

//
//
void KSAC::SetSACHV(SACLHV ,KIDEP dep)
{
   PSACHD->IDep = dep;
}

//
void KSAC::SetSACHV(SACLHV ,KIZTYPE ztype)
{
   PSACHD->IZType=ztype;
}

//
void KSAC::SetSACHV(SACLHV ,KIEVTYP evtyp)
{
   PSACHD->IEVTyp = evtyp;
}

//
void KSAC::SetSACHV(SACLHV ,KIQUAL qual)
{
   PSACHD->IQual = qual;
}

//
void KSAC::SetSACHV(SACLHV ,KISYNTH synth)
{
   PSACHD->ISynth = synth;
}

//- set Sac header value (long)
//
void KSAC::SetSACHV(SACLHV lhv,long lvalue)
{
   switch(lhv){
      case NZYEAR:
         PSACHD->NZYear=lvalue;      break;    //281-284
      case NZJDAY:
         PSACHD->NZJDay=lvalue;      break;    //285-288
      case NZHOUR:
         PSACHD->NZHour=lvalue;      break;    //289-292
      case NZMIN:
         PSACHD->NZMin=lvalue;       break;    //293-296
      case NZSEC:
         PSACHD->NZSec=lvalue;       break;    //297-300
      case NZMSEC:
         PSACHD->NZMSec=lvalue;      break;    //301-304
      case NVHDR:
         PSACHD->NVHdr=lvalue;       break;    //305-308
      case INTERNAL4:
         PSACHD->Internal4=lvalue;   break;    //309-312
      case INTERNAL5:
         PSACHD->Internal5=lvalue;   break;    //313-316
      case NPTS:
         PSACHD->Npts=lvalue;        break;    //317-320
      case INTERNAL6:
         PSACHD->Internal6=lvalue;   break;    //321-324
      case INTERNAL7:
         PSACHD->Internal7=lvalue;   break;    //325-328
      case UNUSED9:
         PSACHD->Unused9=lvalue;     break;    //329-332
      case UNUSED10:
         PSACHD->Unused10=lvalue;    break;    //333-336
      case UNUSED11:
         PSACHD->Unused11=lvalue;    break;    //337-340
      case IFTYPE:
         PSACHD->IFType=lvalue;      break;    //341-344
      case IDEP:
         PSACHD->IDep=lvalue;        break;    //345-348
      case IZTYPE:
         PSACHD->IZType=lvalue;      break;    //349-352
      case UNUSED12:
         PSACHD->Unused12=lvalue;    break;    //353-356
      case IINST:
         PSACHD->IInst=lvalue;       break;    //357-360
      case ISTREG:
         PSACHD->ISTReg=lvalue;      break;    //361-364
      case IEVREG:
         PSACHD->IEVReg=lvalue;      break;    //365-368
      case IEVTYP:
         PSACHD->IEVTyp=lvalue;      break;    //369-372
      case IQUAL:
         PSACHD->IQual=lvalue;       break;    //373-376
      case ISYNTH:
         PSACHD->ISynth=lvalue;      break;    //377-380
      case UNUSED13:
         PSACHD->Unused13=lvalue;    break;    //381-384
      case UNUSED14:
         PSACHD->Unused14=lvalue;    break;    //385-388
      case UNUSED15:
         PSACHD->Unused15=lvalue;    break;    //389-392
      case UNUSED16:
         PSACHD->Unused16=lvalue;    break;    //393-396
      case UNUSED17:
         PSACHD->Unused17=lvalue;    break;    //397-400
      case UNUSED18:
         PSACHD->Unused18=lvalue;    break;    //401-404
      case UNUSED19:
         PSACHD->Unused19=lvalue;    break;    //405-408
      case UNUSED20:
         PSACHD->Unused20=lvalue;    break;    //409-412
      case UNUSED21:
         PSACHD->Unused21=lvalue;    break;    //413-416
      case UNUSED22:
         PSACHD->Unused22=lvalue;    break;    //417-420
      case LEVEN:
         PSACHD->LEven=lvalue;       break;    //421-424
      case LPSPOL:
         PSACHD->LPspol=lvalue;      break;    //425-428
      case LOVROK:
         PSACHD->LOvrok=lvalue;      break;    //429-432
      case LCALDA:
         PSACHD->LCalda=lvalue;      break;    //433-436
      case UNUSED23:
         PSACHD->Unused23=lvalue;
   }
}

//- set Sac header value (char)
//
void KSAC::SetSACHV(SACCHV chv,char *cvalue)
{
   switch(chv){
      case KSTNM:
         strncpy(PSACHD->KStNm,cvalue,8);
         break;
      case KEVNM:
         strncpy(PSACHD->KEvNm,cvalue,16);
         break;
      case KHOLE:
         strncpy(PSACHD->KHole,cvalue,8);
         break;
      case KO:
         strncpy(PSACHD->Ko,cvalue,8);
         break;
      case KA:
         strncpy(PSACHD->Ka,cvalue,8);
         break;
      case KT0:
         strncpy(PSACHD->KT00,cvalue,8);
         break;
      case KT1:
         strncpy(PSACHD->KT01,cvalue,8);
         break;
      case KT2:
         strncpy(PSACHD->KT02,cvalue,8);
         break;
      case KT3:
         strncpy(PSACHD->KT03,cvalue,8);
         break;
      case KT4:
         strncpy(PSACHD->KT04,cvalue,8);
         break;
      case KT5:
         strncpy(PSACHD->KT05,cvalue,8);
         break;
      case KT6:
         strncpy(PSACHD->KT06,cvalue,8);
         break;
      case KT7:
         strncpy(PSACHD->KT07,cvalue,8);
         break;
      case KT8:
         strncpy(PSACHD->KT08,cvalue,8);
         break;
      case KT9:
         strncpy(PSACHD->KT09,cvalue,8);
         break;
      case KF:
         strncpy(PSACHD->Kf,cvalue,8);
         break;
      case KUSER0:
         strncpy(PSACHD->KUser0,cvalue,8);
         break;
      case KUSER1:
         strncpy(PSACHD->KUser1,cvalue,8);
         break;
      case KUSER2:
         strncpy(PSACHD->KUser2,cvalue,8);
         break;
      case KCMPNM:
         strncpy(PSACHD->KCmpNm,cvalue,8);
         break;
      case KNETWK:
         strncpy(PSACHD->KNetwk,cvalue,8);
         break;
      case KDATRD:
         strncpy(PSACHD->KDatrd,cvalue,8);
         break;
      case KINST:
         strncpy(PSACHD->KInst,cvalue,8);
         break;
      default:
         break;
   }
}


// set the data array of sac file -- only dependent variables
//
short KSAC::SetData(float* sdata,long npts)
{

   try{PSACData=new float[npts];}
   catch(xalloc){ kerror(1,"KSAC::SetData(float)"); abort();}

   for(long i=0;i<npts;i++)
      PSACData[i]=sdata[i];

   SetSACHV(NPTS,(long)npts);
   return 0;
}

//- get the Sac header value (float)
//
float KSAC::GetSACHV(SACFHV fhv)
{
   switch(fhv){
      case DELTA:
         return PSACHD->Delta;                 //  1-  4
      case DEPMIN:
         return PSACHD->DepMin;                //  5-  8
      case DEPMAX:
         return PSACHD->DepMax;                //  9- 12
      case SCALE:
         return PSACHD->Scale;                 // 13- 16
      case ODELTA:
         return PSACHD->ODelta;                // 17- 20
      case B:
         return PSACHD->Beg;                   // 21- 24
      case E:
         return PSACHD->End;                   // 25- 28
      case O:
         return PSACHD->Org;                   // 29- 32
      case A:
         return PSACHD->Arr;                   // 33- 36
      case INTERNAL1:
         return PSACHD->Internal1;             // 37- 40
      case T0:
         return PSACHD->T00;                   // 41- 44
      case T1:
         return PSACHD->T01;                   // 45- 48
      case T2:
         return PSACHD->T02;                   // 49- 52
      case T3:
         return PSACHD->T03;                   // 53- 56
      case T4:
         return PSACHD->T04;                   // 57- 60
      case T5:
         return PSACHD->T05;                   // 61- 64
      case T6:
         return PSACHD->T06;                   // 65- 68
      case T7:
         return PSACHD->T07;                   // 69- 72
      case T8:
         return PSACHD->T08;                   // 73- 76
      case T9:
         return PSACHD->T09;                   // 77- 80
      case F:
         return PSACHD->Fini;                  // 81- 84
      case RESP0:
         return PSACHD->Resp0;                 // 85- 88
      case RESP1:
         return PSACHD->Resp1;                 // 89- 92
      case RESP2:
         return PSACHD->Resp2;                 // 93- 96
      case RESP3:
         return PSACHD->Resp3;                 // 97-100
      case RESP4:
         return PSACHD->Resp4;                 //101-104
      case RESP5:
         return PSACHD->Resp5;                 //105-108
      case RESP6:
         return PSACHD->Resp6;                 //109-112
      case RESP7:
         return PSACHD->Resp7;                 //113-116
      case RESP8:
         return PSACHD->Resp8;                 //117-120
      case RESP9:
         return PSACHD->Resp9;                 //121-124
      case STLA:
         return PSACHD->StLa;                  //125-128
      case STLO:
         return PSACHD->StLo;                  //129-132
      case STEL:
         return PSACHD->StEl;                  //133-136
      case STDP:
         return PSACHD->StDp;                  //137-140
      case EVLA:
         return PSACHD->EvLa;                  //141-144
      case EVLO:
         return PSACHD->EvLo;                  //145-148
      case EVEL:
         return PSACHD->EvEl;                  //149-152
      case EVDP:
         return PSACHD->EvDp;                  //153-156
      case UNUSED1:
         return PSACHD->Unused1;               //157-160
      case EV_MS:
         return PSACHD->Ev_ms;                 //161-164
      case EV_MB:
         return PSACHD->Ev_mb;                 //165-168
      case EV_MW:
         return PSACHD->Ev_mw;                 //169-172
      case EV_ML:
         return PSACHD->Ev_ml;                 //173-176
      case EV_MO:
         return PSACHD->Ev_mo;                 //177-180
      case USER5:
         return PSACHD->User5;                 //181-184
      case USER6:
         return PSACHD->User6;                 //185-188
      case USER7:
         return PSACHD->User7;                 //189-192
      case USER8:
         return PSACHD->User8;                 //193-196
      case USER9:
         return PSACHD->User9;                 //197-200
      case DIST:
         return PSACHD->Dist;                  //201-204
      case AZ:
         return PSACHD->Az;                    //205-208
      case BAZ:
         return PSACHD->BAz;                   //209-212
      case GCARC:
         return PSACHD->GCArc;                 //213-216
      case INTERNAL2:
         return PSACHD->Internal2;             //217-220
      case INTERNAL3:
         return PSACHD->Internal3;             //221-224
      case DEPMEN:
         return PSACHD->DepMen;                //225-228
      case CMPAZ:
         return PSACHD->CmpAz;                 //229-232
      case CMPINC:
         return PSACHD->CmpInc;                //233-236
      case XMINMUM:
         return PSACHD->XMinmum;               //237-240
      case XMAXMUM:
         return PSACHD->XMaxmum;               //241-244
      case YMINMUM:
         return PSACHD->YMinmum;               //245-248
      case YMAXMUM:
         return PSACHD->YMaxmum;               //249-252
      case UNUSED2:
         return PSACHD->Unused2;               //253-256
      case UNUSED3:
         return PSACHD->Unused3;               //257-260
      case UNUSED4:
         return PSACHD->Unused4;               //261-264
      case UNUSED5:
         return PSACHD->Unused5;               //265-268
      case UNUSED6:
         return PSACHD->Unused6;               //269-272
      case UNUSED7:
         return PSACHD->Unused7;               //273-276
      case UNUSED8:
         return PSACHD->Unused8;               //277-280
      default:
         return 0.;
   }
}

//- get Sac value (long)
//
int KSAC::GetSACHV(SACLHV lhv)
{
   switch(lhv){
      case NZYEAR:
         return  PSACHD->NZYear;                //281-284
      case NZJDAY:
         return  PSACHD->NZJDay;                //285-288
      case NZHOUR:
         return  PSACHD->NZHour;                //289-292
      case NZMIN:
         return  PSACHD->NZMin;                 //293-296
      case NZSEC:
         return  PSACHD->NZSec;                 //297-300
      case NZMSEC:
         return  PSACHD->NZMSec;                //301-304
      case NVHDR:
         return  PSACHD->NVHdr;                 //305-308
      case INTERNAL4:
         return  PSACHD->Internal4;             //309-312
      case INTERNAL5:
         return  PSACHD->Internal5;             //313-316
      case NPTS:
         return  PSACHD->Npts;                  //317-320
      case INTERNAL6:
         return  PSACHD->Internal6;             //321-324
      case INTERNAL7:
         return  PSACHD->Internal7;             //325-328
      case UNUSED9:
         return  PSACHD->Unused9;               //329-332
      case UNUSED10:
         return  PSACHD->Unused10;              //333-336
      case UNUSED11:
         return  PSACHD->Unused11;              //337-340
      case IFTYPE:
         return (KIFTYPE)PSACHD->IFType;       //341-344
      case IDEP:
         return (KIDEP)PSACHD->IDep;           //345-348
      case IZTYPE:
         return (KIZTYPE)PSACHD->IZType;       //349-352
      case UNUSED12:
         return  PSACHD->Unused12;              //353-356
      case IINST:
         return  PSACHD->IInst;                 //357-360
      case ISTREG:
         return  PSACHD->ISTReg;                //361-364
      case IEVREG:
         return  PSACHD->IEVReg;                //365-368
      case IEVTYP:
         return (KIEVTYP)PSACHD->IEVTyp;       //369-372
      case IQUAL:
         return (KIQUAL)PSACHD->IQual;         //373-376
      case ISYNTH:
         return (KISYNTH)PSACHD->ISynth;       //377-380
      case UNUSED13:
         return  PSACHD->Unused13;              //381-384
      case UNUSED14:
         return  PSACHD->Unused14;              //385-388
      case UNUSED15:
         return  PSACHD->Unused15;              //389-392
      case UNUSED16:
         return  PSACHD->Unused16;              //393-396
      case UNUSED17:
         return  PSACHD->Unused17;              //397-400
      case UNUSED18:
         return  PSACHD->Unused18;              //401-404
      case UNUSED19:
         return  PSACHD->Unused19;              //405-408
      case UNUSED20:
         return  PSACHD->Unused20;              //409-412
      case UNUSED21:
         return  PSACHD->Unused21;              //413-416
      case UNUSED22:
         return  PSACHD->Unused22;              //417-420
      case LEVEN:
         return  PSACHD->LEven;                 //421-424
      case LPSPOL:
         return  PSACHD->LPspol;                //425-428
      case LOVROK:
         return  PSACHD->LOvrok;                //429-432
      case LCALDA:
         return  PSACHD->LCalda;                //433-436
      case UNUSED23:
         return  PSACHD->Unused23;              //437-440
      default:
         return 0;
   }
}

//- get Sac header value (char)
//
char* KSAC::GetSACHV(SACCHV chv)
{
   static char temp[17];
   memset(temp,0,17);
   switch(chv){
      case KSTNM:
         strncpy(temp,PSACHD->KStNm,8);
         break;
      case KEVNM:
         strncpy(temp,PSACHD->KEvNm,16);
         break;
      case KHOLE:
         strncpy(temp,PSACHD->KHole,8);
         break;
      case KO:
         strncpy(temp,PSACHD->Ko,8);
         break;
      case KA:
         strncpy(temp,PSACHD->Ka,8);
         break;
      case KT0:
         strncpy(temp,PSACHD->KT00,8);
         break;
      case KT1:
         strncpy(temp,PSACHD->KT01,8);
         break;
      case KT2:
         strncpy(temp,PSACHD->KT02,8);
         break;
      case KT3:
         strncpy(temp,PSACHD->KT03,8);
         break;
      case KT4:
         strncpy(temp,PSACHD->KT04,8);
         break;
      case KT5:
         strncpy(temp,PSACHD->KT05,8);
         break;
      case KT6:
         strncpy(temp,PSACHD->KT06,8);
         break;
      case KT7:
         strncpy(temp,PSACHD->KT07,8);
         break;
      case KT8:
         strncpy(temp,PSACHD->KT08,8);
         break;
      case KT9:
         strncpy(temp,PSACHD->KT09,8);
         break;
      case KF:
         strncpy(temp,PSACHD->Kf,8);
         break;
      case KUSER0:
         strncpy(temp,PSACHD->KUser0,8);
         break;
      case KUSER1:
         strncpy(temp,PSACHD->KUser1,8);
         break;
      case KUSER2:
         strncpy(temp,PSACHD->KUser2,8);
         break;
      case KCMPNM:
         strncpy(temp,PSACHD->KCmpNm,8);
         break;
      case KNETWK:
         strncpy(temp,PSACHD->KNetwk,8);
         break;
      case KDATRD:
         strncpy(temp,PSACHD->KDatrd,8);
         break;
      case KINST:
         strncpy(temp,PSACHD->KInst,8);
         break;
      default:
         break;
   }
   return temp;
}

//-------------- friend member function ----------------------
//
istream& operator>>(istream& is,KSAC& sac)
{

// read SAC header information
   is.read((unsigned char *) sac.PSACHD,SACHDLen);
   sac.SACHVSwab();
// read SAC data
//
   if(sac.PSACData != 0) delete sac.PSACData;
   sac.PSACData = new float[sac.GetSACHV(NPTS)];

   is.read((unsigned char *) sac.PSACData,
             sac.GetSACHV(NPTS)*sizeof(float));
   sac.SACDataSwab();
   sac.SFlag |= 0x01;
   return is;
}

//
ostream& operator<<(ostream& os,KSAC& sac)
{
   sac.SACHVSwab();


// write SAC header value
//
   os.write((unsigned char *) sac.PSACHD,SACHDLen);
   sac.SACHVSwab();

// output SAC data values
//
   sac.SACDataSwab();
   os.write((unsigned char *)sac.PSACData,
                 sac.GetSACHV(NPTS) * sizeof(float));
   return os;
}

