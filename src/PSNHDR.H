// psn header info

#define	TRUE			1
#define	FALSE			0
#define BYTE			unsigned char
#define WORD			unsigned short
#define GOODTIME		0x55
	
typedef struct {
	short comment[60];
} oldFmt;

typedef struct  {
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
