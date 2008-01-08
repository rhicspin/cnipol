#include <stdio.h>
#include "rcdev.h"

int main (int argc, char *argv[])
{
 cdevData cdat;
 int irc;
 char *cdevval;
 char command[100];
 FILE *aLogFile;

 sprintf(command, "get %s", argv[2]);
 cdevDevice & dev = cdevDevice::attachRef(argv[1]);
 if(!DEVSEND(dev, command, NULL, &cdat, aLogFile, irc)) 
  cdat.get("value", &cdevval);

 printf("%s\n", cdevval);
 exit(0);
}
