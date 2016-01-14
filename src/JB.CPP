#include "as1.h"

// Constructor
JB::JB(char *phase,          // Which phase: P, PP, S, or SS
  float depth)          // Depth of event in km
{
  char buffer[128];
  wave = 0;
  len = 0;
  sprintf(buffer,"%s.JB",phase);
  // The depth of event tells us which one to use
  // In Jeffreys-Bullen tables h = (d-33)/6338 except when d == 0, then use the
  // first column.  For d == 33, use second column, etc.
  //short h=1.5+(depth-33.)/63.38;
  // Change computation so that it changes column for each 33 km starting
  // at 16.5 km
  short h=1+(depth-16.5)/63.38;
  // h goes from 0 to 13
  if(h<0) h = 0;
  if(h>13) h = 13;
  // Arithmetic in next statement gets us pointing to right place in
  // JB table since the degree column is 5 characters wide and each travel time
  // is 7 characters wide.
  char *pjb = buffer + h*7+5;
  FILE *fp = fopen(buffer,"rt");
  if(fp)
  {
    wave = new float[181];
    float *pdata = wave;
    float data;
    while(fgets(buffer,128,fp))
    {
      sscanf(pjb,"%f",&data);
      //*pdata++ = floor(.5+data);
      *pdata++ = data;
      len++;
    }
    fclose(fp);
  }
}

// Destructor
JB::~JB()
{
	if(wave)
		delete wave;
}

