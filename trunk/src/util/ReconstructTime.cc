//
// ReconstructTime.cc 
//
#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
int Example(char *argv[]);


int Usage(char *argv[]){

  cout << "\n Usage:" << argv[0] << "[-h] [-x]" << endl;
  cout << "\n Description: " << endl;
  cout << "\t Calculate time stamp from input syntax. " << endl;
  cout << "\n Options:" << endl;
  cout << "\t -h \t show this help    " << endl;
  cout << "\t -x \t show example    " << endl;
  cout << endl;
  Example(argv);
  exit(0);

}

int Example(char *argv[])
{
  cout << "\n Exapmle: " << endl;
  cout << "\t" << argv[0] << " -x" << endl;
  cout << "\n\tfor (( i=1; i<138; i++)) ; do line.sh $i JetRunBlue.dat | ReconstructTime ; done" << endl;
  cout << endl;
  exit(0);

}


int main(int argc, char *argv[])
{
   int opt;
   while (EOF != (opt = getopt(argc, argv, "hx?"))) {
      switch (opt) {
      case 'x':
         Example(argv);
         break;
      case 'h':
      case '?':
      case '*':
         Usage(argv);
      }
   }

   float X, Y;
   int FillID, Duration, days=0;

   cin >> X >> Y >> FillID >> Duration;
   
   int Hour   = int(X);
   int Minute = int((X-float(Hour))*100);
   int Day    = int(Y/100);
   int Month  = int(Y-Day*100);
   
   if (Month>=1) days=31;
   if (Month>2) days+=29;
   if (Month>3) days+=31;
   if (Month>4) days+=30;
   if (Month>5) days+=31;
   if (Month>6) days+=30;
   if (Month>7) days+=31;

   int time = 60*(Minute + 60*(Hour + 24*(Day + days))); // unit in [sec]

   printf("%8d %15d %10d %10d %10.2f %10.2f\n", FillID,time,time+Duration,Duration,X,Y);

   return 0;
}
