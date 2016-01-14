/*  #include "dt.h"---- now inline at the bottom of this file.   */
/*
===============================================================================
Following taken from PEPP Web site:
http://lasker.princeton.edu/Programs/peppDFF.html
===============================================================================
A PEPP seismogram data file consists of a Header Block, a Data Block, and an End Block.

Header Block
============

NAME           TYPE            EXPLANATION
----------   ----------------  -----------
lebeByte      unsigned short   value=LEBE to indicate if file is in native PC
                               byte-order (lebeByte = 3737 = 0x0e99)
verno         char[15]         header version identifier (="PEPP_HVN_02.40") for
                               current version 2.40
stime         DateAndTime      absolute reference time for the file's data (in UT)
begin_time    float            offset (in seconds) between stime and time of first
                               data point [StartTime=stime+begin_time]
end_time      float            offset (in seconds) between stime and time of the last
                               data point
samplingrate  float            data's sampling rate, in samples per second (sps)
npts          long             number of data (seismogram) points in the data block
idep          short            value indicating units of seismogram [see below]
depmin        float            minimum value of the seismogram's amplitude
depmax        float            maximum value of the seismogram's amplitude
depmen        float            mean value of the seismogram's amplitude
DataType      char[3]          string specifying data block's format (F4=float)
cmpinc        float            seismogram's component incidence angle (in degrees) with
                               respect to the vertical); a vertical component
                               seismogram has cmpinc=0, and a horizontal component
                               seismogram has cmpinc=90
cmpaz         float            seismogram's component azimuth, measured from north
                               through east; a north-component seismogram has 
                               cmpaz=0; an east-component seismogram has cmpaz=90.
cmpname       char             name for this seismogram component orientation 
                               (Z, N, or E)
kcmpname      char[4]          channel identifier, which is a combination of the
                               band code (L,M,B), sensor code (usually H), and the
                               component name (cmpname, like Z, N, or E)
filetype      char[5]          filetype[0] = 'T' if file came from the triggered stream of
                               the DAS, or 'C' if it came from the continuous stream;
                               filetype[1] = 'Q' if file came from QUAKES program, or 'E' if
                               it came from EQDAP program; filetype[2-3] is a string-version
                               of the program version number (so, for EQDAP version 3.2,
                               filetype[1-3] would be "E32").
stla          float            station's latitude, in decimal degrees, with 
                               north positive, south negative
stlo          float            station's longitude, in decimal degrees, with east
                               positive, west negative
stel          short            station's elevation above sea level, in meters
kstnm         char[9]          station's 5-letter designation (e.g., PPHGT)
snetwrk       char[9]          station's network affiliation (PEPPSN for PEPP stations);
                               we arbitrarily limit this string to 6 characters, the 7th
                               character is a NULL, and the 8th character, snetwrk[7], 
                               denotes the type of timing: 'N' for NONE, 'G' for GPS, 
                               'W' for WWVB, and 'A' for NIST-ACTS. snetwrk[8] is a NULL.
station_loc   char[31]         station's geographic location (town, state, country)
gain          short            gain factor used in the ADC of the PC-DAS
LowLimit      float            minimum allowed ADC input voltage (units are Volts, V)
HighLimit     float            maximum allowed ADC input voltage (units are Volts, V)
bitweight     float            unity-gain value to convert counts to millivolts for
                               the ADC (units are millivolts/count)
resolution    short            resolution (in bits) of the ADC digitizer
seismometer   char[17]         station's seismometer ID (model and serial number)
sensitivity   float            velocity sensor's sensitivity, in V/(m/sec)
calper        float            period (in secs) at which sensitivity applies
evot          DateAndTime      absolute origin time and date of the earthquake that
                               produced this seismogram (in UT)
evla          float            earthquake's latitude, in decimal degrees, north
                               positive (i.e., latitude of epicenter)
evlo          float            earthquake's longitude, in decimal degrees, east
                               positive (i.e., longitude of epicenter)
evdp          float            earthquake's source depth (in km)
ev_ms         float            estimated surface wave magnitude for this earthquake  
ev_mb         float            estimated body wave magnitude for this earthquake
ev_mw         float            estimated moment magnitude for this earthquake
ev_ml         float            estimated local magnitude for this earthquake
ev_mo         float            any other magnitude estimate (like mbLg)
ev_type       short            enumerated event type (see below)
evdistgc      float            event-to-station distance, in degrees, along great circle
evdistkm      float            event-to-station distance, in km, along great circle
az            float            event-to-station azimuth, in degrees (N=0, E=90)
baz           float            station-to-event azimuth (back-azimuth), in degrees (N=0, E=90)
event_loc     char[31]         geographic location for this earthquake
TimeStamp     DateAndTime      absolute time (UT) of some data point in the seismogram; this is
                               not actually used. WE do use the short integer TimeStamp.year
                               to denote the timing quality of the data: 'Q' for QUESTIONABLE,
                               'G' for GOOD.
TSOffset      float            offset (in seconds) between the first data point and
                               the data for which the TimeStamp applies
Clock         ClockCorr[2]     2-element array of ClockCorr structures, used to record
                               clock correction values and times pertinent for this
                               data file
Picks         TimePick[M]      M-element array of TimePick structures for holding the
                               phase names, arrival times, and amplitudes of various
                               seismic phases in this seismogram [M=10 in the current
                               header specification].

Data Block
============
This block follows immediately after the Header Block, and contains all the points
of the seismogram: a total of npts points. Each sample is a 4-byte floating point
number, stored on disk in the byte-order native to PC's.

End Block
============
This block follows immediately after the Data Block, and consists of one 4-byte
integer (long int in C parlence), again stored on disk in PC-native byte order.
This block is used by the data retrieval system (SYCLOPS) to ensure that the
complete file has been uploaded to the server. This parameter has the value
DATABLOCKEND, which is stored in the header file keybytes.h. By definition,
DATABLOCKEND=95959595. As an example of its use, if the data retrieval system's
data checking program opens this data file and does NOT find that the last 4
bytes of the file (when interpreted as a 4-byte integer) has the value DATABLOCKEND,
it assumes that the file is either incomplete or corrupted.

===============================================
Explanations of the TYPE column entries below:
===============================================
unsigned short ==> an unsigned, 2-byte integer
short          ==> a signed, 2-byte integer
long           ==> a signed 4-byte integer
float          ==> a 4-byte floating point number
char           ==> a single character value (a character is a byte)
char[n]        ==> an n-element character array (i.e., a string)
DateAndTime    ==> a structure to hold a date and time (see below)
ClockCorr      ==> a structure to hold clock correction information, specifically,
                   the time of the correction and the magnitude of the correction
                   (see below)
TimePick       ==> a structure to hold the names, arrival times, and amplitudes,
                   of important seismic wave arrivals (like P, S, PP, etc.)
                   (see below)

==========================
structure DateAndTime:
NAME     TYPE    EXPLANANTION
----     ----    ------------
year     short   the year, as in 1995, or 2012, etc.
month    short   the numeric month, where JAN=1, FEB=2, ..., DEC=12
day      short   the day of the month, from 1 to 31
jday     short   the day of the year, from 1 to 366
hour     short   the hour of the day, between 0 and 23 (on the 24-hour clock)
min      short   the minute of the hour, between 0 and 59
sec      float   the seconds of the minute

==========================
structure ClockCorr:
NAME          TYPE        EXPLANANTION
----          ----        ------------
Time        DateAndTime   Absolute time, in UT, that the PC-DAS clock
                          was checked against the standard (either NIST,
                          GPS, or other)
correction  float         the magnitude of the correction (in seconds)
                          (what is the sign convention?)

==========================
structure TimePick:
NAME          TYPE        EXPLANANTION
----          ----        ------------
Time        DateAndTime   Absolute arrival time, in UT, of the phase in
                          question
amplitude   float         maximum (in absolute value) amplitude of the phase,
                          in units given by the structure member units
units       short         units the amplitude is in, values taken are same as
                          those allowed for idep and specified in idep.h
id          char[8]       seismological name for the phase (e.g., P, pP, PKP, etc.)

================================================================================

STRUCTURE STORAGE AND OUTPUT IN THE PC-DAS PROGRAM
--------------------------------------------------
  Because of requirements imposed by the original ADC driver software, the headers
are written to disk in the PC-DAS on byte boundaries, so there is no byte padding
between header elements. In the PC-DAS program, the header is a structure, written
to disk in binary with a single fwrite() statement. The data are also written out
in binary to disk, followed by the End Block. The header (including the lebeByte)
is 641 bytes long.
  This is why we have decided to stick with PC-native byte ordering as the formal
ordering scheme for the PEPP seismogram files. Hence, ALL multibyte data elements
should always be written to (and stored on) disk in the PC-native byte ordering
scheme. When these files are read into a Mac or other machine with a different
byte-ordering scheme, BYTES MUST BE SWAPPED, or the data will look like garbage
to these machines. Similarly, when data are written to disk from these machines,
programs must SWAP BYTES BACK to the native PC format.
  The purpose of the header structure member lebeByte is to check that the data
file is in PC-native byte order. If a PC reads these first two bytes, does not
swap them, and sees that lebeByte=LEBE, then it knows the file is in native PC
byte order. If a Mac or Sun reads these bytes, swaps them, and sees LEBE, then
it will know to swap ALL subsequent multi-byte data elements. Hence, the lebeByte
parameter was included as a way to check data byte-ordering across different
machines.

===============================================================================

Notes:

[1] idep: values for idep are defined in the header file idep.h. These are
used to specify the units of the dependent variable in the seismogram, and
also to specify the units of the phase pick amplitudes.

VALUE   SYMBOLIC CONSTANT        UNITS
------  -----------------    -------------
001          PIUNKN           Unknown units (could also be IUNDEF)
002          PICNTS           Raw Counts (not corrected for recording gain)
003          PIVOLTS          Volts (V)
004          PIDISPL          Displacement (nm)
005          PIVELOC          Velocity (nm/sec)
006          PIACCEL          Acceleration (nm/sec/sec)

[2] ev_type: values for ev_type are used to identify what type of event
(source) a seismogram might have come from, such as an earthquake, a quarry
blast, or a nuclear explosion.

VALUE   SYMBOLIC CONSTANT        UNITS
------  -----------------    -------------
103          PINUKE           Nuclear explosion
104          PIQUAKE          Earthquake
105          PIQUARRY         Quarry blast
106          PICHEM           Large chemical explosion
107          PIOTHER          Other source (perhaps unknown)

[3] undefined values: some of the header fields are initialized to undefined
values, usually because the true value for that field is unknown at the time
the file is created or analyzed. CUNDEF is used for undefined single-character
fields or fields less than or equal to 6 characters in length. KUNDEF is used to
set character fields longer than 6 characters long to undefined (the remainder of
the field is set to NULLS).

VALUE   SYMBOLIC CONSTANT        UNITS
------  -----------------    -------------
-12345.0    FUNDEF           undefined floating-point number
-12345      NUNDEF           undefined signed integer (long or short)
-12345      IUNDEF           undefined enumerated constant fields (like ev_type)
0           LUNDEF           undefined logical value (true or false)
"-12345"    KUNDEF           undefined char[] field longer than 6 characters
'?'         CUNDEF           undefined char field


*/
#ifndef HEADER_24__
#define HEADER_24__

/**
 all of these are the same structures used in the pc programs
 quakes and eqdap. Ints have to be qualified as short ints on the
 suns, and bytes have to be swapped.
 **/

/***************************************************************/
/** Structure to hold an absolute date and time. This is the same
structure as is used in the das programs quakes and eqdap. **/
/****************************************************************/
typedef struct DateTime {
                  short int year;
                  short int month;
                  short int day;
                  short int jday;
                  short int hour;
                  short int min;
                  float sec;
                } DateAndTime;

/*************************************************/
/* structure to hold clock corrections: stores the time of the
   correction, and the correction itself          */
/*************************************************/
typedef struct ClockCorrection {
                  DateAndTime Time;
                  float correction;
                } ClockCorr;

/*************************************************/
/* structure to hold information about the arrival time, amplitude,
   and name of a seismic phase picked by users */
/*************************************************/
typedef struct ArrivalTimePick {
                  DateAndTime Time;
                  float amplitude;
                  short int units;
                  char id[8];
                } TimePick;


/*************************************************/
/*   structure for the file headers, version 2.4 */
/*   and a type declaration Header               */
/*************************************************/
#define MAXPICKS 10
#define PHDR_VER_24 "PEPP_HVN_02.40"
#define HEADER_SIZE 641L

typedef struct FileHeader24 {
        unsigned short lebeByte; /* file byte-ordering verification */
        char verno[15];       /* header version id to check */
        DateAndTime stime;    /* reference time for seismogram */
        float begin_time;     /* offset of first data point from stime (secs) */
        float end_time;       /* offset of last data point from stime (secs) */
        float samplingrate;   /* sampling rate in samples per second */
        long int npts;        /* number of data points in seismogram */
        short int idep;       /* type of dependent variable (i.e., units) */
        float depmin;         /* minimum (signed) value of dependent variable */
        float depmax;         /* maximum (signed) value of dependent variable */
        float depmen;         /* mean value of dependent variable */
        char DataType[3];     /* I2,I4,F4 (always F4 now, for 4-byte floats) */
        float cmpinc;         /* component incidence angle (degrees,0=UP) */
        float cmpaz;          /* component azimuth (degrees,0=NORTH,90=EAST) */
        char cmpname;         /* Z=vertical, N=north, E=east */
        char kcmpname[4];     /* component name, or channel id (LHZ,BHN,etc.) */
        char filetype[5];     /* CONT,TRIG,and possibly others */
        float stla;           /* station latitude (degrees, north positive) */
        float stlo;           /* station longitude (degrees, east positive) */
        short int stel;       /* station elevation (meters above sea level) */
        char kstnm[9];        /* station name */
        char snetwrk[9];      /* station's seismic network affiliation */
        char station_loc[31]; /* station location (town, city, etc.) */
        short int gain;       /* gain factor from the ADC (1,2,4,etc.) */
        float LowLimit;       /* lower limit on ADC input voltage (V) */
        float HighLimit;      /* upper limit on ADC input voltage (V) */
        float bitweight;      /* millivolts/count for ADC (mV/count) */
        short int resolution; /* number of bits resolution in ADC */
        char seismometer[17]; /* seismometer id (model and serial number */
        float sensitivity;    /* VOLTS/(Meter/Sec) for flat velocity sensors */
        float calper;         /* period (sec) at which sensitivity applies */
        DateAndTime evot;     /* event origin time (UT), if known */
        float evla;           /* event latitude, (degrees, north positive) */
        float evlo;           /* event longitude, (degrees, east positive) */
        float evdp;           /* event depth (km) below sea level */
        float ev_ms;          /* event surface wave magnitude */
        float ev_mb;          /* event body wave magnitude */
        float ev_mw;          /* evenrt moment magnitude */
        float ev_ml;          /* event local magnitude, or maybe mbLg */
        float ev_mo;          /* other magnitude, or maybe moment in dyne-cm */
        short int ev_type;    /* enumerated event type id */
        float evdistgc;       /* distance in degrees, along great circle */
        float evdistkm;       /* distance in km, along great circle */
        float az;             /* event-to-station azimuth (degrees,N=0,E=90) */
        float baz;            /* station-to-event azimuth (degrees,N=0,E=90) */
        char event_loc[31];   /* event location and/or name */
        DateAndTime TimeStamp;  /* absolute time of some data point in file */
        float TSOffset;      /* secs from TimeStamp point from 1st data point */
        ClockCorr Clock[2];    /* clock correction information */
        TimePick Picks[MAXPICKS];     /* user phase pick information */
  } Header24;

class KSAC;
class PEPPException {
};

class PEPP
{
	protected:
    char     SFlag;

	public:
  	// constructors
  	// Constructor given a file name
  	PEPP(char *PPEPPFile)
       { PHdr = new Header24; Initiate(PPEPPFile);}
  	// Default constructor
  	PEPP() { PHdr = new Header24; Initiate(); }

  	// Constructor to build an instance of PEPP in memory
  	//PEPP(long npts, float *data,STATION station, SYSTEMTIME st);

    // Constructor to build PEPP from KSAC
    PEPP(KSAC *ksac);

  	// Destructor
  	~PEPP();

  	void Initiate(char *PPEPPFile = NULL,short iflag=0);
  	Header24 *PHdr;

  	char *GetFileName(){return filename;}
  	void PutFileName(char *fn){strcpy(filename,fn);}

    // File name
   	char filename[64];

		// PEPP waveform data:
  	float* PPEPPData;

		// file I/O
   	short ReadPEPP(char*) throw(PEPPException);
   	short WritePEPP(char *fn = 0);

		// for PC version to do bytes swab
		//
   	// void PEPPSwab();
   	// void PEPPDataSwab();

		// set value
   	short SetData(float*,long);          	// set SAC data array(real)
                                        	//    data pointer, and npts
   	float* GetData(){ return PPEPPData; }

		// friend member function
   	friend ostream& operator<<(ostream& ,PEPP& );
   	friend istream& operator>>(istream& ,PEPP& );

};


#endif   /*  HEADER_24__ */
