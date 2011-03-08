/********************************************************
 *	RHIC Polarimeter DAQ Program			*
 *	I. Alekseev and D. Svirida			*
 *		2001-2010				*
 * This is the main datataking code. Reads the          *
 * configuration, programs CAMAC, takes data, writes    *
 * file.                                                *
 ********************************************************/

/**
 *
 * Dec 22, 2010 - Dmitri Smirnov
 *    - Added stream ID (up or down) in the raw data
 *
 */

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

//	Global variables
FILE *LogFile;
char DeviceName[128] ="None";		// our CDEV name (like polarimeter.yel1 etc)
char ourTargetCDEVName[20] = "None";	// we will write what is appropriate here in getAdoInfo()
beamDataStruct beamData;		// beam information from CDEV
beamDataStruct beamOtherData;		// we need both beams for hjet
V124Struct V124;			// V124 settings
polDataStruct polData;			// polarization structure
configRhicDataStruct Conf;		// CAMAC configuration
SiChanStruct * SiConf = NULL;		// silicon channels
wcmDataStruct wcmData;			// Wall current monitor data from CDEV
wcmDataStruct wcmOtherData;		// we need both beams for hjet
jetPositionStruct jetPosition;		// Jet position from CDEV


//	Run parameters and flags
int NoADO	= 0;			// don't take anything from CDEV
float mTime 	= -1.0;			// time to run (negative means not set in command line)
int mEvent  	= -1;			// number of events to take (negative means not set in the command line)
int iSig 	= 0;			// signal received
int iDebug 	= 1;			// Debug level
int iCntrlC 	= 1;			// ignore CntrlC on readout
int iPulseProg 	= 0;			// Pulse Prog before run
int iExtClock   = 1;			// use external clock
int iCicleRun   = 0;			// run ciclely instead of single pass - used for HJET
int nLoop       = 1;			// Number of subruns
int iRamp       = 0;			// going to be ramp
extern int IStop;			// Stop flag

float ANALPOW;				// very approximate analyzing power
float ANALPOWE;				// its error

int recRing = 0;			// data mask with ring information etc.
int recStream = 0;

//	Code to exit - this is error only exit
void polexit(void) 
{
    if (iSig == SIGTERM) polData.statusS |= WARN_CANCELLED;
    if (NoADO == 0 && (recRing & REC_JET) == 0) UpdateStatus();
    closeDataFile("Abnormal rhicpol termination.");
    if (((polData.statusS) >> 16 ) & 0xFF) fprintf(LogFile, "RHICPOL-FATAL : Exiting due to severe error...\n");
    exit(10);
}

//	remove \n from string time
char * cctime(time_t *itime)
{
    char *str;
    str = ctime(itime);
    str[strlen(str)-1] = '\0';
    return str;
}

//	Main
int main(int argc, char **argv)
{
    const char ETLUTNames[][10]={"Kine","Rect","Thresh"};
    const char IALUTNames[][10]={"Line","Open"};
    char fname[512] = "polout.data";
    char cfgname[512] = "./config/rhicpol.ini";
    char logname[512] = "";
    char comment[512] = "RHIC Polarimeter Run";
    char workdir[512];
    int iHelp = 0;
    int iNotify = 0;
    int iHistOnly = 0;	
    int i, j, k, ev;
    char * buf;
    time_t t;
    float tshift = 0;			// Global time shift

    LogFile = stdout;
    recRing = 0;    

    while ((i = getopt(argc, argv, "c:Cd:e:f:ghi:IJl:MPR::t:v:")) != -1)
    switch(i) {
    case 'c' :	// comment
	strncpy(comment, optarg, sizeof(comment));
	break;
    case 'C':	// keep internal Clock
	iExtClock = 0;
        break;
    case 'd' :	// device name
	strncpy(DeviceName, optarg, sizeof(DeviceName));
	int iPol;
        for (iPol=0; iPol<4; iPol++) {
           if (strcmp(polCDEVName[iPol], DeviceName) == 0) {
	      recStream = ( iPol==0 || iPol==3 ) ? REC_UPSTREAM : REC_DOWNSTREAM;
	      // ds:
	      //printf("pol: %s, %d, %x\n", DeviceName, iPol, recStream); break;
	      break;
	   }
	}
	break;    
    case 'e' :	// events
	mEvent = strtol(optarg, NULL, 0);
	break;
    case 'f' :	// output data file
	strncpy(fname, optarg, sizeof(fname));
	break;
    case 'g' :	// no CDEV
	NoADO=1;
	break;
    case 'i' :	// config file
	strncpy(cfgname, optarg, sizeof(cfgname));
	break;
    case 'I':	// ignore CntrlC
	iCntrlC = 0;
        break;
    case 'J' :	// JET mode
	iCicleRun = 1;
	break;
    case 'l' :	// log file 
	strncpy(logname, optarg, sizeof(logname));
	break;
    case 'M' :	// no Memory
	iHistOnly = 1;
	break;
    case 'P':	// pulse prog
	iPulseProg = 1;
	break;
    case 'R':	// number of subruns for ramp mode
	iRamp = 1;
	if (optarg) nLoop = strtol(optarg, NULL, 0);
        break;
    case 't' : 	// time to go
	mTime = strtod(optarg, NULL);
	break;
    case 'v':	// debug level
	iDebug = strtol(optarg, NULL, 0);
        break;
    default:	// print help and exit
	iHelp = 1;
	break;
    }

//	Type info and exit on unknown options...
    if (iHelp) {
	printf("\n\tPolarimeter command line data taking utility\n"
	    "Usage: rhicpol [options]. Possible options are:\n"
	    "\t-c comment : comment string;\n"
	    "\t-C : Stay with internal clocks (for debugging only);\n"
	    "\t-d device : current polarimeter CDEV name;\n"
	    "\t-e number : number of events to get;\n"
	    "\t-f filename : output data file name;\n"
	    "\t-g : do not communicate to CDEV, stand alone mode;\n"
	    "\t-h : print this message and exit;\n"
	    "\t-i filename : basic config file name. All other filenames are relative to its directory;\n"
	    "\t-I : don't sense ^C while reading out memory;\n"
	    "\t-J : Jet DAQ run;\n"
	    "\t-l filename : logfile name, default is stdout;\n"
	    "\t-M : do not read memory - only histograms;\n"
	    "\t-P : Pulse PROG pin for all WFDs;\n"
	    "\t-R : make subruns - ramp measurement;\n"
	    "\t-t time : maximum time of the measurement, seconds;\n"
	    "\t-v level : verbose output.\n");
	return 1;
    } 

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
	setvbuf(LogFile, NULL, _IOLBF, BUFSIZ);		// make line buffering of the log file
    }
    t = time(NULL);
    fprintf(LogFile, ">>>>> %s Starting measurement for device=%s\n", cctime(&t), DeviceName);
/* Reading main configuration file. */
    if (readConfig(buf, CFG_INIT)) {
	fprintf(LogFile,"RHICPOL-FATAL : Cannot open config file %s\n",cfgname);
	polData.statusS |= (STATUS_ERROR | ERR_INT);
	polexit();
    }
    
    if (iHistOnly) Conf.OnlyHist = 1;	// command line has priority
    
//	zero structures first    
    memset(&beamData, 0, sizeof(beamData));
    memset(&wcmData, 0, sizeof(wcmData));
    memset(&beamOtherData, 0, sizeof(beamOtherData));
    memset(&wcmOtherData, 0, sizeof(wcmOtherData));
    beamData.beamEnergyM = 100.0;	// defalut for no CDEV
//	check CDEV
    if (NoADO == 0 && getenv("CDEVDDL") == NULL) {	// we check if CDEV varables (at least one) are defined
	NoADO = 1;
        fprintf(LogFile,"RHICPOL-WARN : No CDEV environment found.\n");    
        fprintf(LogFile,"               Run may be unusable. Try -g to suppress this message.\n");
	polData.statusS |= WARN_INT;
    }
//	make recRing
    if (iCicleRun) {	// for HJET
	recRing = REC_JET | REC_YELLOW;	// jet has always yellow color
    } else {
	if (NoADO == 0 && DeviceName[0] == 'N') {	// we MUST have device name for CDEV when not in jet mode
	    fprintf(LogFile,"RHICPOL-INFO : no device name for CDEV. Disabling CDEV\n");
	    polData.statusS |= (WARN_INT);
	    NoADO = 1;
	} else if (strcasestr(DeviceName, "blu")) {
	    recRing = REC_BLUE;
	} else if (strcasestr(DeviceName, "yel")) {
	    recRing = REC_YELLOW;
	}
    }
//	get CDEV information
    if (NoADO == 0) {
        getAdoInfo();
        //	nonzero Pol/fill pattern in the config has priority over measured (we need this for debugging)
        for (i = 0; i < 120; i++) if (Conf.Pattern[i] != 0) break;
        if (i < 120) {
    	    fprintf(LogFile, "RHICPOL-INFO: Debugging run - not real polarization pattern !\n");
/* copy pattern from config file to beamdata */
	    for (i=0;i<120;i++) {
    		if (Conf.Pattern[i] > 0) beamData.measuredFillPatternM[3*i] = 1;
    		if (Conf.Pattern[i] == 1) beamData.polarizationFillPatternS[3*i] = 1;
    		if (Conf.Pattern[i] == 2) beamData.polarizationFillPatternS[3*i] = -1;
	    }
	    for (i=0;i<120;i++) {
    		if (Conf.Pattern[i] > 0) beamOtherData.measuredFillPatternM[3*i] = 1;
    		if (Conf.Pattern[i] == 1) beamOtherData.polarizationFillPatternS[3*i] = 1;
    		if (Conf.Pattern[i] == 2) beamOtherData.polarizationFillPatternS[3*i] = -1;
	    }
        } else {
	    /* copy pol pattern from CDEV */
	    for (i=0;i<120;i++) if (beamData.measuredFillPatternM[3*i] > 0) {
		if (beamData.polarizationFillPatternS[3*i] > 0) {
	    	    Conf.Pattern[i] = 1;
	    	} else if (beamData.polarizationFillPatternS[3*i] < 0) {
	    	    Conf.Pattern[i] = 2;
	    	} else {
	    	    Conf.Pattern[i] = 3;
		}
	    }
	}	    
    } else {
        for (i = 0; i < 120; i++) if (Conf.Pattern[i] != 0) break;
	if (i == 120) {	// no pattern set yet -> set +-+-... pattern
    	    fprintf(LogFile, "RHICPOL-INFO: Debugging run - not real polarization pattern !\n");
	    for (i=0; i < 120; i++) {
		Conf.Pattern[i] = 1 + (i & 1);
		beamData.measuredFillPatternM[3*i] = 1;
		beamData.polarizationFillPatternS[3*i] = 1 - 2*(i & 1);
		beamOtherData.measuredFillPatternM[3*i] = 1;
		beamOtherData.polarizationFillPatternS[3*i] = 1 - 2*(i & 1);
	    }
	}	
    }
//	Check and print.
    if (CheckConfig()) {
        fprintf(LogFile,"RHICPOL-WARN : Strange things found in configuration.\n");    
        fprintf(LogFile,"               Run may be unusable. Try -v20 for more information...\n");    
	polData.statusS |= WARN_INT;
    }
//	Check time and events to go - some defaults
    if (mTime < 0) mTime = 10;
    if (mEvent < 0) mEvent = 20000000;
//	Open CAMAC
    if (camacOpen() != 0) {
        fprintf(LogFile,"RHICPOL-FATAL : Cannot connect to CAMAC\n");
        polData.statusS |= (STATUS_ERROR | ERR_CAMAC);
        polexit();
    }
//	Fill some usefull information to pass to data2hbook
    for (j = strlen(cfgname); j >= 0; j--) if (cfgname[j] == '/') break;
    j++;
    sprintf(polData.daqVersionS,"%1d.%1d.%1d %s", DAQVERSION/10000, (DAQVERSION%10000)/100, DAQVERSION%100, &cfgname[j]);
    i = Conf.ETLookUp;
    if (i>2) i=2;
    sprintf(polData.cutIdS, "ET:%s_IA:%s", ETLUTNames[i], IALUTNames[Conf.IALookUp]);

    // Copy to polData ...
    polData.beamEnergyS          = beamData.beamEnergyM;
    polData.analyzingPowerS      = ANALPOW;
    polData.analyzingPowerErrorS = ANALPOWE;
    polData.numberEventsS        = mEvent;
    polData.maxTimeS             = (int) mTime;

/* If tshift in effect move sensitive window boundaries */
    if (iRamp) {
	tshift = Conf.TshiftHigh;	// always like flattop for ramp
    } else if (beamData.beamEnergyM > 50.0) {
	tshift = Conf.TshiftHigh;	// flattop
    } else {
	tshift = Conf.TshiftLow;	// injection
    }
    fprintf(LogFile, "RHICPOL-INFO : TSHIFT = %6.1f\n", tshift);
    for (i=0; i<Conf.NumChannels; i++) {
        j = (int) (SiConf[i].Window.split.Beg + tshift/Conf.WFDTUnit);
        if (j<1) j=1; 
        if(j>255) j=255;
        SiConf[i].Window.split.Beg = j;
        k = (int) (SiConf[i].Window.split.End + tshift/Conf.WFDTUnit);
        if (k<j) k=j; 
        if(k>255) k=255;
        SiConf[i].Window.split.End = k;
    }
    CreateLookup(tshift); // Must go after window correction with tshift
    
    fprintf(LogFile,"RHICPOL-INFO : %s RunID: %8.3f; E=%6.2f GeV; Target: %s\n",
	DeviceName, polData.runIdS, beamData.beamEnergyM, polData.targetIdS);

    if (setOutReg()) polexit();
    if (NoADO == 0) ProgV124((recRing & REC_BLUE) ? 1 : 0);	// set V124
    if (IStop != 0) polexit();
    if (openDataFile(fname, comment, NoADO)) polexit();
    setInhibit();
    initScalers();
    if (initWFDs()) {
        fprintf(LogFile,"RHICPOL-FATAL : Cannot find reqired CAMAC modules\n");
        polData.statusS |= (STATUS_ERROR | ERR_CAMAC | ERR_WFD);
        polexit();
    } 
	
    for (j=0; j<nLoop; j++) {
        fastInitWFDs(1);
        writeSubrun(j);
        if (NoADO == 0 && (recRing & REC_JET) == 0 && j == 0) UpdateMessage("Running...");
        setAlarm(mTime/nLoop);
	polData.startTimeS = time(NULL);
	clearVetoFlipFlop();
	resetInhibit();
	writeJetStatus();	// hopefully fast (we can not get jet state with crate inhibit)
	
	ev = getEvents(mEvent);
	
	setInhibit();
	polData.stopTimeS = time(NULL);
	clearAlarm();

	if (iSig == SIGTERM) break;	// we are supposed to quit fast

	if (iSig == SIGINT) j = nLoop - 1;

	if (iCntrlC) {
	    signal(SIGINT, alarmHandler);
	} else {
	    signal(SIGINT, SIG_IGN);
	}
	if (NoADO == 0 && (recRing & REC_JET) == 0 && j == (nLoop-1)) UpdateMessage("Reading Data...");
	signal(SIGTERM, alarmHandler);
	if (iDebug > 1000) fprintf(LogFile, "RHICPOL-INFO : Reading scalers.\n");
	readScalers();
	if (iDebug > 1000) fprintf(LogFile, "RHICPOL-INFO : Reading WFD xilinxes.\n");
	readWFD();
	if (iDebug > 1000) fprintf(LogFile, "RHICPOL-INFO : Reading WFD memory.\n");
	if (!Conf.OnlyHist) readMemory();
	signal(SIGTERM, SIG_DFL);
	signal(SIGINT, SIG_DFL);
	t = time(NULL);
	if (nLoop == 1) {
	    fprintf(LogFile, 	">>> %s Measurement finished with %9d events.\n", cctime(&t), ev);
	} else {
	    fprintf(LogFile, 	">>> %s Subrun %d finished with %9d events.\n", cctime(&t), j, ev);
	}
// Writing is done, send a message to mcr...
	if (NoADO == 0 && (recRing & REC_JET) == 0 && j == (nLoop-1)) UpdateMessage("Reading Data Finished.");
    }
    resetInhibit();
    camacClose();
    writeSubrun(-1);
    if (iSig == SIGTERM) polData.statusS |= WARN_CANCELLED;
    closeDataFile("End of RHICpol run.");
    if (NoADO == 0 && (recRing & REC_JET) == 0) UpdateStatus();
// High 16 bits of status are errors - tell this to calling script to avoid data analysis
    return (((polData.statusS) >> 16) & 0xFFFF) ? 10 : 0;
}
