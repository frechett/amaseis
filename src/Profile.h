//----------------------------------------------------------------------------
//  Project AS1
//  RunTime Software
//  Copyright © 2000. All Rights Reserved.
//
//  SUBSYSTEM:    AS1 Application
//  FILE:         as1mdiclient.h
//  AUTHOR:       Alan Jones
//
//  OVERVIEW
//  ~~~~~~~~
//  Class definition for Profile
//
//----------------------------------------------------------------------------

#if !defined(profile_h)              // Sentry, use file only if it's not already included.
#define profile_h

class Profile {
	public:
  	Profile();		// Constructor
    ~Profile();
		enum Device {
			AS1 = 1,
      DATAQ145,
  		DATAQ151,
  		DATAQ154,
      DATAQ155,
      DATAQ158,
  		DATAQ194,
      EQ1,
      INFILTECQM1,
      RHS3,
      RHV1,
      SEPUK1,
		};
    enum FilterType {
    	NO_FILTER,
      LOW_PASS,
      HIGH_PASS,
      BAND_PASS,
      LONG_PERIOD
    };
    Device device;			// Which device?
    int size_mru;				// Number of lines in MRU section
  	double latitude;		// Station latitude
  	double longitude;		// Station longitude
  	float elevation;		// Station elevation
    float samprate;			// Sample rate
    int init_samp_sec;	// How long to sample at the beginning to get sps
  	int baud;						// Interface baud.  Default: 2400
  	int comport;				// COM port
    int decimate;				// Decimate factor
    int lines_per_hour;	// For output display.  If lines_per_hour = 1,
    										// then it takes an hour to cross the screen.  If = 2
                        // it takes 30 minutes, etc.
    int DaysToRetainRecords;
    int lines_per_screen;
    int save_screen_minutes;
    char save_screen_path[255];
    FilterType type;
    FilterType event_filter_type;
    bool helicorder_filter;
    int zero_level;			//  for AS-1
    float gain;
    char component;	// Z, E, or N
    char extension[8];	// File extension
    char station_code[8];
    char station_name[128];
    char network_id[8];
    char seismic_waves[48];
    char log_file[48];
    char calibration_file[48];
    char data_directory[128];
    float lp_cutoff;		// Cutoff frequency for low pass
    float bp_low_cutoff; 	// For band pass filter used for events
    float bp_high_cutoff;
    float Pp, Qp, Pf;			// Parameters for Bob McClure's filter
    float LowPassPeriod;	// Helicorder Cutoff period for Bob McClure's low pass filter
    float HighPassPeriod;	// Helicorder Cutoff period for Bob McClure's high pass filter
    float EventLowPassPeriod;
    float EventHighPassPeriod;
    bool  LowPassSec;			// Use seconds, not Hz on both highpass and lowpass filter dialog box
    bool  EnableLowPass;
    bool  EnableHighPass;
    bool x_period;
    bool passive;	// Run in passive mode. Never write files. Just read.
    int glitch_removal;
    float cal_mass;
    float cal_distance;
    float center_of_gravity;
    float mass_of_arm;			// Mass of arm with magnet
    float raw_amp;					// Data for magnitude
    float ms_amp;						// Amplitude for Ms
    float mbLg_amp;					// mbLg amplitude
    float inst_res;					// Instrument response
    float per;							// Period (s)
    float ms_per;						// Period for Ms
    float mbLg_per;		 			// Period for lgMb
    float gca;							// Great circle arc
    float depth;						// Focal depth
    int tcp_port;						// Only specified if this instance is a server
    char tcp_host_addr[64];			// Only specified if this instance is a client
    bool tcp_server;
    bool tcp_client;
    float  tcp_refresh_seconds;
    bool ftp_upload;
    bool ftp_download;
    char ftp_addr[64];
    char ftp_user[64];
    char ftp_pw[64];
    char ftp_upload_path[64];
    char ftp_download_path[64];
    int ftp_days_to_save;
    float beam_off_centre_value;
    void read();						// Read AS1.INI
    void write();						// Write AS1.INI
};

// Replacement for Windows WinHelp.
// Needed it for Vista.
void myHtmlHelp(int id);

#endif
