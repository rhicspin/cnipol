//
// 
//
#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include "Kinema.h"

int
Usage(char *argv[]){

  cout << "\n Usage:" << argv[0] << "[-w <dwidth>][-d <dE>][-h] [-x]" << endl;
  cout << "\n Description: " << endl;
  cout << "\t calculate error propagation   " << endl;
  cout << "\n Options:" << endl;
  cout << "\t -w <dwidth>  deadlayer width in [ug/cm^2]" << endl;
  cout << "\t -d <dE>      energy deposit in [keV]" << endl;
  cout << "\t -h \t      show this help    " << endl;
  cout << "\t -x \t      show example    " << endl;
  cout << endl;
  exit(0);

}

int 
Example(char *argv[]){

  cout << "\n Exapmle: " << endl;
  cout << "\t" << argv[0] << " -x" << endl;
  cout << endl;
  exit(0);

}


int 
main(int argc, char *argv[]) {

  float x, dE;
  // defaults
  x=50;   // [ug/cm*2]
  dE=400; // 400 keV
  
  int opt;
   while (EOF != (opt = getopt(argc, argv, "w:d:hx?"))) {
    switch (opt) {
    case 'w':
      x=atof(optarg);
      break;
    case 'd':
      dE=atof(optarg);
      break;
    case 'x':
      Example(argv);
      break;
    case 'h':
    case '?':
    case '*':
      Usage(argv);
    }

   }
      

  float Tkin = ekin(dE,x);
  printf("%10.2f%10.2f\n",dE,Tkin);

   return 0;

}

