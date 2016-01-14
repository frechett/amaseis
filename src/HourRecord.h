/*******************
Hour Record class
*******************/
#if !defined(HourRecord_h)              // Sentry, use file only if it's not already included.
#define HourRecord_h

class FtpThread;
extern float samprate;
extern int samples_per_hour;
extern int block_size;		// Number of DATAQ samples in a chunk
class HourRecord {
	public:
  	HourRecord();						// Creates an empty hour record starting at this hour
    // In following, set flag if this is the current hour.
    HourRecord(long hour0, bool current = false);	// Reads a record from disk for this hour where hour
                          	// is hours from day 0;
    ~HourRecord();
    void ZeroOut();			// Build a record of zeros
    bool Process(unsigned char *p,int size);	// Process one buffer of data
    bool add(short d);	// Add a datapoint at end
    bool save(bool more = false);				// Save in record year\month\day\hour.ext
    										// If saved at an hour, more == false.  Otherwise
                        // more == true and the file is padded out to an hour.
    										// E.g. 2000\02\22\09.ext
		char *getPath();		// Create path to this hour's file
		char *getRemotePath();		// Create path to this record's file on remote system
    char *createPath();	// Create path for this file
    void SetSPS(float s) {sps = s;}
    float GetSPS() {return sps;}
    void SetXSF(float x_sf) {xsf = x_sf;}
    float GetXSF() {return xsf;}
    int GetSize() {return size;}
    bool WriteZeros(HANDLE handle,int n);	// Write n zero samples into file
    //static void filterCoeff();	// Compute filter coefficients
    void filter();			// Filter this record
    void IIR(int i);		// Time Domain filter
    void UploadOneMinute(int minute);		// FTP one minute of data to host
    // Filter coefficients
    static float c0, c1, d1;
    static long st2hour(SYSTEMTIME st);						// Converts a file time to an hour since 1901
    static SYSTEMTIME hour2st(long hour);					// Converts a hour from 1601 to a SYSTEMTIME
    static SYSTEMTIME hourSec2st(long hour, double sec);	// Convert hour, sec to SYSTEMTIME
    short year;					// Year of this record, etc.
    short month;				// Month of this record
    short day;					// Day of this record
    short hour;					// Hour of this record
    int   npts;      		// Number of data points at this time
    int   last_npts;		// Keeps track of the last time we sent data to FTP site
  	short *data;        // Data array
    short *data2;				// Copy of data but without glitches removed
    float *floatdata;		// Data in float format
    int hour0;					// Number of hours in this era (since 1601)
    int last_drawn;			// Last datapoint drawn.
    int last_minute_downloaded;
    int last_minute_checked;
    static FtpThread *ftp_instance;
  private:
  	ButterworthLP *butterworthLP;
    ButterworthHP *butterworthHP;
    void filterCoeff();
    int block_ctr;			// Block counter
    int average;				// Place to hold total while computing average
    static unsigned char tmp_buf[12];		// Hold portion of a record
    float sec0;					// Seconds into the hour when this record began
    //float depmen;				// Mean of dependent variable
    int size;						// Number of points allocated for this record
    bool at_end;				// flag
    float sps;					// Sample rate for this hour
    float xsf;					// x-scale factor for this hour
  public:
	  int operator==(const HourRecord& hourrecord) const {
      return hour==hourrecord.hour;
    }
};



#endif