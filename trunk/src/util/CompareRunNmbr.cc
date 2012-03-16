#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;


int main (int argc, char* argv[]) {

  //  cout<<atof(argv[1])<<endl;

  double RefN = atof(argv[1]);
  //  cout<<"RefN = "<<RefN<<endl;
  double RunN = atof(argv[2]);
  //  cout<<"RunN = "<<RunN<<endl;
  
  if (RefN < RunN)
    cout<<0<<endl;
  else
    cout<<1<<endl;

}
