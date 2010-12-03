#include <stdio.h>
#include "rcdev.h"

int main (int argc, char *argv[])
{
 cdevData cdat;
 int irc;
 char *cdevval;
 char command[100];

    if (argc < 3) return -9999;
 sprintf(command, "get %s", argv[2]);
 cdevDevice & dev = cdevDevice::attachRef(argv[1]);
 if(!DEVSEND(dev, command, NULL, &cdat, stdout, irc)) {
  cdat.get("value", &cdevval);
  printf("%s\n", cdevval);
    }
    return irc;
}
