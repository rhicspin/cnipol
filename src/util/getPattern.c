/* getPattern.c written: J.Wood April 12, 2006
   program to get the current spin or fill pattern from CDEV 
   and write output to a text file in a usable format
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define f2cFortran
#include <math.h>
#include <errno.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>

typedef struct {
    short int cdevFillPattern[360];
    short int cdevSpinPattern[360];    
} patternDataStruct;

patternDataStruct patternData;

extern void getSpin(int ring);
extern void getFill(int ring);

int main(int argc, char **argv)
{
  int i;
  int specRing=-1;    // ring specified by user: 0=yel, 1=blu
  int getfill=0;
  char defFileS[2][20] = {"currentSpinPat.yel", "currentSpinPat.blu"};
  char defFileF[2][20] = {"currentFillPat.yel", "currentFillPat.blu"};
  char OutFile[256];
  
  FILE *fout;

  // get arguments
  while ((i = getopt(argc, argv, "hybfo:")) != -1) 
    switch(i) {
    case 'y':
      specRing = 0;
      sprintf(OutFile, defFileS[specRing]);
      break;
    case 'b':
      specRing = 1;
      sprintf(OutFile, defFileS[specRing]);
      break;
    case 'f':
      if (specRing>=0 && specRing<=1) sprintf(OutFile, defFileF[specRing]);
      getfill = 1;
      break;
    case 'o':
      // only if optarg is not NULL 
      sprintf(OutFile, optarg);
      break;
    case 'h':
	default:
	  printf("Read pattern from CDEV, output to file\n");
	  printf("Usage: ./getPattern -y|b [-options] \n");
	  printf("Possible options are:\n");
	  printf("-h -- print this message;\n");
	  printf("-f -- get FILL pattern, rather than SPIN pattern;\n");
	  printf("-o <out file> -- specify output file name\n"); 
	  printf("   default file is %s(.blu) or %s(.blu);\n",defFileS[0], defFileF[0]);
	  return 1;
    }

  if(specRing<0 || specRing>1) {
    printf("ERROR: Must specify Yellow (-y) or Blue (-b)\n");
    return specRing;
  }
  
  // open output file
  fout = fopen(OutFile, "wb");    
  if (fout == NULL) {
    printf("ERROR: Cannot open output file %s\n", OutFile);
    return -1;
  }

  // read pattern from CDEV
  if(getfill) {
    getFill(specRing);
    for (i=0;i<120;i++) {
      fprintf(fout,"%d ",patternData.cdevFillPattern[3*i]);
    }
    fprintf(fout,"\n");
  } else {
    getSpin(specRing);
    for (i=0;i<120;i++) {
      fprintf(fout,"%d ",patternData.cdevSpinPattern[3*i]);
    }
    fprintf(fout,"\n");
  }
  printf("Pattern is written to file %s\n",OutFile);

  fclose(fout);

  return 0;
}
