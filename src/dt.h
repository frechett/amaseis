#ifndef DT_H__
#define DT_H__

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

#endif /* DT_H__ */

