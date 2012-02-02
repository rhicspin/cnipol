#include "rcdev.h"
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <math.h>

#include "globals.h"
#include "rhicpol.h"
#include "rpoldata.h"

extern "C" {
    void sendresult_(int *recRing);
}

extern "C" {
    void sendsubresult_(int *recRing);
}

extern polDataStruct poldat_;

//	Common /subrun/
//struct {
//    int nofsubruns;
//    long timestamp[500];
//    float asymX[500];
//    float asymErrX[500];
//    float asymX90[500];
//    float asymErrX90[500];
//    float asymX45[500];
//    float asymErrX45[500];
//} subrun_;

extern SubRun subrun_;


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
    
    printf("Sending results to %s\n", polCDEVName[N]);
    
    cdevDevice & pol = cdevDevice::attachRef(polCDEVName[N]);
    cdevDevice & spec =  cdevDevice::attachRef(specCDEVName[N]);
    
    irc = 0;
    
    printf("In sendresult targetIdS = %s\n", poldat_.targetIdS);

    if(!DEVSEND(spec, "get fillNumberM", NULL, &data, stdout, irc))
	data.get("value", &fillNumberM);

/* Now done in rhicpol, after data file is closed. 3/19/2009
// Set the new run number in CDEV...
//	Update runId only if it really corresponds to fillNumber
    if ((int)(poldat_.runIdS) == fillNumberM) {
	 data.insert("value", poldat_.runIdS);
//	Update runIdS or emitRunIdS 
//	3/6/09 note: emittance scans are NOT analyzed by this program, so
//	the update of emitRunIdS should never be executed here.
//	 if ((((int)(10*poldat_.runIdS)) % 10) >= 4) {
//	    DEVSEND(pol, "set emitRunIdS", &data, NULL, stdout, irc);
//        printf("CDEV: set emitRunIdS %9.3f\n",poldat_.runIdS);
//	 } 
//	 else {
	    DEVSEND(pol, "set runIdS", &data, NULL, stdout, irc);
        printf("CDEV: set runIdS %9.3f\n",poldat_.runIdS);
//	 }
    }
*/
    data.insert("value", poldat_.startTimeS);
    DEVSEND(pol, "set startTimeS", &data, NULL, stdout, irc);
    printf("CDEV: set startTimeS %d\n",poldat_.startTimeS);

    data.insert("value", poldat_.stopTimeS);
    DEVSEND(pol, "set stopTimeS", &data, NULL, stdout, irc);
	printf("CDEV: set stopTimeS %d\n",poldat_.stopTimeS);
    
    data.insert("value", poldat_.daqVersionS);
    DEVSEND(pol, "set daqVersionS", &data, NULL, stdout, irc);
	printf("CDEV: set daqVersionS %s\n",poldat_.daqVersionS);

    data.insert("value", poldat_.cutIdS);
    DEVSEND(pol, "set cutIdS", &data, NULL, stdout, irc);
	printf("CDEV: set cutIdS %s\n",poldat_.cutIdS);

//    data.insert("value", poldat_.targetIdS);
//    DEVSEND(pol, "set targetIdS", &data, NULL, stdout, irc);

    data.insert("value", poldat_.encoderPositionS, 2);
    DEVSEND(pol, "set encoderPositionS", &data, NULL, stdout, irc);
	printf("CDEV: set encoderPositionS %d %d\n",poldat_.encoderPositionS[0], 
			poldat_.encoderPositionS[1]);

    data.insert("value", poldat_.statusS);
    DEVSEND(pol, "set statusS", &data, NULL, stdout, irc);
	printf("CDEV: set statusS %d\n",poldat_.statusS);
   
    strncpy(poldat_.statusStringS, stat2str(poldat_.statusS), sizeof(poldat_.statusStringS));
    data.insert("value", poldat_.statusStringS);
    DEVSEND(pol, "set statusStringS", &data, NULL, stdout, irc);
	printf("CDEV: set statusStringS %s\n",poldat_.statusStringS);
    
    data.insert("value", poldat_.totalCountsS);
    DEVSEND(pol, "set totalCountsS", &data, NULL, stdout, irc);
	printf("CDEV: set totalCountsS %d\n",poldat_.totalCountsS);
      
    data.insert("value", poldat_.upCountsS);
    DEVSEND(pol, "set upCountsS", &data, NULL, stdout, irc);
	printf("CDEV: set upCountsS %d\n",poldat_.upCountsS);
    
    data.insert("value", poldat_.downCountsS);
    DEVSEND(pol, "set downCountsS", &data, NULL, stdout, irc);
	printf("CDEV: set downCountsS %d\n",poldat_.downCountsS);
    
    data.insert("value", poldat_.unpolCountsS);
    DEVSEND(pol, "set unpolCountsS", &data, NULL, stdout, irc);
	printf("CDEV: set unpolCountsS %d\n",poldat_.unpolCountsS);
    
    data.insert("value", poldat_.countsUpLeftS, 360);
    DEVSEND(pol, "set countsUpLeftS", &data, NULL, stdout, irc);
	printf("CDEV: set countsUpLeftS \n");
    
    data.insert("value", poldat_.countsLeftS, 360);
    DEVSEND(pol, "set countsLeftS", &data, NULL, stdout, irc);
	printf("CDEV: set countsLeftS \n");
    
    data.insert("value", poldat_.countsDownLeftS, 360);
    DEVSEND(pol, "set countsDownLeftS", &data, NULL, stdout, irc);
	printf("CDEV: set countsDownLeftS \n");
    
    data.insert("value", poldat_.countsDownRightS, 360);
    DEVSEND(pol, "set countsDownRightS", &data, NULL, stdout, irc);
	printf("CDEV: set countsDownRightS \n");
    
    data.insert("value", poldat_.countsRightS, 360);
    DEVSEND(pol, "set countsRightS", &data, NULL, stdout, irc);
	printf("CDEV: set countsRightS \n");
    
    data.insert("value", poldat_.countsUpRightS, 360);
    DEVSEND(pol, "set countsUpRightS", &data, NULL, stdout, irc);
	printf("CDEV: set countsUpRightS \n");
    
    data.insert("value", poldat_.avgAsymXS);
    DEVSEND(pol, "set avgAsymXS", &data, NULL, stdout, irc);
	printf("CDEV: set avgAsymXS %f\n",poldat_.avgAsymXS);

    data.insert("value", poldat_.avgAsymX45S);
    DEVSEND(pol, "set avgAsymX45S", &data, NULL, stdout, irc);
	printf("CDEV: set avgAsymX45S %f\n",poldat_.avgAsymX45S);

    data.insert("value", poldat_.avgAsymX90S);
    DEVSEND(pol, "set avgAsymX90S", &data, NULL, stdout, irc);
	printf("CDEV: set avgAsymX90S %f\n",poldat_.avgAsymX90S);
    
    data.insert("value", poldat_.avgAsymYS);
    DEVSEND(pol, "set avgAsymYS", &data, NULL, stdout, irc);
	printf("CDEV: set avgAsymYS %f\n",poldat_.avgAsymYS);
    
    data.insert("value", poldat_.avgAsymErrorXS);
    DEVSEND(pol, "set avgAsymErrorXS", &data, NULL, stdout, irc);
	printf("CDEV: set avgAsymErrorXS %f\n",poldat_.avgAsymErrorXS);
    
    data.insert("value", poldat_.avgAsymErrorX45S);
    DEVSEND(pol, "set avgAsymErrorX45S", &data, NULL, stdout, irc);
	printf("CDEV: set avgAsymErrorX45S %f\n",poldat_.avgAsymErrorX45S);
    
    data.insert("value", poldat_.avgAsymErrorX90S);
    DEVSEND(pol, "set avgAsymErrorX90S", &data, NULL, stdout, irc);
	printf("CDEV: set avgAsymErrorX90S %f\n",poldat_.avgAsymErrorX90S);
    
    data.insert("value", poldat_.avgAsymErrorYS);
    DEVSEND(pol, "set avgAsymErrorYS", &data, NULL, stdout, irc);
	printf("CDEV: set avgAsymErrorYS %f\n",poldat_.avgAsymErrorYS);
    
    data.insert("value", poldat_.bunchAsymXS, 360);
    DEVSEND(pol, "set bunchAsymXS", &data, NULL, stdout, irc);
	printf("CDEV: set bunchAsymXS \n");
    
    data.insert("value", poldat_.bunchAsymYS, 360);
    DEVSEND(pol, "set bunchAsymYS", &data, NULL, stdout, irc);
	printf("CDEV: set bunchAsymYS \n");
    
    data.insert("value", poldat_.bunchAsymErrorXS, 360);
    DEVSEND(pol, "set bunchAsymErrorXS", &data, NULL, stdout, irc);
	printf("CDEV: set bunchAsymErrorXS \n");
    
    data.insert("value", poldat_.bunchAsymErrorYS, 360);
    DEVSEND(pol, "set bunchAsymErrorYS", &data, NULL, stdout, irc);
	printf("CDEV: set bunchAsymErrorYS \n");

    data.insert("value", poldat_.beamEnergyS);
    DEVSEND(pol, "set beamEnergyS", &data, NULL, stdout, irc);
	printf("CDEV: set beamEnergyS %f\n",poldat_.beamEnergyS);
    
    data.insert("value", poldat_.analyzingPowerS);
    DEVSEND(pol, "set analyzingPowerS", &data, NULL, stdout, irc);
	printf("CDEV: set analyzingPowerS %f\n",poldat_.analyzingPowerS);
    
    data.insert("value", poldat_.analyzingPowerErrorS);
    DEVSEND(pol, "set analyzingPowerErrorS", &data, NULL, stdout, irc);
	printf("CDEV: set analyzingPowerErrorS %f\n",poldat_.analyzingPowerErrorS);

    data.insert("value", poldat_.numberEventsS);
    DEVSEND(pol, "set numberEventsS", &data, NULL, stdout, irc);
	printf("CDEV: set numberEventsS %d\n",poldat_.numberEventsS);

    data.insert("value", poldat_.maxTimeS);
    DEVSEND(pol, "set maxTimeS", &data, NULL, stdout, irc);
	printf("CDEV: set maxTimeS %d\n",poldat_.maxTimeS);

    if (irc != 0) printf("RHIC2HBOOK : %d errors sending polarimeter data\n", irc);
}


void sendsubresult_(int *recRing) {
    char polCDEVName[2][20] = {"polarimeter.yel", "polarimeter.blu"};
    int N,i;
    int irc;
    cdevData data;

    N = -1;
    if (*recRing == REC_YELLOW) N = 0;
    if (*recRing == REC_BLUE) N = 1;
    if (N < 0) {
	printf("Unknown polarimeter (%X): no result sent to ADO!\n", *recRing);
	return;
    }
    
    cdevDevice & pol = cdevDevice::attachRef(polCDEVName[N]);
    
    irc = 0;

    if (subrun_.nofsubruns != 0) 
	printf("Sending ramp polarization arrays for %d subruns to %s\n", subrun_.nofsubruns, polCDEVName[N]);
    else {
	printf("No subruns to send to %s\n", polCDEVName[N]);
	return;
    }

    data.insert("value", poldat_.statusS);
    DEVSEND(pol, "set statusS", &data, NULL, stdout, irc);
    
    strncpy(poldat_.statusStringS, stat2str(poldat_.statusS), sizeof(poldat_.statusStringS));
    data.insert("value", poldat_.statusStringS);
    DEVSEND(pol, "set statusStringS", &data, NULL, stdout, irc);
    
    // Check all arrays for 'nan'/'inf' before sending
    for (i=0; i<subrun_.nofsubruns; i++) {
	if (isnanf(subrun_.asymX[i])      || isinff(subrun_.asymX[i]))      subrun_.asymX[i]      = 0.;
	if (isnanf(subrun_.asymErrX[i])   || isinff(subrun_.asymErrX[i]))   subrun_.asymErrX[i]   = 0.;
	if (isnanf(subrun_.asymX45[i])    || isinff(subrun_.asymX45[i]))    subrun_.asymX45[i]    = 0.;
	if (isnanf(subrun_.asymErrX45[i]) || isinff(subrun_.asymErrX45[i])) subrun_.asymErrX45[i] = 0.;
	if (isnanf(subrun_.asymX90[i])    || isinff(subrun_.asymX90[i]))    subrun_.asymX90[i]    = 0.;
	if (isnanf(subrun_.asymErrX90[i]) || isinff(subrun_.asymErrX90[i])) subrun_.asymErrX90[i] = 0.;
    }
    
    data.insert("value", subrun_.asymX, subrun_.nofsubruns);
    DEVSEND(pol, "set asymXS", &data, NULL, stdout, irc);
    
    data.insert("value", subrun_.asymX45, subrun_.nofsubruns);
    DEVSEND(pol, "set asymX45S", &data, NULL, stdout, irc);

    data.insert("value", subrun_.asymX90, subrun_.nofsubruns);
    DEVSEND(pol, "set asymX90S", &data, NULL, stdout, irc);
    
    data.insert("value", subrun_.asymErrX, subrun_.nofsubruns);
    DEVSEND(pol, "set asymErrorXS", &data, NULL, stdout, irc);
    
    data.insert("value", subrun_.asymErrX45, subrun_.nofsubruns);
    DEVSEND(pol, "set asymErrorX45S", &data, NULL, stdout, irc);
    
    data.insert("value", subrun_.asymErrX90, subrun_.nofsubruns);
    DEVSEND(pol, "set asymErrorX90S", &data, NULL, stdout, irc);
    
    data.insert("value", subrun_.timestamp, subrun_.nofsubruns);
    DEVSEND(pol, "set asymXTsS", &data, NULL, stdout, irc);

    if (irc != 0) printf("%d errors sending polarimeter data\n", irc);
}

