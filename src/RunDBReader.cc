//  RunDB Reader
//  file name :   RunDBReader.cc
//
//
//  Author    :   Itaru Nakagawa
//  Creation  :   02/06/2006
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
using namespace std;
#include <iostream.h>
#include <fstream.h>
#include "rhicpol.h"
#include "rpoldata.h"
#include "Asym.h"
#include "AsymMain.h"


int
Usage(char *argv[]){

  cout << "\n Usage:" << argv[0] << "[-h] [-x]" << endl;
  cout << "\n Description: " << endl;
  cout << "\t calculate error propagation   " << endl;
  cout << "\n Options:" << endl;
  cout << "\t -h \t show this help    " << endl;
  cout << "\t -x \t show example    " << endl;
  cout << endl;
  exit(0);

}

int 
Example(char *argv[]){

  cout << "\n Exapmle: " << endl;
  cout << "\t" << argv[0] << " -r 7279.005" << endl;
  cout << 
  cout << endl;
  exit(0);

}


int 
main(int argc, char *argv[]) {

  extern StructRunDB rundb;
  recordConfigRhicStruct  *cfginfo;
  extern int printConfig(recordConfigRhicStruct *);


  char * RunID;
  confdir = getenv("CONFDIR");


  int opt;
   while (EOF != (opt = getopt(argc, argv, "r:hx?"))) {
    switch (opt) {
    case 'x':
      Example(argv);
      break;
    case 'r':
      RunID = optarg;
      break;
    case 'h':
    case '?':
    case '*':
      Usage(argv);
    }

   }
      

    // RunID 
    double RUNID = strtod(RunID,NULL);

    // Read Conditions from run.db
    readdb(RUNID);

    printConfig(cfginfo);


   return 0 ;
}

