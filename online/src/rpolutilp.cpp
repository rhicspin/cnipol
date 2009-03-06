#include <ctype.h>
#include "rcdev.h"
#include "rpoldata.h"

extern FILE *LogFile;
extern beamDataStruct beamData;
extern beamDataStruct beamOtherData;
extern targetDataStruct targetData[2];
extern polDataStruct polData;
extern int recRing;
extern wcmDataStruct wcmData;
extern wcmDataStruct wcmOtherData;
extern jetPositionStruct jetPosition;
extern int iDebug;

//**************************************************
extern int NoADO;


extern "C" {
    int getJetBits(void);
}

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


extern "C" {
    void getJetPosition(void);
}

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

extern "C" {
    int getTagetMovementInfo(long **data);
}

int getTagetMovementInfo(long **res)
{
    int N, irc;
    cdevData data;
    size_t len;
    char polCDEVName[2][20] = {"polarimeter.yel", "polarimeter.blu"};
//	determine the ring
    N = 0;
    if (recRing & REC_BLUE) N = 1; 
    if (iDebug > 1000) fprintf(LogFile, "RHICPOL-INFO : AdoInfo - %s\n", polCDEVName[N]);
//	getting the data
    cdevDevice & pol =  cdevDevice::attachRef(polCDEVName[N]);
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

extern "C" {
    void getCarbTarg(carbTargStat * targstat);
}

void getCarbTarg(carbTargStat * targstat)
{
    char targetCDEVName[4][20] = {"pol.y-htarget", "pol.y-vtarget", "pol.b-htarget", "pol.b-vtarget"};	
    cdevData data;
    int irc=0;
    int i;


//***************************************************************
 if (NoADO != 0) {
   targstat->good = 1; 
   for (i=0; i<4; i++) strcpy(targstat->carbtarg[i],"P");
   return;
 }
//***************************************************************


    if (iDebug > 1000) fprintf(LogFile, "RHICPOL-INFO : GetCarbTarget\n");
//	create devices
    cdevDevice & yhtarget = cdevDevice::attachRef(targetCDEVName[0]);
    cdevDevice & yvtarget = cdevDevice::attachRef(targetCDEVName[1]);
    cdevDevice & bhtarget = cdevDevice::attachRef(targetCDEVName[2]);
    cdevDevice & bvtarget = cdevDevice::attachRef(targetCDEVName[3]);

    if(!DEVSEND(yhtarget, "get positionS", NULL, &data, LogFile, irc))
	data.get("value", targstat->carbtarg[0], sizeof(targstat->carbtarg[0]));
    if(!DEVSEND(yvtarget, "get positionS", NULL, &data, LogFile, irc))
	data.get("value", targstat->carbtarg[1], sizeof(targstat->carbtarg[1]));
    if(!DEVSEND(bhtarget, "get positionS", NULL, &data, LogFile, irc))
	data.get("value", targstat->carbtarg[2], sizeof(targstat->carbtarg[2]));
    if(!DEVSEND(bvtarget, "get positionS", NULL, &data, LogFile, irc))
	data.get("value", targstat->carbtarg[3], sizeof(targstat->carbtarg[3]));

    targstat->good = 1;
    if (irc != 0) {
	fprintf(LogFile, "%d errors getting RHIC information.\n", irc);
	polData.statusS |= (STATUS_ERROR | ERR_NOADO);
    }
    for (i=0;i<4;i++) 
	if ( toupper(targstat->carbtarg[i][0]) != 'P' && toupper(targstat->carbtarg[i][0]) != 'R'
	    && toupper(targstat->carbtarg[i][0]) != 'N' ) break;
    if (i<4) targstat->good = 0;

}


extern "C" {
    void getWcmInfo(void);
}

void getWcmInfo(void)
{
    char wcmCDEVName[2][20] = {"yi2-wcm3","bo2-wcm3"};
    int irc, N;
    cdevData data;
//********************************************************
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

extern "C" {
    void getAdoInfo(char mode);
}

void getAdoInfo(char mode)
{
//	gather CDEV information we could need
    char polCDEVName[2][20] = {"polarimeter.yel", "polarimeter.blu"};
    char specCDEVName[2][20] = {"ringSpec.yellow", "ringSpec.blue"};
    char bucketsCDEVName[2][20] = {"buckets.yellow", "buckets.blue"};
    char targetCDEVName[4][20] = {"pol.y-htarget", "pol.y-vtarget", "pol.b-htarget", "pol.b-vtarget"};	
    char wcmCDEVName[2][20] = {"yi2-wcm3","bo2-wcm3"};
//    char muxCDEVName[2][20] = {"polarMux.RHIC.yel", "polarMux.RHIC.blu"};

    int irc, N;
//    int iTarget = -1;

    cdevData data;

//**************************************************************
 if (NoADO != 0) return;

//	determine the ring
    N = 0;
    if (recRing & REC_BLUE) N = 1; 
    if (iDebug > 1000) fprintf(LogFile, "RHICPOL-INFO : AdoInfo - %s\n", polCDEVName[N]);
//	create devices
    cdevDevice & htarget = cdevDevice::attachRef(targetCDEVName[2*N]);
    cdevDevice & vtarget = cdevDevice::attachRef(targetCDEVName[2*N+1]);
    cdevDevice & spec =  cdevDevice::attachRef(specCDEVName[N]);
    cdevDevice & buckets =  cdevDevice::attachRef(bucketsCDEVName[N]);
    cdevDevice & wcm =  cdevDevice::attachRef(wcmCDEVName[N]);
    cdevDevice & pol =  cdevDevice::attachRef(polCDEVName[N]);
//    cdevDevice & mux = cdevDevice::attachRef(muxCDEVName[N]);
//	the other ring for jet mode
    cdevDevice & speco =  cdevDevice::attachRef(specCDEVName[1-N]);
    cdevDevice & bucketso =  cdevDevice::attachRef(bucketsCDEVName[1-N]);
    cdevDevice & wcmo =  cdevDevice::attachRef(wcmCDEVName[1-N]);

//	get data one by one
    irc = 0;

//  target name in targetIdS
    if(!DEVSEND(pol, "get targetIdS", NULL, &data, LogFile, irc))
	   data.get("value", polData.targetIdS, sizeof(polData.targetIdS));
//	horizontal target
    if(!DEVSEND(htarget, "get movingM", NULL, &data, LogFile, irc))
	   data.get("value", &targetData[0].movingM);
    if(!DEVSEND(htarget, "get positionEncM", NULL, &data, LogFile, irc))
        data.get("value", targetData[0].positionEncM);
    if(!DEVSEND(htarget, "get statusM", NULL, &data, LogFile, irc))
	   data.get("value", targetData[0].statusM, sizeof(targetData[0].statusM));
    if(!DEVSEND(htarget, "get positionS", NULL, &data, LogFile, irc))
	   data.get("value", targetData[0].positionS, sizeof(targetData[0].positionS));
//	vertical target
    if(!DEVSEND(vtarget, "get movingM", NULL, &data, LogFile, irc))
	   data.get("value", &targetData[1].movingM);
    if(!DEVSEND(vtarget, "get positionEncM", NULL, &data, LogFile, irc))
	   data.get("value", targetData[1].positionEncM);
    if(!DEVSEND(vtarget, "get statusM", NULL, &data, LogFile, irc))
	   data.get("value", targetData[1].statusM, sizeof(targetData[0].statusM));
    if(!DEVSEND(vtarget, "get positionS", NULL, &data, LogFile, irc))
	   data.get("value", targetData[1].positionS, sizeof(targetData[0].positionS));

//	ringSpec
    if(!DEVSEND(spec, "get beamEnergyM", NULL, &data, LogFile, irc))
	   data.get("value", &beamData.beamEnergyM);
    if(!DEVSEND(spec, "get fillNumberM", NULL, &data, LogFile, irc))
	   data.get("value", &beamData.fillNumberM);
//	buckets
    if(!DEVSEND(buckets, "get measuredFillPatternM", NULL, &data, LogFile, irc))
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
    if (recRing & REC_JET) {
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
//	pol
//    DEVSEND(mux, "get polNumM", NULL, &data, LogFile, irc);
//    data.get("value", &iTarget);
/*
    if (mode == 'e') {
 	 if(!DEVSEND(pol, "get emitRunIdS", NULL, &data, LogFile, irc))
	    data.get("value", &polData.runIdS);
    } 
	else {
	 if(!DEVSEND(pol, "get runIdS", NULL, &data, LogFile, irc))
	    data.get("value", &polData.runIdS);
    }
*/

    if (irc != 0) {
	fprintf(LogFile, "%d errors getting RHIC information.\n", irc);
	polData.statusS |= (STATUS_ERROR | ERR_NOADO);
    }
}

extern "C" {
    void GetTargetEncodings(long *res);
}

void GetTargetEncodings(long *res)
{
    char targetCDEVName[4][20] = {"pol.y-htarget", "pol.y-vtarget", "pol.b-htarget", "pol.b-vtarget"};	
    int irc, N;
    cdevData data;

//	determine the ring
    N = 0;
    if (recRing & REC_BLUE) N = 1; 
    cdevDevice & htarget = cdevDevice::attachRef(targetCDEVName[2*N]);
    cdevDevice & vtarget = cdevDevice::attachRef(targetCDEVName[2*N+1]);
    irc = 0;
    if(!DEVSEND(htarget, "get positionEncM", NULL, &data, LogFile, irc))
        data.get("value", res);
    if(!DEVSEND(vtarget, "get positionEncM", NULL, &data, LogFile, irc))
        data.get("value", &res[2]);
    if (irc != 0) {
	fprintf(LogFile, "%d errors getting target information.\n", irc);
	polData.statusS |= (STATUS_ERROR | ERR_NOADO);
    }
}

extern "C" {
    void UpdateProgress(int evDone, int rate, double ts);
}

void UpdateProgress(int evDone, int rate, double ts) {

    char polCDEVName[2][20] = {"polarimeter.yel", "polarimeter.blu"};
    static int StopUpdate=0;
    int N, irc;
    cdevData data;
    
    if (StopUpdate) return;

    N = (recRing & REC_BLUE) ? 1 : 0;
    if (iDebug > 1000) fprintf(LogFile, "RHICPOL-INFO : UpdateProgress - %s\n", polCDEVName[N]);
    cdevDevice & pol = cdevDevice::attachRef(polCDEVName[N]);
    
    irc = 0;
    data = evDone;
    DEVSEND(pol, "set progressS", &data, NULL, LogFile, irc);
//    data = rate;
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

extern "C" {
    void UpdateStatus(void);
}

void UpdateStatus(void) 
{

    char polCDEVName[2][20] = {"polarimeter.yel", "polarimeter.blu"};
    int N, irc;
    cdevData data;

    strncpy(polData.statusStringS, stat2str(polData.statusS), sizeof(polData.statusStringS));
    N = (recRing & REC_BLUE) ? 1 : 0;
    if (iDebug > 1000) fprintf(LogFile, "RHICPOL-INFO : UpdateStatus - %s\n", polCDEVName[N]);
    cdevDevice & pol = cdevDevice::attachRef(polCDEVName[N]);    

    irc = 0;
    data = polData.statusS;
    DEVSEND(pol, "set statusS", &data, NULL, LogFile, irc);
    data.insert("value", polData.statusStringS);
    DEVSEND(pol, "set statusStringS", &data, NULL, LogFile, irc);

    if (irc != 0) {
	fprintf(LogFile, "Error updating status.\n");
	polData.statusS |= (WARN_INT);
    }
}

extern "C" {
    void UpdateMessage(char * msg);
}

void UpdateMessage(char * msg) 
{
    char polCDEVName[2][20] = {"polarimeter.yel", "polarimeter.blu"};
    int N, irc;
    cdevData data;

    N = (recRing & REC_BLUE) ? 1 : 0;
    if (iDebug > 1000) fprintf(LogFile, "RHICPOL-INFO : UpdateMessage - %s\n", polCDEVName[N]);

    cdevDevice & pol = cdevDevice::attachRef(polCDEVName[N]);    

    irc = 0;
    data.insert("value", msg);
    DEVSEND(pol, "set statusStringS", &data, NULL, LogFile, irc);

    if (irc != 0) {
	fprintf(LogFile, "Error updating message.\n");
	polData.statusS |= (WARN_INT);
    }
}
