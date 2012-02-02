#include <stdio.h>
#include "rcdev.h"

int main (int argc, char *argv[])
{
 cdevData cdat;
 int irc;
 char command[100];
 FILE *aLogFile;

 sprintf(command, "set %s", argv[2]);
 cdevDevice & dev = cdevDevice::attachRef(argv[1]);
 cdat.insert("value", &argv[3]);
 DEVSEND(dev, command, &cdat, NULL, aLogFile, irc);

 printf("%d\n", irc);
 exit(0);
}
