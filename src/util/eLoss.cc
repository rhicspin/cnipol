//
// 
//
#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

int
Usage(char *argv[]){

  cout << "\n Usage:" << argv[0] << "[-h] [-x]" << endl;
  cout << "\n Description: " << endl;
  cout << "\t Calculate incident energy from energy deposit " << endl;
  cout << "\t (excluding eloss in dead layer thickness w [ug/cm^2])" << endl;
  cout << "\n Options:" << endl;
  cout << "\t -w \t dead layer thickness [ug/cm^2]  def:50" << endl;
  cout << "\t -d \t energy loss of 12C in silicon [keV]  def:100" << endl;
  cout << "\t -h \t show this help    " << endl;
  cout << "\t -x \t show example    " << endl;
  cout << endl;
  exit(0);

}

int 
Example(char *argv[]){

  cout << "\n Exapmle: " << endl;
  cout << "\t" << argv[0] << " -x" << endl;
  cout << "\t" << argv[0] << " -d 100 -w 50" << endl;
  cout << endl;
  exit(0);

}


//
// Class name  :
// Method name : dE2E
//
// Description : Calculate Incident Energy (Ekin) from Energy Deposit (dE) in Silicon
//             : excluding Eloss within deadlayer thickness (w).
// Input       : Energy Deposit dE [keV], dead layer thickness w [ug/cm^2]
// Return      : Incident 12C energy [keV]
//
double dE2E(double dE, double w){
    
    const double cp0[4] = {-0.5174,0.4172,0.3610E-02,-0.1286E-05}; 
    const double cp1[4] = {1.0000,0.8703E-02,0.1252E-04,0.6948E-07};
    const double cp2[4] = {0.2990E-05,-0.7937E-05,-0.2219E-07,-0.2877E-09};
    const double cp3[4] = {-0.8258E-08,0.4031E-08,0.9673E-12,0.3661E-12};
    const double cp4[4] = {0.3652E-11,-0.8652E-12,0.4059E-14,-0.1294E-15};
    double pp[5];
    
    pp[0] = cp0[0] + cp0[1]*w + cp0[2]*pow(w,2) + cp0[3]*pow(w,3);
    pp[1] = cp1[0] + cp1[1]*w + cp1[2]*pow(w,2) + cp1[3]*pow(w,3);
    pp[2] = cp2[0] + cp2[1]*w + cp2[2]*pow(w,2) + cp2[3]*pow(w,3);
    pp[3] = cp3[0] + cp3[1]*w + cp3[2]*pow(w,2) + cp3[3]*pow(w,3);
    pp[4] = cp4[0] + cp4[1]*w + cp4[2]*pow(w,2) + cp4[3]*pow(w,3);
    
    double Ekin = pp[0] + pp[1]*dE + pp[2]*pow(dE,2) 
        + pp[3]*pow(dE,3) + pp[4]*pow(dE,4);
    
    return Ekin;

};



int 
main(int argc, char *argv[]) {
  double dE = 100 ; // [keV]
  double w = 50; // [ug/cm2]

  int opt;
   while (EOF != (opt = getopt(argc, argv, "hx?d:w:"))) {
    switch (opt) {
    case 'x':
      Example(argv);
      break;
    case 'w':
      w = atof(optarg);
      break;
    case 'd':
      dE = atof(optarg);
      break;
    case 'h':
    case '?':
    case '*':
      Usage(argv);
    }

   }
      
   double Ekin = dE2E(dE,w);
   double dElossInDeadLayer=Ekin-dE;
   printf("%10.4f%10.4f%10.4f\n", w, Ekin-dE, Ekin);


}


