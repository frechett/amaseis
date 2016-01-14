#include "as1.h"

static char *getline(char *buf,int n,FILE *fp) {
	char *p = fgets(buf,n,fp);
  if(p == 0)
  	return p;
  int i = strlen(p);
  if(i>0) {
  	p[i-1] = 0;
  }
  return p;
}
// Class profile Constructor
Profile::Profile() {
	read();
}
Profile::~Profile() {
}
void Profile::read()
{
	// Set defaults
  char inbuf[100];
  char temp[64];
  device = AS1;
  latitude = 0;
  samprate = 6;
  longitude = 0;
  elevation = 0;
  zero_level = 2048;
  baud = 2400;
  comport = 1;
  gain = 20;
  lines_per_hour = 1;
  lines_per_screen = 24;
  lp_cutoff = 1;
  bp_low_cutoff = 0;
  bp_high_cutoff = 2;
  Pp = 2;    		// Pendulum period
  Qp = 0.707;		// Pendulum Q
  Pf = 20;			// Long period
  LowPassPeriod = 1;	// Low pass cutoff period (s)
  HighPassPeriod = 1;	// High pass cutoff period (s)
  LowPassSec = true; 	// Use seconds,not Hz
  glitch_removal = 0;
  decimate = 1;
  init_samp_sec = 5;
  cal_mass = 0.0318;
  cal_distance = 10;
  center_of_gravity = 28.8;
  mass_of_arm = 465.5;
  strcpy(station_name,"Station Name");
  strcpy(network_id,"EN");
  component = 'Z';
  strcpy(extension,"LPZ");
  strcpy(calibration_file,"Calibration.txt");
  strcpy(station_code,"SUNYB");
  strcpy(seismic_waves,"c:\\sw");
  helicorder_filter = Profile::NO_FILTER;
  EnableLowPass = false;
  EnableHighPass = false;
  HighPassPeriod = 20;
  LowPassPeriod = 1;
  EventHighPassPeriod = 20;
  EventLowPassPeriod = 1;
  EnableHighPass = false;
  *log_file = 0;
  FILE *fp = fopen("AS1.INI","rt");
  per = 2;
  raw_amp = 100;
  inst_res = 100;
  ms_per = 20;
  ms_amp = 100;
  mbLg_amp = 100;
  mbLg_per = 1;
  gca = 50;
  depth = 33;
  lines_per_hour = 1;
  tcp_port = 0;
  *tcp_host_addr = 0;
  *ftp_addr = 0;
  *ftp_user = 0;
  *ftp_pw = 0;
  *ftp_upload_path = 0;
  *ftp_download_path = 0;
  tcp_client = false;
  tcp_server = false;
  ftp_upload = false;
  ftp_download = false;
  *data_directory = 0;
  ftp_days_to_save = 3;
  if(fp) {
    // First read the lines before our [AS1] section
    size_mru = 0;
    while(fgets(inbuf,99,fp)) {
    	if(strstr(inbuf,"[A"))
        break;
    }
  	while(getline(inbuf,99,fp)) {
      if(strlen(inbuf) == 0)
      	continue;
  		if(sscanf(inbuf,"Latitude=%lg",&latitude)) ;
    	else if(sscanf(inbuf,"Longitude=%lg",&longitude));
    	else if(sscanf(inbuf,"Elevation=%g",&elevation));
    	else if(sscanf(inbuf,"BAUD=%d",&baud));
    	else if(sscanf(inbuf,"COM=%d",&comport));
      else if(sscanf(inbuf,"Gain=%g",&gain));
      else if(sscanf(inbuf,"Component=%c",&component));
      else if(sscanf(inbuf,"StationName=%s",temp)) {
        strcpy(station_name,inbuf+12);
      }
      else if(sscanf(inbuf,"StationCode=%s",station_code));
      else if(sscanf(inbuf,"NetworkID=%s",network_id));
      else if(sscanf(inbuf,"FileExtension=%s",extension));
      else if(sscanf(inbuf,"LinesPerHour=%d",&lines_per_hour));
      else if(sscanf(inbuf,"LinesPerScreen=%d",&lines_per_screen));
      else if(sscanf(inbuf,"DecimateFactor=%d",&decimate));
      else if(sscanf(inbuf,"DaysToRetainRecords=%d",&DaysToRetainRecords));
      else if(sscanf(inbuf,"LowPassCutoff=%g",&lp_cutoff));
      else if(sscanf(inbuf,"ZeroLevel=%d",&zero_level));
      else if(sscanf(inbuf,"InitSampleSeconds=%d",&init_samp_sec));
      else if(sscanf(inbuf,"SampleRate=%g",&samprate));
      else if(sscanf(inbuf,"BandPassLowCutoff=%g",&bp_low_cutoff));
      else if(sscanf(inbuf,"BandPassHighCutoff=%g",&bp_high_cutoff));
      else if(sscanf(inbuf,"LPNaturalFrequency=%g",&Pp));
      else if(sscanf(inbuf,"LPQ=%g",&Qp));
      else if(sscanf(inbuf,"LPPeriod=%g",&Pf));
      else if(sscanf(inbuf,"LowPassPeriod=%g",&LowPassPeriod));
      else if(sscanf(inbuf,"LowPassSec=%d",&LowPassSec));
      else if(sscanf(inbuf,"HighPassPeriod=%g",&HighPassPeriod));
      else if(sscanf(inbuf,"EventHighPassPeriod=%g",&EventHighPassPeriod));
      else if(sscanf(inbuf,"EventLowPassPeriod=%g",&EventLowPassPeriod));
      else if(sscanf(inbuf,"HelicorderFilter=%d",&helicorder_filter));
      else if(sscanf(inbuf,"EnableLowPass=%d",&EnableLowPass));
      else if(sscanf(inbuf,"EnableHighPass=%d",&EnableHighPass));
      else if(sscanf(inbuf,"CalibrationMass=%g",&cal_mass));
      else if(sscanf(inbuf,"CalibrationDistance=%g",&cal_distance));
      else if(sscanf(inbuf,"MassOfArm=%g",&mass_of_arm));
      else if(sscanf(inbuf,"CenterOfGravity=%g",&center_of_gravity));
      else if(sscanf(inbuf,"mbAmplitude=%g",&raw_amp));
      else if(sscanf(inbuf,"MsAmplitude=%g",&ms_amp));
      else if(sscanf(inbuf,"mbLgAmplitude=%g",&mbLg_amp));
      else if(sscanf(inbuf,"mgPeriod=%g",&per));
      else if(sscanf(inbuf,"MsPeriod=%g",&ms_per));
      else if(sscanf(inbuf,"mbLgPeriod=%g",&mbLg_per));
      else if(sscanf(inbuf,"InstrumentResponse=%g",&inst_res));
      else if(sscanf(inbuf,"GreatCircleArc=%g",&gca));
      else if(sscanf(inbuf,"Depth=%g",&depth));
      else if(sscanf(inbuf,"DataDirectory=%s",data_directory));
      else if(sscanf(inbuf,"SeismicWaves=%s",seismic_waves));
      else if(sscanf(inbuf,"GlitchRemoval=%d",&glitch_removal));
      else if(sscanf(inbuf,"TCPPort=%d",&tcp_port));
      else if(sscanf(inbuf,"TCPHostAddress=%s",tcp_host_addr));
      else if(sscanf(inbuf,"TCPClient=%s",temp)) {
      	if(0==stricmp(temp,"YES"))
        	tcp_client = true;
      }
      else if(sscanf(inbuf,"TCPServer=%s",temp)) {
      	if(0==stricmp(temp,"YES"))
        	tcp_server = true;
      }
      else if(sscanf(inbuf,"FTPAddr=%s",ftp_addr));
      else if(sscanf(inbuf,"FTPUser=%s",ftp_user));
      else if(sscanf(inbuf,"FTPPW=%s",ftp_pw));
      else if(sscanf(inbuf,"FTPUploadPath=%s",ftp_upload_path));
      else if(sscanf(inbuf,"FTPDownloadPath=%s",ftp_download_path));
      else if(sscanf(inbuf,"FTPDaysToSave=%d",&ftp_days_to_save));
      else if(sscanf(inbuf,"FTPUpload=%s",temp)) {
      	if(0==stricmp(temp,"YES"))
        	ftp_upload = true;
      }
      else if(sscanf(inbuf,"FTPDownload=%s",temp)) {
      	if(0==stricmp(temp,"YES"))
        	ftp_download = true;
      }
      else if(sscanf(inbuf,"LogFile=%s",log_file));
      else if(sscanf(inbuf,"CalibrationFile=%s",calibration_file));
      else if(sscanf(inbuf,"Device=%s",temp)) {
      	if(0 == stricmp(temp,"DATAQ151"))
        	device = DATAQ151;
        else if(0 == stricmp(temp,"DATAQ145"))
      		device = DATAQ145;
        else if(0 == stricmp(temp,"DATAQ154"))
      		device = DATAQ154;
        else if(0 == stricmp(temp,"DATAQ158"))
      		device = DATAQ158;
        else if(0 == stricmp(temp,"DATAQ194"))
      		device = DATAQ194;
        else if(0 == strncmpi(temp,"DATAQ",5))
        	device = DATAQ194;
       	else if(0 == stricmp(temp,"INFILTECQM1")) {
        	device = INFILTECQM1;
          samprate = 16;
          zero_level = 0;
        }
       	else if(0 == stricmp(temp,"SEPUK1")) {
        	device = SEPUK1;
          samprate = 20;
          zero_level = 0;
        }
       	else if(0 == stricmp(temp,"EQ1")) {
        	device = EQ1;
          samprate = 10;
          zero_level = 0;
        }
        else if(0 == stricmp(temp,"AS1")) {
        	device = AS1;
          //samprate = 6;
        }
        else {
        	device = AS1;
          //samprate = 6;
				}
    	}
  	}
    switch(device) {
    case DATAQ145:
    case DATAQ151:
    case DATAQ154:
    case DATAQ194:
    	baud = 4800;
      break;
    case AS1:
    	// baud = 2400;
      // 2007.02.23: Let AS-1 run at any baud
      break;
    case INFILTECQM1:
    	baud = 9600;
      break;
    case EQ1:
    	baud = 38400;
      break;
    case SEPUK1:
    	baud = 9600;
      break;
    }
    fclose(fp);
    // Add \ to data directory
    int len = strlen(data_directory);
    if(len) {
      if(data_directory[len-1] != '\\')
    		strcat(data_directory,"\\");
    }
  }
}

extern TAS1 *papp;
// 2006.10.18
// Replacement for MS HtmlHelp which has a problem when htmlhelp.lib is called.
void myHtmlHelp(int id)
{
	char sz1[512], sz2[512], path_to_help[512];
 	strcpy(sz1,getenv("WINDIR"));
 	strcat(sz1, "\\hh.exe");
  GetCurrentDirectory(512,path_to_help);
  strcat(path_to_help,"\\AS1.CHM");
 	//char *path_to_help = make_home_fs("AS1.CHM");
 	if(id) {
 		sprintf(sz2,"-mapid %d %s",id,path_to_help);
	}
	else {
 		sprintf(sz2,"%s",path_to_help);
  }
 	if((int)ShellExecute(NULL, "open", sz1, sz2, NULL, SW_SHOWNORMAL) <= 32)
 	{
  	sprintf(sz1,"Problem opening %s or MS hh.exe",path_to_help);
 	}
}

