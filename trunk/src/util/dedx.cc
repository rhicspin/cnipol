//
// 
//
#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

int Usage(char *argv[])
{
  cout << "\n Usage:" << argv[0] << "[-h] [-x]" << endl;
  cout << "\n Description: " << endl;
  cout << "\t integrate de/dx for 12C in silicon thickness of w " << endl;
  cout << "\n Options:" << endl;
  cout << "\t -w \t silicon thickness [ug/cm^2]    def:50" << endl;
  cout << "\t -E \t incident 12C kinetic energy  [keV]  def:400" << endl;
  cout << "\t -h \t show this help    " << endl;
  cout << "\t -x \t show example    " << endl;
  cout << endl;
  exit(0);
}

int Example(char *argv[])
{
  cout << "\n Exapmle: " << endl;
  cout << "\t" << argv[0] << " -w 50 -E 400" << endl;
  cout << endl;
  exit(0);
}


double dedx(double E, double dx)
{
  double de=0.3799 + 0.87e-2*E - 0.7653e-5*E*E + 0.3313e-8*E*E*E - 0.5516e-12*E*E*E*E;

  return de*dx;
}


int main(int argc, char *argv[])
{
   int nstep=10000;
   double w = 50; // [ug/cm^2]
   double Ei=400; // [keV]
 
   int opt;
   while (EOF != (opt = getopt(argc, argv, "hxw:E:?"))) {
      switch (opt) {
      case 'x':
         Example(argv);
         break;
      case 'w':
         w = atof(optarg);
         break;
      case 'E':
         Ei = atof(optarg);
         break;
      case 'h':
      case '?':
      case '*':
         Usage(argv);
      }
   }
      
   double dx = w/float(nstep);
   double E, dE ;
 
   E=Ei;
   for (int i=0; i<nstep; i++) {
     dE =  dedx(E, dx);
     E -= dE ;  
   }
 
   double Eloss = Ei - E;
   printf("%10.3f%10.3f%10.3f\n", w, Ei, Eloss);
 
   return 0;
}
