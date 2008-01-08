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

extern "C" {
    void sendresult_(int *recRing);
}

extern polDataStruct poldat_;

void sendresult_(int *recRing) {
    char polCDEVName[2][20] = {"polarimeter.yel", "polarimeter.blu"};
    char specCDEVName[2][20] = {"ringSpec.yellow", "ringSpec.blue"};
    int N;
    int irc;
    cdevData data;
    int fillNumberM = 0;

    N = -1;
    if (*recRing == REC_YELLOW) N = 0;
    if (*recRing == REC_BLUE) N = 1;
    if (N < 0) {
	printf("Unknown polarimeter: no result sent to ADO!\n");
	return;
    }
    
    cdevDevice & pol = cdevDevice::attachRef(polCDEVName[N]);
    cdevDevice & spec =  cdevDevice::attachRef(specCDEVName[N]);
    
    irc = 0;

    if(!DEVSEND(spec, "get fillNumberM", NULL, &data, stdout, irc))
	data.get("value", &fillNumberM);
    
//	Update runId only if it really corresponds to fillNumber
    if ((int)(poldat_.runIdS) == fillNumberM) {
	data.insert("value", poldat_.runIdS);
//	Update runIdS or emitRunIdS
	if ((((int)(10*poldat_.runIdS)) % 10) >= 4) {
	    DEVSEND(pol, "set emitRunIdS", &data, NULL, stdout, irc);
	} else {
	    DEVSEND(pol, "set runIdS", &data, NULL, stdout, irc);
	}
    }

    data.insert("value", poldat_.startTimeS);
    DEVSEND(pol, "set startTimeS", &data, NULL, stdout, irc);
    
    data.insert("value", poldat_.stopTimeS);
    DEVSEND(pol, "set stopTimeS", &data, NULL, stdout, irc);
    
    data.insert("value", poldat_.daqVersionS);
    DEVSEND(pol, "set daqVersionS", &data, NULL, stdout, irc);
    
    data.insert("value", poldat_.cutIdS);
    DEVSEND(pol, "set cutIdS", &data, NULL, stdout, irc);

    data.insert("value", poldat_.targetIdS);
    DEVSEND(pol, "set targetIdS", &data, NULL, stdout, irc);

    data.insert("value", poldat_.encoderPositionS, 2);
    DEVSEND(pol, "set encoderPositionS", &data, NULL, stdout, irc);
    
    data.insert("value", poldat_.statusS);
    DEVSEND(pol, "set statusS", &data, NULL, stdout, irc);
    
    strncpy(poldat_.statusStringS, stat2str(poldat_.statusS), sizeof(poldat_.statusStringS));
    data.insert("value", poldat_.statusStringS);
    DEVSEND(pol, "set statusStringS", &data, NULL, stdout, irc);
    
    data.insert("value", poldat_.totalCountsS);
    DEVSEND(pol, "set totalCountsS", &data, NULL, stdout, irc);
       
    data.insert("value", poldat_.upCountsS);
    DEVSEND(pol, "set upCountsS", &data, NULL, stdout, irc);
    
    data.insert("value", poldat_.downCountsS);
    DEVSEND(pol, "set downCountsS", &data, NULL, stdout, irc);
    
    data.insert("value", poldat_.unpolCountsS);
    DEVSEND(pol, "set unpolCountsS", &data, NULL, stdout, irc);
    
    data.insert("value", poldat_.countsUpLeftS, 360);
    DEVSEND(pol, "set countsUpLeftS", &data, NULL, stdout, irc);
    
    data.insert("value", poldat_.countsLeftS, 360);
    DEVSEND(pol, "set countsLeftS", &data, NULL, stdout, irc);
    
    data.insert("value", poldat_.countsDownLeftS, 360);
    DEVSEND(pol, "set countsDownLeftS", &data, NULL, stdout, irc);
    
    data.insert("value", poldat_.countsDownRightS, 360);
    DEVSEND(pol, "set countsDownRightS", &data, NULL, stdout, irc);
    
    data.insert("value", poldat_.countsRightS, 360);
    DEVSEND(pol, "set countsRightS", &data, NULL, stdout, irc);
    
    data.insert("value", poldat_.countsUpRightS, 360);
    DEVSEND(pol, "set countsUpRightS", &data, NULL, stdout, irc);
    
    data.insert("value", poldat_.avgAsymXS);
    DEVSEND(pol, "set avgAsymXS", &data, NULL, stdout, irc);

    data.insert("value", poldat_.avgAsymX45S);
    DEVSEND(pol, "set avgAsymX45S", &data, NULL, stdout, irc);

    data.insert("value", poldat_.avgAsymX90S);
    DEVSEND(pol, "set avgAsymX90S", &data, NULL, stdout, irc);
    
    data.insert("value", poldat_.avgAsymYS);
    DEVSEND(pol, "set avgAsymYS", &data, NULL, stdout, irc);
    
    data.insert("value", poldat_.avgAsymErrorXS);
    DEVSEND(pol, "set avgAsymErrorXS", &data, NULL, stdout, irc);
    
    data.insert("value", poldat_.avgAsymErrorX45S);
    DEVSEND(pol, "set avgAsymErrorX45S", &data, NULL, stdout, irc);
    
    data.insert("value", poldat_.avgAsymErrorX90S);
    DEVSEND(pol, "set avgAsymErrorX90S", &data, NULL, stdout, irc);
    
    data.insert("value", poldat_.avgAsymErrorYS);
    DEVSEND(pol, "set avgAsymErrorYS", &data, NULL, stdout, irc);
    
    data.insert("value", poldat_.bunchAsymXS, 360);
    DEVSEND(pol, "set bunchAsymXS", &data, NULL, stdout, irc);
    
    data.insert("value", poldat_.bunchAsymYS, 360);
    DEVSEND(pol, "set bunchAsymYS", &data, NULL, stdout, irc);
    
    data.insert("value", poldat_.bunchAsymErrorXS, 360);
    DEVSEND(pol, "set bunchAsymErrorXS", &data, NULL, stdout, irc);
    
    data.insert("value", poldat_.bunchAsymErrorYS, 360);
    DEVSEND(pol, "set bunchAsymErrorYS", &data, NULL, stdout, irc);

    data.insert("value", poldat_.beamEnergyS);
    DEVSEND(pol, "set beamEnergyS", &data, NULL, stdout, irc);
    
    data.insert("value", poldat_.analyzingPowerS);
    DEVSEND(pol, "set analyzingPowerS", &data, NULL, stdout, irc);
    
    data.insert("value", poldat_.analyzingPowerErrorS);
    DEVSEND(pol, "set analyzingPowerErrorS", &data, NULL, stdout, irc);

    data.insert("value", poldat_.numberEventsS);
    DEVSEND(pol, "set numberEventsS", &data, NULL, stdout, irc);

    data.insert("value", poldat_.maxTimeS);
    DEVSEND(pol, "set maxTimeS", &data, NULL, stdout, irc);

    if (irc != 0) printf("%d errors sending polarimeter data\n", irc);
}
