#include <stdio.h>
#include <string.h>
#include <math.h>
#include <alloc.h>
#include "constant.h"
#include "swutil.h"

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

// *********************
// month_day()
//
// Gives month, day if given year and julian day
// *********************
void month_day(short j, short year,short &month,short &day)
{
  int *d;
  if(leap_year(year))
    d = jul_ly;
  else
    d = jul;
  for(int i = 0;i<12;i++)
    if(j>d[i] && j <= d[i+1])
      break;
  month = i+1;
  day = j-d[i];
}
/***********************
** julian()
**
** Gives julian day if given year, month, day
**********************/
static int julian(int year, int month, int day)
{
  int i;
  int d;
  i = leap_year(year);
  if(i) d = jul_ly[month];
  else d = jul[month];
  d += day;
  return d;
}
void main(
  int argc,
  char **argv)
{
  SAC *sac = new SAC(argv[1]);
}
float *get_float(FILE *fp,short &n)
{
  static float f[5];
  char buffer[80];
  fgets(buffer,80,fp);
  //  n = sscanf(buffer,"%15.7f%15.7f%15.7f%15.7f%15.7f",
  n = sscanf(buffer,"%f%f%f%f%f",
        f,f+1,f+2,f+3,f+4);
  return f;
}
short *get_int(FILE *fp,short &n)
{
  static short s[5];
  char buffer[80];
  fgets(buffer,80,fp);
  n = sscanf(buffer,"%d %d %d %d %d",
        s,s+1,s+2,s+3,s+4);
  return s;
}

SAC::SAC(const char* filename)
{
  float *f;
  short *in;
  short n;
  char buffer[80];
  FILE *fp = fopen(filename,"rt");
  if(fp == NULL) {
    delta = 0;
    return;
  }
  f = get_float(fp,n);
  if(n==5) {
    delta = f[0];
    depmin = f[1];
    depmax = f[2];
  } else return;
  f = get_float(fp,n);
  if(n == 5) {
    b = f[0];
    e = f[1];
    o = f[2];
  } else return;
  for(short i=0;i<12;i++) {
    f = get_float(fp,n);
  }
  in = get_int(fp,n);
  nzyear = in[0];
  nzjday = in[1];
  nzhour = in[2];
  nzmin = in[3];
  nzsec = in[4];
  in = get_int(fp,n);
  nzmsec = in[0];
  npts = in[4];
  for(i=0;i<6;i++) {
    in = get_int(fp,n);
  }
  fgets(buffer,80,fp);
  strcpy(kevnm,buffer+8);
  buffer[8] = 0;
  strcpy(kstnm,buffer);
  for(i=0;i<6;i++)
    fgets(buffer,80,fp);
  sscanf(buffer,"%*s %*s %s",kcmpnm);
  fgets(buffer,80,fp);
  data = new float[npts];
  float *p = data;
  while(fgets(buffer,80,fp)) {
    n = sscanf(buffer,"%f %f %f %f %f",&f[0],&f[1],&f[2],&f[3],&f[4]);
    if(n == 0) break;
    for(i=0;i<n;i++)
      *p++ = f[i];
  }
  // We can't count on SAC having the proper depmin and depmax
  depmin =  1e10;
  depmax = -1e10;
  float ff;
  for(i=0;i<npts;i++) {
   ff = data[i];
   if(ff > depmax) depmax = ff;
   if(ff < depmin) depmin = ff;
  }
  // Compute month and day
  month_day(nzjday,nzyear,month,day);
}
