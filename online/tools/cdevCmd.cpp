/************************************************
 *	RHIC polarimeters - 2010		*
 *	I. Alekseev & D. Svirida		*
 *	CDEV command tool - get/set only 	*
 ************************************************/

#include <stdio.h>
#include "rcdev.h"

int main (int argc, char *argv[])
{
    cdevData cdat;
    int irc;
    char *cdevval;
    char command[100];

    if (argc < 3) {
	printf("Usage: cdevCmd device object [value]\n");
	return -9999;
    }
    cdevDevice & dev = cdevDevice::attachRef(argv[1]);
    if (argc > 3) {	// set
	sprintf(command, "set %s", argv[2]);
	cdat.insert("value", argv[3]);
	DEVSEND(dev, command, &cdat, NULL, stdout, irc);
    } else {		// get
	sprintf(command, "get %s", argv[2]);
	if(!DEVSEND(dev, command, NULL, &cdat, stdout, irc)) {
	    cdat.get("value", &cdevval);
	    printf("%s\n", cdevval);
	}
    }
    return irc;
}
