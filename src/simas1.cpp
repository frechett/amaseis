/************************************************************************/
/* Send file out COM1 port                                              */
/* Alan Jones, 11/19/91                                                 */
/************************************************************************/
#include "stdlib.h"
#include "string.h"
#include "stdio.h"      /* standard header definitions                  */
#include "bios.h"
#include "math.h"

#define LF 10           /* line feed */
#define CR 13           /* Carriage Return */
#define EVEN 0x18       /* Turbo open command for even parity */
#define NONE 0x00       /* Turbo open command for no parity   */
#define STOP_1 0x00     /* 1 stop bit */
#define STOP_2 0x04     /* 2 stop bits */
#define BITS_7 0x02     /* 7 bit code */
#define BITS_8 0x03     /* 8 bit code */
#define BAUD_300  0x40
#define BAUD_600  0x60
#define BAUD_1200 0x80
#define BAUD_2400 0xa0
#define BAUD_4800 0xc0
#define BAUD_9600 0xe0
#define COM1 0          /* COM1 */
#define IER 1           /* offset to Interrupt Enable Register      */
#define IIR 2           /* offset to Interrupt Ident Register       */
#define LCR 3           /* offset to Line Control Register          */
#define MCR 4           /* offset to Modem Control Register         */
#define LSR 5           /* offset to Line Status Register           */
#define MSR 6           /* offset to Modem Status Register          */

void sendstr(char *string)
{
    int i;
    char c = 1;
    int a;

    string[strlen(string)-1] = CR;      /* Turn NL into CR */
    for(i=0;i<strlen(string);i++) {
        c = string[i];
        //do {
        //  a = bioscom(3,0,COM1);
        //  a &= 0x0010;        /* wait until CTS low   */
        //} while (!a);
        do {
          a = bioscom(3,0,COM1);
          a &= 0x2000;
        } while (!a);             /* Wait until transmitter reg empty */
        a=bioscom(1,c,COM1);                        /* Send character */
    }
}

#define PI 3.141592654
#define TWOPI 6.283185307
int main(int ,char *)
{
    char string[16];
    char *s;
    char c;
    float t;
    float f1 = 0.1;
    float f2 = 0.11;
    float f3 = 0.33
    int a;
	f1 = TWOPI*f1;
	f2 = TWOPI*f2;
	f3 = TWOPI*f3;
	float a1 = 100;
	float a2 = 110;
	float a3 = 75;
    /* COM1, 2400 baud, no parity, 8 bits */
    bioscom(0,BITS_8 | STOP_1 | NONE | BAUD_2400,COM1);
    while (1) {
      a = 2048+ (int)(a1*sin(f1*t)+a2*sin(f2*t)+a3*sin(f3*t));
      sprintf(string,"%u\r");
      printf(string);
      Sleep(100);
    }
    fclose(fp);
   return 0;
}
