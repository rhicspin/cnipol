#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

int
Usage(char *argv[]){

  cout << "\n Usage:" << argv[0] << "[-chx]" << endl;
  cout << "\n Description: " << endl;
  cout << "    make tables from standard input. Insert spaces for interval between" << endl;
  cout << "    numbers. " << endl;
  cout << "\n Options:" << endl;
  cout << "\t -c \t\t carrige return in output " << endl;
  cout << "\t -h \t\t show this help    " << endl;
  cout << "\t -x \t\t show example    " << endl;
  cout << endl;
  exit(0);

}

int
Example(char *argv[]){

  cout << "\n Exapmle: " << endl;
  cout << "\t" << "echo 8 9 10 15 27 29 |" <<  argv[0] << endl;
  cout << "               8 9 10             15                                  27    29" << endl; 
  cout << endl;
  exit(0);

}

int
main(int argc, char *argv[]) {
  int CarrigeReturn=0;

    int opt;
    while (EOF != (opt = getopt(argc, argv, "hxt:?"))) {
        switch (opt) {
        case 'x':
	  Example(argv);
	  break;
	case 'c':
	  CarrigeReturn = 1;
	  break;
        case 'h':
        case '?':
        case '*':
            Usage(argv);
        }
        
    }

  
    const int N=72;
    int x[N], i=0;

    while (cin) {
      cin >> x[i] ;
      ++i;
    }
    
    int nentry = i-1, NSTRIP=72;
    int st, k=0;
    if (nentry) {
      for (int j=1; j<=NSTRIP; ++j){
	if (j == x[k]) {
	  printf(" %d", j);
	  ++k;
	}else{
	  j<10 ? printf("  "): printf("   ");
	}
      }
    }

    if (CarrigeReturn) cout << endl;

    return 0;

} 
