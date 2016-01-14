
#define D2R M_PI/180.
#define R2D 180./M_PI
#define M_2PI 6.28318530717958647692

#ifndef _KUTILDLL
#define _KUTILDLL _export
#endif

#ifndef _KUTILCLASS
        #if defined (SETKUTIL_DLL)
                #define _KUTILCLASS __export
        #elif defined (GETKUTIL_DLL)
                #define _KUTILCLASS __import
        #else
                #define _KUTILCLASS
        #endif
#endif

int julian(int year, int month, int day);


//
/*
//enum BOOL
//{
//        FALSE,TRUE
//};
*/
enum KUNIT
{
        DEGREE, RADIAN
};

enum KDOMAIN
{
        DOMAINNONE, TIME, FREQUENCY, SPACE, WAVENUMBER
};

enum KFILTER
{
        FNONE, AUTOCORRELATION, CROSSCORRELATION, CONVOLUTION,
        BARTLETT, HAMMING, HANNING, WELCH, CHEBYSHEV_I, CHEBYSHEV_II,
        SIN, COS
};

union PTYPETF  // pointer of type transferation
{
        char                 *pc;
        unsigned char        *puc;
        short                *ps;
        unsigned short       *pus;
                  long            *pl;
                  unsigned long   *pul;
                  float           *pf;
                  double          *pd;
};

#define YMin RMin
#define YMax RMax
struct _KUTILCLASS KWorldRange
{
                  float XMin,XMax,RMin,RMax,IMin,IMax;    //
                  float Width(){ return XMax-XMin;}
                  float RHeight(){ return RMax-RMin;}
                  float YHeight(){ return YMax-YMin;}
                  float IHeight(){ return IMax-IMin;}
};
template<class T> void _KUTILDLL KValueSwap(T& t1, T& t2);
float _KUTILDLL ksign(float& f);
short  ksign(short& s);
template<class T> void  KPointerSwap(T* t1,T* t2);
void  SWAP(float &, float &);
void  SWAP(short &, short &);
void  Swab(unsigned short& iv);
void  Swab(short& iv);
void  Swab(unsigned long& iv);
void  Swab(long& iv);
void  Swab(float& fv);
void  ArraySwab(unsigned short *av,int len);
void  ArraySwab(short *av,int len);
void  ArraySwab(unsigned long *av,int len);
void  ArraySwab(long *av,int len);
void  ArraySwab(float *av,int len);
unsigned short  Ebc(char ebc);
void  kgetline(ifstream& is,char* buff, short nb, short n);
char*  kgetstring(char* str);
//TPoint  kgetwp(TPoint& point,TRect& rect);
short  kerror(short ,char* func="#");
void month_day(int j, int year,int &month,int &day);
double sec_1901(int year,int month, int day, int hour, int minute, int second, int msecond);
void sec2ymdhms(
  double t,  		// Number of seconds since 1901.01.01
  int &year,    // Pointer to year (returned)
  int &month,   // Pointer to month (returned)
  int &day,     // Pointer to day (returned)
  int &hour,    // Pointer to hour (returned)
  int &minute,  // Pointer to minute (returned)
  float &second);  // Pointer to second (returned)





