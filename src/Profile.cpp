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
  write();
  x_period = false;
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
	DaysToRetainRecords = 365;
  save_screen_minutes = 0;
  lp_cutoff = 1;
  bp_low_cutoff = 0;
  bp_high_cutoff = 2;
  Pp = 5.5;    		// Pendulum period
  Qp = 0.5;		// Pendulum Q
  Pf = 16;			// Long period
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
  strcpy(station_code,"unknown");
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
  *save_screen_path = 0;
  tcp_client = false;
  tcp_server = false;
  ftp_upload = false;
  ftp_download = false;
  *data_directory = 0;
  ftp_days_to_save = 3;
  tcp_refresh_seconds = 0.2;
  if(strlen(working_dir)) {
  	// Use working directory only if a TCP Client or FTP client
    // Does this directory exist?
    WIN32_FIND_DATA fd;
    if(FindFirstFile(working_dir,&fd)) {
    	if(!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
      	CreateDirectory(working_dir,0);
      }
    } else
     	CreateDirectory(working_dir,0);
  	sprintf(temp,"%sAS1.INI",working_dir);
  } else
  	sprintf(temp,"AS1.INI");
  // Does the directory exist?

  FILE *fp = fopen(temp,"rt");
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
      else if(sscanf(inbuf,"SaveScreenMinutes=%d",&save_screen_minutes));
      else if(sscanf(inbuf,"SaveScreenPath=%s",save_screen_path));
      else if(sscanf(inbuf,"Component=%c",&component));
      else if(sscanf(inbuf,"StationName=%s",station_name));
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
      else if(sscanf(inbuf,"TCPRefreshSeconds=%g",&tcp_refresh_seconds));
      else if(sscanf(inbuf,"TCPClient=%s",temp)) {
      	if(0==stricmp(temp,"YES"))
        	tcp_client = true;
      }
      else if(sscanf(inbuf,"TCPServer=%s",temp)) {
      	if(0==stricmp(temp,"YES"))
        	tcp_server = true;
      }
      else if(sscanf(inbuf,"Passive=%s",temp)) {
      	if(0==stricmp(temp,"YES"))
        	passive = true;
      }
      else if(sscanf(inbuf,"FTPAddr=%s",ftp_addr));
      else if(sscanf(inbuf,"FTPUser=%s",ftp_user));
      else if(sscanf(inbuf,"FTPPW=%s",ftp_pw));
      else if(sscanf(inbuf,"FTPUploadPath=%s",ftp_upload_path));
      else if(sscanf(inbuf,"FTPDownloadPath=%s",ftp_download_path));
      else if(sscanf(inbuf,"FTPDaysToSave=%d",&ftp_days_to_save));
      //else if(sscanf(inbuf,"FTPUpload=%s",temp)) {
      //	if(0==stricmp(temp,"YES"))
      //  	ftp_upload = true;
      //}
      else if(sscanf(inbuf,"FTPDownload=%s",temp)) {
      	if(0==stricmp(temp,"YES"))
        	ftp_download = true;
      }
      else if(sscanf(inbuf,"LogFile=%s",log_file));
      else if(sscanf(inbuf,"CalibrationFile=%s",calibration_file));
      else if(sscanf(inbuf,"Device=%s",temp)) {
      	if(0 == stricmp(temp,"DATAQ151"))
        	device = DATAQ151;
        else if(0 == stricmp(temp,"DATAQ154"))
      		device = DATAQ154;
        else if(0 == stricmp(temp,"DATAQ155"))
      		device = DATAQ155;
        else if(0 == stricmp(temp,"DATAQ158"))
      		device = DATAQ158;
        else if(0 == stricmp(temp,"DATAQ194"))
      		device = DATAQ194;
        else if(0 == stricmp(temp,"DATAQ145"))
      		device = DATAQ145;
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
        }
       	else if(0 == stricmp(temp,"EQ1")) {
        	device = EQ1;
          samprate = 10;
          //zero_level = 0;
        }
       	else if(0 == stricmp(temp,"RHS3")) {
        	device = RHS3;
        }
       	else if(0 == stricmp(temp,"RHV1")) {
        	device = RHV1;
        }
        else if(0 == stricmp(temp,"AS1")) {
        	device = AS1;
          //samprate = 6;
        }
        else {
        	MdiClient->MessageBox("Invalid device.  Set to AS1.");
        	device = AS1;
          //samprate = 6;
				}
    	}
  	}
    switch(device) {
    case DATAQ151:
    case DATAQ154:
    case DATAQ194:
    	baud = 4800;
      break;
    case DATAQ145:
    	//baud = 2400;
      baud = 4800;
      break;
    case DATAQ158:
    case DATAQ155:
    	baud = 38400;
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
      gain = 1;
      zero_level = 32768;
      break;
    case SEPUK1:
    	baud = 9600;
      break;
    case RHS3:
    case RHV1:
    	baud = 19200;
      zero_level = 0;
      //samprate = 6;
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
   //Debug("Profile:working_dir:%s",working_dir);
   if(strlen(working_dir)) {
   		strcpy(station_code,working_dir);
      // Eliminate trailing
			station_code[strlen(station_code)-1] = 0;
    	// If a different working directory is specified, assume this
    	// is a tcp client or an ftp client.
      if(strlen(TCPHostAddress)>1) {
    		tcp_client = true;
        ftp_download = false;
      	tcp_refresh_seconds = 1;	// Set to 1-second refresh time
      	//Debug("tcp_client:%d",tcp_client);
    		// Following items carried in as arguments to the program
    		// See as1.cpp, OwlMain()
    		strcpy(tcp_host_addr,TCPHostAddress);
    		tcp_port = TCPHostPort;
      } else if(strlen(FTPAddr)) {
      	ftp_download = true;
        tcp_client = false;
      	strcpy(ftp_addr,FTPAddr);
      	strcpy(ftp_user,FTPUser);
      	strcpy(ftp_pw,FTPPW);
      	strcpy(ftp_download_path,FTPPath);
        // Remove \ at end
        //ftp_download_path[strlen(ftp_download_path)-1] = 0;
      }
    	// Make the working directory the same as the data directory
    	strcpy(data_directory,working_dir);
  }
}

extern TAS1 *papp;
void Profile::write()
{
  char temp[64];
  sprintf(temp,"%sAS1.INI",working_dir);
  FILE *fp = fopen(temp,"wt");
  if(fp) {
    //char p[64];
    //for(int i=0;i<4;i++) {
    // 	if(papp->GetMenuText(i,p, 63))
    // 		continue;
    //	//fprintf(fp,p);
    //}
    fprintf(fp,"[AS1]\n");
    fprintf(fp,"Version=%s\n",GetResString(IDS_VERSION));
    fprintf(fp,"Latitude=%lg\n",latitude);
    fprintf(fp,"Longitude=%lg\n",longitude);
    fprintf(fp,"Elevation=%g\n",elevation);
    fprintf(fp,"BAUD=%d\n",baud);
    fprintf(fp,"COM=%d\n",comport);
    fprintf(fp,"SampleRate=%g\n",samprate);
    fprintf(fp,"Gain=%lg\n",gain);
    fprintf(fp,"SaveScreenMinutes=%d\n",save_screen_minutes);
    fprintf(fp,"SaveScreenPath=%s\n",save_screen_path);
  	fprintf(fp,"Component=%c\n",component);
    fprintf(fp,"StationName=%s\n",station_name);
    fprintf(fp,"NetworkID=%s\n",network_id);
  	fprintf(fp,"StationCode=%s\n",station_code);
    fprintf(fp,"FileExtension=%s\n",extension);
    fprintf(fp,"LinesPerHour=%d\n",lines_per_hour);
    fprintf(fp,"LinesPerScreen=%d\n",lines_per_screen);
    fprintf(fp,"DecimateFactor=%d\n",decimate);
    fprintf(fp,"DaysToRetainRecords=%d\n",DaysToRetainRecords);
    fprintf(fp,"LowPassCutoff=%g\n",lp_cutoff);
    fprintf(fp,"LPNaturalFrequency=%g\n",Pp);
    fprintf(fp,"LPQ=%g\n",Qp);
    fprintf(fp,"LPPeriod=%g\n",Pf);
    fprintf(fp,"LowPassPeriod=%g\n",LowPassPeriod);
    fprintf(fp,"LowPassSec=%d\n",LowPassSec);
    fprintf(fp,"HighPassPeriod=%g\n",HighPassPeriod);
    fprintf(fp,"EventHighPassPeriod=%g\n",EventHighPassPeriod);
    fprintf(fp,"EventLowPassPeriod=%g\n",EventLowPassPeriod);
    fprintf(fp,"EnableLowPass=%d\n",EnableLowPass);
    fprintf(fp,"EnableHighPass=%d\n",EnableHighPass);
    fprintf(fp,"HelicorderFilter=%d\n",helicorder_filter);
    fprintf(fp,"ZeroLevel=%d\n",zero_level);
    fprintf(fp,"InitSampleSeconds=%d\n",init_samp_sec);
    fprintf(fp,"BandPassLowCutoff=%g\n",bp_low_cutoff);
    fprintf(fp,"BandPassHighCutoff=%g\n",bp_high_cutoff);
    fprintf(fp,"CalibrationMass=%g\n",cal_mass);
    fprintf(fp,"CalibrationDistance=%g\n",cal_distance);
    fprintf(fp,"MassOfArm=%g\n",mass_of_arm);
    fprintf(fp,"CenterOfGravity=%g\n",center_of_gravity);
    fprintf(fp,"mbAmplitude=%g\n",raw_amp);
    fprintf(fp,"MsAmplitude=%g\n",ms_amp);
    fprintf(fp,"mbLgAmplitude=%g\n",mbLg_amp);
    fprintf(fp,"mbPeriod=%g\n",per);
    fprintf(fp,"MsPeriod=%g\n",ms_per);
    fprintf(fp,"mbLgPeriod=%g\n",mbLg_per);
    fprintf(fp,"InstrumentResponse=%g\n",inst_res);
    fprintf(fp,"GreatCircleArc=%g\n",gca);
    fprintf(fp,"Depth=%g\n",depth);
    fprintf(fp,"DataDirectory=%s\n",data_directory);
    fprintf(fp,"SeismicWaves=%s\n",seismic_waves);
    fprintf(fp,"GlitchRemoval=%d\n",glitch_removal);
    fprintf(fp,"TCPPort=%d\n",tcp_port);
    fprintf(fp,"TCPHostAddress=%s\n",tcp_host_addr);
    fprintf(fp,"TCPRefreshSeconds=%g\n",tcp_refresh_seconds);
    fprintf(fp,"TCPClient=%s\n",tcp_client ? "YES" : "NO");
    fprintf(fp,"TCPServer=%s\n",tcp_server ? "YES" : "NO");
    fprintf(fp,"FTPAddr=%s\n",ftp_addr);
    fprintf(fp,"FTPUser=%s\n",ftp_user);
    fprintf(fp,"FTPPW=%s\n",ftp_pw);
    fprintf(fp,"FTPUploadPath=%s\n",ftp_upload_path);
    fprintf(fp,"FTPDownloadPath=%s\n",ftp_download_path);
    fprintf(fp,"FTPDaysToSave=%d\n",ftp_days_to_save);
    //fprintf(fp,"FTPUpload=%s\n",ftp_upload ? "YES" : "NO");
    fprintf(fp,"FTPDownload=%s\n",ftp_download ? "YES" : "NO");
    fprintf(fp,"Passive=%s\n",passive ? "YES" : "NO");
    fprintf(fp,"LogFile=%s\n",log_file);
    fprintf(fp,"CalibrationFile=%s\n",calibration_file);
    switch(device) {
    case DATAQ145: 		fprintf(fp,"Device=DATAQ145\n"); 		break;
    case DATAQ151: 		fprintf(fp,"Device=DATAQ151\n"); 		break;
    case DATAQ154: 		fprintf(fp,"Device=DATAQ154\n"); 		break;
    case DATAQ155: 		fprintf(fp,"Device=DATAQ155\n"); 		break;
    case DATAQ158: 		fprintf(fp,"Device=DATAQ158\n"); 		break;
    case DATAQ194: 		fprintf(fp,"Device=DATAQ194\n"); 		break;
    case INFILTECQM1: fprintf(fp,"Device=INFILTECQM1\n"); break;
    case EQ1: 				fprintf(fp,"Device=EQ1\n"); 				break;
    case RHS3: 				fprintf(fp,"Device=RHS3\n"); 				break;
    case RHV1: 				fprintf(fp,"Device=RHV1\n"); 				break;
    case AS1: 				fprintf(fp,"Device=AS1\n"); 				break;
    case SEPUK1:			fprintf(fp,"Device=SEPUK1\n"); 			break;
    default: 					fprintf(fp,"Device=unknown\n");		 	break;
    }
    fclose(fp);
  }
}

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
    MdiClient->MessageBox(sz1);
 	}
}

