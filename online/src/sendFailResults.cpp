#include "rcdev.h"
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <math.h>
#include "rhicpol.h"
#include "rpoldata.h"

int main (int argc, char *argv[])
{
 int N;
 int irc;
 int fillNumberM = 0;
 float runno;
 long cnts[360];
 float basym[360];
 
 char polCDEVName[2][20] = {"polarimeter.yel", "polarimeter.blu"};
 char specCDEVName[2][20] = {"ringSpec.yellow", "ringSpec.blue"};
 
 cdevData data;

 memset(&cnts, 0, sizeof(cnts));
 memset(&basym, 0, sizeof(basym));
 
// Get the ring "y"=0, "b"=1 and run number...     
 N = 0;
 if (strncmp(argv[1],"b",1) == 0) N = 1;
 runno = atof(argv[2]);
 
//Define the Cdev devices...
 cdevDevice & pol = cdevDevice::attachRef(polCDEVName[N]);
 cdevDevice & spec =  cdevDevice::attachRef(specCDEVName[N]);
 
 if(!DEVSEND(spec, "get fillNumberM", NULL, &data, stdout, irc))
 data.get("value", &fillNumberM);

// Set the new run number in CDEV...
//	Update runId only if it really corresponds to the current fillNumber
 if ((int)(runno) == fillNumberM) {
  data.insert("value", runno);
  if ((((int)(10*runno)) % 10) >= 4) {
   DEVSEND(pol, "set emitRunIdS", &data, NULL, stdout, irc);
  } 
  else {
   DEVSEND(pol, "set runIdS", &data, NULL, stdout, irc);
  }
 }

 data.insert("value", time(NULL));
 DEVSEND(pol, "set startTimeS", &data, NULL, stdout, irc);

 data.insert("value", time(NULL));
 DEVSEND(pol, "set stopTimeS", &data, NULL, stdout, irc);
    
 data.insert("value", "unknown");
 DEVSEND(pol, "set daqVersionS", &data, NULL, stdout, irc);

 data.insert("value", 0x8000);
 DEVSEND(pol, "set statusS", &data, NULL, stdout, irc);

 data.insert("value", "E-ANAF Analysis Failed");
 DEVSEND(pol, "set statusStringS", &data, NULL, stdout, irc);
    
 data.insert("value", 0);
 DEVSEND(pol, "set totalCountsS", &data, NULL, stdout, irc);
      
 data.insert("value", 0);
 DEVSEND(pol, "set upCountsS", &data, NULL, stdout, irc);
    
 data.insert("value", 0);
 DEVSEND(pol, "set downCountsS", &data, NULL, stdout, irc);
    
 data.insert("value", 0);
 DEVSEND(pol, "set unpolCountsS", &data, NULL, stdout, irc);



 data.insert("value", cnts, 360);
 DEVSEND(pol, "set countsUpLeftS", &data, NULL, stdout, irc);
   
 data.insert("value", cnts, 360);
 DEVSEND(pol, "set countsLeftS", &data, NULL, stdout, irc);
    
 data.insert("value", cnts, 360);
 DEVSEND(pol, "set countsDownLeftS", &data, NULL, stdout, irc);

 data.insert("value", cnts, 360);
 DEVSEND(pol, "set countsDownRightS", &data, NULL, stdout, irc);

 data.insert("value", cnts, 360);
 DEVSEND(pol, "set countsRightS", &data, NULL, stdout, irc);

 data.insert("value", cnts, 360);
 DEVSEND(pol, "set countsUpRightS", &data, NULL, stdout, irc);
    
 data.insert("value", 0.);
 DEVSEND(pol, "set avgAsymXS", &data, NULL, stdout, irc);

 data.insert("value", 0.);
 DEVSEND(pol, "set avgAsymX45S", &data, NULL, stdout, irc);

 data.insert("value", 0.);
 DEVSEND(pol, "set avgAsymX90S", &data, NULL, stdout, irc);
    
 data.insert("value", 0.);
 DEVSEND(pol, "set avgAsymYS", &data, NULL, stdout, irc);
    
 data.insert("value", 0.);
 DEVSEND(pol, "set avgAsymErrorXS", &data, NULL, stdout, irc);
    
 data.insert("value", 0.);
 DEVSEND(pol, "set avgAsymErrorX45S", &data, NULL, stdout, irc);
    
 data.insert("value", 0.);
 DEVSEND(pol, "set avgAsymErrorX90S", &data, NULL, stdout, irc);
	
 data.insert("value", 0.);
 DEVSEND(pol, "set avgAsymErrorYS", &data, NULL, stdout, irc);
 
 data.insert("value", basym, 360);
 DEVSEND(pol, "set bunchAsymXS", &data, NULL, stdout, irc);

 data.insert("value", basym, 360);
 DEVSEND(pol, "set bunchAsymYS", &data, NULL, stdout, irc);

 data.insert("value", basym, 360);
 DEVSEND(pol, "set bunchAsymErrorXS", &data, NULL, stdout, irc);

 data.insert("value", basym, 360);
 DEVSEND(pol, "set bunchAsymErrorYS", &data, NULL, stdout, irc);

// Skipped beamEnergyS

 data.insert("value", 0.);
 DEVSEND(pol, "set analyzingPowerS", &data, NULL, stdout, irc);
    
 data.insert("value", 0.);
 DEVSEND(pol, "set analyzingPowerErrorS", &data, NULL, stdout, irc);

 data.insert("value", 0);
 DEVSEND(pol, "set numberEventsS", &data, NULL, stdout, irc);

 data.insert("value", 0);
 DEVSEND(pol, "set maxTimeS", &data, NULL, stdout, irc);

 if (irc != 0) printf("SENDFAILRESULTS : errors sending polarimeter data\n");
}

