// This program generates a key based on an input string.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static float mq[108][17];
//static float test[2][3] = {
//{1,2,3},
//{4,5,6}
//};
void main(int argc, char *argv[])
{
  char buffer[80];
  int i, j;
  float t;
	FILE *fp = fopen("mqtab.txt","rt");
  if(fp) {
  	// Discard first line
    fgets(buffer,80,fp);
    FILE *fpout = fopen("mq.c","wt");
    fprintf(fpout,"static float mbq[108][17] = {\n");
  	while(3 == fscanf(fp,"%d %d %g",&i,&j,&t)) {
    	mq[--i][--j] = t;
    }
    fclose(fp);
    printf("Done array.  i: %d j: %d t: %g\n",i,j,t);
    printf("Write\n");
    for(int i=0;i<108;i++) {
    	fprintf(fpout,"{");
    	for(int j=0;j<17;j++) {
      	fprintf(fpout,"%g",mq[i][j]);
        if(j<16)
        	fprintf(fpout,",");
        else
        	fprintf(fpout,"},\n");
      }
    }
    fprintf(fpout,"};\n");
    fclose(fpout);
  }
}
