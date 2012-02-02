//
// calcPhi
// 
// calculate phi angle of pC detectors
// Oct.13, 2005 
// I.Nakagawa
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
  cout << "\n Description: An utility program to calculate phi angles of strips for given distance " ;
  cout << "\n              from target to detector. The output is in the format defined in AsymMain.h";
  cout << "\n              Edit change DIST in main() routine here for relevant distance in the unit"; 
  cout << "\n              of [mm]. And then run calcPhi" << endl;
  cout << "\n Options:" << endl;
  cout << "\t -t \t trancated phi {45,90,135,225,270,315} [def]:off  " << endl;
  cout << "\t -h \t show this help    " << endl;
  cout << "\t -x \t show example    " << endl;
  cout << endl;
  exit(0);
}

int Example(char *argv[])
{
  cout << "\n Exapmle: " << endl;
  cout << "\t" << argv[0] << " -x" << endl;
  cout << endl;
  exit(0);
}


int main(int argc, char *argv[])
{
  int Trancate = 0;

  int opt;

  while (EOF != (opt = getopt(argc, argv, "htx?"))) {
     switch (opt) {
     case 'x':
        Example(argv);
        break;
     case 't':
        Trancate = 1;
        break;
     case 'h':
     case '?':
     case '*':
        Usage(argv);
     }
  }
   
  int DIST = 180; // distance between target to detector [mm]
  float PHI[6];
  PHI[0] = 1 * M_PI/4.; 
  PHI[1] = 2 * M_PI/4.;
  PHI[2] = 3 * M_PI/4.;
  PHI[3] = 5 * M_PI/4.;
  PHI[4] = 6 * M_PI/4.;
  PHI[5] = 7 * M_PI/4.;

  int str = 0;
  float phi[72];
  for (int det=0; det<=5; det++) {
    
    for (int i=1; i<=12; i++) {

      str = det + i;
      phi[str] = Trancate ? PHI[det] : PHI[det] + atan(2*(i-6.5)/DIST) ; 
      /* 
         cout << str << " " << PHI[det]/M_PI*180 << " " 
           << phi[str] << " " << phi[str]/M_PI*180 << endl;
      */
      printf("%7.5f,", phi[str]);
    }

    cout << endl;
  }

  return 0;
}
