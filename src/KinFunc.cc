//
// 
//
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include "Kinema.h"

using namespace std;

// defaults
float x=50;   // [ug/cm*2]
float dE=400; // [keV]
const float Precision = 0.1; // [keV]

int
Usage(char *argv[]){

  cout << "\n Usage:" << argv[0] << "[-w <dwidth>][-e <energy>][-i][-s][-h][-x]" << endl;
  cout << "\n Description: " << endl;
  cout << "\t Convert energy deposit <Edep> in [keV] to kinetic energy <Ekin> of 12C " << endl;
  cout << "\t for given deadlayer thickness <dwidth> [ug/cm^2] in silicon. With invert " << endl;
  cout << "\t option -i, it calculates invert kinematics <Ekin> -> <Edep> for <dwidth>" << endl;
  cout << "\n Options:" << endl;
  cout << "\t -w <dwidth>  deadlayer width in [ug/cm^2]. Def:" << x << endl;
  cout << "\t -e <energy>  energy deposit (kinetic energy with -i option) in [keV]. Def:" 
       << dE << endl;
  cout << "\t -i           calculate invert kinematics. kinetic energy as input." << endl; 
  cout << "\t -s           show unit in output" << endl;
  cout << "\t -h \t      show this help    " << endl;
  cout << "\t -x \t      show example    " << endl;
  cout << endl;
  exit(0);

}

int 
Example(char *argv[]){

  cout << "\n Exapmle: " << endl;
  cout << "  1) Calculate <Ekin> for <Edep>=400 and <dwidth>=55 " << endl;
  cout << "\t" << argv[0] << " -e 400 -w 55 " << endl << endl;
  cout << "  2) Calculate <Edep> for <Ekin>=900 and <dwidth>=70 " << endl;
  cout << "\t" << argv[0] << " -e 900 -w 70 -i " << endl;
  cout << endl;
  exit(0);

}


//
// Class name  : 
// Method name : CalcInvertKinema
//
// Description : Scan dE from 1 [keV] and calcualte Kinetic energy <Ekin0>
//             : until resulting <Ekin0> is close enough to desired value Ekin.
// Input       : (float Ekin, float x)
// Return      : dE
//
float 
CalcInvertKinema(float Ekin, float x){

  float Ekin0 = 0;
  float dE = 1; // [keV]
  while (fabs(Ekin-Ekin0)>Precision) {

    Ekin0 = ekin(dE,x);
    dE += Precision;
    if (Ekin-Ekin0<0) return dE;

  }

  return dE;

}


int 
main(int argc, char *argv[]) {

  bool InvertKinema = false;
  bool ShowUnit = false;
  float Ekin;

  int opt;
   while (EOF != (opt = getopt(argc, argv, "w:e:ishx?"))) {
    switch (opt) {
    case 'w':
      x=atof(optarg);
      break;
    case 'e':
      dE=atof(optarg);
      break;
    case 'i':
      InvertKinema = true;
      break;
    case 's':
      ShowUnit = true;
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
      
   if (InvertKinema) {// Search dE close enough to resulting Ekin
    Ekin = dE; 
    dE = CalcInvertKinema(Ekin, x);
   }
   
   // Call Main routine
   Ekin = ekin(dE,x);

   // print results
   if (ShowUnit) {
     printf("=====================================================\n");
     printf("   Ekin[keV]   Edep[keV]  Eloss[keV]  dwidth[ug/cm^2]\n");
     printf("=====================================================\n");
   }

   float Eloss = Ekin - dE;
   printf("%12.2f%12.2f%12.2f%12.2f\n",Ekin,dE,Eloss,x);

   return 0;

}

