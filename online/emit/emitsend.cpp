#define _FILE_OFFSET_BITS 64	    // to handle >2Gb files
#include "rcdev.h"
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include "rhicpol.h"
#include "rpoldata.h"
#include "emitdefn.h"

extern polDataStruct poldat;
extern emitDataStruct tocdev;

void sendemit(char *devName) {
    int irc;
    cdevData data;

    cdevDevice & pol = cdevDevice::attachRef(devName);
    
    irc = 0;

    data.insert("value", poldat.beamEnergyS);
    DEVSEND(pol, "set beamEnergyS", &data, NULL, stdout, irc);

    data.insert("value", tocdev.peak);
    DEVSEND(pol, "set emitPeakS", &data, NULL, stdout, irc);
        
    data.insert("value", tocdev.width);
    DEVSEND(pol, "set emitWidthS", &data, NULL, stdout, irc);
        
    data.insert("value", tocdev.relwidth);
    DEVSEND(pol, "set emitRelWidthS", &data, NULL, stdout, irc);
        
    data.insert("value", tocdev.peaktoEvt);
    DEVSEND(pol, "set emitPeaktoEvtS", &data, NULL, stdout, irc);
        
    data.insert("value", tocdev.peakb, 360);
    DEVSEND(pol, "set emitPeakBunchS", &data, NULL, stdout, irc);

    data.insert("value", tocdev.widthb, 360);
    DEVSEND(pol, "set emitWidthBunchS", &data, NULL, stdout, irc);

    data.insert("value", tocdev.relwidthb, 360);
    DEVSEND(pol, "set emitRelWidthBunchS", &data, NULL, stdout, irc);

    data.insert("value", tocdev.peaktoEvtb, 360);
    DEVSEND(pol, "set emitPeaktoEvtBunchS", &data, NULL, stdout, irc);

    printf("Emittance scan results sent to CDEV %s\n", devName);

    if (irc != 0) printf("%d errors sending emittance data\n", irc);
}
