/************************************************
 *      RHIC polarimeters - 2010                *
 *      I. Alekseev & D. Svirida                *
 *      CDEV command tool - get/set only        *
 *      Arrays are not supported                *
 ************************************************/

#include <stdio.h>
#include <string.h>
#include "rcdev.h"

int main (int argc, char *argv[])
{
    cdevData cdat;
    int irc;
    char *cdevval;
    const char devTypes[][20] = {"StringType", "DoubleType", "FloatType", "ShortType", "LongType", "UShortType"};
    char command[100];
    char *type;
    int i;

    if (argc < 3) {
        printf("Usage: cdevCmd device object [value]\n");
        return -9999;
    }
    irc = 0;
    type = 0;
    cdevDevice & dev = cdevDevice::attachRef(argv[1]);
    if (argc == 3) {    // get - we know how to do... Arrays not supported ...
        sprintf(command, "get %s", argv[2]);
        if(!DEVSEND(dev, command, NULL, &cdat, stdout, irc)) {
            cdat.get("value", &cdevval);
            printf("%s\n", cdevval);
        }
    } else {            // set - we need to find type first.
        sprintf(command, "getMetaData %s", argv[2]);
        DEVSEND(dev, command, NULL, &cdat, stdout, irc);
        if (irc) return irc;    // error already ...
//    cdat.asciiDump();
        irc = cdat.get(7116, &type);    // 7116 is "propType"
        if (irc) return irc;
        for (i=0; i<sizeof(devTypes)/sizeof(devTypes[0]); i++) if (!strcmp(type, devTypes[i])) break;
        sprintf(command, "set %s", argv[2]);
        switch (i) {
        case 0: // StringType
            cdat.insert("value", argv[3]);
            break;
        case 1: // DoubleType
            cdat.insert("value", strtod(argv[3], NULL));
            break;
        case 2: // FloatType
            cdat.insert("value", (float)strtod(argv[3], NULL));
            break;
        case 3: // ShortType
            cdat.insert("value", (short) strtol(argv[3], NULL, 0));
            break;
        case 4: // LongType
            cdat.insert("value", strtol(argv[3], NULL, 0));
            break;
        case 5: // UShortType
            cdat.insert("value", (unsigned short) strtol(argv[3], NULL, 0));
            break;
        default:        // Unknown
            printf("Unknown type (%s) to be set - call developers.\n", type);
            return -1;
        }
        DEVSEND(dev, command, &cdat, NULL, stdout, irc);
    }
    return irc;
}
