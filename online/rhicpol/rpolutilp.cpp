#include <fstream>
#include <ctype.h>
#include <string.h>

#include "rcdev.h"
#include "rpoldata.h"
#include "rpolutil.h"

extern FILE *LogFile;
extern beamDataStruct beamData;
extern beamDataStruct beamOtherData;
extern polDataStruct polData;
extern jetPositionStruct jetPosition;
extern V124Struct V124;                 // V124 settings
extern int iDebug;
extern float mTime;
extern int mEvent;
extern char DeviceName[128];    // our CDEV name
extern int nLoop;       // Number of subruns
extern int iRamp;       // going to be ramp


// Get HJET valves status
int getJetBits(void)
{
   cdevData data;
   char msg[20];
   int irc, i, j;
   char jetBitsName[2][40] = {"iobitText.12a-hjet.11", "iobitText.12a-hjet.15"};

   if (!gUseCdev) return 0;
   j = 0;
   irc  = 0;
   for (i=0; i<2; i++) {
       cdevDevice & dev = cdevDevice::attachRef(jetBitsName[i]);
       if(!DEVSEND(dev, "get outBitsText", NULL, &data, LogFile, irc))
           data.get("value", msg, sizeof(msg));        // msg = "on"/"off"
       if (tolower(msg[1]) == 'n') j |= 1;
       j <<= 1;
   }
   return j;
}


// Get BPMs around Jet position
void getJetPosition(void)
{
    cdevData data;
    int irc, i;

    char rbpmName[8][20] = {"rbpm.b-g11-bhx", "rbpm.b-g12-bhx", "rbpm.b-g12-bvx",
        "rbpm.b-g11-bvx", "rbpm.y-g11-bhx", "rbpm.y-g12-bhx", "rbpm.y-g11-bvx", "rbpm.y-g12-bvx"};

    if (!gUseCdev) return;
    irc = 0;
    cdevDevice & dev = cdevDevice::attachRef("stepper.12a-hjet.A.U");

    if(!DEVSEND(dev, "get absolutePosition", NULL, &data, LogFile, irc)) data.get("value", &jetPosition.jetAbsolutePosition);

    for (i=0; i<8; i++) {
        cdevDevice & rbpm = cdevDevice::attachRef(rbpmName[i]);
        if(!DEVSEND(rbpm, "get avgOrbPositionM", NULL, &data, LogFile, irc))
            data.get("value", &jetPosition.rbpm[i]);
    }
}


// Get active target movement
int getTargetMovementInfo(long **res)
{
    int irc;
    cdevData data;
    size_t len = 0;
    *res = NULL;

    if (!gUseCdev) return 0;
    // determine the ring
    if (iDebug > 1000) fprintf(LogFile, "RHICPOL-INFO : AdoInfo - %s get encPosProfileS\n", DeviceName);

    //  getting the data
    cdevDevice & pol =  cdevDevice::attachRef(DeviceName);
    irc = 0;

    if(!DEVSEND(pol, "get encPosProfileS", NULL, &data, LogFile, irc)) {
        data.getElems("value", &len);
        *res = (long *) malloc(len*sizeof(long));
        data.get("value", *res);
    } else {
        return 0;
    }

    return len;
}


// Check if we have any carbon target in the beam (for HJET)
void getCarbTarg(carbTargStat * targstat)
{
    char targetCDEVName[8][20] = {"pol.y-htarget", "pol.y-vtarget", "pol.b-htarget", "pol.b-vtarget",
                                  "pol2.y-htarget", "pol2.y-vtarget", "pol2.b-htarget", "pol2.b-vtarget"};
    cdevData data;
    int irc;
    int i;

    if (!gUseCdev) {
        targstat->good = 1;
        for (i=0; i<8; i++) strcpy(targstat->carbtarg[i],"Park");
        return ;
    }

    if (iDebug > 1000) fprintf(LogFile, "RHICPOL-INFO : GetCarbTarget\n");

    //  create devices and get info
    irc = 0;

    for (i=0; i<8; i++) {
        cdevDevice & target = cdevDevice::attachRef(targetCDEVName[i]);
        if(!DEVSEND(target, "get positionS", NULL, &data, LogFile, irc))
            data.get("value", targstat->carbtarg[i], sizeof(targstat->carbtarg[i]));
    }

    targstat->good = 1;

    if (irc != 0) {
        fprintf(LogFile, "%d errors getting RHIC information.\n", irc);
        polData.statusS |= (STATUS_ERROR | ERR_NOADO);
        return;
    }

    //  Check positions (in beam positions are named "Target1" ... "Target6")
    for (i=0; i<8; i++) if (strlen(targstat->carbtarg[i]) == 7 && strncasecmp(targstat->carbtarg[i], "Target", 6) == 0) break;

    if (i<8) targstat->good = 0;
}


/** Get wall current monitor data */
void getWcmInfo(void)
{
    cdevData data;

    if (!gUseCdev) return;

    //  determine the ring
    int N = 1;
    if (recRing & REC_BLUE) N = 0;
    if (iDebug > 1000) fprintf(LogFile, "RHICPOL-INFO : WcmInfo - %s\n", wcmCDEVName[N]);

    cdevDevice & wcm =  cdevDevice::attachRef(wcmCDEVName[N]);
    int irc = 0;

    //  wcm
    if(!DEVSEND(wcm, "get fillDataM", NULL, &data, LogFile, irc))
        data.get("value", wcmData.fillDataM);

    if(!DEVSEND(wcm, "get wcmBeamM", NULL, &data, LogFile, irc))
        data.get("value", &wcmData.wcmBeamM);

    if (recRing & REC_JET) { // Get also info for the other beam in jet mode

        if (iDebug > 1000) fprintf(LogFile, "RHICPOL-INFO : WcmInfo - %s\n", wcmCDEVName[1-N]);
        cdevDevice & wcmo =  cdevDevice::attachRef(wcmCDEVName[1-N]);

        // wcm
        if(!DEVSEND(wcmo, "get fillDataM", NULL, &data, LogFile, irc))
            data.get("value", wcmOtherData.fillDataM);

        if(!DEVSEND(wcmo, "get wcmBeamM", NULL, &data, LogFile, irc))
            data.get("value", &wcmOtherData.wcmBeamM);
    }

    if (irc != 0) {
        fprintf(LogFile, "%d errors getting RHIC information.\n", irc);
        polData.statusS |= (STATUS_ERROR | ERR_NOADO);
    }
}


/** Get measurement type from CDEV */
EMeasType getCDEVMeasType()
{
   if (!gUseCdev) return kMEASTYPE_UNKNOWN;

   // determine the ring
   if (iDebug > 1000) fprintf(LogFile, "RHICPOL-INFO : getCDEVMeasType() - %s get dataAcquisitionType\n", DeviceName);

   // getting the data
   cdevDevice &pol =  cdevDevice::attachRef(DeviceName);

   int irc = 0;
   cdevData data;
   std::string sMeasType(64, ' ');

   if (!DEVSEND(pol, "get dataAcquisitionType", NULL, &data, LogFile, irc)) {
       data.get("value", &sMeasType[0], sMeasType.size());
   } else {
       return kMEASTYPE_UNKNOWN;
   }

   if (sMeasType.find("Profile Sweep")  != std::string::npos) return kMEASTYPE_SWEEP;
   if (sMeasType.find("Fixed")          != std::string::npos) return kMEASTYPE_FIXED;
   if (sMeasType.find("Target Scan")    != std::string::npos) return kMEASTYPE_TARGET_SCAN;
   if (sMeasType.find("Emittance Scan") != std::string::npos) return kMEASTYPE_EMIT_SCAN;

   return kMEASTYPE_UNKNOWN;
}


/** */
void getCdevInfoBeamData(beamDataStruct *bds)
{
   int irc = 0;
   int N   = 1;

   // Determine the ring (my ring <-> other ring...
   if (recRing & REC_BLUE) N = 0;

   cdevData    data;
   cdevDevice &spec = cdevDevice::attachRef(specCDEVName[N]);

   if(!DEVSEND(spec, "get beamEnergyM", NULL, &data, LogFile, irc))
      data.get("value", &(bds->beamEnergyM));
}

/* */
void getCdevInfoTargetParams(cdevDevice &polDev)
{
   cdevData data;
   int irc = 0;

   // target name in targetIdS - this is our target !
   if (!DEVSEND(polDev, "get targetIdS", NULL, &data, LogFile, irc))
      data.get("value", polData.targetIdS, sizeof(polData.targetIdS));

   // translate from polarimeter.blu2 etc to pol2.b-vtarget etc
   sprintf(ourTargetCDEVName, "pol%s.%c-%ctarget", (DeviceName[15] == '2') ? "2" : "", DeviceName[12], tolower(polData.targetIdS[0]));

   if ( !DEVSEND(polDev, "get velocityM", NULL, &data, LogFile, irc))
      data.get("value", &gRecordTargetParams.fVelocity);

   fprintf(LogFile, "RHICPOL-INFO : gRecordTargetParams.fVelocity             : %10d\n", gRecordTargetParams.fVelocity);

   if ( !DEVSEND(polDev, "get profileStartPositionM", NULL, &data, LogFile, irc))
      data.get("value", &gRecordTargetParams.fProfileStartPosition);

   fprintf(LogFile, "RHICPOL-INFO : gRecordTargetParams.fProfileStartPosition : %10d\n", gRecordTargetParams.fProfileStartPosition);

   if ( !DEVSEND(polDev, "get profileEndPositionM", NULL, &data, LogFile, irc))
      data.get("value", &gRecordTargetParams.fProfileEndPosition);

   fprintf(LogFile, "RHICPOL-INFO : gRecordTargetParams.fProfileEndPosition   : %10d\n", gRecordTargetParams.fProfileEndPosition);

   if ( !DEVSEND(polDev, "get profilePeakPositionM", NULL, &data, LogFile, irc))
      data.get("value", &gRecordTargetParams.fProfilePeakPosition);

   fprintf(LogFile, "RHICPOL-INFO : gRecordTargetParams.fProfilePeakPosition  : %10d\n", gRecordTargetParams.fProfilePeakPosition);
}


/** */
void getCdevInfoMachineParams()
{
   int irc = 0;
   cdevData data;

   int voltages[1000];

   for (uint16_t iRing=0; iRing<N_BEAMS; iRing++)
   {
      if (iRing == 0)      fprintf(LogFile, "RHICPOL-INFO : Blue ring\n");
      else if (iRing == 1) fprintf(LogFile, "RHICPOL-INFO : Yellow ring\n");
      else                 fprintf(LogFile, "RHICPOL-ERR  : Unknown ring\n");

      // 197MHz cavity voltage
      cdevDevice &cavityDevice = cdevDevice::attachRef(gCavityCdevNames[iRing]);

      if ( !DEVSEND(cavityDevice, "get probeMagInVoltsScaledM", NULL, &data, LogFile, irc))
      {
         data.get("value", voltages);
      }

      gRecordMachineParams.fCavity197MHzVoltage[iRing] = voltages[0]; // was [1]. why?
      fprintf(LogFile,"RHICPOL-INFO : 197MHZ Cavity: gRecordMachineParams.fCavity197MHzVoltage[%d]     = %d\n",
         iRing, gRecordMachineParams.fCavity197MHzVoltage[iRing]);

      // Snakes
      cdevDevice &snakeDevice = cdevDevice::attachRef(gSnakeCdevNames[iRing]);
      if ( !DEVSEND(snakeDevice, "get dataBarM", NULL, &data, LogFile, irc))
         data.get("value", &gRecordMachineParams.fSnakeCurrents[iRing]);

      fprintf(LogFile,"RHICPOL-INFO : Snake: gRecordMachineParams.fSnakeCurrents[%d]                   = %10.4f\n",
         iRing, gRecordMachineParams.fSnakeCurrents[iRing]);

      // Rotators: STAR
      cdevDevice &starRotDevice = cdevDevice::attachRef(gStarRotatorCdevNames[iRing]);
      if ( !DEVSEND(starRotDevice, "get dataBarM", NULL, &data, LogFile, irc))
         data.get("value", &gRecordMachineParams.fStarRotatorCurrents[iRing]);

      fprintf(LogFile,"RHICPOL-INFO : STAR Rotators:   gRecordMachineParams.fStarRotatorCurrents[%d]   = %10.4f\n",
         iRing, gRecordMachineParams.fStarRotatorCurrents[iRing]);

      // Rotators: Phenix
      cdevDevice &phenixRotDevice = cdevDevice::attachRef(gPhenixRotatorCdevNames[iRing]);
      if ( !DEVSEND(phenixRotDevice, "get dataBarM", NULL, &data, LogFile, irc))
         data.get("value", &gRecordMachineParams.fPhenixRotatorCurrents[iRing]);

      fprintf(LogFile,"RHICPOL-INFO : Phenix Rotators: gRecordMachineParams.fPhenixRotatorCurrents[%d] = %10.4f\n",
         iRing, gRecordMachineParams.fPhenixRotatorCurrents[iRing]);
   }
}


// Get most of CDEV data at the beginnig of the run
void getCdevInfo()
{
   // gather CDEV information we could need
   char defName[128];
   cdevData data;
   int ival[6];

   if (!gUseCdev) return;

   if (iDebug > 1000) fprintf(LogFile, "RHICPOL-INFO : CDEV Info - %s\n", DeviceName);

   int irc = 0;

   // Get polarimeter information if DeviceName != "None"
   if (DeviceName[0] != 'N')
   {
      cdevDevice &pol = cdevDevice::attachRef(DeviceName);

      getCdevInfoTargetParams(pol);

      // Get time of measurement if it was not set in the command line
      if (mTime < 0) {
         if (iRamp) {
            if (!DEVSEND(pol, "get rampMeasTimeS", NULL, &data, LogFile, irc)) data.get("value", &mTime);
         } else {
            if (!DEVSEND(pol, "get maxTimeToRunS", NULL, &data, LogFile, irc)) data.get("value", &mTime);
         }
      }

      // Get number of events if it was not set in the command line
      if (mEvent < 0) {
         if (iRamp) {
            mEvent = 2000000000;        // nearly max 32 bit signed
         } else {
            if (!DEVSEND(pol, "get numberEventsToDoS", NULL, &data, LogFile, irc)) data.get("value", &mEvent);
         }
      }

      if (iRamp != 0 && nLoop == 1) {
         int tgtId = polData.targetIdS[strlen(polData.targetIdS) - 1] - '1';    // target number

         if (tgtId < 0 || tgtId > 5) {
            fprintf(LogFile, "Unexpected target name %s.\n", polData.targetIdS);
            polData.statusS |= (WARN_INT);
            tgtId = 0;
         }

         memset(ival, 0, sizeof(ival));
         sprintf(defName, "polarControlDefaults%s", &DeviceName[11]);
         cdevDevice & def = cdevDevice::attachRef(defName);
         if (!DEVSEND(def, "get rampInterval", NULL, &data, LogFile, irc)) data.get("value", ival);
         if (ival[tgtId] < 0) ival[tgtId] = 2;  // just default
         nLoop = (int) (mTime / ival[tgtId]);
      }
   }

   // Get info from other devices
   getCdevInfoMachineParams();

   // Determine the ring (my ring <-> other ring...
   int ringId = 1;                      // yellow
   if (recRing & REC_BLUE) ringId = 0;  // blue

   // ringSpec
   cdevDevice &spec = cdevDevice::attachRef(specCDEVName[ringId]);

   if (!DEVSEND(spec, "get beamEnergyM", NULL, &data, LogFile, irc))
       data.get("value", &beamData.beamEnergyM);

   if (!DEVSEND(spec, "get fillNumberM", NULL, &data, LogFile, irc))
       data.get("value", &beamData.fillNumberM);

   // buckets Cdev
   cdevDevice &bucketsCdev = cdevDevice::attachRef(bucketsCDEVName[ringId]);

   //if(!DEVSEND(bucketsCdev, "get intendedFillPatternS", NULL, &data, LogFile, irc))   -- uncomment for tests
   if (!DEVSEND(bucketsCdev, "get measuredFillPatternM", NULL, &data, LogFile, irc))
      data.get("value", beamData.measuredFillPatternM);

   if (!DEVSEND(bucketsCdev, "get polarizationFillPatternS", NULL, &data, LogFile, irc)) {
      data.get("value", beamData.polarizationFillPatternS);

      // It looks they changed type to char ...
      for (int i=0; i<360; i++) {
         if (beamData.polarizationFillPatternS[i] > 127) beamData.polarizationFillPatternS[i] -= 256;
      }
   }

   if (!DEVSEND(bucketsCdev, "get bunchLengthS", NULL, &data, LogFile, irc))
      data.get("value", &beamData.bunchLengthS);

   fprintf(LogFile, "RHICPOL-INFO : beamData.bunchLengthS = %10.4f\n", beamData.bunchLengthS);

   // wcm
   cdevDevice &wcm = cdevDevice::attachRef(wcmCDEVName[ringId]);

   if (!DEVSEND(wcm, "get fillDataM", NULL, &data, LogFile, irc)) data.get("value", wcmData.fillDataM);
   if (!DEVSEND(wcm, "get wcmBeamM",  NULL, &data, LogFile, irc)) data.get("value", &wcmData.wcmBeamM);

   if (!DEVSEND(wcm, "get fillDataM",      NULL, &data, LogFile, irc)) data.get("value",  gRecordWcm.fFillDataM);
   if (!DEVSEND(wcm, "get wcmBeamM",       NULL, &data, LogFile, irc)) data.get("value", &gRecordWcm.fWcmBeamM);
   if (!DEVSEND(wcm, "get bunchDataM",     NULL, &data, LogFile, irc)) data.get("value",  gRecordWcm.fBunchDataM);
   if (!DEVSEND(wcm, "get bunchLengthM",   NULL, &data, LogFile, irc)) data.get("value", &gRecordWcm.fBunchLengthM);
   if (!DEVSEND(wcm, "get profileHeaderM", NULL, &data, LogFile, irc)) data.get("value",  gRecordWcm.fProfileHeaderM);


   // the other ring for jet mode
   if (recRing & REC_JET)
   {
      cdevDevice &speco    = cdevDevice::attachRef(specCDEVName[1-ringId]);
      cdevDevice &bucketso = cdevDevice::attachRef(bucketsCDEVName[1-ringId]);
      cdevDevice &wcmo     = cdevDevice::attachRef(wcmCDEVName[1-ringId]);

      // ringSpecOther
      if(!DEVSEND(speco, "get beamEnergyM", NULL, &data, LogFile, irc))
          data.get("value", &beamOtherData.beamEnergyM);

      if(!DEVSEND(speco, "get fillNumberM", NULL, &data, LogFile, irc))
          data.get("value", &beamOtherData.fillNumberM);

      // bucketsOther
      if(!DEVSEND(bucketso, "get measuredFillPatternM", NULL, &data, LogFile, irc))
          data.get("value", beamOtherData.measuredFillPatternM);

      if(!DEVSEND(bucketso, "get polarizationFillPatternS", NULL, &data, LogFile, irc)) {
          data.get("value", beamOtherData.polarizationFillPatternS);

      // It looks that they had changed type to char ...
      for (int i=0; i<360; i++) if (beamOtherData.polarizationFillPatternS[i] > 127)
          beamOtherData.polarizationFillPatternS[i] -= 256;
      }

      // wcmOther
      if(!DEVSEND(wcmo, "get fillDataM", NULL, &data, LogFile, irc))
           data.get("value", wcmOtherData.fillDataM);

      if(!DEVSEND(wcmo, "get wcmBeamM", NULL, &data, LogFile, irc))
           data.get("value", &wcmOtherData.wcmBeamM);
   }

   if (irc != 0) {
       fprintf(LogFile, "%d errors getting RHIC information.\n", irc);
       polData.statusS |= (STATUS_ERROR | ERR_NOADO);
   }
}


// We look how our carbon target is moving
void GetTargetEncodings(long *res)
{
    int irc;
    cdevData data;

    if (!gUseCdev) return;

    cdevDevice & target = cdevDevice::attachRef(ourTargetCDEVName);
    irc = 0;

    if(!DEVSEND(target, "get positionEncM", NULL, &data, LogFile, irc))
        data.get("value", res);

    res[2] = res[0];    // do just a copy for compatibility ...
    res[3] = res[1];

    if (irc != 0) {
       fprintf(LogFile, "%d errors getting target information.\n", irc);
       polData.statusS |= (STATUS_ERROR | ERR_NOADO);
    }
}


//      Send our progress to operator...
void UpdateProgress(int evDone, int rate, double ts)
{
    static int StopUpdate=0;
    int irc;
    cdevData data;

    if (!gUseCdev) return;
    if (StopUpdate) return;

    if (iDebug > 1000) fprintf(LogFile, "RHICPOL-INFO : UpdateProgress - %s\n", DeviceName);
    cdevDevice & pol = cdevDevice::attachRef(DeviceName);

    irc = 0;
    data = evDone;
    DEVSEND(pol, "set progressS", &data, NULL, LogFile, irc);
    data.addTag("timeStamp");
    data.insert("value", rate);
    data.insert("timeStamp", ts);
    DEVSEND(pol, "set countRateS", &data, NULL, LogFile, irc);

    if (irc != 0) {
        fprintf(LogFile, "Error updating progress.\n");
        polData.statusS |= (WARN_INT);
        StopUpdate = 1;
    }

}


//      Update status
void UpdateStatus(void)
{
    cdevData data;

    if (iDebug > 1000) fprintf(LogFile, "RHICPOL-INFO : UpdateStatus - %s\n", DeviceName);

    cdevDevice &pol  = cdevDevice::attachRef(DeviceName);
    data = polData.statusS;
    int irc = 0;

    DEVSEND(pol, "set statusS", &data, NULL, LogFile, irc);

    if (irc != 0) {
        fprintf(LogFile, "Error updating status.\n");
        polData.statusS |= (WARN_INT);
    }
}


// Update message
void UpdateMessage(char * msg)
{
    int irc;
    cdevData data;

    if (iDebug > 1000) fprintf(LogFile, "RHICPOL-INFO : UpdateMessage - %s, %s\n", DeviceName, msg);

    cdevDevice & pol = cdevDevice::attachRef(DeviceName);

    irc = 0;
    data.insert("value", msg);
    DEVSEND(pol, "set statusStringS", &data, NULL, LogFile, irc);

    if (irc != 0) {
        fprintf(LogFile, "RHICPOL-WARN : Error updating message.\n");
        polData.statusS |= (WARN_INT);
    }
}


// Program V124 - ring = 0 - yellow, 1 - blue
void ProgV124(int ring)
{
    char name[128];
    int irc;
    int i, j;
    unsigned char buckets[360];
    cdevData data;

    irc = 0;

    // Global setting
    if (V124.flags & 0x8000) {
        sprintf(name, "bsTrigMod.12a-polar1.%c", (ring) ? 'D' : 'C');
        cdevDevice & dev = cdevDevice::attachRef(name);
        data.insert("value", V124.positionDelay);
        DEVSEND(dev, "set positionDelay", &data, NULL, LogFile, irc);
        if (iDebug > 1000) fprintf(LogFile, "RHICPOL-INFO : Set V124 - %s:positionDelay\n", name);
    }

    //  Channel settings
    for (i=0; i<8; i++) if (V124.flags & (1 << i)) {
        sprintf(name, "bsTrigChan.12a-polar1.%c.%d", (ring) ? 'D' : 'C', i+1);
        cdevDevice & dev = cdevDevice::attachRef(name);
        data.insert("value", V124.chan[i].fineDelay);
        DEVSEND(dev, "set fineDelayCounter", &data, NULL, LogFile, irc);
        data.insert("value", V124.chan[i].pulseWidth);
        DEVSEND(dev, "set pulseWidthCounter", &data, NULL, LogFile, irc);
        data.insert("value", V124.chan[i].bucketOffset);
        DEVSEND(dev, "set bucketOffsetS", &data, NULL, LogFile, irc);
        memset(buckets, 0, sizeof(buckets));
        if (V124.chan[i].bucketPeriod) for (j=0; j*V124.chan[i].bucketPeriod < 360; j++) buckets[j*V124.chan[i].bucketPeriod] = 1;
        data.insert("value", buckets, 360);
        DEVSEND(dev, "set buckets", &data, NULL, LogFile, irc);
        if (iDebug > 1000) fprintf(LogFile, "RHICPOL-INFO : Set V124.chan%d - %s\n", i+1, name);
    }
    if (irc != 0) {
        fprintf(LogFile, "RHICPOL-WARN : Error setting V124.\n");
        polData.statusS |= (WARN_INT);
    }
}
