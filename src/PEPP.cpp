#include "as1.h"

// undefined value in PEPP format and header length
static short          PEPPHLen=HEADER_SIZE;           // bytes
static float          FUNDEF=-12345.;
static long           NUNDEF=-12345;
//static bool						LUNDEF= false;
static char*          KUNDEF="-12345";
static char						CUNDEF = '?';

//void PEPPException::exception()
//{
//	MdiClient->MessageBox("PEPP file error");
//}
PEPPException pe;
// PEPP member functions
// PEPP constructor for constructing internally
PEPP::PEPP(KSAC *ksac) {
	PHdr = new FileHeader24;
  Initiate();
  // Make a copy of the data
  float *f = ksac->GetData();
  PHdr->npts = ksac->GetSACHV(NPTS);
  PPEPPData = new float[PHdr->npts];
  memcpy(PPEPPData,f,(PHdr->npts)*sizeof(float));
  strcpy(filename,"untitled");
  PHdr->stime.year = (short)ksac->GetSACHV(NZYEAR);
  PHdr->stime.jday = (short)ksac->GetSACHV(NZJDAY);
  int month, day;
  month_day(PHdr->stime.jday,PHdr->stime.year,
  	month,day);
  PHdr->stime.month = (short)month;
  PHdr->stime.day = (short)day;
  PHdr->stime.hour = (short)ksac->GetSACHV(NZHOUR);
  PHdr->stime.min  = (short)ksac->GetSACHV(NZMIN);
  PHdr->stime.sec  = (float)ksac->GetSACHV(NZSEC);
	PHdr->stime.sec +=	0.001*ksac->GetSACHV(NZMSEC);
  PHdr->begin_time = 0;
  PHdr->end_time = (PHdr->npts-1)*ksac->GetSACHV(DELTA);
  PHdr->samplingrate = 1.0/ksac->GetSACHV(DELTA);
  PHdr->depmin = ksac->GetSACHV(DEPMIN);
  PHdr->depmax = ksac->GetSACHV(DEPMAX);
  PHdr->depmen = ksac->GetSACHV(DEPMEN);
 	PHdr->stla = ksac->GetSACHV(STLA);
  PHdr->stlo = ksac->GetSACHV(STLO);
  PHdr->stel = ksac->GetSACHV(STEL);
  strncpy(PHdr->kstnm,ksac->GetSACHV(KSTNM),8);
  PHdr->cmpname,*ksac->GetSACHV(KCMPNM);
  PHdr->evla = ksac->GetSACHV(EVLA);
  PHdr->evlo = ksac->GetSACHV(EVLO);
  // NOTE:
  // SAC uses meter for depth;  PEPP uses km.
  PHdr->evdp = ksac->GetSACHV(EVDP)/1000;
  PHdr->az = ksac->GetSACHV(AZ);
  PHdr->baz = ksac->GetSACHV(BAZ);
  PHdr->evdistgc = ksac->GetSACHV(GCARC);
  PHdr->evdistkm = ksac->GetSACHV(DIST);
  PHdr->ev_ms = ksac->GetSACHV(EV_MS);
  PHdr->ev_mb = ksac->GetSACHV(EV_MB);
  PHdr->ev_mw = ksac->GetSACHV(EV_MW);
  PHdr->ev_ml = ksac->GetSACHV(EV_ML);
  PHdr->ev_mo = ksac->GetSACHV(EV_MO);
	}
// Destructor
PEPP::~PEPP(){
  delete PPEPPData;
  delete PHdr;
}
//- initialize new sac header values
//
void PEPP::Initiate(char *PPEPPFile, short iflag)
{
  // set the floating value
  //

  if(PPEPPFile != NULL) {
  	strcpy(filename,PPEPPFile);
  } else {
  	strcpy(filename,"untitled");
  }
  if(iflag != 0)
    if(SFlag & 0x01)
      delete(PPEPPData);
  PPEPPData = 0;
  //char junk[32];
  //sprintf(junk,": %d",sizeof(Header24));
  //MdiClient->MessageBox(junk);


  PHdr->lebeByte = 3737;
  strcpy(PHdr->verno,PHDR_VER_24);
  PHdr->idep = (short)NUNDEF;
  strcpy(PHdr->DataType,"F4");	// 4-byte floating point
  strcpy(PHdr->filetype,"TRIG");
  PHdr->gain = 1;
  PHdr->LowLimit = FUNDEF;
  PHdr->HighLimit = FUNDEF;
  PHdr->bitweight = FUNDEF;
  strcpy(PHdr->seismometer,"AS-1");
  PHdr->sensitivity = FUNDEF;
  PHdr->calper = FUNDEF;
  PHdr->evla = FUNDEF;
  PHdr->evlo = FUNDEF;
  PHdr->evdp = FUNDEF;
  PHdr->ev_ms = FUNDEF;
  PHdr->ev_mb = FUNDEF;
  PHdr->ev_ml = FUNDEF;
  PHdr->ev_mo = FUNDEF;
  PHdr->ev_type = (short)NUNDEF;
  PHdr->evdistgc = FUNDEF;
  PHdr->evdistkm = FUNDEF;
  PHdr->az = FUNDEF;
  PHdr->baz = FUNDEF;
  PHdr->cmpname = CUNDEF;
  strncpy(PHdr->kcmpname,KUNDEF,3);
  *PHdr->event_loc = 0;
  PHdr->resolution = 12;
  memset(&PHdr->TimeStamp,0,sizeof(DateAndTime));
  PHdr->TSOffset = 0;
  for(int i=0;i<MAXPICKS;i++) {
  	memset(&PHdr->Picks[i],0,sizeof(TimePick));
  }
  SFlag = 0x00;
  PHdr->npts = PEPPHLen;
  if(PPEPPFile)
  {
    ReadPEPP(PPEPPFile);
  }
}

//- ReadSac : read sac file
//
short PEPP::ReadPEPP(char *PPEPPFile) throw(PEPPException)
{
	ifstream PEPPStrm(PPEPPFile,ios::binary);
  // We have found that sometimes the first 36 bytes of a PEPP file
  // contain information on the source of the file.  Have to check
  // to see if that is the case.
	PEPPStrm.read((unsigned char *) PHdr,36);
  // If the first short is 3737, we're okay
  if(PHdr->lebeByte == (unsigned short)3737) {
		// read rest of PEPP header information
    unsigned char *p = (unsigned char *)PHdr;
    p += 36;
		PEPPStrm.read(p,PEPPHLen-36);
  } else {
		PEPPStrm.read((unsigned char *) PHdr,PEPPHLen);
  	if(PHdr->lebeByte != (unsigned short)3737) {
      PEPPStrm.close();
      //MdiClient->MessageBox("problem with lebe");
    	throw (pe);
    }
  }

	// Read PEPP data
	if(PPEPPData != 0)
   	delete PPEPPData;
  long npts = PHdr->npts;
  if(npts < 0 || npts > 1e6) {
    //MdiClient->MessageBox("Problem with npts");
  	throw(pe);
  }
  PPEPPData = new float[npts];

  PEPPStrm.read((unsigned char *) PPEPPData,npts*sizeof(float));

  // Should read last 4 bytes to see if it has proper end
  long ending;
  PEPPStrm.read((unsigned char *) &ending,sizeof(long));
  if(ending != 95959595) {
    PEPPStrm.close();
    //MdiClient->MessageBox("problem with last word");
  	throw(pe);
  }

  SFlag |= 0x01;
  PEPPStrm.close();
   return 0;
}

//-WriteSac
//
short PEPP::WritePEPP(char* PPEPPFile)
{
   long npts = PHdr->npts;
   if(npts == NUNDEF)
      return kerror(2001,"PEPP::WritePEPP");
   if(PPEPPFile != 0)
   	strcpy(filename,PPEPPFile);
   ofstream PEPPStrm(filename,ios::binary);
   PEPPStrm.write((unsigned char *) PHdr,PEPPHLen);

   // output SAC data values
   PEPPStrm.write((unsigned char *)PPEPPData,npts * sizeof(float));

   // Output datablock end
   long datablockend=95959595;
   PEPPStrm.write((unsigned char *)&datablockend,sizeof(long));
   return 0;
}




// set the data array of sac file -- only dependent variables
//
short PEPP::SetData(float* sdata,long npts)
{

   try{PPEPPData=new float[npts];}
   catch(xalloc){ kerror(1,"PEPP::SetData(float)"); abort();}

   for(long i=0;i<npts;i++)
      PPEPPData[i]=sdata[i];

   PHdr->npts = npts;
   return 0;
}

//-------------- friend member function ----------------------
//
istream& operator>>(istream& is,PEPP& pepp)
{

// read SAC header information
   is.read((unsigned char *) pepp.PHdr,PEPPHLen);
// read SAC data
//
   if(pepp.PPEPPData != 0) delete pepp.PPEPPData;
   pepp.PPEPPData = new float[pepp.PHdr->npts];

   is.read((unsigned char *) pepp.PPEPPData,
             pepp.PHdr->npts*sizeof(float));
   pepp.SFlag |= 0x01;
   return is;
}

//
ostream& operator<<(ostream& os,PEPP& pepp)
{

// write SAC header value
//
   os.write((unsigned char *) pepp.PHdr,PEPPHLen);

// output SAC data values
//
   os.write((unsigned char *)pepp.PPEPPData,
                 pepp.PHdr->npts * sizeof(float));
   // Output datablock end
   long datablockend=95959595;
   os.write((unsigned char *)datablockend,sizeof(long));
   return os;
}

