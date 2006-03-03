#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int indexx,delim,xrot,yrot;

int main(){
  
  ifstream in_file;
  int ch=0;
  int i=0;
  int t=0;
  int time=0;
  int indexxx=0;
  in_file.open("x.dat");

  while (( ch=in_file.peek()) != EOF )
    {
      in_file >> indexx >> delim >> xrot >> yrot;

      if (i) {
	if (indexxx != indexx) cout << indexx << " " << delim-t <<" " <<  xrot << " " << yrot << endl;;
      } else {
	cout << indexx << " " << delim <<" " <<  xrot << " " << yrot << endl;;
      }
      t=delim;
      indexxx=indexx;

      ++i;
    }


 return 0;

} 
