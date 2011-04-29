#include <ctype.h>

#include "rcdev.h"
#include "rpoldata.h"
#include "rpolutil.h"

extern FILE *LogFile;
extern beamDataStruct beamData;
extern beamDataStruct beamOtherData;
extern polDataStruct polData;
extern int recRing;
extern wcmDataStruct wcmData;
extern wcmDataStruct wcmOtherData;
extern jetPositionStruct jetPosition;
extern V124Struct V124;			// V124 settings
extern int iDebug;
extern int NoADO;
extern float mTime;
extern int mEvent;
extern char DeviceName[128];	// our CDEV name
extern char ourTargetCDEVName[20];	// we will write what is appropriate here in getAdoInfo()
extern int nLoop;	// Number of subruns
extern int iRamp;	// going to be ramp

//	Get HJET valves status
int getJetBits(void)
{
    cdevData data;
    char msg[20];
    int irc, i, j;
    char jetBitsName[2][40] = {"iobitText.12a-hjet.11", "iobitText.12a-hjet.15"};
//*********************************************
 if (NoADO != 0) return 0;
    j = 0;
    irc  = 0;
    for (i=0; i<2; i++) {
	cdevDevice & dev = cdevDevice::attachRef(jetBitsName[i]);
	if(!DEVSEND(dev, "get outBitsText", NULL, &data, LogFile, irc)) 
	    data.get("value", msg, sizeof(msg));	// msg = "on"/"off"
	if (tolower(msg[1]) == 'n') j |= 1;
	j <<= 1;
    }
    return j;
}

//	Get BPMs around Jet position
void getJetPosition(void)
{
    cdevData data;
    int irc, i;
    
    char rbpmName[8][20] = {"rbpm.b-g11-bhx", "rbpm.b-g12-bhx", "rbpm.b-g12-bvx",
	"rbpm.b-g11-bvx", "rbpm.y-g11-bhx", "rbpm.y-g12-bhx", "rbpm.y-g11-bvx", "rbpm.y-g12-bvx"};
//**********************************************************
 if (NoADO != 0) return;
    irc = 0;
    cdevDevice & dev = cdevDevice::attachRef("stepper.12a-hjet.A.U");
    if(!DEVSEND(dev, "get absolutePosition", NULL, &data, LogFile, irc)) 
	data.get("value", &jetPosition.jetAbsolutePosition);
    for (i=0; i<8; i++) {
	cdevDevice & rbpm = cdevDevice::attachRef(rbpmName[i]);
	if(!DEVSEND(rbpm, "get avgOrbPositionM", NULL, &data, LogFile, irc)) 
	    data.get("value", &jetPosition.rbpm[i]);
    }
}

//	Get active target movement
int getTargetMovementInfo(long **res)
{
    int irc;
    cdevData data;
    size_t len;
    *res = NULL;

    if (NoADO != 0) return 0;
//	determine the ring
    if (iDebug > 1000) fprintf(LogFile, "RHICPOL-INFO : AdoInfo - %s get encPosProfileS\n", DeviceName);
//	getting the data
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

//	Check if we have any carbon target in the beam (for HJET)
void getCarbTarg(carbTargStat * targstat)
{
    char targetCDEVName[8][20] = {"pol.y-htarget", "pol.y-vtarget", "pol.b-htarget", "pol.b-vtarget",
    		"pol2.y-htarget", "pol2.y-vtarget", "pol2.b-htarget", "pol2.b-vtarget"};
    cdevData data;
    int irc;
    int i;
    
    if (NoADO != 0) {
	targstat->good = 1; 
	for (i=0; i<8; i++) strcpy(targstat->carbtarg[i],"Park");
	return ;
    }

    if (iDebug > 1000) fprintf(LogFile, "RHICPOL-INFO : GetCarbTarget\n");

//	create devices and get info
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
//	Check positions (in beam positions are named "Target1" ... "Target6")
    for (i=0; i<8; i++) if (strlen(targstat->carbtarg[i]) == 7 && strncasecmp(targstat->carbtarg[i], "Target", 6) == 0) break;
    if (i<8) targstat->good = 0;
}

//	Get wall current monitor data
void getWcmInfo(void)
{
    char wcmCDEVName[2][20] = {"yi2-wcm3","bo2-wcm3"};
    int irc, N;
    cdevData data;

    if (NoADO != 0) return;

//	determine the ring
    N = 0;
    if (recRing & REC_BLUE) N = 1; 
    if (iDebug > 1000) fprintf(LogFile, "RHICPOL-INFO : WcmInfo - %s\n", wcmCDEVName[N]);
    cdevDevice & wcm =  cdevDevice::attachRef(wcmCDEVName[N]);
    irc = 0;
//	wcm
    if(!DEVSEND(wcm, "get fillDataM", NULL, &data, LogFile, irc))
        data.get("value", wcmData.fillDataM);
    if(!DEVSEND(wcm, "get wcmBeamM", NULL, &data, LogFile, irc))
        data.get("value", &wcmData.wcmBeamM);
    
    if (recRing & REC_JET) {	// Get also info for the other beam in jet mode
    
	if (iDebug > 1000) fprintf(LogFile, "RHICPOL-INFO : WcmInfo - %s\n", wcmCDEVName[1-N]);
	cdevDevice & wcmo =  cdevDevice::attachRef(wcmCDEVName[1-N]);
//	wcm
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

//	Get most of CDEV data at the beginnig of the run
void getAdoInfo(void)
{
//	gather CDEV information we could need 
    char specCDEVName[2][20] = {"ringSpec.yellow", "ringSpec.blue"};
    char bucketsCDEVName[2][20] = {"buckets.yellow", "buckets.blue"};
    char wcmCDEVName[2][20] = {"yi2-wcm3","bo2-wcm3"};
    char defName[128];
    int irc, N;
    cdevData data;
    int ival[6];

    if (NoADO != 0) return;
    if (iDebug > 1000) fprintf(LogFile, "RHICPOL-INFO : CDEV Info - %s\n", DeviceName);
    irc = 0;
//	Get polarimeter information if DeviceName != "None"
    if (DeviceName[0] != 'N') {
	cdevDevice & pol =  cdevDevice::attachRef(DeviceName);
//  target name in targetIdS - this is our target !
	if(!DEVSEND(pol, "get targetIdS", NULL, &data, LogFile, irc))
	    data.get("value", polData.targetIdS, sizeof(polData.targetIdS));
//		translate from polarimeter.blu2 etc to pol2.b-vtarget etc
	sprintf(ourTargetCDEVName, "pol%s.%c-%ctarget", (DeviceName[15] == '2') ? "2" : "", 
	    DeviceName[12], tolower(polData.targetIdS[0]));
//	Get time of measurement if it was not set in the command line
	if (mTime < 0) {
	    if (iRamp) {
		if(!DEVSEND(pol, "get rampMeasTimeS", NULL, &data, LogFile, irc)) data.get("value", &mTime);
	    } else {
		if(!DEVSEND(pol, "get maxTimeToRunS", NULL, &data, LogFile, irc)) data.get("value", &mTime);
	    }
	}
//	Get number of events if it was not set in the command line
	if (mEvent < 0) {
	    if (iRamp) {
		mEvent = 2000000000;	// nearly max 32 bit signed
	    } else {
		if (!DEVSEND(pol, "get numberEventsToDoS", NULL, &data, LogFile, irc)) data.get("value", &mEvent);
	    }
	}
	if (iRamp != 0 && nLoop == 1) {
	    N = polData.targetIdS[strlen(polData.targetIdS) - 1] - '1';	// target number
	    if (N < 0 || N > 5) {
		fprintf(LogFile, "Unexpected target name %s.\n", polData.targetIdS);
		polData.statusS |= (WARN_INT);
		N = 0;
	    }
	    memset(ival, 0, sizeof(ival));
	    sprintf(defName, "polarControlDefaults%s", &DeviceName[11]);
	    cdevDevice & def = cdevDevice::attachRef(defName);
	    if (!DEVSEND(def, "get rampInterval", NULL, &data, LogFile, irc)) data.get("value", ival);
	    if (ival[N] < 0) ival[N] = 2;	// just default
	    nLoop = (int) (mTime / ival[N]);
	}
    }
//	determine the ring (my ring <-> other ring...
    N = 0;
    if (recRing & REC_BLUE) N = 1; 
//	create devices
    cdevDevice & spec =  cdevDevice::attachRef(specCDEVName[N]);
    cdevDevice & buckets =  cdevDevice::attachRef(bucketsCDEVName[N]);
    cdevDevice & wcm =  cdevDevice::attachRef(wcmCDEVName[N]);
//	ringSpec
    if(!DEVSEND(spec, "get beamEnergyM", NULL, &data, LogFile, irc))
	data.get("value", &beamData.beamEnergyM);
    if(!DEVSEND(spec, "get fillNumberM", NULL, &data, LogFile, irc))
	data.get("value", &beamData.fillNumberM);
//	buckets
    if(!DEVSEND(buckets, "get measuredFillPatternM", NULL, &data, LogFile, irc))
//    if(!DEVSEND(buckets, "get intendedFillPatternS", NULL, &data, LogFile, irc))	-- uncomment for tests
	data.get("value", beamData.measuredFillPatternM);    
    if(!DEVSEND(buckets, "get polarizationFillPatternS", NULL, &data, LogFile, irc)) {
	data.get("value", beamData.polarizationFillPatternS);
//	It looks that they changed type to char ...
    for (int i=0; i<360; i++) if (beamData.polarizationFillPatternS[i] > 127)
	beamData.polarizationFillPatternS[i] -= 256;
    }
//	wcm
    if(!DEVSEND(wcm, "get fillDataM", NULL, &data, LogFile, irc))
        data.get("value", wcmData.fillDataM);
    if(!DEVSEND(wcm, "get wcmBeamM", NULL, &data, LogFile, irc))
        data.get("value", &wcmData.wcmBeamM);
//	the other ring for jet mode
    if (recRing & REC_JET) {
	cdevDevice & speco =  cdevDevice::attachRef(specCDEVName[1-N]);
	cdevDevice & bucketso =  cdevDevice::attachRef(bucketsCDEVName[1-N]);
	cdevDevice & wcmo =  cdevDevice::attachRef(wcmCDEVName[1-N]);
//	ringSpecOther
	if(!DEVSEND(speco, "get beamEnergyM", NULL, &data, LogFile, irc))
	    data.get("value", &beamOtherData.beamEnergyM);
	if(!DEVSEND(speco, "get fillNumberM", NULL, &data, LogFile, irc))
	    data.get("value", &beamOtherData.fillNumberM);
//	bucketsOther
	if(!DEVSEND(bucketso, "get measuredFillPatternM", NULL, &data, LogFile, irc))
	    data.get("value", beamOtherData.measuredFillPatternM);    
	if(!DEVSEND(bucketso, "get polarizationFillPatternS", NULL, &data, LogFile, irc)) {
	    data.get("value", beamOtherData.polarizationFillPatternS);
//	It looks that they had changed type to char ...
	for (int i=0; i<360; i++) if (beamOtherData.polarizationFillPatternS[i] > 127)
	    beamOtherData.polarizationFillPatternS[i] -= 256;
	}
//	wcmOther
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

    if (NoADO != 0) return;

    cdevDevice & target = cdevDevice::attachRef(ourTargetCDEVName);
    irc = 0;

    if(!DEVSEND(target, "get positionEncM", NULL, &data, LogFile, irc))
        data.get("value", res);

    res[2] = res[0];	// do just a copy for compatibility ...
    res[3] = res[1];	

    if (irc != 0) {
	fprintf(LogFile, "%d errors getting target information.\n", irc);
	polData.statusS |= (STATUS_ERROR | ERR_NOADO);
    }
}

//	Send our progress to operator...
void UpdateProgress(int evDone, int rate, double ts) 
{
    static int StopUpdate=0;
    int irc;
    cdevData data;
    
    if (NoADO != 0) return;
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

//	Update status
void UpdateStatus(void) 
{
    int irc;
    cdevData data;

    if (iDebug > 1000) fprintf(LogFile, "RHICPOL-INFO : UpdateStatus - %s\n", DeviceName);
    cdevDevice & pol = cdevDevice::attachRef(DeviceName);

    irc = 0;
    data = polData.statusS;
    DEVSEND(pol, "set statusS", &data, NULL, LogFile, irc);

    if (irc != 0) {
	fprintf(LogFile, "Error updating status.\n");
	polData.statusS |= (WARN_INT);
    }
}

//	Update message
void UpdateMessage(char * msg) 
{
    int irc;
    cdevData data;

    if (iDebug > 1000) fprintf(LogFile, "RHICPOL-INFO : UpdateMessage - %s\n", DeviceName);

    cdevDevice & pol = cdevDevice::attachRef(DeviceName);

    irc = 0;
    data.insert("value", msg);
    DEVSEND(pol, "set statusStringS", &data, NULL, LogFile, irc);

    if (irc != 0) {
	fprintf(LogFile, "RHICPOL-WARN : Error updating message.\n");
	polData.statusS |= (WARN_INT);
    }
}

//	Program V124 - ring = 0 - yellow, 1 - blue
void ProgV124(int ring)
{
    char name[128];
    int irc;
    int i, j;
    unsigned char buckets[360];
    cdevData data;
    
    irc = 0;
//	Global setting
    if (V124.flags & 0x8000) {
	sprintf(name, "bsTrigMod.12a-polar1.%c", (ring) ? 'D' : 'C');
	cdevDevice & dev = cdevDevice::attachRef(name);
	data.insert("value", V124.positionDelay);
	DEVSEND(dev, "set positionDelay", &data, NULL, LogFile, irc);
	if (iDebug > 1000) fprintf(LogFile, "RHICPOL-INFO : Set V124 - %s:positionDelay\n", name);
    }
//	Channel settings
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
