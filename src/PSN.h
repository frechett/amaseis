// psn header info

#ifndef PSN_HDR
#define PSN_HDR

#define	TRUE			1
#define	FALSE			0
#define BYTE			unsigned char
#define WORD			unsigned short
#define GOODTIME		0x55

typedef struct {
	short comment[60];
} oldFmt;

typedef struct  {                          //Roger added this structure
		short adcType, adcNull, adcMin, adcMax;
		short numConv;
		short magCorrInt, magCorrDec;
		char goodTime, neicHour, neicDay, neicMonth;		// new NEIC extended time info
		BYTE msPeriod, mbPeriod;
		short magType[2];
		short magnitude;
		short depth;
		short quakeLatInt, quakeLatDec;
		short quakeLongInt, quakeLongDec;
		WORD pPickTime, sPickTime;
		BYTE pYLocation, sYLocation;
		WORD pickTableInfo;
		BYTE lockChr, bSpare;
		WORD mdSec;
		short magCorrMult, magCorrNum;
		short mbCorrMult, mbCorrNum;
		short spare1[2];
		short comment[30];
} newFmt;

// Quake file header info for format 2 and 3
typedef struct {
	short format,
		sYear, sMonth, sDay,
		sHour, sMin, sSec, s100th,
		fHour, fMin, fSec, f100th;
    WORD dataCount;
	short atodBase,
		dataMin, dataMax,
		orientation,
		latInt, latDec,
		longInt, longDec,
		neicFlag,
		neicMin, neicSec, neic100th,
		location[15];
		union  {
			oldFmt oldfmt;
			newFmt newfmt;
		} u;
} PsnHeader;


class KSAC;
class PEPP;
class PSNException {
};

class PSN
{
	protected:
    char     SFlag;
    BYTE bsave[8];				// holds pre header information
 		bool goodPSNQuakeFile();
	public:
  	// constructors
  	// Constructor given a file name
  	PSN(char *PPSNFile)
       { PHdr = new PsnHeader; Initiate(PPSNFile);}
  	// Default constructor
  	PSN() { PHdr = new PsnHeader; Initiate(); }

  	// Constructor to build an instance of PSN in memory
  	//PSN(long npts, float *data,STATION station, SYSTEMTIME st);

    // Constructor to build PSN from KSAC
    PSN(KSAC *ksac);

  	// Destructor
  	~PSN();

  	void Initiate(char *PPSNFile = NULL,short iflag=0);
  	PsnHeader *PHdr;
   oldFmt *oldfmt;       //Roger added this definition
   newFmt *newfmt;       //Roger added this definition

  	char *GetFileName(){return filename;}
  	void PutFileName(char *fn){strcpy(filename,fn);}
    static double getLatLong(short num, short dec);
    static void getIntString(char *to, short *from, int cnt);
		static void getIntStringNew(char *to, short *from, int cnt);
		static void fixstr(char *str);
    static void saveIntString(short *to, char *from, int cnt);
		static void saveIntStringNew(short *to, char *from, int cnt);
    static void makeLatLong(short *iptr, short *dptr, const char *str);

    // File name
   	char filename[64];
		double calcSPS();

    // Number of samples
    long npts;

		// PSN waveform data:
  	short* PPSNData;

		// file I/O
   	short ReadPSN(char*) throw(PSNException);
   	short WritePSN(char *fn = 0);

		// for PC version to do bytes swab
		//
   	// void PSNSwab();
   	// void PSNDataSwab();

		// set value
   	short SetData(float*,long);          	// set SAC data array(real)
   	short SetData(short*,long);          	// set SAC data array(short)
   	short* GetData(){ return PPSNData; }

		// friend member function
   	friend ostream& operator<<(ostream& ,PSN& );
   	friend istream& operator>>(istream& ,PSN& );

};


#endif /* HEADER_PSN */
