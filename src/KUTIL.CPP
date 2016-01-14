#include "as1.h"

// utility.h            09/06/93        Yu-Long Kung
//

#if 0
template<class T> void _export KValueSwap(T far& t1, T far& t2)
{
        T temp;
        temp=t1; t1=t2; t2=temp;
};
#endif

//float ksign(float& f)
//{
//        return f/fabs(f);
//}
//short ksign(short& s)
//{
//        return s/abs(s);
//}

#if 0
template<class T> void _export KPointerSwap(T* t1,T* t2)
{
        T* temp;
        temp=t1; t1=t2 ; t2=temp;
};
#endif
//- swap two values
//
void SWAP(float &temp1,float &temp2)
{
        float temp;
        temp=temp1; temp1=temp2; temp2=temp;
}
void SWAP(short &temp1,short &temp2)
{
        short temp;
        temp=temp1; temp1=temp2; temp2=temp;
}


void Swab(unsigned short& iv)
{
   iv= (iv<<8) | ((iv>>8) & 0x00ff);
}

void Swab(short& iv)
{
   iv= (iv<<8) | ((iv>>8) & 0x00ff);
}

void Swab(unsigned long& iv)
{
   unsigned long temp1,temp2;
   temp1= iv<<16;
        temp1= (temp1<<8) | ((temp1>>8) & 0x00ff0000L);
        temp2= (iv>>16) & 0x0000ffffL ;
        temp2= (temp2>>8) | ((temp2<<8) & 0x0000ff00L);
   iv=(temp1) | temp2;
}

void Swab(long& iv)
{
   unsigned long temp1,temp2;
   temp1= iv<<16;
        temp1= (temp1<<8) | ((temp1>>8) & 0x00ff0000L);
        temp2= (iv>>16) & 0x0000ffffL ;
        temp2= (temp2>>8) | ((temp2<<8) & 0x0000ff00L);
   iv=(temp1) | temp2;
}

// for 4-bytes float value
void Swab(float& fv)
{
        unsigned long* temp;
   temp = (unsigned long *) &fv;
   Swab(*temp);
}

//template<classs T>T ArraySwab(T &av, long len)
void ArraySwab(unsigned short *av,int len)
{
        unsigned short i;
   for(i=0; i<len ; i++)
       Swab(*(av+i));
}

void ArraySwab(short *av,int len)
{
   int i;
   for(i=0; i<len ; i++)
       Swab(*(av+i));
}

// byte swab for data array
//
void ArraySwab(unsigned long *av,int len)
{
   int  i;
   for(i=0; i<len ; i++)
       Swab(*(av+i));
}

void ArraySwab(long *av,int len)
{
   int i;
   for(i=0; i<len ; i++)
       Swab(*(av+i));
}

void ArraySwab(float *av,int len)
{
   int i;
   for(i=0; i<len ; i++)
       Swab(*(av+i));
}

// decipher the EBC code
//
// unsigned short Ebc(char ebc)
//{
//   unsigned short temp;
//   temp=((ebc>>4) & 0x0f) * 10;
//   temp+= (ebc & 0x0f);
//   return temp;
//}

//-
//



// Time and date functions
// ******************
// leap_year(y)
//
// Returns 1 if y is a leap year
// *****************
static int leap_year(int y)
{
  int i = 0;
  if(0 == (y%400)) i = 1;
  else if (0 == (y%100)) i = 0;
  else if (0 == (y%4)) i = 1;
  return i;
}

static int jul[] =    {0,31,59,90,120,151,181,212,243,273,304,334,365};
static int jul_ly[] = {0,31,60,91,121,152,182,213,244,274,305,335,366};

/********************
** days_since_0001()
**
** Given the year, gives the days before Jan 1 of this year
** counting from the year 0001
********************/
static int days_since_1901(int year)
{
  year -= 1901;
  return 365*year + (year/4) - (year/100) + (year/400);
}

// *********************
// month_day()
//
// Gives month, day if given year and julian day
// *********************
void month_day(int j, int year,int &month,int &day)
{
  int *d;
  if(leap_year(year))
    d = jul_ly;
  else
    d = jul;
  int i;
  for(i = 0;i<12;i++)
    if(j>d[i] && j <= d[i+1])
      break;
  month = i+1;
  day = j-d[i];
}
// Gives year, month, day, hour, minutes, seconds given seconds since 1901.
#define SPD 86400
#define SPD_FLOAT 86400.0
// Minutes per day
#define MPD 1440
#define MPD_FLOAT 1440.0

void sec2ymdhms(
  double t,  		// Number of seconds since 1901.01.01
  int &year,    // Pointer to year (returned)
  int &month,   // Pointer to month (returned)
  int &day,     // Pointer to day (returned)
  int &hour,    // Pointer to hour (returned)
  int &minute,  // Pointer to minute (returned)
  float &second)  // Pointer to second (returned)
{
  int i;

  int y = 2100;
  long tday = t/SPD;
  hour = ((long)(t/3600))%24;
  minute = (long)(t/60);
  second = t - 60*((double)minute);
  minute = minute%60;

  // Don't handle anything past 2100
  if(tday>days_since_1901(y))
  	tday=days_since_1901(y);
  while((tday-days_since_1901(y)) <= 0)
  	y--;
  year = y;
  day = (int)(tday-days_since_1901(y));
  if(0 == (y%400)) i = 1;
  else if (0 == (y%100)) i = 0;
  else if (0 == (y%4)) i = 1;
  else i = 0;
  if(i) {
    for(i=1;day>jul_ly[i];i++);
    i--;
    day -= jul_ly[i];
  }
  else {
    for(i=1;day>jul[i];i++);
    i--;
    day -= jul[i];
  }
  month = i;
}

/***********************
** julian()
**
** Gives julian day if given year, month, day
**********************/
int julian(int year, int month, int day)
{
  int i;
  int d;
  i = leap_year(year);
  if(i) d = jul_ly[month];
  else d = jul[month];
  d += day;
  return d;
}
/******************************
** sec_1901()
**
** Given year, month, day, hour, minute, second, millisec
** gives seconds since January 1, 1901.  1901 is picked
** so that leap years are easily computed
******************************/
double sec_1901(
  int year,
  int month,
  int day,
  int hour,
  int minute,
  int second,
  int msecond)
{
  int y = year - 1901;
  return 0.001*msecond + (double)second +
    60.0*(minute +
    60.0*(hour +
    24.0*(julian(year,month,day) +
    (double)((365*y + y/4)))));
}

float *get_float(FILE *fp,short &n)
{
  static float f[5];
  char buffer[80];
  char buf2[16];
  fgets(buffer,80,fp);
  buf2[15] = 0;
  short i;
  for(i=0;i<5;i++) {
    memcpy(buf2,buffer + i*15,15);
    f[i] = atof(buf2);
  }
  n = i;
	return f;
}
short *get_int(FILE *fp,int &n)
{
  static short s[5];
  char buffer[80];
  fgets(buffer,80,fp);
  n = sscanf(buffer,"%d %d %d %d %d",
        s,s+1,s+2,s+3,s+4);
  return s;
}


// file IO function
//
//    Input:
//       is     : stream of file
//       buff   : buffer
//       n      : how many line
//
void kgetline(ifstream& is,char* buff, short nb, short n)
{
   for(int i=0;i<n;i++) is.getline(buff,nb,'\n');
}

// kgetstring -- try to extract the string out if there is a number of
//               space (0x20) , tab (0x0b), return (0x0d 0x0a)
//               in front of this string st
//
//    ex :  intput = "      ttt"
//          output = "ttt"
//
char* kgetstring(char* str)
{
   int j=0;
        char *str1;


   while(1){
      if(!(str[j] == 0x20 || str[j] == 0x09 ||
           str[j] == 0x0d || str[j] == 0x0a))break;
      if( str[j++] == 0x00 ) break;
   }

   str1=new char(strlen(str)-j+1);
   return strcpy(str1,str+j);
}


//   kerror function -- this is for dos version if you want to change to
//                      window version, just modify the io part of this
//                      module
//
//      ERROR
//
//          1 - 199     :  System error
//        201 - 399     :  Instrument response error
//        401 - 599     :  data analysis error
//        601 - 799     :  forward modeling error
//        801 - 999     :  inversion error
//
short kerror(short err,char* func)
{
        ifstream iser("kerror.doc");
        short ierr;
        char serr[256];
        iser >> ierr;
        iser.getline(serr,256,'\n');
        while( (ierr != err) && (ierr != EOF)){
                iser >> ierr;
                iser.getline(serr,256,'\n');
        }
#ifdef _WINERROR
#else
        cout << "----------------------------------- MESSAGE "
        << "-----------------------------------" << endl;
        if(func[0] != '#') cout << " ***  In module " << func << endl;
        cout <<  "    Message " << ierr << " : " << serr << endl;
        cout << "--------------------------------------------"
        << "-----------------------------------" << endl;
#endif
        return ierr;
}






