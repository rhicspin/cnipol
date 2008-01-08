#define _FILE_OFFSET_BITS 64	    // to handle >2Gb files
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <errno.h>
#include "rhicpol.h"
#include "rpolutil.h"

FILE *LogFile;
beamDataStruct beamData;
beamDataStruct beamOtherData;
targetDataStruct targetData[2];
polDataStruct polData;
configRhicDataStruct Conf;
SiChanStruct * SiConf = NULL;
wcmDataStruct wcmData;
wcmDataStruct wcmOtherData;
jetPositionStruct jetPosition;

int NoADO	= 0;
float mTime 	= 10.0;
int mEvent  	= 20000000;
float tavg 	= 0.0;
int iSig 	= 0;
int iDebug 	= 1;
int iCntrlC 	= 1;
int iPulseProg 	= 0;
int iExtClock   = 1;
int iCicleRun   = 0;
extern int IStop;

float ANALPOW;
float ANALPOWE;

int recRing = 0;

void polexit(void) 
{
    int erc;
    if (iSig == SIGTERM) polData.statusS |= WARN_CANCELLED;
    if (NoADO == 0 && (recRing & REC_JET) == 0) UpdateStatus();
    erc = ((polData.statusS) >> 16 ) & 0xFF;
    closeDataFile("Abnormal handpol termination.");
    if (erc != 0) fprintf(LogFile, "RHICPOL-FATAL : Exiting due to severe error...\n");
    if (iSig != 0) exit(128+iSig);
    exit(erc);
}

char * cctime(time_t *itime)
{
    char *str;
    str = ctime(itime);
    str[strlen(str)-1] = '\0';
    return str;
}

int main(int argc, char **argv)
{
    char fname[512] = "polout.data";
    char cfgname[512] = "./config/rhicpol.ini";
    char logname[512] = "";
    char comment[512] = "RHIC Polarimeter Run";
    char workdir[512];
    float tshift = 0.;
    int iHelp = 0;
    int iNotify = 0;
    int i, j, k, ev;
    char * buf;
    time_t t;
    char mode = 'd';	// default is data mode, but actually only emittance mode changes anything here

    LogFile = stdout;
    recRing = 0;
    
    while ((i = getopt(argc, argv, "c:Ce:f:ghi:IJl:nPt:T:v:m:")) != -1) switch(i) {
    case 'c' :
	strncpy(comment, optarg, sizeof(comment));
	break;
    case 'C':
	iExtClock = 0;
        break;
    case 'e' :
	mEvent = strtol(optarg, NULL, 0);
	break;
    case 'f' :
	strncpy(fname, optarg, sizeof(fname));
	break;
    case 'g' :
	NoADO=1;
	break;
    case 'i' :
	strncpy(cfgname, optarg, sizeof(cfgname));
	break;
    case 'I':
	iCntrlC = 0;
        break;
    case 'J' :
	iCicleRun = 1;
	break;
    case 'l' :
	strncpy(logname, optarg, sizeof(logname));
	break;
    case 'n':
	iNotify = 1;
        break;
    case 'P':
	iPulseProg = 1;
	break;
    case 't' : 
	mTime = strtod(optarg, NULL);
	break;
    case 'T':
	tshift = strtod(optarg, NULL);
        break;
    case 'v':
	iDebug = strtol(optarg, NULL, 0);
        break;
    case 'm':
	mode = optarg[0];
	break;
    default:
	iHelp = 1;
	break;
    }
    if (iHelp) {
	printf("\n\tPolarimeter command line data taking utility\n"
"Usage: rhicpol [options]. Possible options are:\n"
"\t-c comment : comment string.\n"
"\t-C Stay with internal clocks (for debugging only).\n"
"\t-e number : number of events to get.\n"
"\t-f filename : output data file name.\n"
"\t-g : do not get/record beam/target ADO information.\n"
"\t-h : print this message and exit.\n"
"\t-i filename : basic config file name. All other filenames are raltive to its directory\n"
"\t-I : don't sense ^C while reading out memory.\n"
"\t-J : Jet DAQ run.\n"
"\t-l filename : logfile name, default is stdout\n"
"\t-n : notify parent process (SIGUSR1) about the end of data taking\n"
"\t     (to take the target out before memory readout)\n"
"\t-P Pulse PROG pin for all WFDs.\n"
"\t-t time : maximum time of the measurement, seconds.\n"
"\t-T value : ns, everything (lookups, windows) is shifted according to this value.\n"
"\t-v level : verbose output.\n");
	return -1;
    } else {
	polData.statusS = 0;
/* We will set configuration file directory as current */
	strncpy(workdir, cfgname, sizeof(workdir));
	buf = strrchr(workdir, '/');
	if (buf != NULL) {
	    buf[0] = '\0';
	    chdir(workdir);
	    buf++;
	} else {
	    buf = cfgname;
	}
/* Open LogFile */
	if (strlen(logname) > 0) {
	    LogFile=fopen(logname,"at");
	    if (LogFile == NULL) {
		printf("Cannot open logfile %s. Logging to stdout.\n",logname);
		polData.statusS |= (WARN_INT);
		LogFile = stdout;
	    }
// No buffering for log file : Ron 4/06 -- Also comment out fflush(LogFile) throughout code
        setlinebuf(LogFile);
	}
	t = time(NULL);
	fprintf(LogFile, 
	    ">>>>> %s Starting measurement for time %7.2f s or %d events.\n",
	    cctime(&t), mTime, mEvent);
/* Reading main configuratyion file */
	if (readConfig(buf, CFG_INIT)) {
	    fprintf(LogFile,"RHICPOL-FATAL : Cannot open config file %s\n",cfgname);
	    polData.statusS |= (STATUS_ERROR | ERR_INT);
	    polexit();
	}
//	fflush(LogFile);
/* If tshift in effect move sensitive window boundaries */
	for (i=0; i<Conf.NumChannels; i++) {
	    j = (int)SiConf[i].Window.split.Beg + tshift/Conf.WFDTUnit;
	    if (j<1) j=1; if(j>255) j=255;
	    SiConf[i].Window.split.Beg = j;
	    k = (int)SiConf[i].Window.split.End + tshift/Conf.WFDTUnit;
	    if (k<j) k=j; if(k>255) k=255;
	    SiConf[i].Window.split.End = k;
	}
	CreateLookup(tshift); // Must go after window correction with tshift
//	zero structures first    
	memset(&beamData, 0, sizeof(beamData));
	memset(targetData, 0, sizeof(targetData));
	memset(&wcmData, 0, sizeof(wcmData));
/* Always first copy default pattern to beamdata */
	for (i=0;i<120;i++) {
	    if (Conf.Pattern[i] > 0) beamData.measuredFillPatternM[3*i] = 1;
	    if (Conf.Pattern[i] == 1) beamData.polarizationFillPatternS[3*i] = 1;
	    if (Conf.Pattern[i] == 2) beamData.polarizationFillPatternS[3*i] = -1;
	}    

	if (NoADO == 0) {
	    getAdoInfo(mode);
	    /* change default pol pattern */
	    memset(&Conf.Pattern, 0, sizeof(Conf.Pattern));
	    for (i=0;i<120;i++) {
		if (beamData.measuredFillPatternM[3*i] > 0) {
		    if (beamData.polarizationFillPatternS[3*i] > 0)
			Conf.Pattern[i]=1;
		    else if (beamData.polarizationFillPatternS[3*i] < 0)
			Conf.Pattern[i]=2;
		    else
			Conf.Pattern[i]=3;
		}
	    }
	}
	
	if (CheckConfig()) {
	    fprintf(LogFile,"RHICPOL-WARN : Strange things found in configuration.\n");    
	    fprintf(LogFile,"               Run may be unusable. Try -v20 for more information...\n");    
	    polData.statusS |= WARN_INT;
	}

	if (camacOpen() != 0) {
	    fprintf(LogFile,"RHICPOL-FATAL : Cannot connect to CAMAC\n");
	    polData.statusS |= (STATUS_ERROR | ERR_CAMAC);
	    polexit();
	}

//	Fill some usefull information to pass to data2hbook

	for (j=strlen(cfgname);j>=0;j--) if (cfgname[j] == '/') break;
	j++;
	sprintf(polData.daqVersionS,"%1d.%1d.%1d %s", DAQVERSION/10000, 
	    (DAQVERSION%10000)/100, DAQVERSION%100, &cfgname[j]);
	
	{ 
	    char ETLUTNames[][10]={"Kine","Rect","Thresh"};
	    char IALUTNames[][10]={"Line","Open"};
	    i=Conf.ETLookUp;
	    if (i>2) i=2;
	    sprintf(polData.cutIdS,"ET:%s_IA:%s", ETLUTNames[i], IALUTNames[Conf.IALookUp]);
	}

	polData.targetIdS[0] = '\0';
	for (j=0,k=-1;j<2;j++) {
	    if (toupper(targetData[j].positionS[0]) != 'R' && 
		toupper(targetData[j].positionS[0]) != 'P' ) {
		    sprintf(&polData.targetIdS[strlen(polData.targetIdS)],
			"%s%s",(j==0)?"Horz":"Vert",targetData[j].positionS);
		    k = j;
		}
	}
	if (k == -1) {
	    sprintf(polData.targetIdS,"NoTarget");
	    polData.statusS |= (STATUS_ERROR | ERR_TARGET);
	}
	polData.encoderPositionS[0] = targetData[k].positionEncM[0];
	polData.encoderPositionS[1] = targetData[k].positionEncM[1];
	polData.beamEnergyS = beamData.beamEnergyM;
	polData.analyzingPowerS = ANALPOW;
	polData.analyzingPowerErrorS = ANALPOWE;
	polData.numberEventsS = mEvent;
	polData.maxTimeS = (int)mTime;
	// run number
	if (beamData.fillNumberM <= 0 || (NoADO !=0)) {
	// if we can't get real fill number, let's assume it yymmddhh, rather unique
	    {
		time_t tm;
		char sss[80];
		tm = time(NULL);
		strftime(sss, sizeof(sss), "%y%m%d%H", localtime(&tm));
		beamData.fillNumberM = strtol(sss, NULL, 0);
	    }
	    if (NoADO == 0) {
		polData.statusS |= WARN_NOBEAM;
		fprintf(LogFile,"Can't get fill number, setting %d\n",&beamData.fillNumberM);
	    }
	}
	
	if (((int) polData.runIdS) == beamData.fillNumberM) {
	    polData.runIdS += 0.001;
	} else {
	    polData.runIdS = beamData.fillNumberM + 0.001;
	    if (recRing == REC_YELLOW) polData.runIdS += 0.1;
	    if (recRing == REC_JET) polData.runIdS += 0.2;
	    if (mode == 'e') polData.runIdS += 0.4;
	}
	if (NoADO == 0) 
	    fprintf(LogFile,"CNIPOL-INFO : RunID: %8.3f; E=%6.2f GeV; Target: %s @ %d %d\n",
		polData.runIdS, beamData.beamEnergyM,
		polData.targetIdS, polData.encoderPositionS[0], polData.encoderPositionS[1]);
	if (setRing()) polexit();
	if (IStop != 0) polexit();
	if (openDataFile(fname, comment, NoADO)) polexit();
	setInhibit();
	initScalers();
	if (initWFDs()) {
	    fprintf(LogFile,"RHICPOL-FATAL : Cannot find reqired CAMAC modules\n");
	    polData.statusS |= (STATUS_ERROR | ERR_CAMAC | ERR_WFD);
	    polexit();
	} 
	
	if (NoADO == 0 && (recRing & REC_JET) == 0) UpdateMessage("Running...");
	setAlarm(mTime);
	polData.startTimeS = time(NULL);
	clearVetoFlipFlop();
	resetInhibit();
	writeJetStatus();	// hopefully fast (we can not get jet state with crate inhibit)
	
	ev = getEvents(mEvent);
	
	setInhibit();
	polData.stopTimeS = time(NULL);
	clearAlarm();
	if (iNotify) kill( getppid(), SIGUSR1);
	if (iCntrlC) signal(SIGINT, alarmHandler);
	else signal(SIGINT, SIG_IGN);
	if (NoADO == 0 && (recRing & REC_JET) == 0) UpdateMessage("Reading Data...");
	signal(SIGTERM, alarmHandler);
	if (iDebug > 1000) fprintf(LogFile, "RHICPOL-INFO : Reading scalers.\n");
	readScalers();
	if (iDebug > 1000) fprintf(LogFile, "RHICPOL-INFO : Reading WFD xilinxes.\n");
	readWFD();
	if (iDebug > 1000) fprintf(LogFile, "RHICPOL-INFO : Reading WFD memory.\n");
	if (!Conf.OnlyHist) readMemory();
	resetInhibit();
	closeDataFile("End of RHICpol run.");
	camacClose();
	signal(SIGTERM, SIG_DFL);
	signal(SIGINT, SIG_DFL);
	t = time(NULL);
	fprintf(LogFile, 
	">>> %s Measurement finished with %9d events.\n",
	    cctime(&t), ev);

    }
    if (iSig == SIGTERM) polData.statusS |= WARN_CANCELLED;
    if (NoADO == 0 && (recRing & REC_JET) == 0) UpdateStatus();
    if (iSig != 0) return 128+iSig;
    return ((polData.statusS) >> 16 ) & 0xFF;
}
