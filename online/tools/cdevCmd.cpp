/************************************************
 *	RHIC polarimeters - 2010		*
 *	I. Alekseev & D. Svirida		*
 *	CDEV command tool		 	*
 ************************************************/

#include <stdio.h>
#include "rcdev.h"

int main (int argc, char *argv[])
{
    cdevData cdat;
    int irc;
    char *cdevval;
    char command[100];

    if (argc < 4) {
	printf("Usage: cdevCmd device verb object\n");
	return -9999;
    }
    sprintf(command, "%s %s", argv[2], argv[3]);
    cdevDevice & dev = cdevDevice::attachRef(argv[1]);
    if(!DEVSEND(dev, command, NULL, &cdat, stdout, irc)) {
	cdat.get("value", &cdevval);
	printf("%s\n", cdevval);
    }
    return irc;
}
