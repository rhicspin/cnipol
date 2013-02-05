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

extern "C" {
   void sendresult_(char *device, int len);
}

extern "C" {
   void sendsubresult_(char *device, int len);
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


void sendresult_(char *device, int len)
{
   int irc;
   cdevData data;
   char *name;

   name = (char *)malloc(len + 1);
   memcpy(name, device, len);
   name[len] = '\0';

   printf("Sending results to %s\n", name);
   cdevDevice & pol = cdevDevice::attachRef(name);

   irc = 0;

   data.insert("value", poldat_.startTimeS);
   DEVSEND(pol, "set startTimeS", &data, NULL, stdout, irc);

   data.insert("value", poldat_.stopTimeS);
   DEVSEND(pol, "set stopTimeS", &data, NULL, stdout, irc);

   data.insert("value", poldat_.daqVersionS);
   DEVSEND(pol, "set daqVersionS", &data, NULL, stdout, irc);

   data.insert("value", poldat_.cutIdS);
   DEVSEND(pol, "set cutIdS", &data, NULL, stdout, irc);

   data.insert("value", poldat_.statusS);
   DEVSEND(pol, "set statusS", &data, NULL, stdout, irc);

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

   if (irc != 0) printf("RHIC2HBOOK : %d errors sending polarimeter data\n", irc);
}


void sendsubresult_(char *device, int len)
{
   int i;
   int irc;
   cdevData data;
   char *name;

   name = (char *)malloc(len + 1);
   memcpy(name, device, len);
   name[len] = '\0';

   cdevDevice & pol = cdevDevice::attachRef(name);

   irc = 0;

   if (subrun_.nofsubruns != 0)
      printf("Sending ramp polarization arrays for %d subruns to %s\n", subrun_.nofsubruns, name);
   else {
      printf("No subruns to send to %s\n", name);
      return;
   }

   data.insert("value", poldat_.statusS);
   DEVSEND(pol, "set statusS", &data, NULL, stdout, irc);

   // Check all arrays for 'nan'/'inf' before sending
   for (i = 0; i < subrun_.nofsubruns; i++) {
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
