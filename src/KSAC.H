//  KSAC.H               01/30/94        Yu-Long Kung
//
//      (Note: 1  data are assumed to be stored under SUN mechine )
//             2  So far, only deal with 1-d array
//
//
// 0: History
//      ver 1: 01/30/94 SAC data format
//
//
//
// 1: Initialization:
//     SAC()                        : default for SUN mechine
//     SAC(PC)                      : for PC mechine
//
//
// 2: Functions:
//     void ReadSAC( char *filename)             : read SAC data file
//     short WriteSAC( char *filename)           : write SAC data file
//     void Initiate();                          : initialize new SAC hv
//     void SetSACHV( SACLHV | SACFHV | SACCHV, header value)
//                                               : set SAC header value
//     (long | float | char* ) GetSACHV( SACLHV | SACFHV | SACCHV)
//                                               : get SAC header value
//     void PrintSACHV()                         : print SAC header
//     float * PSACData                          : pointer of data array
//     short * ShortData                         : Pointer to scaled data
//     unsigned char * CharData                  : Pointer to scaled
//                                               : data for sound
//
// 3: Set data array
//     Time array       : IFTYPE  -  TIME SERIES FILE
//                        IEVEN   -  TRUE
//     complex.re (im)  : IFTYPE  -  GENERAL X VS Y FILE
//            .am (ph)    IEVEN   -  TRUE
//
//
// Added by Alan Jones, 2000/04/07:
// SAC has no place for magnitudes and PEPP does so we'll
// take over the USERn for magnitudes:
// SAC   Replacment PEPP      comment
// name  name       name
// USER0 EV_MS      ev_ms     estimated surface wave magnitude for this earthquake
// USER1 EV_MB      ev_mb     estimated body wave magnitude for this earthquake
// USER2 EV_MW      ev_mw     estimated moment magnitude for this earthquake
// USER3 EV_ML      ev_ml     estimated local magnitude for this earthquake
// USER4 EV_MO      ev_mo     any other magnitude estimate (like mbLg)
//
// add to determine whick kind of file
enum FILETYPE
{
  SACFILE,
  PEPPFILE,
  PSNFILE
};

enum SACFHV
{  DELTA,DEPMIN,DEPMAX,SCALE,ODELTA,B,E,O,A,INTERNAL1,
   T0,T1,T2,T3,T4,T5,T6,T7,T8,T9,F,RESP0,RESP1,RESP2,
   RESP3,RESP4,RESP5,RESP6,RESP7,RESP8,RESP9,
   STLA,STLO,STEL,STDP,EVLA,EVLO,EVEL,EVDP,UNUSED1,
   EV_MS,EV_MB,EV_MW,EV_ML,EV_MO,USER5,USER6,USER7,USER8,
   USER9,DIST,AZ,BAZ,GCARC,INTERNAL2,INTERNAL3,
   DEPMEN,CMPAZ,CMPINC,XMINMUM,XMAXMUM,YMINMUM,YMAXMUM,
   UNUSED2,UNUSED3,UNUSED4,UNUSED5,UNUSED6,UNUSED7,UNUSED8
};

enum SACLHV
{  NZYEAR,NZJDAY,NZHOUR,NZMIN,NZSEC,NZMSEC,NVHDR,
   INTERNAL4,INTERNAL5,NPTS,INTERNAL6,INTERNAL7,UNUSED9,UNUSED10,
   UNUSED11,IFTYPE,IDEP,IZTYPE,UNUSED12,IINST,ISTREG,IEVREG,
   IEVTYP,IQUAL,ISYNTH,UNUSED13,UNUSED14,UNUSED15,
   UNUSED16,UNUSED17,UNUSED18,UNUSED19,UNUSED20,UNUSED21,
   UNUSED22,LEVEN,LPSPOL,LOVROK,LCALDA,UNUSED23
};
enum SACCHV
{
   KSTNM,KEVNM,KHOLE,KO,KA,KT0,KT1,KT2,KT3,KT4,KT5,KT6,KT7,KT8,
   KT9,KF,KUSER0,KUSER1,KUSER2,KCMPNM,KNETWK,KDATRD,KINST
};

//
//   ITIME      : Time series file
//   IRLIM      : Spectral file-real and imaginary
//   IAMPH      : Spectral file-amplitude and phase
//   IXY        : General x versus y data
//
enum KIFTYPE
{ ITIME=1, IRLIM, IAMPH, IXY };

//
//   IDUNKN      : Unknown
//   IDISP      : DIsplacement in nm
//   IVEL       : Velocity in nm/sec
//   IVOLTS     : Velocity in volts
//   IACC       : Acceleration in nm/sec/sec
//
enum KIDEP
{ IDUNKN=1, IDISP, IVEL, IVOLTS, IACC};

//
//   IZUNKN     : Unknown
//   IB         : Begin time
//   IDAY       : Midnight of reference GMT day
//   IO         : Event origin time
//   IA         : First arrival time
//   ITn        : User defined time pick n, n=0,9
//
enum KIZTYPE
{ IZUNKN=1, IB, IDAY, IO, IA, IT0, IT1, IT2, IT3, IT4, IT5,
  IT6, IT7, IT8, IT9 };

//
//   IEUNKN      : Unknown
//   INUCL      : Nuclear event
//   IPREN      : Nuclear pre-shot event
//   IPOSTN     : Nuclear post-shot event
//   IQUAKE     : Earthquake
//   IPREQ      : Foreshock
//   ICHEM      : Chemical explosion
//   IEOTHER     : Other
//
enum KIEVTYP
{ IEUNKN=1, INUCL, IPREN, IPOSTN, IQUAKE, IPREQ, IPOSTQ, ICHEM,
  IEOTHER};

//
//   IGOOD      : Good data
//   IGLCH      : Glitches
//   IDROP      : Dropouts
//   ILOWSN     : Low signal to noise ratio
//   IQOTHER     : Other
//
enum KIQUAL
{ IGOOD=1, IGLCH, IDROP, ILOWSN, IQOTHER};

//
//   IRLDTA     : Real data
//
enum KISYNTH
{ KRLDTA=1};

// Added by Alan Jones, 2000-03-09
typedef struct _STATION
{
	float stla;		// Latitude
  float stlo;		// Longitude
  float stel;		// Elevation
  float stdp;		// ???
  float delta;	// Sampling interval
  char name[16];
  char component[8];
} STATION;

struct KSACHD
{
   float        Delta;                  //  1-  4
   float        DepMin;                 //  5-  8
   float        DepMax;                 //  9- 12
   float        Scale;                  // 13- 16
   float        ODelta;                 // 17- 20
   float        Beg;                    // 21- 24
   float        End;                    // 25- 28
   float        Org;                    // 29- 32
   float        Arr;                    // 33- 36
   float        Internal1;              // 37- 40
   float        T00;                    // 41- 44
   float        T01;                    // 45- 48
   float        T02;                    // 49- 52
   float        T03;                    // 53- 56
   float        T04;                    // 57- 60
   float        T05;                    // 61- 64
   float        T06;                    // 65- 68
   float        T07;                    // 69- 72
   float        T08;                    // 73- 76
   float        T09;                    // 77- 80
   float        Fini;                   // 81- 84
   float        Resp0;                  // 85- 88
   float        Resp1;                  // 89- 92
   float        Resp2;                  // 93- 96
   float        Resp3;                  // 97-100
   float        Resp4;                  //101-104
   float        Resp5;                  //105-108
   float        Resp6;                  //109-112
   float        Resp7;                  //113-116
   float        Resp8;                  //117-120
   float        Resp9;                  //121-124
   float        StLa;                   //125-128
   float        StLo;                   //129-132
   float        StEl;                   //133-136
   float        StDp;                   //137-140
   float        EvLa;                   //141-144
   float        EvLo;                   //145-148
   float        EvEl;                   //149-152
   float        EvDp;                   //153-156
   float        Unused1;                //157-160
   float        Ev_ms;                  //161-164
   float        Ev_mb;                  //165-168
   float        Ev_mw;                  //169-172
   float        Ev_ml;                  //173-176
   float        Ev_mo;                  //177-180
   float        User5;                  //181-184
   float        User6;                  //185-188
   float        User7;                  //189-192
   float        User8;                  //193-196
   float        User9;                  //197-200
   float        Dist;                   //201-204
   float        Az;                     //205-208
   float        BAz;                    //209-212
   float        GCArc;                  //213-216
   float        Internal2;              //217-220
   float        Internal3;              //221-224
   float        DepMen;                 //225-228
   float        CmpAz;                  //229-232
   float        CmpInc;                 //233-236
   float        XMinmum;                //237-240
   float        XMaxmum;                //241-244
   float        YMinmum;                //245-248
   float        YMaxmum;                //249-252
   float        Unused2;                //253-256
   float        Unused3;                //257-260
   float        Unused4;                //261-264
   float        Unused5;                //265-268
   float        Unused6;                //269-272
   float        Unused7;                //273-276
   float        Unused8;                //277-280
   long         NZYear;                 //281-284
   long         NZJDay;                 //285-288
   long         NZHour;                 //289-292
   long         NZMin;                  //293-296
   long         NZSec;                  //297-300
   long         NZMSec;                 //301-304
   long         NVHdr;                  //305-308
   long         Internal4;              //309-312
   long         Internal5;              //313-316
   long         Npts;                   //317-320
   long         Internal6;              //321-324
   long         Internal7;              //325-328
   long         Unused9;                //329-332
   long         Unused10;               //333-336
   long         Unused11;               //337-340
   long         IFType;                 //341-344
   long         IDep;                   //345-348
   long         IZType;                 //349-352
   long         Unused12;               //353-356
   long         IInst;                  //357-360
   long         ISTReg;                 //361-364
   long         IEVReg;                 //365-368
   long         IEVTyp;                 //369-372
   long         IQual;                  //373-376
   long         ISynth;                 //377-380
   long         Unused13;               //381-384
   long         Unused14;               //385-388
   long         Unused15;               //389-392
   long         Unused16;               //393-396
   long         Unused17;               //397-400
   long         Unused18;               //401-404
   long         Unused19;               //405-408
   long         Unused20;               //409-412
   long         Unused21;               //413-416
   long         Unused22;               //417-420
   long         LEven;                  //421-424
   long         LPspol;                 //425-428
   long         LOvrok;                 //429-432
   long         LCalda;                 //433-436
   long         Unused23;               //436-440
   char         KStNm[8];               //441-448
   char         KEvNm[16];              //449-464
   char         KHole[8];               //465-472
   char         Ko[8];                  //473-480
   char         Ka[8];                  //481-488
   char         KT00[8];                //489-496
   char         KT01[8];                //497-504
   char         KT02[8];                //505-512
   char         KT03[8];                //513-520
   char         KT04[8];                //521-528
   char         KT05[8];                //529-536
   char         KT06[8];                //537-544
   char         KT07[8];                //545-552
   char         KT08[8];                //553-560
   char         KT09[8];                //561-568
   char         Kf[8];                  //569-576
   char         KUser0[8];              //577-584
   char         KUser1[8];              //585-592
   char         KUser2[8];              //593-600
   char         KCmpNm[8];              //601-608
   char         KNetwk[8];              //609-616
   char         KDatrd[8];              //617-624
   char         KInst[8];               //625-632
};

class PEPP;
class PSN;

// KSACException is an empty class for throwing exceptions
// An instance of this class (ke) is put in KSAC.cpp and is
// thrown when there is a problem
class KSACException {
};

//
// = SAC header information
//
class KSAC
{
protected:
   KSACHD*      PSACHD;                  // SAC Header
                                        //  =   SUN  version (default)
                                        //  =   PC   version
//    SFlag value:
//         bit
//          0 :0x01  with data
//          1 :0x02
//          2 :0x04
//          3 :0x08
//          4 :0x10
//          5 :0x20
//          6 :0x40
//          7 :0x80
   char         SFlag;
   char filename[128];
   FILETYPE filetype;

public:
  // constructors
  // Constructor given a file name
  KSAC(char *PSACFile)
       { PSACHD = new KSACHD; Initiate(PSACFile); }
  // Default constructor
  KSAC() { PSACHD = new KSACHD; Initiate(); }

  // Constructor using a PEPP file as input
  KSAC(PEPP *pepp);

  // Constructor using a PSN file as input
  KSAC(PSN *psn);

  // Constructor to build an instance of SAC in memory
  KSAC(long npts, float *data,STATION station, SYSTEMTIME st);

  // Destructor
  ~KSAC();

  void Initiate(char *PSACFile = NULL,short iflag=0);

  char *GetFileName(){return filename;}
  void PutFileName(char *fn){strcpy(filename,fn);}
  void GetMinMax();			// Compute dependent variable min, max, mean
  void ClearPickTimes();
  void PutTn(int i,float time);					// Stuff first pick time
  float GetTn(int i);										// Get ith pick time
  void Restore();												// Restore original data
  																			// from PSACDataOriginal
  void ResetOriginal();									// Store new set of PSACDataOriginal
  // Following not part of SAC but are here as utilities
  SYSTEMTIME SACTimeToSystemTime();
	LONGLONG PickTimeToLLTime(int i);
	double PickTimeToDouble(int i);
  void SACTimeToLongDouble();
  void SystemTimeToSACTime(SYSTEMTIME st);
  void AdjustZTime(float t);
  SYSTEMTIME PickTimeToSystemTime(int i);
  long double zTime;		// Z-time as a floating point number
 	float sec_offset;			// Number of seconds offset from starting point
  int y0;								// vertical base
  float scale;					// scale factor
  float y_offset;				// y-offset

  // Following variables are for internal use
  // We add these since these are unique for each SAC file and
  // therefore we only have to carry around one object for each
  // SAC file
  //LATLON sta;           // Station latitude/longitude
  //LATLON evt;           // Event latitude/longitude
  //char sta_name[32];    // Station long name as a C string
  //char sta_code[8];     // Station code as a C string
  //char comp_name[8];    // Component name as a C string
  //char sta_cmp_name[12];// Station and component name, e.g. BINY-Z
  //char sac_filename[12];// HRV.LHZ
  //short month;          // Computed from Julian day
  //short day;            // Computed from Julian day
  //float record_start;   // Start time measured from time of event
  //float record_end;     // End time measured from time of event
  //short p_time,         // Time for various phases to reach this station
 	//s_time,
 	//pp_time,
  //ss_time;
  //short base_line;      // Baseline for this component
  //POINT_S cp;           // Current position of pen
  //bool pen_down;        // Pen up/down

// SAC waveform data:
  float* PSACData;

// SAC original data before filtering
// This is so that filtering can be un-done
	float *PSACDataOriginal;

// File type
	FILETYPE GetFileType() {return filetype;}

// file I/O
   short ReadSAC(char*) throw(KSACException);
   short WriteSAC(char* filename = 0);

// for PC version to do bytes swab
//
   void SACHVSwab();
   void SACDataSwab();

// set value
   void SetSACHV(SACLHV,KIFTYPE);
   void SetSACHV(SACLHV,KIDEP);
   void SetSACHV(SACLHV,KIZTYPE);
   void SetSACHV(SACLHV,KIEVTYP);
   void SetSACHV(SACLHV,KIQUAL);
   void SetSACHV(SACLHV,KISYNTH);

   void SetSACHV(SACFHV,float);
   void SetSACHV(SACLHV,long );
   void SetSACHV(SACCHV,char*);

   short SetData(float*,long);          // set SAC data array(real)
                                        //    data pointer, and npts

// get value
   float GetSACHV(SACFHV);              // get SAC header value
   int GetSACHV(SACLHV);
   char* GetSACHV(SACCHV);
   char GetSFlag(){ return SFlag; }
   float* GetData(){ return PSACData; }

// friend member function
   friend ostream& operator<<(ostream& ,KSAC& );
   friend istream& operator>>(istream& ,KSAC& );
   void PrintSACHV();

  // overloaded operators necessary for use with a sorted container
  // We will sort the records in order of great-circle arc distance
  int operator<(const KSAC& sac) const {
    return PSACHD->GCArc < sac.PSACHD->GCArc ? 1 : 0;
  }
  int operator==(const KSAC& sac) const {
    return PSACHD->GCArc == sac.PSACHD->GCArc ? 1 : 0;
  }
};
//typedef TISArrayAsVector<KSAC> SACarray;

//extern SACarray sacarray;

