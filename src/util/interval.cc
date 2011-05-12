#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

struct StructData{
  int index;
  int delim;
  int xrot;
  int yrot;
} pres, prev;


int main()
{
  ifstream in_file;
  int ch       = 0;
  int i        = 0;
  int interval = 0;

  in_file.open("x.dat");

  while (( ch=in_file.peek()) != EOF )
  {
      in_file >> pres.index >> pres.delim >> pres.xrot >> pres.yrot;

      interval = prev.delim != pres.delim ? pres.delim - prev.delim : 1;
      if (i>0) 
	cout << prev.index << " " << interval << " " << prev.xrot << " " << prev.yrot << endl;

      prev.index = pres.index;
      prev.delim = pres.delim;
      prev.xrot  = pres.xrot;
      prev.yrot  = pres.yrot;

      ++i;
  }
  return 0;
} 
