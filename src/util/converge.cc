#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

int
Usage(char *argv[]){

  cout << "\n Usage:" << argv[0] << "[-hx] [-t <tolerance>]" << endl;
  cout << "\n Description: " << endl;
  cout << "    return ratio beteen 1st & 2nd arguments input. With -t option," << endl;
  cout << "    returns 1 when ratio is less than tolerance,otherwise returns 0"<< endl;
  cout << "\t   " << endl;
  cout << "\n Options:" << endl;
  cout << "\t -t <tolerance>  tolerance in [%] " << endl;
  cout << "\t -h \t\t show this help    " << endl;
  cout << "\t -x \t\t show example    " << endl;
  cout << endl;
  exit(0);

}

int
Example(char *argv[]){

  cout << "\n Exapmle: " << endl;
  cout << "\t" << "echo 65.5 65.7 |" <<  argv[0] << " -t 1" << endl;
  cout << endl;
  exit(0);

}

int
main(int argc, char *argv[]) {
    float Tolerance = 0; // 1 [%]

    int opt;
    while (EOF != (opt = getopt(argc, argv, "hxt:?"))) {
        switch (opt) {
        case 'x':
            Example(argv);
            break;
        case 't':
            Tolerance = atof(optarg);
            break;
        case 'h':
        case '?':
        case '*':
            Usage(argv);
        }
        
    }

  
    int boolian=0;
    float x, y, z;
    x=y=z=0;

    cin >> x >> y ;

    if (y) {
        z=x/y;
        if (Tolerance) boolian = fabs(z-1)*100<Tolerance ? 1 : 0 ;
    }

    if (Tolerance) {
        printf("%d",boolian);
    }else{
        printf("%10.5f",z);
    }

    return 0;

} 
