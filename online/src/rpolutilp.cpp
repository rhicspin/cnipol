#include "rcdev.h"
#include "rpoldata.h"

extern FILE *LogFile;
extern beamDataStruct beamData;
extern targetDataStruct targetData[2];
extern polDataStruct polData;
extern int recRing;
extern wcmDataStruct wcmData;
extern int iDebug;

extern "C" {
    void getWcmInfo(void);
}

void getWcmInfo(void)
{
    char wcmCDEVName[2][20] = {"yi2-wcm3","bo2-wcm3"};
    int irc, N;
    cdevData data;

//	determine the ring
    N = 0;
    if (recRing & REC_BLUE) N = 1; 
    if (iDebug > 1000) fprintf(LogFile, "RHICPOL-INFO : WcmInfo - %s\n", wcmCDEVName[N]);
    cdevDevice & wcm =  cdevDevice::attachRef(wcmCDEVName [N]);
    irc = 0;
//	wcm
    if(!DEVSEND(wcm, "get fillDataM", NULL, &data, LogFile, irc))
        data.get("value", wcmData.fillDataM);
    if(!DEVSEND(wcm, "get wcmBeamM", NULL, &data, LogFile, irc))
        data.get("value", &wcmData.wcmBeamM);

    if (irc != 0) {
	fprintf(LogFile, "%d errors getting RHIC information.\n", irc);
	polData.statusS |= (STATUS_ERROR | ERR_NOADO);
    }
}

extern "C" {
    void getAdoInfo(void);
}

void getAdoInfo(void)
{
//	gather CDEV information we could need
    char polCDEVName[2][20] = {"polarimeter.yel", "polarimeter.blu"};
    char specCDEVName[2][20] = {"ringSpec.yellow", "ringSpec.blue"};
    char bucketsCDEVName[2][20] = {"buckets.yellow", "buckets.blue"};
    char targetCDEVName[4][20] = {"pol.y-htarget", "pol.y-vtarget", "pol.b-htarget", "pol.b-vtarget"};	
    char wcmCDEVName[2][20] = {"yi2-wcm3","bo2-wcm3"};
    int irc, N;
    cdevData data;

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
//	get data one by one
    irc = 0;
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
//	pol
    if(!DEVSEND(pol, "get runIdS", NULL, &data, LogFile, irc))
	data.get("value", &polData.runIdS);
//	wcm
    if(!DEVSEND(wcm, "get fillDataM", NULL, &data, LogFile, irc))
        data.get("value", wcmData.fillDataM);
    if(!DEVSEND(wcm, "get wcmBeamM", NULL, &data, LogFile, irc))
        data.get("value", &wcmData.wcmBeamM);

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
    void UpdateProgress(int evDone, int rate);
}

void UpdateProgress(int evDone, int rate) {

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
    data = rate;
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
