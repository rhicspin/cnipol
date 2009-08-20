#define _FILE_OFFSET_BITS 64	    // to handle >2Gb files
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <envz.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>

#include "../libcmc/libcmc.h"
#include "rpolutil.h"

extern FILE *LogFile;
extern beamDataStruct beamData;
extern beamDataStruct beamOtherData;
extern targetDataStruct targetData[2];
extern polDataStruct polData;
extern wcmDataStruct wcmData;
extern wcmDataStruct wcmOtherData;
extern jetPositionStruct jetPosition;
extern int NoADO;
extern float mTime;
extern int mEvent;
extern int iPulseProg;
extern int recRing;
extern int iExtClock;
extern float ANALPOW;
extern float ANALPOWE;
extern int iCicleRun;
int MyRing = 0;

char ReadMode[4];
int OutRegBits = 0;	// we can not read it and it just writes on F16
			// and does pulse on F17
int OutRegSet = 0;	// to set output multiplexer correctly
int CrateRequired[MAXCRATES];    
int Crate[MAXCRATES];

int dataNum = 0;
FILE *OutFile;
extern configRhicDataStruct Conf;
extern SiChanStruct * SiConf;
static SiChanStruct DefSiConf;

static struct {
    int yes;
    int si[4];
} WFDinCAMAC[MAXCRATES][MAXSTATIONS]; // Maximum number of CAMAC stations

int IStop;
extern int iSig;
extern int iDebug;

long memReadCount[MAXCRATES][MAXSTATIONS][4];

carbTargStat ctss = { -1, "Undef", "Undef", "Undef", "Undef"};

int RP_CommitChain(CMC_chain *ch, int C)
{
    int irc = CMC_CommitChain(ch, C);
    if (irc == -ETIMEDOUT || irc == -ENODEV) {
	fprintf(LogFile, "FATAL: USB timedout or CAMAC switched off. Aborting the run.\n");
	polData.statusS |= (STATUS_ERROR | ERR_CAMAC);
	polexit();
    }
    return irc;
}

void alarmHandler(int sig)
{
    iSig  = sig;
    if (iSig == SIGTERM) polData.statusS |= WARN_CANCELLED;
    IStop = 1;
}

void setAlarm(float mTime)
{
    struct itimerval t;
    int irc;
    IStop = 0;
    signal(SIGALRM, alarmHandler);
    signal(SIGINT,  alarmHandler);
    signal(SIGTERM, alarmHandler);
    signal(SIGQUIT, alarmHandler);
    signal(SIGHUP,  alarmHandler);
    t.it_interval.tv_sec = 0;
    t.it_interval.tv_usec = 0;
    t.it_value.tv_sec = mTime;
    t.it_value.tv_usec = (mTime - t.it_value.tv_sec) * 1000000;
    irc = setitimer(ITIMER_REAL, &t, NULL);
    if (irc != 0) {
	fprintf(LogFile, "Error setting timer (%d.%d): %s.\n",
	    t.it_value.tv_sec, t.it_value.tv_usec, 
	    strerror(errno));
	polData.statusS |= WARN_INT;
    }
}

void clearAlarm(void)
{
    struct itimerval t;
    t.it_interval.tv_sec = 0;
    t.it_interval.tv_usec = 0;
    t.it_value.tv_sec = 0;
    t.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &t, NULL);
    signal(SIGALRM, SIG_DFL);
    signal(SIGINT,  SIG_DFL);
    signal(SIGTERM, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    signal(SIGHUP,  SIG_DFL);
}

int readConfig (char * cfgname, int update) {

    char chanconf[512] = "";
    char pattern[129];
    char inclfile[512] = "";

    struct knw { 
	char name[40];	// Parameter name as in .ini file
	char type[10];	// S(string), I(int), F(float), L(logical) or B:st:len(bit fields)
	int len;	// max string length with type='S'
	void * ptr;	// address to be filled
	char def[256];	// default string value
    } KnownWords[] = {
    {"Pattern",               "S", sizeof(pattern), &pattern,                      
"0+-+-+-+-+-+-+-+-+0-+-+-+-+-+-+-+-+-0+-+-+-+-+-+-+-+-+0-+-+-+-+-+-+-+-+-0+-+-+-+-+-+-+-+-+0-+-+-+-+-+-+-+-+-00.........."},
    {"TrigEmin",              "F", 0, &Conf.TrigMin,               "200.0"},
    {"Emin",                  "F", 0, &Conf.Emin,                  "200.0"},
    {"Emax",                  "F", 0, &Conf.Emax,                  "900.0"},
    {"TrigThreshold",         "I", 0, &Conf.TRG.split.TrigThreshold, "220"},
    {"WinBegin",              "I", 0, &DefSiConf.Window.split.Beg, "16"},
    {"WinEnd",                "I", 0, &DefSiConf.Window.split.End, "40"},
    {"WFDMode",               "B:0:2",  0, &Conf.CSR,              "2"},
    {"IDiv",                  "B:2:2",  0, &Conf.CSR,              "2"},
    {"UseMemory",             "B:4:1",  0, &Conf.CSR,              "1"},
    {"2DHistFine",            "B:5:1",  0, &Conf.CSR,              "1"},
    {"Filter",                "B:8:1",  0, &Conf.CSR,              "1"},
    {"CFDThreshold",          "B:8:1",  0, &Conf.CSR,              "1"},
    {"DisableAllDelimeters",  "B:9:1",  0, &Conf.CSR,              "0"},
    {"120Bunch",              "B:10:1", 0, &Conf.CSR,              "1"},
    {"EnableTrigWin",         "B:11:1", 0, &Conf.CSR,              "0"},
    {"MemRectLookUp",         "B:12:1", 0, &Conf.CSR,              "1"},
    {"RevolutionDelimeter",   "B:13:1", 0, &Conf.CSR,              "0"},
    {"Calibration",           "S", sizeof(chanconf), &chanconf[0], ""},
    {"ETLookUp",              "I", 0, &Conf.ETLookUp,              "0"},
    {"IALookUp",              "I", 0, &Conf.IALookUp,              "0"},
    {"ETCutW",                "F", 0, &DefSiConf.ETCutW,           "8.0"},
    {"IACutW",                "F", 0, &DefSiConf.IACutW,           "20.0"},
    {"AnalyzingPower",        "F", 0, &ANALPOW,                    "0.01212"},
    {"AnalyzingPowerError",   "F", 0, &ANALPOWE,                   "0.00606"},
    {"WFDTUnit",              "F", 0, &Conf.WFDTUnit,              "2.369"},
    {"TOFLength",             "F", 0, &Conf.TOFLength,             "15.0"},
    {"BZDelay",               "I", 0, &Conf.BZDelay,               "1"},
    {"DelimPrescaler",        "I", 0, &Conf.DelimPrescaler,        "100"},
    {"OnlyHistograms",        "I", 0, &Conf.OnlyHist,               "0"},
    {"NumChannels",           "I", 0, &Conf.NumChannels,           "50"},
    {"NScaler",               "A", 0, &Conf.CrSc,                  "-1.23"},	// -1 2/06 - rlg
    {"NOutput",               "A", 0, &Conf.CrOut,                 "-1.22"},	// -1 2/06 - rlg
    {"NInput",                "A", 0, &Conf.CrIn,                  "-1.1"},	// -1 2/06 - rlg
    {"SleepFraction",	      "F", 0, &Conf.SleepFraction,	   "0.0"},
    {"CicleTime",	      "F", 0, &Conf.CicleTime,	   	   "0.1"},
    {"AtomicNumber",	      "F", 0, &Conf.AtomicNumber,	   "12.0"},
    {"ReadMode",              "S", sizeof(ReadMode), &ReadMode[0], "D2"},
    {"Ring",		      "I", 0, &MyRing,			   "0"},
    {"OutReg",	      	      "I", 0, &OutRegSet,		   "0"},
    {"FPGAVersion",           "I", 0, &Conf.FPGAVersion,           "9"},
    {"JetDelay",              "I", 0, &Conf.JetDelay,              "0x520"}
    };

    FILE * fconf;
    char *ENVZ, *ENVZC;
    size_t ENVLEN, ENVLENC;
    char *buf, *bufc;
    struct stat finfo;
    int i, j, k, ii, mask;
    char * endptr;
    char str[512];

    if ((fconf=fopen(cfgname, "rt")) == NULL) return 1;
    
    fprintf(LogFile, "RHICPOL-INFO : Reading configuration from file %s.\n",
	cfgname);
// List the contents of each configuration file...
    while (!feof(fconf)) {
     fgets(str, 512, fconf);
     if (!feof(fconf)) fprintf(LogFile,"%s",str);
    }
    fprintf(LogFile,"\n******************************************************************\n");
    rewind(fconf);
    
    stat(cfgname, &finfo);
    buf = (char *)malloc(finfo.st_size);
    if (buf==NULL) {
	fprintf(LogFile, "RHICPOL-FATAL : Cannot allocate memory for configuration\n");
	polData.statusS |= (STATUS_ERROR | ERR_CONF);
	polexit();
    }
    fread(buf, 1, finfo.st_size, fconf);
    argz_create_sep(buf,'\n', &ENVZ, &ENVLEN);
    free(buf);
    fclose(fconf);
 
    if (!update) {
	memset(&Conf, 0, sizeof(Conf));
	memset(&DefSiConf, 0, sizeof(DefSiConf));
    }
 
/* General decode */
    for (i=0;i<sizeof(KnownWords)/sizeof(struct knw);i++) {
	if (!(buf=envz_get(ENVZ, ENVLEN, KnownWords[i].name))) {
	    if (update) continue;
	    buf = KnownWords[i].def;
	}
	if (iDebug > 1500) fprintf(LogFile, "RHICPOL-INFO : configuration: %s = %s\n", 
	    KnownWords[i].name, buf);
	switch (KnownWords[i].type[0]) {
	  case 'S' :
	    if (((char *)buf)[0] != '"')
		strncpy(KnownWords[i].ptr, buf, KnownWords[i].len);
	    else {
		((char *)buf)[strlen(buf)-1]='\0';
		strncpy(KnownWords[i].ptr, &((char *)buf)[1], KnownWords[i].len);
	    }
	    if (strlen((char *)buf) > KnownWords[i].len) {
		fprintf(LogFile,"RHICPOL-WARN : Parameter %s too long, truncated\n",
		    KnownWords[i].name);
		polData.statusS |= WARN_INT;
	    }
	    break;
	  case 'I' :
	    ((int *)KnownWords[i].ptr)[0] = strtol(buf, NULL, 0);
	    break;
	  case 'F' :
	    ((float *)KnownWords[i].ptr)[0] = strtod(buf, NULL);
	    break;
	  case 'A' :
	    ((short int *)KnownWords[i].ptr)[0] = (short int) strtol(buf, &endptr, 0);
	    endptr++;
	    ((short int *)KnownWords[i].ptr)[1] = (short int) strtol(endptr, NULL, 0);
	    break;
	  case 'L' :
	    ((int *)KnownWords[i].ptr)[0] = (toupper(((char *)buf)[0]) == 'T') ? 1 : 0;
	    break;
	  case 'B' :
	    ii = strtol(&KnownWords[i].type[2], &endptr, 0);
	    endptr++;
	    j = strtol(endptr, NULL, 0);
	    for (k=ii, mask=0; k<ii+j; k++) mask |= (1<<k);
	    ((int *)KnownWords[i].ptr)[0] &= ~mask;
	    ((int *)KnownWords[i].ptr)[0] |= (strtol(buf, NULL, 0) << ii) & mask;
	    break;
	  default :
	    fprintf(LogFile,"RHICPOL-WARN : Unknown data type in config name %s: %c\n",
		KnownWords[i].name, KnownWords[i].type);
	    polData.statusS |= WARN_INT;
	    break;
	}
    }

/* Particular parameters addition */
    if (!update || envz_get(ENVZ, ENVLEN, "Pattern")) {
	memset(&Conf.Pattern, 0, sizeof(Conf.Pattern));
	j = strlen(pattern);
	if (j > sizeof(Conf.Pattern)) {
	    fprintf(LogFile, "RHICPOL-WARN : Length of specified pol. pattern >%d\n", sizeof(Conf.Pattern));
	    polData.statusS |= WARN_INT;
	    j = sizeof(Conf.Pattern);
	}
	for (i=0; i<j; i++) switch (pattern[i]) {
	case '0' : 
	    Conf.Pattern[i] = 3; 
	    break;
	case '+' : 
	    Conf.Pattern[i] = 1; 
	    break;
	case '-' : 
	    Conf.Pattern[i] = 2; 
	    break;
	case '.' : 
	    Conf.Pattern[i] = 0; 
	    break;
	default :
	    fprintf(LogFile, "RHICPOL-WARN : Unknown character %c found in pol. pattern specification.\n", pattern[i]);
	    polData.statusS |= WARN_INT;
	    break;
	}
    }
/* Individual channels */
    /* Another Numchannels directive makes new Si configuration !!! */
    if (!update || envz_get(ENVZ, ENVLEN, "NumChannels")) {
	if (SiConf != NULL) { free(SiConf); SiConf = NULL; }
	SiConf = (SiChanStruct *)malloc(Conf.NumChannels*sizeof(SiChanStruct));
	for (i=0;i<Conf.NumChannels;i++) 
	    memcpy(&SiConf[i], &DefSiConf, sizeof(DefSiConf));
    }
    
//	Update all setttings for particular parameters
    if (update) for (i=0;i<Conf.NumChannels;i++) {
	if (buf=envz_get(ENVZ, ENVLEN, "WinBegin"))
	    SiConf[i].Window.split.Beg = strtol(buf, NULL, 0);
	if (buf=envz_get(ENVZ, ENVLEN, "WinEnd"))
	    SiConf[i].Window.split.End = strtol(buf, NULL, 0);
	if (buf=envz_get(ENVZ, ENVLEN, "ETCutW"))
	    SiConf[i].ETCutW = strtod(buf, NULL);
	if (buf=envz_get(ENVZ, ENVLEN, "IACutW"))
	    SiConf[i].IACutW = strtod(buf, NULL);
    }
    
/*
 * No longer needed since contents of ini files are now written to
 * the log file. - Ron 8/20/2009
//  Write WinBegin, WinEnd values into LogFile... Ron - 2/15/2008
	if (buf=envz_get(ENVZ, ENVLEN, "WinBegin")) 
        fprintf(LogFile,"WinBegin = %d\n", strtol(buf, NULL, 0));
	if (buf=envz_get(ENVZ, ENVLEN, "WinEnd"))
        fprintf(LogFile,"WinEnd = %d\n", strtol(buf, NULL, 0));
*/

//	Process channels
    for (i=0;i<Conf.NumChannels;i++) {
	char sss[] = {"ChannelXX"};
	sprintf(&sss[7],"%2.2d",i+1);
	if (buf=envz_get(ENVZ, ENVLEN, sss)) {
	    SiConf[i].CrateN = strtol(buf, (char**)&buf, 10);
	    buf++;
	    SiConf[i].CamacN = strtol(buf, (char**)&buf, 10);
	    buf++;
	    SiConf[i].VirtexN = strtol(buf, (char**)&buf, 10);
	    argz_create_sep(buf,' ', &ENVZC, &ENVLENC);
	    if (buf=envz_get(ENVZC, ENVLENC, "WinBegin"))
		SiConf[i].Window.split.Beg = strtol(buf, NULL, 0);
	    if (buf=envz_get(ENVZC, ENVLENC, "WinEnd"))
		SiConf[i].Window.split.End = strtol(buf, NULL, 0);
	    if (buf=envz_get(ENVZC, ENVLENC, "ETCutW"))
		SiConf[i].ETCutW = strtod(buf, NULL);
	    if (buf=envz_get(ENVZC, ENVLENC, "IACutW"))
		SiConf[i].IACutW = strtod(buf, NULL);
	    free(ENVZC);
	} 
    }
//	Process includes
    for (i=0; i<16;i++) {
	char sss[] = {"IncludeXX"};
	sprintf(&sss[7],"%2.2d",i);
	if (buf=envz_get(ENVZ, ENVLEN, sss)) {
	    j = strcspn(buf, " \t\n\r");	// remove end scpaces (comments ?)
	    if (j > 0) {
		buf[j] = '\0';
		readConfig(buf, CFG_UPDATE);
	    }
	}
    }
    free(ENVZ);
    
    if (strlen(chanconf) > 0) {
	if ((fconf=fopen(chanconf, "rt")) == NULL) {
	    fprintf(LogFile,"RHICPOL-FATAL : Cannot open channel calibration file %s.\n",
    		chanconf);
	    polData.statusS |= (STATUS_ERROR | ERR_CONF);
	    polexit();
	} 
	fprintf(LogFile,"RHICPOL-INFO : Reading calibration parameters from file %s\n", chanconf);
// List the contents of the calibration file...
    while (!feof(fconf)) {
     fgets(str, 512, fconf);
     if (!feof(fconf)) fprintf(LogFile,"%s",str);
    }
    fprintf(LogFile,"\n******************************************************************\n\n");
    rewind(fconf);	
	
	stat(chanconf, &finfo);
	buf = (char *)malloc(finfo.st_size);
	fread(buf, 1, finfo.st_size, fconf);
	argz_create_sep(buf,'\n', &ENVZC, &ENVLENC);
	free(buf);
	fclose(fconf);

	for (i=0;i<Conf.NumChannels;i++) if (SiConf[i].CamacN != 0) {
	    char sss[] = {"ChannelXX"};
	    float *ptr = &SiConf[i].t0;
	    sprintf(&sss[7],"%2.2d",i+1);
	    if (bufc=envz_get(ENVZC, ENVLENC, sss)) {
		for (j=0, k=0; j < CALIB_CONSTANTS; j++) {
		    if (bufc == NULL) k++;
		    ptr[j] = strtod(bufc, (char **)&bufc);
		}
		if (k) {
		    fprintf(LogFile,"RHICPOL-ERR : %s : missing %d initializer(s) in calibration data file\n", sss, k);
		    polData.statusS |= (STATUS_ERROR | ERR_CONF);
		    free(ENVZC);
		    return -10;
		}
	    } 
	}
	free(ENVZC);
    }
//	update Ring infromation
    switch (MyRing) {
    case 1 :
	recRing = REC_YELLOW;
	break;
    case 2 :
	recRing = REC_BLUE;
	break;
    case 3 :
	recRing = REC_YELLOW | REC_JET;
	break;
    case 4 :
	recRing = REC_BLUE | REC_JET;
	break;
    default :
	fprintf(LogFile, "RHICPOL-FATAL : None or invalid ring specified. Exitting.\n");
	return -20;
    }
    
    return 0;
}

int CheckConfig() {
    char ModNames[][10]={"Raw","SubMean","AT","ReadAll"};
    char Enb[][10]={"DIS","ENB"};
    char YN[][10]={"NO","YES"};
    char H2d[][10]={"Coarse","Fine"};
    char MLU[][10]={"Regular","Rect"};
    char Pat[4]={'.','+','-','0'};
    char ETLUTNames[][10]={"Kine","Rect","Thresh"};
    char IALUTNames[][10]={"Line","Open"};
    char *ringMode;
    char *particleName;
    char str[20];
    int iRC=0, i, nch;
    switch (MyRing) {
    case 1 :
	ringMode = "YELLOW";
	break;
    case 2 :
	ringMode = "BLUE";
	break;
    case 3 :
	ringMode = "JET-YELLOW";
	break;
    case 4 :
	ringMode = "JET-BLUE";
	break;
    default :
	iRC++;
	fprintf(LogFile, "RHICPOL-FATAL : You must specify the ring !\n");
	ringMode = "INVALID";
	break;
    }
    switch ((int)Conf.AtomicNumber) {
    case 1:
	particleName = "proton";
	break;
    case 2:
	particleName = "deutron";
	break;
    case 12:
	particleName = "carbon";
	break;
    default:
	sprintf(str, "A=%5.1f", Conf.AtomicNumber);
	particleName = str;
    }
    fprintf(LogFile,"RHICPOL-INFO : Running for %s in %s ring (%5.1f-%5.1f KeV) in mode:\n", 
	particleName, ringMode, Conf.Emin, Conf.Emax);
    fprintf(LogFile," %s Bunch:%d Mem:%s 2D-Hist:%s OnlyHist:%s Filter:%s MemLUp:%s iDiv=%d\n",
	ModNames[Conf.CSR.split.Mode], 
	60*(1+Conf.CSR.split.B120),
	Enb[Conf.CSR.split.MemEn], 
	H2d[Conf.CSR.split.Mod2D],
	YN[Conf.OnlyHist],
	Enb[Conf.CSR.split.Filter],
	MLU[Conf.CSR.split.RMemLUp],
	Conf.CSR.split.iDiv
	);
    fprintf(LogFile,"Trigger threshold for enegry : %5.1f keV\n",Conf.TrigMin);
    if (Conf.CSR.split.MemEn == 0) {
	fprintf(LogFile,"RHICPOL-WARN : Modes without memory are not supported. Enabling memory.\n");
	polData.statusS |= WARN_INT;
	Conf.CSR.split.MemEn = 1;
	iRC++; 	
    }
    if (Conf.FPGAVersion == 10 && Conf.CSR.split.Mode == 0) {
	fprintf(LogFile,"RHICPOL-WARN : V10 FPGA does not support RAW mode. Switching to waveform mode.\n");
	polData.statusS |= WARN_INT;
	Conf.CSR.split.Mode = 1;
	iRC++; 		
    }
    if (Conf.BZDelay < 0) {
	fprintf(LogFile,"RHICPOL-WARN : BZDelay must be >=0. Set to 0.\n");
	polData.statusS |= WARN_INT;
	Conf.BZDelay = 0;
	iRC++; 	
    }
    if (Conf.BZDelay > 14) {
	fprintf(LogFile,"RHICPOL-WARN : BZDelay must be <=14. Set to 14.\n");
	polData.statusS |= WARN_INT;
	Conf.BZDelay = 14;
	iRC++; 	
    }
    if (iDebug>10) {
	fprintf(LogFile," TrigThreshold:%d BZDelay:%d DelimPrescaler:%d Lc2551L:%d.%d OutReg:%d.%d\n",
	    Conf.TRG.split.TrigThreshold,
	    Conf.BZDelay-1,
	    Conf.DelimPrescaler,
	    Conf.CrSc, Conf.NSc, Conf.CrOut, Conf.NOut
	);
    }
    i=Conf.ETLookUp;
    if (i>2) i=2;
    fprintf(LogFile,"RHICPOL-INFO : Lookups in effect: ET:%s IA:%s\n",
	ETLUTNames[i], IALUTNames[Conf.IALookUp]);
    fprintf(LogFile,"RHICPOL-INFO : Polarization pattern found :\n");
    fprintf(LogFile," Bunches 1-60   : ");
    for (i=0;i<60;i++) fprintf(LogFile,"%c", Pat[Conf.Pattern[i]]);
    fprintf(LogFile,"\n Bunches 61-120 : ");
    for (;i<120;i++) fprintf(LogFile,"%c", Pat[Conf.Pattern[i]]);
    fprintf(LogFile,"\n");
    fprintf(LogFile,"RHICPOL-INFO : Channel configuration found (max:%d):\n", Conf.NumChannels);
    if (iDebug > 10) fprintf(LogFile, 
    " ChN Cr St Vir Beg End Amin Amax ETCut IACut    t0 ecoef edead    A0    A1 acoef\n");
	
    for (i=0, nch=0; i<Conf.NumChannels; i++) {
	if (SiConf[i].CamacN == 0 || SiConf[i].VirtexN == 0 || SiConf[i].CrateN < 0) {
	    SiConf[i].CrateN = -1;
	    SiConf[i].CamacN = 0;
	    SiConf[i].VirtexN = 0;
	    continue;
	}
	if (iDebug > 10) {
	    fprintf(LogFile, 
		" %3d %2d %2d %3d %3d %3d %4.0f %4.0f %5.1f %5.1f %5.1f %5.3f %5.1f %5.1f %5.1f %5.3f\n",
	    i+1, SiConf[i].CrateN, SiConf[i].CamacN, SiConf[i].VirtexN,
	    SiConf[i].Window.split.Beg, SiConf[i].Window.split.End,
	    (Conf.Emin-SiConf[i].edead)/SiConf[i].ecoef,
	    (Conf.Emax-SiConf[i].edead)/SiConf[i].ecoef,
	    SiConf[i].ETCutW, SiConf[i].IACutW,
	    SiConf[i].t0, SiConf[i].ecoef, SiConf[i].edead,
	    SiConf[i].A0, SiConf[i].A1, SiConf[i].acoef
	    );
	}
	if ( SiConf[i].VirtexN != 0 &&
	    SiConf[i].ETCutW != 0 && SiConf[i].IACutW != 0  &&
	    SiConf[i].t0 != 0     && SiConf[i].ecoef != 0   &&
	    SiConf[i].edead != 0  && SiConf[i].A0 != 0      &&
	    SiConf[i].A1 != 0     && SiConf[i].acoef != 0 
	) {
	    if (iDebug <= 10) {
		fprintf(LogFile,"%2.2d.",i+1);
		if (nch%20 == 19) fprintf(LogFile,"\n");
	    }
	    nch++;
	} else {
	    SiConf[i].CrateN = -1;
	    SiConf[i].CamacN = 0;
	    SiConf[i].VirtexN = 0;
	    iRC++;
	}
    }
    if (nch != 0 ) {
	fprintf(LogFile,"OK\n %d Total.\n", nch);
    } else { 
	fprintf(LogFile,"RHICPOL-WARN : No channels configured\n"); 
	polData.statusS |= WARN_INT;
	iRC++; 
    }
//	Check crates configuration
    memset(CrateRequired, 0, sizeof(CrateRequired));
    if (Conf.CrOut >= 0 && Conf.CrOut < MAXCRATES) {
	CrateRequired[Conf.CrOut] = 1;
    } else {
	fprintf(LogFile,"RHICPOL-WARN : Wrong crate number %d for the output register\n", Conf.CrOut); 
	polData.statusS |= WARN_INT;
	iRC++;
    }
    if (Conf.CrSc >= 0 && Conf.CrSc < MAXCRATES) {
	CrateRequired[Conf.CrSc] = 1;
    } else if (Conf.CrSc >= 0) {
	fprintf(LogFile,"RHICPOL-WARN : Wrong crate number %d for the scalers\n", Conf.CrSc); 
	polData.statusS |= WARN_INT;
	iRC++;
    }
    for (i=0; i<Conf.NumChannels; i++) {
	if (SiConf[i].CrateN >= 0 && SiConf[i].CrateN < MAXCRATES) { 
	    CrateRequired[SiConf[i].CrateN] = 1;
	} else if (SiConf[i].CrateN < 0) {
	    continue;
	} else {
	    fprintf(LogFile,"RHICPOL-WARN : Wrong crate number %d for si channel %d\n", Conf.CrSc, i+1); 
	    polData.statusS |= WARN_INT;
	    iRC++;
	}
    }
    
    if (iCicleRun != 0 && Conf.NIn == 0) {
	fprintf(LogFile,"RHICPOL-WARN : Cycle jet run requested, but no information about input register position.\n"); 
	iCicleRun = 0;
	polData.statusS |= WARN_INT;
	iRC++;
    }
    
    return iRC;
}

void CreateLookup(float tshift)
{
    int i, j, k;
    int iAmin, iAmax;
    int iTmin, iTmax;
    float tof;
    
    for (i=0;i<Conf.NumChannels;i++) {

	if (SiConf[i].CrateN == 0 || SiConf[i].CamacN == 0 || SiConf[i].VirtexN == 0) {
	    for (j=0; j<512; j++) SiConf[i].LookUp[j] = 0x00FF; // Closed
	    continue;
	}

	/* ET LookUp */
	
	/* Amplitude range */
	iAmin = (Conf.Emin-SiConf[i].edead)/SiConf[i].ecoef;
	iAmax = (Conf.Emax-SiConf[i].edead)/SiConf[i].ecoef;
	if (iAmin < 0) iAmin = 0; if (iAmin > 255) iAmin = 255; 
	if (iAmax < iAmin) iAmax = iAmin; if (iAmax > 255) iAmax = 255; 

	switch (Conf.ETLookUp) {
	  case 0 :	// Carbon Kinematic
	    for (j=0; j<iAmin; j++) SiConf[i].LookUp[j] = 0x00FF; // Closed
	    for (   ; j<iAmax; j++) {
		tof = 22.7 * sqrt(Conf.AtomicNumber) * Conf.TOFLength / 
		    sqrt(j * SiConf[i].ecoef + SiConf[i].edead); // ns
		iTmin = 2 * (tof + SiConf[i].t0 - SiConf[i].ETCutW + tshift) / Conf.WFDTUnit;
		iTmax = 2 * (tof + SiConf[i].t0 + SiConf[i].ETCutW + tshift) / Conf.WFDTUnit;
		if (iTmin < SiConf[i].Window.split.Beg * 2 - 1) iTmin = SiConf[i].Window.split.Beg * 2 - 1; 
		if (iTmin > 255) iTmin = 255; 
		if (iTmax < iTmin) iTmax = iTmin; if (iTmax > 255) iTmax = 255; 
		SiConf[i].LookUp[j] = ((iTmax << 8) | iTmin); // Open
	    }
	    for (   ; j<256;   j++) SiConf[i].LookUp[j] = 0x00FF; // Closed
	    break;
	  case 1 :	// Rectangular
	    /* Window limits already contain tshift at lookup creation */
	    iTmin = SiConf[i].Window.split.Beg * 2 - 1;
	    iTmax = SiConf[i].Window.split.End * 2;
	    if (iTmin < 0) iTmin = 0; if (iTmin > 255) iTmin = 255; 
	    if (iTmax < iTmin) iTmax = iTmin; if (iTmax > 255) iTmax = 255; 
	    for (j=0; j<iAmin; j++) SiConf[i].LookUp[j] = 0x00FF; // Closed
	    for (   ; j<iAmax; j++) SiConf[i].LookUp[j] = ((iTmax << 8) | iTmin); // Open
	    for (   ; j<256;   j++) SiConf[i].LookUp[j] = 0x00FF; // Closed
	    break;
	  default :	// Threshold
	    iAmin=Conf.ETLookUp;
	    if (iAmin < 0) iAmin = 0; if (iAmin > 255) iAmin = 255; 
	    for (j=0; j<iAmin; j++) SiConf[i].LookUp[j] = 0x00FF; // Closed
	    for (   ; j<256;   j++) SiConf[i].LookUp[j] = 0xFF00; // Fully open
	    break;
	}
	
	/* IA LookUp */
	
	switch (Conf.IALookUp) {
	  case 0 :	// Linear
	    for (j=0; j<256; j++) {
		iAmin = (SiConf[i].A0 + SiConf[i].A1 * j - SiConf[i].IACutW) / 
			(Conf.CSR.split.iDiv +2);
		iAmax = (SiConf[i].A0 + SiConf[i].A1 * j + SiConf[i].IACutW) / 
			(Conf.CSR.split.iDiv +2);
		if (iAmin < 0) iAmin = 0; if (iAmin > 255) iAmin = 255; 
		if (iAmax < iAmin) iAmax = iAmin; if (iAmax > 255) iAmax = 255; 
		SiConf[i].LookUp[j+256] = ((iAmax << 8) | iAmin); // Open
	    }
	    break;
	  case 1 :	// Fully open
	  default :
	    for (j=256; j<512; j++) SiConf[i].LookUp[j] = 0xFF00; // Fully open
	    break;
	}
    }
}


int polWrite(recordHeaderStruct *header, long *data)
{
    int irc;
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    if (OutFile == NULL) return 2;

//	Thread protected portion start:
    pthread_mutex_lock(&mutex);
    header->num = dataNum++;
    irc = fwrite(header, sizeof(recordHeaderStruct), 1, OutFile);
    if (irc != 1) {
	fprintf(LogFile, "RHICPOL-FATAL : Writing output file error (header) rec=%8.8X: %s.\n",
	    header->type, strerror(errno));
	polData.statusS |= (STATUS_ERROR | ERR_INT);
	fclose(OutFile);
	OutFile = NULL;
	pthread_mutex_unlock(&mutex);
	return 1;
    }
    irc = fwrite(data, header->len - sizeof(recordHeaderStruct), 1, OutFile);
    if (irc != 1) {
	fprintf(LogFile, "RHICPOL-FATAL : Writing output file error (body) rec=%8.8X: %s.\n",
	    header->type, strerror(errno));
	polData.statusS |= (STATUS_ERROR | ERR_INT);
	fclose(OutFile);
	OutFile = NULL;
	pthread_mutex_unlock(&mutex);
	return 1;
    }
    fflush(OutFile);
    pthread_mutex_unlock(&mutex);
    return 0;	
}

void camacClose(void)
{
    int i, j;
    CMC_chain *ch;
    OutRegBits = 0;
    if (Crate[Conf.CrOut] >= 0) 
	CMC_Single(Crate[Conf.CrOut], Conf.NOut, 16, 0, OutRegBits);
    ch = CMC_AllocateChain(0, 0);
    for (j=0; j<MAXCRATES; j++) if (Crate[j] >= 0) {
	CMC_ResetChain(ch);
	CMC_Add2Chain(ch, CMC_CMDDATA + 0);		// we will always write 0 - internal clocks
	for(i=0; i<MAXSTATIONS; i++) {
	// Switch to internal clocks to be safe in case RF goes out
	// whatever the configuration is
	    if (!((i == Conf.NSc && j == Conf.CrSc) || (i == Conf.NOut && j == Conf.CrOut))) 
		CMC_Add2Chain(ch, CMC_STDNFA(i, 16, 8)); 
	}
	RP_CommitChain(ch, Crate[j]);
	CMC_Close(Crate[j]);
	Crate[j] = -1;
    }
    CMC_ReleaseChain(ch);
}

int camacOpen(void)
{
    int i, j, k;
    static int first = 1;
    
    if (!first) {
	camacClose();	// always try to close CAMAC before reopening
    } else {
	memset(Crate, -1, sizeof(Crate));
    }
    if (iDebug > 700) {
	fprintf(LogFile, "Crates requested :");
	for (i = 0; i<MAXCRATES; i++) if (CrateRequired[i]) fprintf(LogFile, " %d", i);
	fprintf(LogFile, "\n");
    }
    j = 0;
    for (i = 0; i<MAXCRATES; i++) if (CrateRequired[i]) {	// open only if we need
	Crate[i] = CMC_Open(i);
	if (Crate[i] < 0) {
	    fprintf(LogFile, "RHICPOL-ERR : Can not open camac crate %d because %s.\n",
		i, strerror(-Crate[i]));
	    j++;
	    Crate[i] = -1;
	}
    }
    if (j != 0) {
	polData.statusS |= (STATUS_ERROR | ERR_CAMAC);
	return -1;
    }
    
    OutRegBits = 0;
    if (Crate[Conf.CrOut] >= 0) CMC_Single(Crate[Conf.CrOut], Conf.NOut, 16, 0, OutRegBits);
    
    /* Fill in CAMAC table */
    memset(&WFDinCAMAC, -1, sizeof(WFDinCAMAC));
    for (j=0; j<MAXCRATES; j++) for (i=0; i<MAXSTATIONS; i++) WFDinCAMAC[j][i].yes = 0;
    for (i=0; i<Conf.NumChannels; i++) {
	// Camac station always goes as is, starting from 1
	if (SiConf[i].CamacN >= 0) WFDinCAMAC[SiConf[i].CrateN][SiConf[i].CamacN].yes = 1;
	if (SiConf[i].VirtexN != 0) 
	    WFDinCAMAC[SiConf[i].CrateN][SiConf[i].CamacN].si[SiConf[i].VirtexN-1] = i;
    }
    
    if (iDebug > 30) for (k = 0; k < MAXCRATES; k++) if (CrateRequired[k]) {
	fprintf(LogFile, "RHICPOL-INFO : Camac allocation table for crate %d\n", k);
	for (i=1; i<MAXSTATIONS; i++) fprintf(LogFile," %2d",i);
	fprintf(LogFile,"\n");
	for (i=1; i<MAXSTATIONS; i++) 
	    fprintf(LogFile,"  %c", (WFDinCAMAC[k][i].yes) ? '*' : ' ');
	fprintf(LogFile,"\n");
	for (j=0; j<4; j++) {
	    for (i=1; i<MAXSTATIONS; i++) {
		if (WFDinCAMAC[k][i].si[j]>=0) 
		    fprintf(LogFile," %2d", WFDinCAMAC[k][i].si[j]+1);
		else
		    fprintf(LogFile,"   ");
	    }
	    fprintf(LogFile,"\n");
	}
    }
    return 0;
}

void setOutInhibit(void)
{
    if (Conf.CrOut < 0 || Crate[Conf.CrOut] < 0) return;
    OutRegBits |= OUT_INHIBIT;
    CMC_Single(Crate[Conf.CrOut], Conf.NOut, 16, 0, OutRegBits);
}

void setInhibit(void)
{
    int k;
    setOutInhibit();
    for (k = 0; k < MAXCRATES; k++) if (CrateRequired[k]) CMC_Single(Crate[k], 30, 26, 9, 0);
}

void resetOutInhibit(void)
{
    int k;
    if (Conf.CrOut < 0 || Crate[Conf.CrOut] < 0) return;
    OutRegBits &= ~OUT_INHIBIT;
    CMC_Single(Crate[Conf.CrOut], Conf.NOut, 16, 0, OutRegBits);
}

void resetInhibit(void)
{
    int k;
    for (k = 0; k < MAXCRATES; k++) if (CrateRequired[k]) CMC_Single(Crate[k], 30, 24, 9, 0);
    resetOutInhibit();
}

void clearVetoFlipFlop(void)
{
    CMC_chain *ch;
    ch = CMC_AllocateChain(0, 0);
    CMC_Add2Chain(ch, OutRegBits | OUT_CLRLAM | CMC_CMDDATA);
    CMC_Add2Chain(ch, CMC_STDNFA(Conf.NOut, 16, 0));
    CMC_Add2Chain(ch, OutRegBits | CMC_CMDDATA);
    CMC_Add2Chain(ch, CMC_STDNFA(Conf.NOut, 16, 0));
    RP_CommitChain(ch, Crate[Conf.CrOut]);
    CMC_ReleaseChain(ch);
}

void pulseDelimiter(void)
{
    CMC_chain *ch;
    ch = CMC_AllocateChain(0, 0);
    CMC_Add2Chain(ch, OutRegBits | OUT_DELIM | CMC_CMDDATA);
    CMC_Add2Chain(ch, CMC_STDNFA(Conf.NOut, 16, 0));
    CMC_Add2Chain(ch, OutRegBits | CMC_CMDDATA);
    CMC_Add2Chain(ch, CMC_STDNFA(Conf.NOut, 16, 0));
    RP_CommitChain(ch, Crate[Conf.CrOut]);
    CMC_ReleaseChain(ch);
}


void nsleep(double time)
{
    struct timespec t;
    t.tv_sec = time;
    t.tv_nsec = (time - t.tv_sec) * 1000000000;
    nanosleep(&t, NULL);
}

void setRunGo(void)
{
 OutRegSet |= OUT_RUN;
 OutRegBits = OutRegSet;
 CMC_Single(Crate[Conf.CrOut], Conf.NOut, 16, 0, OutRegBits);
}

int setRing(void)
{
// Get the MUX setting for the current polarimeter...
	int n = atoi(getenv("MUX"));
	fprintf(LogFile,"RHICPOL-INFO : MUX setting = %d\n", n);

	if (n == 1) OutRegSet |= OUT_MUXA;
	else if (n == 2) OutRegSet |= OUT_MUXB;

    OutRegBits = OutRegSet;
    if (iCicleRun) OutRegBits |= OUT_JETINH;
    CMC_Single(Crate[Conf.CrOut], Conf.NOut, 16, 0, OutRegBits);
    return 0;
}

void pulseAllProgs(void)
{
    int i, j;
    CMC_chain *ch;
    ch = CMC_AllocateChain(0, 0);
    for (j=0; j<MAXCRATES; j++) if (CrateRequired[j]) {
	CMC_ResetChain(ch);
	CMC_Add2Chain(ch, CMC_CMDDATA | 8); 	// Pulse PROG
	for(i=0; i<MAXSTATIONS; i++) if (WFDinCAMAC[j][i].yes) 
	    CMC_Add2Chain(ch, CMC_STDNFA(i, 16, 8));
	RP_CommitChain(ch, Crate[j]);
    }
    CMC_ReleaseChain(ch);
    nsleep(4.0); 				// should be enough
}

/************************************************************************
 *	WFD V9_200 CSR bits						*
 *   0,1   - WFD mode : RAW(0), SUB(1), AT(2), ALL(3)			*
 *   2,3   - integral divider (shift right)				*
 *   4     - enable output to SDRAM					*
 *   5     - 2-dim histogram mode 					*
 *   6,7   - channel ID							*
 *   8     - enable 3-point filter, v10 : CFD threshold			*
 *   9     - enable maximum correction, v10 : Disable all delimeters	*
 *   10    - 120/60 bunch mode 						*
 *   11    - 140 MHz output ON (obsolete), v10, jet mode: enable window	*
 *   12    - rectangular lookup for events going to memory (AT mode only)*
 *   13    - external (0) /revolution (1) delimeter switch		*
 *   14 (RO) - dumping histogramms to emmory is active			*
 *   15 (RO) - delimeter to memory is active				*
 ************************************************************************/
int initWFDs(void)
{
    int cr, i, j, k, ii, jj, iRC=0, disFlag, nSi;
    CMC_chain *ch;
//	Pulse Prog if required
    if (iPulseProg) pulseAllProgs();
    ch = CMC_AllocateChain(0, MAXSTATIONS);
//	check modules present and Xilinxes are DONE
    for (cr=0; cr<MAXCRATES; cr++) if (CrateRequired[cr]) {
	CMC_ResetChain(ch);
	for(i=0; i<MAXSTATIONS; i++) if (WFDinCAMAC[cr][i].yes) 
	    CMC_Add2Chain(ch, CMC_STDNFA(i, 0, 8));
	RP_CommitChain(ch, Crate[cr]);
	ii = 0;
	for(i=0; i<MAXSTATIONS; i++) if (WFDinCAMAC[cr][i].yes) {
	    disFlag = 0;
	    j = ch->rdata[ii];
	    ii++;
	    if ((j & (CMC_QMASK | CMC_XMASK)) != (CMC_QMASK | CMC_XMASK)) {
	    /* Module not found */
		fprintf(LogFile, "RHICPOL-ERR : Cannot find WFD module at station %d.%d\n", cr, i);
		polData.statusS |= (STATUS_ERROR | ERR_CAMAC | ERR_WFD);
		disFlag=1;
		iRC++;
	    } else if ((j & 0x8000) == 0) { 
	    /* Not done, try to repulse */
		CMC_Single(Crate[cr], i, 16, 8, 8);	// pulse prog - now one by one
		for (k=0;k<40;k++) {
		    if (CMC_Single(Crate[cr], i, 0, 8, 0) & 0x8000) break;
		    nsleep(0.1);
		}
		if (k == 40) {
		    fprintf(LogFile, "RHICPOL-ERR : Found unloaded WFD module at station %d.%d and failed to reload\n", cr, i);
		    polData.statusS |= ERR_WFD;
		    disFlag=1;
		    iRC++;
		}
	    }
	    if (disFlag) {
	    /* Disable module */
		WFDinCAMAC[cr][i].yes = 0;
		for (k=0;k<3;k++) 
		    if (WFDinCAMAC[cr][i].si[k] >= 0) {
			SiConf[WFDinCAMAC[cr][i].si[k]].CrateN = -1;
			SiConf[WFDinCAMAC[cr][i].si[k]].CamacN = 0;
			SiConf[WFDinCAMAC[cr][i].si[k]].VirtexN = 0;
			WFDinCAMAC[cr][i].si[k] = -1;
		    }
		continue;
	    }
	}
    }	
//	Select clock source, set BZ delay and reset dlls
    for (cr=0; cr<MAXCRATES; cr++) if (CrateRequired[cr]) {
	CMC_ResetChain(ch);
	for(i=0; i<MAXSTATIONS; i++) if (WFDinCAMAC[cr][i].yes) {
	    CMC_Add2Chain(ch, CMC_CMDDATA | iExtClock | ((Conf.BZDelay+1) << 4)); // external clocks & BZdelay
	    CMC_Add2Chain(ch, CMC_STDNFA(i, 16, 8));
	    CMC_Add2Chain(ch, CMC_CMDDATA | 0x1F);	// select all virtexes
	    CMC_Add2Chain(ch, CMC_STDNFA(i, 16, 9));
	    CMC_Add2Chain(ch, CMC_STDNFA(i, 9, 7));	// reset DLL (to be done in WFD)
	}
	RP_CommitChain(ch, Crate[cr]);
    }
    nsleep(0.005);	// 5 ms is quite enough to reset DLLs
    for (cr=0; cr<MAXCRATES; cr++) if (CrateRequired[cr]) {
	CMC_ResetChain(ch);
	for(i=0; i<MAXSTATIONS; i++) if (WFDinCAMAC[cr][i].yes) 
	    CMC_Add2Chain(ch, CMC_STDNFA(i, 9, 0));	// reset ALL
	RP_CommitChain(ch, Crate[cr]);
    }
    nsleep(0.001);	// 1 ms is quite enough
    /*	Set registers */
    for (cr=0; cr<MAXCRATES; cr++) if (CrateRequired[cr]) 
	for(i=0; i<MAXSTATIONS; i++) if (WFDinCAMAC[cr][i].yes) {
	CMC_ResetChain(ch);
	for (j=0; j<4; j++) {
	    CMC_Add2Chain(ch, CMC_CMDDATA | (1 << j));		// select virtex
	    CMC_Add2Chain(ch, CMC_STDNFA(i, 16, 9));
	    if (WFDinCAMAC[cr][i].si[j] >= 0) {
		nSi = WFDinCAMAC[cr][i].si[j];
		Conf.CSR.split.VirtN = j;			// Virtex signature in memory
		CMC_Add2Chain(ch, CMC_CMDDATA | Conf.CSR.reg);	// set CSR
		CMC_Add2Chain(ch, CMC_STDNFA(i, 17, 1));
		k = (Conf.TrigMin-SiConf[nSi].edead)/SiConf[nSi].ecoef;
		if (k < 0) k = 0; if (k > 255) k = 255;
		Conf.TRG.split.FineHistBeg = k;
		CMC_Add2Chain(ch, CMC_CMDDATA | Conf.TRG.reg);	// set trigger
		CMC_Add2Chain(ch, CMC_STDNFA(i, 17, 2));
		CMC_Add2Chain(ch, CMC_CMDDATA | SiConf[nSi].Window.reg); // set window
		CMC_Add2Chain(ch, CMC_STDNFA(i, 17, 3));
		CMC_Add2Chain(ch, CMC_CMDDATA | (Conf.JetDelay & 0xFFFF));// set long waveform 
		CMC_Add2Chain(ch, CMC_STDNFA(i, 17, 4));
		CMC_Add2Chain(ch, CMC_STDNFA(i, 9, 2));		// reset addresses
		for (k=0; k<512; k++) {				// load lookup table
		    CMC_Add2Chain(ch, CMC_CMDDATA | SiConf[nSi].LookUp[k]);
		    CMC_Add2Chain(ch, CMC_STDNFA(i, 17, 0));
		}
		for (k=0; k<15; k++) {			// load polarization pattern
		    for (ii=7, jj=0; ii>=0; ii--, jj=jj << 1) {
			jj = jj | (Conf.Pattern[k*8+ii] & 1) | ((Conf.Pattern[k*8+ii] & 2) << 7); 
		    }
		    CMC_Add2Chain(ch, CMC_CMDDATA | (jj >> 1));
		    CMC_Add2Chain(ch, CMC_STDNFA(i, 17, 5));
		}
		CMC_Add2Chain(ch, CMC_CMDDATA | 0);		// write last pattern word
		CMC_Add2Chain(ch, CMC_STDNFA(i, 17, 5));
		CMC_Add2Chain(ch, CMC_STDNFA(i, 9, 1));		// reset scalers
		CMC_Add2Chain(ch, CMC_STDNFA(i, 9, 3));		// reset revolution counter
		CMC_Add2Chain(ch, CMC_STDNFA(i, 9, 6));		// reset delimiter counter
	    } else {	// Put unused virtex into the most passive state
		CMC_Add2Chain(ch, CMC_CMDDATA | MOD_AT);	// set CSR
		CMC_Add2Chain(ch, CMC_STDNFA(i, 17, 1));
		CMC_Add2Chain(ch, CMC_CMDDATA | 0);		// set trigger
		CMC_Add2Chain(ch, CMC_STDNFA(i, 17, 2));
		CMC_Add2Chain(ch, CMC_CMDDATA | 0x0801);	// set window
		CMC_Add2Chain(ch, CMC_STDNFA(i, 17, 3));
		CMC_Add2Chain(ch, CMC_STDNFA(i, 9, 2));		// reset addresses
		for (k=0; k<512; k++) {
		    CMC_Add2Chain(ch, CMC_CMDDATA | 0x00FF);	// Closed lookup
		    CMC_Add2Chain(ch, CMC_STDNFA(i, 17, 0));	// load lookup table
		}
		for (k=0; k<16; k++) {
		    CMC_Add2Chain(ch, CMC_CMDDATA | 0);		// load empty polarization pattern
		    CMC_Add2Chain(ch, CMC_STDNFA(i, 17, 5));
		}
		CMC_Add2Chain(ch, CMC_STDNFA(i, 9, 1));		// reset scalers
	    }
	}
	RP_CommitChain(ch, Crate[cr]);
    }
/*	Enable memory	*/
    for (cr=0; cr<MAXCRATES; cr++) if (CrateRequired[cr]) {
	CMC_ResetChain(ch);
	for(i=0; i<MAXSTATIONS; i++) if (WFDinCAMAC[cr][i].yes) {
	    CMC_Add2Chain(ch, CMC_CMDDATA | 0x10);	// select memory controller
	    CMC_Add2Chain(ch, CMC_STDNFA(i, 16, 9));
	    CMC_Add2Chain(ch, CMC_CMDDATA | 0);		// disable memory functions
	    CMC_Add2Chain(ch, CMC_STDNFA(i, 16, 1));
	    CMC_Add2Chain(ch, CMC_CMDDATA | 0);		// reset address
	    CMC_Add2Chain(ch, CMC_STDNFA(i, 17, 0));
	    CMC_Add2Chain(ch, CMC_CMDDATA | 0);		// reset address
	    CMC_Add2Chain(ch, CMC_STDNFA(i, 17, 1));
	}
	CMC_Add2Chain(ch, CMC_CMDDELAY | 0x40);		// some delay ~ 100 us 
	for(i=0; i<MAXSTATIONS; i++) if (WFDinCAMAC[cr][i].yes) {	
	    CMC_Add2Chain(ch, CMC_CMDDATA | 0);		// reset address
	    CMC_Add2Chain(ch, CMC_STDNFA(i, 17, 0));
	    CMC_Add2Chain(ch, CMC_CMDDATA | 0);		// reset address
	    CMC_Add2Chain(ch, CMC_STDNFA(i, 17, 1));
	    CMC_Add2Chain(ch, CMC_CMDDATA | 1);		// enable accept data
	    CMC_Add2Chain(ch, CMC_STDNFA(i, 16, 1));
	}
	RP_CommitChain(ch, Crate[cr]);
    }
    CMC_ReleaseChain(ch);
//	wait while we reset scalers
    nsleep(0.001); // 1 ms
    return iRC;
}

void fastInitWFDs(int clr_hist)
{
    int cr, i, j;
    CMC_chain *ch;
    ch = CMC_AllocateChain(0, 0);
    for (cr=0; cr<MAXCRATES; cr++) if (CrateRequired[cr]) {
	CMC_ResetChain(ch);
	for(i=0; i<MAXSTATIONS; i++) if (WFDinCAMAC[cr][i].yes) {
	    CMC_Add2Chain(ch, CMC_CMDDATA | iExtClock | ((Conf.BZDelay+1) << 4)); // restore external clocks & BZdelay
	    CMC_Add2Chain(ch, CMC_STDNFA(i, 16, 8));
	    CMC_Add2Chain(ch, CMC_CMDDATA | 0xF);	// select all but memory
	    CMC_Add2Chain(ch, CMC_STDNFA(i, 16, 9));
	    CMC_Add2Chain(ch, CMC_STDNFA(i, 9, 2));	// RS_ADR
	    if (clr_hist != 0) {
		CMC_Add2Chain(ch, CMC_STDNFA(i, 9, 1));	// Clear histograms/scalers
	    }
	    CMC_Add2Chain(ch, CMC_CMDDATA | 0x10);	// select memory controller
	    CMC_Add2Chain(ch, CMC_STDNFA(i, 16, 9));
	    CMC_Add2Chain(ch, CMC_CMDDATA | 0);		// disable cache
	    CMC_Add2Chain(ch, CMC_STDNFA(i, 16, 1));
	    CMC_Add2Chain(ch, CMC_CMDDATA | 0);		// reset address
	    CMC_Add2Chain(ch, CMC_STDNFA(i, 17, 0));
	    CMC_Add2Chain(ch, CMC_CMDDATA | 0);		// reset address
	    CMC_Add2Chain(ch, CMC_STDNFA(i, 17, 1));
	}
	RP_CommitChain(ch, Crate[cr]);
    }
/*	Enable memory	*/
    for (cr=0; cr<MAXCRATES; cr++) if (CrateRequired[cr]) {
	CMC_ResetChain(ch);
	CMC_Add2Chain(ch, CMC_CMDDELAY | 0x40);		// some delay ~ 100 us 
	for(i=0; i<MAXSTATIONS; i++) if (WFDinCAMAC[cr][i].yes) {
	    CMC_Add2Chain(ch, CMC_CMDDATA | 0);		// reset address
	    CMC_Add2Chain(ch, CMC_STDNFA(i, 17, 0));
	    CMC_Add2Chain(ch, CMC_CMDDATA | 0);		// reset address
	    CMC_Add2Chain(ch, CMC_STDNFA(i, 17, 1));
	    CMC_Add2Chain(ch, CMC_CMDDATA | 1);		// enable accept data
	    CMC_Add2Chain(ch, CMC_STDNFA(i, 16, 1));
	}
	RP_CommitChain(ch, Crate[cr]);
    }
    CMC_ReleaseChain(ch);
}

void initScalers(void)
{
    if (Conf.NSc < 1 || Conf.CrSc < 0) return;
    CMC_Single(Crate[Conf.CrSc], Conf.NSc, 9, 0, 0);
}

void getJetStatus(unsigned short * data)
{
    int i;    
    CMC_Single(Crate[Conf.CrIn], Conf.NIn, 16, 0, JET_VETO | JET_PLUS | JET_MINUS); 	// Set mask
    CMC_Single(Crate[Conf.CrIn], Conf.NIn, 2, 0, 0);			// reset previous state
    data[0] = CMC_Single(Crate[Conf.CrIn], Conf.NIn, 2, 0, 0);		// read qand reset
    data[1] = getJetBits();						// read CDEV Jet bits
    data[2] = 0;
}

int testJetVeto(void)
{
    unsigned short data[3];
    getJetStatus(data);
    return (data[0] & JET_VETO) ? 1 : 0;
}

static char *getJetStatusString(void)
{
    unsigned short data[3];
    static char statuses[][10] = {"NONE", "PLUS", "MINUS", "ZERO"};
    getJetStatus(data);
    return statuses[(data[0]/JET_PLUS) & 3];
}

void writeJetStatus(void)
{
    static int num = 0;
    recordHJetSwitchStruct rec;
    recordHeaderStruct header;

    memset(&rec, 0, sizeof(rec));
    num++;
    rec.header.type = REC_HJETSWITCH | recRing;
    rec.header.len = sizeof(rec);
    rec.header.timestamp.time = time(NULL);
    rec.Num = num;
    getJetStatus(rec.data);
    polWrite(&rec.header, (long *)&rec.Num);
/*	We will also write positions here	*/
    header.type = REC_HJPOSADO | recRing;
    header.len = sizeof(header) + sizeof(jetPosition);
    header.timestamp.time = time(NULL);
    getJetPosition();
    polWrite(&header, (long *)&jetPosition);
}

int testCarbTarg(void)
// returns true on target state change
{
    carbTargStat cts;
    static time_t t0;
  
    if (ctss.good == -1) {
	getCarbTarg(&ctss);
	return !(ctss.good);
    }
    getCarbTarg(&cts);
    switch (ctss.good) {
	case 1:
	    if (!(cts.good)) {
		memcpy(&ctss, &cts, sizeof(cts));
		fprintf(LogFile, "\nCarbon Target State Change. Now: INSERTED (%s %s %s %s)\n",
		    cts.carbtarg[0], cts.carbtarg[1], cts.carbtarg[2], cts.carbtarg[3]);
		return 1;
	    }
	    break;
	case 0:
	    if (cts.good) {
		memcpy(&ctss, &cts, sizeof(cts));
		t0 = time(NULL);	// start waiting 1 minute for the targets to finish
		ctss.good=-2;
		return 0;
	    }
	    break;
	case -2:
	    if (!(cts.good)) break;
	    if ((time(NULL)-t0) > JETWAIT4CARB) {  // assume carbon targets in safe place
		memcpy(&ctss, &cts, sizeof(cts));
		fprintf(LogFile, "\nCarbon Target State Change. Now: RETRACTED (%s %s %s %s)\n",
		    cts.carbtarg[0], cts.carbtarg[1], cts.carbtarg[2], cts.carbtarg[3]);
		return 1;
	    } 
	    return 0;
	default:
	    fprintf(LogFile,"\nTestCarbTarg-ERROR-Should never be here\n");
	    break;
    }
    memcpy(&ctss, &cts, sizeof(cts));
    return 0;
}

void writeCarbTarg(void)
{
    recordHJetCarbTargStruct rec;

    memset(&rec, 0, sizeof(rec));
    rec.header.type = REC_HJCARBT | recRing;
    rec.header.len = sizeof(rec);
    rec.header.timestamp.time = time(NULL);
    memcpy(&rec.targstat, &ctss, sizeof(ctss));
    if (rec.targstat.good < 0) rec.targstat.good = 0;
    polWrite(&rec.header, (long *)&rec.targstat);

}

void writeWcmInfo(void)
{
    recordHeaderStruct header;    
    getWcmInfo();
    //   Wcm data
    header.type = REC_WCMADO | recRing;
    header.len = sizeof(recordWcmAdoStruct);
    header.timestamp.time = time(NULL);
    polWrite(&header, (long *)&wcmData);    
}

void writeSubrun(int n)
{
    recordSubrunStruct rec;

    memset(&rec, 0, sizeof(rec));
    rec.header.type = REC_SUBRUN | recRing;
    rec.header.len = sizeof(rec);
    rec.header.timestamp.time = time(NULL);
    rec.subrun = n;
    polWrite(&rec.header, (long *)&rec.subrun);

}


int checkChainResult(CMC_chain *ch, int cr)
{
    int i, err, mask, val;
    
    mask = CMC_CMASK + CMC_QMASK + CMC_XMASK;
    val = (cr << 28) + CMC_QMASK + CMC_XMASK;
    err = 0;
    for (i=0; i<ch->rptr; i++) if ((ch->rdata[i] & mask) != val) {
	err++;
	if (iDebug > 200) fprintf(LogFile, "RHICPOL-ERROR : No Q/X/wrong crate number (%d) @%d: %8.8X.\n",
	    cr, i, ch->rdata[i]);
    }
    return err;
}

int getNumberOfEvents(void)	// Read from WFD dedicated scalers
{
    int array[8];
    int Cnt;
    int cr, i, j, ii, k, kk;
    CMC_chain *ch;
    
    Cnt = 0;
    ch = CMC_AllocateChain(0, MAXSTATIONS*150);
    for (cr=0; cr<MAXCRATES; cr++) if (CrateRequired[cr]) {
//	send read chain
	CMC_ResetChain(ch);
	ii = 0;
	for(i=0; i<MAXSTATIONS; i++) if (WFDinCAMAC[cr][i].yes) 
	    for (j=0; j<4; j++ ) if (WFDinCAMAC[cr][i].si[j] >= 0) {
	    CMC_Add2Chain(ch, CMC_CMDDATA | (1 << j));	// select virtex
	    CMC_Add2Chain(ch, CMC_STDNFA(i, 16, 9));
	    CMC_Add2Chain(ch, CMC_STDNFA(i, 0, 3));
	    CMC_Add2Chain(ch, CMC_CMDREPEAT | 31); // 21 = 16 + 16 - 1 - must be enough
	    ii += 33;
	}
	if (iDebug > 1500) fprintf(LogFile, "RHICPOL-INFO : Read dedicated scalers crate #%d CAMAC cmds = %d.\n",
	    cr, ii);
	if (ii == 0) continue;
	i = RP_CommitChain(ch, Crate[cr]);
	if (i < 0) {
	    fprintf(LogFile, "RHICPOL-ERROR : Read dedicated scalers crate #%d error %d (%s)\n", 
	        cr, -i, strerror(-i));
	    continue;
	}
	if (i != ii) {
	    fprintf(LogFile, "RHICPOL-ERROR : Read dedicated scalers crate #%d wrong number of words  %d (%d)\n", 
		cr, i, ii);
	    continue;
	}
	i = checkChainResult(ch, cr);
	if (i != 0) {
	    fprintf(LogFile, "RHICPOL-ERROR : Read dedicated scalers crate #%d - %d bad responses\n", cr, i);
	    continue;
	}
//	process the result
	ii = 0;
	for(i=0; i<MAXSTATIONS; i++) if (WFDinCAMAC[cr][i].yes) 
	    for (j=0; j<4; j++ ) if (WFDinCAMAC[cr][i].si[j] >= 0) {
	    for (k=1; k<17; k++) if ((ch->rdata[k+ii] & 0xF0000) == 0) break; // read zero scaler also latches all
	    for (kk = 0; kk < 16; kk += 2) {
		if ((ch->rdata[kk+k+ii] & 0xF0000) != kk*0x10000 || 
		    (ch->rdata[kk+k+ii+1] & 0xF0000) != (kk+1)*0x10000) {
		    fprintf(LogFile, "\nRHICPOL-WARN: Wrong dedicated scalers readout at %d.%d.%d@%d: %8.8X %8.8X\n",
			cr, i, j+1, kk, ch->rdata[kk+k+ii], ch->rdata[kk+k+ii+1]);
		} else {
		    array[kk/2] = (ch->rdata[kk+k+ii] & 0xFFFF) + ((ch->rdata[kk+k+ii+1] & 0xFFFF) << 16);
		}
	    }
	    switch (Conf.CSR.split.Mode) {
	    case MOD_AT:
	    case MOD_ALL:
		Cnt += array[0] + array[1] + array[2];
		break;
	    case MOD_SUB:
		Cnt += array[5] - array[6];
	    }
	    ii += 33;
	}
    }
    CMC_ReleaseChain(ch);
    return Cnt;
}

int getEvents(int Number)
{
    int Cnt, i, j, k, l, lCnt, l10Cnt, l10Val, l10;
    double t, t0, tlast;
    recordHeaderStruct header;
    int forseWrite = 0;
    int sc;
    struct timeval tv;
    long *targetHistory;
    long targetHistoryLen;
    long targetHistoryPtr;
    long *countHistory;
    long countHistoryLen;
    long countHistoryPtr;
    
    Cnt = 0;
    lCnt = 0;
    l10Cnt = 0;
    l10Val = 0;
    l10 = 0;
    
    if (NoADO == 0 && (recRing & REC_JET) == 0) {
	targetHistoryLen = 8192;
	targetHistory = (long *)malloc(targetHistoryLen);
	if (targetHistory == NULL) {
	    targetHistoryLen = 0;
	} else {
	    GetTargetEncodings(targetHistory);
    	    targetHistoryPtr = 4*sizeof(long);
	}
    } else {
	targetHistoryPtr = 0;
	targetHistoryLen = 0;
	targetHistory = NULL;
    }

    if ((recRing & REC_JET) == 0) {
	countHistoryLen = 8192;
	countHistory = (long *)malloc(countHistoryLen);
	countHistoryPtr = 0;	
    } else {
	countHistoryPtr = 0;
	countHistoryLen = 0;
	countHistory = NULL;
    }
    
    gettimeofday(&tv, NULL);
    t = tv.tv_sec + tv.tv_usec * 1.0E-6;
    t0 = t;
    tlast = t;
//	Main Cicle
    for (;;) {
        if (Number > 0 && Cnt >= Number) {
	    fprintf(LogFile, "\nRHICPOL-INFO : %d events collected. Exiting...\n", Cnt);
	    break; 
        }
        if (IStop != 0) {
	    fprintf(LogFile, "\nRHICPOL-INFO : TimedOut or Stopped. Exiting...\n");
	    break; 
        }
//	Here we wait ~ 1sec. and toggle Inhibit to ignore excessive events
	if (Conf.SleepFraction > 0.0 && Conf.SleepFraction < 1.0) {
	    nsleep(Conf.CicleTime*(1.0 - Conf.SleepFraction));
	    setInhibit();
	    nsleep(Conf.CicleTime*Conf.SleepFraction);
	    resetInhibit();	    
	} else { 
	    nsleep(Conf.CicleTime);	// just wait	
	}
//	increment event counter if indirect readout mode
	gettimeofday(&tv, NULL);
	t = tv.tv_sec + tv.tv_usec * 1.0E-6;
        if (t >= tlast + 1.0 || IStop != 0 || (Number > 0 && Cnt >= Number)) { // at least 1 second passed
	    Cnt = getNumberOfEvents();
	    l10++;
	    if (l10 == 10) {
		l10 = 0;
		l10Val = Cnt - l10Cnt;
		l10Cnt = Cnt;
	    }
	    if (LogFile == stdout) {
		printf("Sec: %4.4d  Total: %9d  Last: %6d   Rate:  %6d  Last10: %8d  ", 
		    (int)(t-t0), Cnt, Cnt - lCnt, (int)((Cnt-lCnt)/(t-tlast)), l10Val);
		if (iCicleRun) printf("%s", getJetStatusString());
		printf("\r");
		fflush(stdout);
	    } 
// indicate progress
	    if (NoADO == 0 && (recRing & REC_JET) == 0) {
		UpdateProgress(Cnt, (int)((Cnt-lCnt)/(t-tlast)), t);
//	get target information
		if (targetHistoryPtr >= targetHistoryLen) {
		    targetHistory = (long *)realloc(targetHistory, targetHistoryLen+8192);
		    targetHistoryLen += 8192;
		}
		if (targetHistory != NULL) {
		    GetTargetEncodings(&targetHistory[targetHistoryPtr/sizeof(long)]);
		    targetHistoryPtr += 4*sizeof(long);
		} else {
		    targetHistoryPtr = 0;
		    targetHistoryLen = 0;
		}
		if (Conf.CSR.split.RevDelim == 0) pulseDelimiter();
	    }
	    if ((recRing & REC_JET) == 0) {
//	get target information
		if (countHistoryPtr >= countHistoryLen) {
		    countHistory = (long *)realloc(countHistory, countHistoryLen+8192);
		    countHistoryLen += 8192;
		}
		if (countHistory != NULL) {
		    countHistory[countHistoryPtr/sizeof(long)] = (Cnt-lCnt)/(t-tlast);
		    countHistoryPtr += sizeof(long);
		} else {
		    countHistoryPtr = 0;
		    countHistoryLen = 0;		    
		}
	    }
	    tlast = t;
	    lCnt = Cnt;
    	}		
	if (iCicleRun) forseWrite = (testJetVeto() || testCarbTarg());
	if (iDebug > 140 && forseWrite) fprintf(LogFile, "Debug: Jet state changed. Writing.\n");
	if (forseWrite && !Conf.OnlyHist) {
	    setOutInhibit();
	    readMemory();
	    writeJetStatus();
	    writeCarbTarg();
	    fastInitWFDs(0);
	    clearVetoFlipFlop();
	    resetOutInhibit();
	    writeWcmInfo();	    
	}
    }
    if (targetHistoryPtr != 0) {
	header.len = sizeof(header)+targetHistoryPtr;
	header.type = REC_PCTARGET;
	header.timestamp.time = time(NULL);
	polWrite(&header, targetHistory);
    }
    if (countHistoryPtr != 0) {
	header.len = sizeof(header)+countHistoryPtr;
	header.type = REC_COUNTRATE;
	header.timestamp.time = time(NULL);
	polWrite(&header, countHistory);
    }
    return Cnt;
}

void readWFD()
{
    int i, j, k, N, cr, ii;
    recordWFDV8ArrayStruct rec;
    int cnt=0, cnt1=0;
    CMC_chain *ch;

    if (Conf.CSR.split.Mode == MOD_RAW || Conf.CSR.split.Mode == MOD_SUB) return;

    rec.header.type = REC_WFDV8SCAL | recRing;
    if (Conf.CSR.split.B120) rec.header.type |= REC_120BUNCH;
    rec.header.len = sizeof(rec);

    ch = CMC_AllocateChain(0, 6400);
    for (cr=0; cr<MAXCRATES; cr++) if (CrateRequired[cr]) {
	if (iDebug > 1500) fprintf(LogFile, "RHICPOL-INFO : Reading WFD xilinxes crate %d. Chain %p\n", cr, ch);
//	send read chain
	for(i=0; i<MAXSTATIONS; i++) if (WFDinCAMAC[cr][i].yes) {
	    ii = 1;
	    CMC_ResetChain(ch);
    	CMC_Add2Chain(ch, CMC_CMDDATA | 0);	// better go to internal clocks for readout
	    CMC_Add2Chain(ch, CMC_STDNFA(i, 16, 8));	    
	    for (j=0; j<4; j++ ) if (WFDinCAMAC[cr][i].si[j] >= 0) {
		CMC_Add2Chain(ch, CMC_CMDDATA | (1 << j));	// select virtex
		CMC_Add2Chain(ch, CMC_STDNFA(i, 16, 9));	//			0-1
		CMC_Add2Chain(ch, CMC_STDNFA(i, 1, 1));		// read CSR		1-1
		CMC_Add2Chain(ch, CMC_STDNFA(i, 1, 2));		// read TRIG		2-1
		CMC_Add2Chain(ch, CMC_STDNFA(i, 1, 3));		// read Window		3-1
		CMC_Add2Chain(ch, CMC_STDNFA(i, 9, 2));		// reset adr.		4-1
		CMC_Add2Chain(ch, CMC_STDNFA(i, 0, 3));		// read scalers		5-16
		CMC_Add2Chain(ch, CMC_CMDREPEAT | 15);		// 15 = 16 - 1
		CMC_Add2Chain(ch, CMC_STDNFA(i, 0, 1));		// read histograms	21-1536
		CMC_Add2Chain(ch, CMC_CMDREPEAT | 1535);	// 1535 = 1536 - 1
	    ii += 1557;
	    }
	    j = RP_CommitChain(ch, Crate[cr]);
    	if (j < 0) {
		fprintf(LogFile, "RHICPOL-ERROR : Read WFD Xilinxes WFD %d.%d error %d (%s)\n", cr, i, -j, strerror(-j));
		continue;
	    }
    	if (j != ii) {
		fprintf(LogFile, "RHICPOL-ERROR : Read WFD Xilinxes WFD %d.%d wrong number of words  %d (%d)\n", cr, i, j, ii);
	    continue;
	    }
    	j = checkChainResult(ch, cr);
	    if (j != 0) {
		fprintf(LogFile, "RHICPOL-ERROR : Read WFD Xilinxes WFD %d.%d - %d bad responses\n", cr, i, j);
	    continue;
	    }
//	process the result
	    ii = 1; // to account for select clock command	    
	    for (j=0; j<4; j++ ) if (WFDinCAMAC[cr][i].si[j] >= 0) {
	    rec.siNum = WFDinCAMAC[cr][i].si[j];
		rec.csr = ch->rdata[ii+1] & CMC_DMASK;
		rec.trg = ch->rdata[ii+2] & CMC_DMASK;
		rec.win = ch->rdata[ii+3] & CMC_DMASK;
	        for(k=0; k<8; k++) {
	    	rec.scalers[k] = (ch->rdata[ii+2*k+5] & 0xFFFF) + 
			((ch->rdata[ii+2*k+6] & 0xFFFF) << 16);
	    	if (k<3) cnt+=rec.scalers[k];
		}
	        for(k=0; k<1536; k++) {
		    rec.hist[k] = ch->rdata[ii+k+21] & CMC_DMASK;
	    	if (k<128) cnt1 += rec.hist[k];
		}
	    ii += 1557;
		rec.header.timestamp.time = time(NULL);
		polWrite(&rec.header, (long *)&rec.siNum);
	    }
	}	    	
    }
    CMC_ReleaseChain(ch);
    fprintf(LogFile,"RHICPOL-INFO : Total number events from scalers: %d, from histo %d\n", cnt, cnt1);
}

long long totalSum(long *data, int len)
{
    long long summa;
    int i;
    
    summa = 0LL;
    for (i=0; i<len; i++) summa += (long long) data[i];
    return summa;
}

typedef enum {REC_NONE, REC_DATA, REC_DELIM} MEM_REC_TYPE;

void *readThread(void *arg)
{
    int i, j, l, rpt;
    unsigned len;
    int Vn, tlen, err;
    MEM_REC_TYPE type;
    CMC_chain *ch;
    unsigned short *bf[4];
    recordHeaderStruct head;
    int ptr[4], evt[4], evtot[4], delim[4];
    int cr = (int)arg;
    unsigned short w;
    
//	We will have the same type for all records.
    switch (Conf.CSR.split.Mode) {
    case MOD_RAW:
        head.type = REC_READRAW;
        break;
    case MOD_SUB:
        head.type = (Conf.FPGAVersion == 10) ? REC_READLONG : REC_READSUB;
        break;
    case MOD_AT:
        head.type = REC_READAT;
        break;
    case MOD_ALL:
        head.type = REC_READALL;
        break;
    }
    head.type |= (recRing | REC_FROMMEMORY);
    if (Conf.CSR.split.B120) head.type |= REC_120BUNCH;

    for (i=0; i<4; i++) bf[i] = malloc(BSIZE*sizeof(short));
    ch = CMC_AllocateChain(0, BSIZE);
    if (bf[0] == NULL || bf[1] == NULL || bf[2] == NULL || bf[3] == NULL || ch == NULL) {
	   CMC_ReleaseChain(ch);
	   for (i=0; i<4; i++) if (bf[i] != NULL) free(bf[i]);
	   fprintf(LogFile, "RHICPOL-ERROR : Unable to allocate buffers to readout memory at Crate #%d\n", cr);
	   polData.statusS |= WARN_INT;
	   return NULL;
    }
    if(iDebug > 100) fprintf(LogFile, "RHICPOL-INFO : Thread for crate %d started successfully.\n", cr);
    for (i=0; i<MAXSTATIONS && iSig!=SIGTERM; i++) if (WFDinCAMAC[cr][i].yes) {
	err = 0;
/*	get the length and prepare for readout		*/	
	CMC_ResetChain(ch);
	CMC_Add2Chain(ch, CMC_CMDDATA | 0);		// better go to internal clocks for readout	0
	CMC_Add2Chain(ch, CMC_STDNFA(i, 16, 8));
	CMC_Add2Chain(ch, CMC_CMDDATA | 0x10);		// select memory controller 1
	CMC_Add2Chain(ch, CMC_STDNFA(i, 16, 9));
	CMC_Add2Chain(ch, CMC_CMDDATA | 0);		// disable memory functions
	CMC_Add2Chain(ch, CMC_STDNFA(i, 16, 1));	
	CMC_Add2Chain(ch, CMC_CMDDELAY | 10);		// delay ~ 25 us
	CMC_Add2Chain(ch, CMC_STDNFA(i, 1, 0));		// read the address	3
	CMC_Add2Chain(ch, CMC_STDNFA(i, 1, 1));		// read the address	4
	CMC_Add2Chain(ch, CMC_CMDDATA | 0);		// clear the address
	CMC_Add2Chain(ch, CMC_STDNFA(i, 17, 0));	
	CMC_Add2Chain(ch, CMC_CMDDATA | 0);		// clear the address
	CMC_Add2Chain(ch, CMC_STDNFA(i, 17, 1));	
	switch (ReadMode[0]) {
	case 'D' :
	    CMC_Add2Chain(ch, CMC_CMDDATA | 6);		// enable cache & both edges 
	    break;
	case 'A' :
	    CMC_Add2Chain(ch, CMC_CMDDATA | 2);		// enable cache		
	    break;
	default :
	    if (ReadMode[1] < '4') {
		CMC_Add2Chain(ch, CMC_CMDDATA | 2);		// enable cache		
	    } else {
		CMC_Add2Chain(ch, CMC_CMDDATA | 0);		// no cache for standard read
	    }
	    break;	
	}
	CMC_Add2Chain(ch, CMC_STDNFA(i, 16, 1));	
	CMC_Add2Chain(ch, CMC_CMDDELAY | 40);		// delay ~ 100 us
	RP_CommitChain(ch, Crate[cr]);
	len  = ((ch->rdata[3] & 0xFFFF) + ((ch->rdata[4] & 0xFFFF) << 16));	// bytes
	if (len >= WFDMEMSIZE) {
	    fprintf(LogFile, "RHICPOL-WARN : Memory overflow at station %d.%d, the tail of data is lost\n", cr, i);
	    polData.statusS |= WARN_WFD;
	    len = WFDMEMSIZE;
	}
	if (iDebug > 100) fprintf(LogFile, "RHICPOL-INFO : Found %d bytes in WFD %d.%d\n", len, cr, i);
	len /= 2;	// we count in 16-bit words from here
//	prepare to read
	memset(evtot, 0, sizeof(evtot));
	for (j = 0; j<4; j++) ptr[j] = 2;		// leave space for subhead
	memset(evt, 0, sizeof(evt));
	memset(delim, 0, sizeof(delim));
	type = REC_NONE;
	tlen = 0;
//	read cycle
	for (l = 0; l < len && err<MAXERR; l += rpt) {
//		Read
	    rpt = (len - l < BSIZE) ? len - l : BSIZE;
	    CMC_ResetChain(ch);
	    switch (ReadMode[0]) {
	    case 'D' :
		CMC_Add2Chain(ch, CMC_CMDLOADLOOP | (rpt+1)/2);
		CMC_Add2Chain(ch, CMC_FASTNFA((ReadMode[1] - '0'), 0, 0, 0, 1, 2, i, 0, 0));
    	if (!(rpt & 1)) CMC_Add2Chain(ch, CMC_FASTNFA((ReadMode[1] - '0'), 0, 0, 0, 0, 0, i, 0, 0));
		break;
	    case 'A' :
		CMC_Add2Chain(ch, CMC_CMDLOADLOOP | rpt);
		CMC_Add2Chain(ch, CMC_FASTNFA((ReadMode[1] - '0'), 0, 0, 0, 0, 1, i, 0, 0));
		break;
	    default :
    	CMC_Add2Chain(ch, CMC_FASTNFA((ReadMode[1] - '0'), 0, 0, 0, 0, 0, i, 0, 0));
		CMC_Add2Chain(ch, CMC_CMDREPEAT | (rpt - 1));
	    }
	    j = RP_CommitChain(ch, Crate[cr]);
    	if (j < 0) {
		fprintf(LogFile, "RHICPOL-ERROR : Read memory WFD %d.%d error %d (%s)\n", cr, i, -j, strerror(-j));
		err++;
		rpt = 0;
		continue;
	    }
    	if (j != rpt) {
		fprintf(LogFile, "RHICPOL-ERROR : Read memory WFD %d.%d wrong number of words  %d (%d)\n", cr, i, j, rpt);
		err++;
		rpt = j;
	    }
    	j = checkChainResult(ch, cr);
	    if (j != 0) fprintf(LogFile, "RHICPOL-ERROR : Read memory WFD %d.%d - %d bad responses\n", cr, i, j);
//		Process
	    for(j=0; j<rpt && err < MAXERR; j++) {
		w = ch->rdata[j] & 0xFFFF;
		switch (type) {
		case REC_NONE:
		    if ((w & 0x3F3F) != (Conf.CSR.reg & 0x3F3F)) {
			if (iDebug > 200) fprintf(LogFile, 
			    "RHICPOL-ERR : WFD %d.%d CSR %4.4X found @ %X (EXP: %4.4X)\n",
			    cr, i, w, 2*j, Conf.CSR.reg);
			err++;
			continue;
		    }
		    Vn = (w & 0xC0) >> 6;
		    if (w & 0x8000) {
			type = REC_DELIM;
			tlen = 2;
		    } else {
			type = REC_DATA;
			switch (Conf.CSR.split.Mode) {
			case MOD_SUB :
			    tlen = (Conf.FPGAVersion == 10) ? 3*(Conf.JetDelay & 0xFF) + 4 : 49;
			    break;
			case MOD_RAW :
			case MOD_ALL :
			    tlen = 49;
			    break;
			case MOD_AT :
			    tlen = 4;
			    break;
			}
		    }
		    break;
		case REC_DELIM:
		    delim[Vn] = w;
		    break;
		case REC_DATA:
		    bf[Vn][ptr[Vn]] = w;
		    ptr[Vn]++;
		    if (tlen == 1) {
			evt[Vn]++;
//	correct backward for 120 bunch mode short waveforms
			if (Conf.CSR.split.Mode != MOD_AT && Conf.CSR.split.B120 && Conf.FPGAVersion < 10) {
			    ptr[Vn] -= 24;
			    bf[Vn][ptr[Vn]-3] = bf[Vn][ptr[Vn]+21];
			    bf[Vn][ptr[Vn]-2] = bf[Vn][ptr[Vn]+22];
			    bf[Vn][ptr[Vn]-1] = bf[Vn][ptr[Vn]+23];
			}
		    }
		    break;
		}
		if (tlen > 0) tlen --;	// record word counter
		if (tlen == 0) {
//			We shell write here if delimeter changed or no space left and tlen = 0
		    if (((type == REC_DELIM && ptr[Vn] > 2) || (type == REC_DATA && ptr[Vn] > BSIZE - 1000))
		    	&& WFDinCAMAC[cr][i].si[Vn] >= 0) {
			bf[Vn][0] = WFDinCAMAC[cr][i].si[Vn] + (((evt[Vn] - 1) & 0xFF) << 8);
			bf[Vn][1] = (evt[Vn] - 1) >> 8;
			head.len = ptr[Vn] * sizeof(short) + sizeof(head);
			head.timestamp.delim = delim[Vn];
			polWrite(&head, (long *)bf[Vn]);
			evtot[Vn] += evt[Vn];
			ptr[Vn] = 2;
			evt[Vn] = 0;
		    }
		    type = REC_NONE;
		}
	    }
	}
//		Finsih this module processing
//		Check if we have dangling data from memory - print warning and cut the tail
	if (tlen != 0) {
	    if (iDebug > 200) fprintf(LogFile, 
		"RHICPOL-ERR : WFD %d.%d:%d unfinished record found\n", cr, i, Vn+1);
	    if (type == REC_DATA) {
		if (Conf.CSR.split.Mode == MOD_AT) {
		    ptr[Vn] -= 3 - tlen;
		} else if (Conf.FPGAVersion == 10 && Conf.CSR.split.Mode == MOD_SUB) {
		    ptr[Vn] -= 3*(Conf.JetDelay & 0xFF) + 3 - tlen;
		} else {
		    ptr[Vn] -= 48 - tlen;
		}
	    }
	}
//		Flush all buffers	
	for(Vn=0; Vn<4; Vn++) if (ptr[Vn] > 2 && WFDinCAMAC[cr][i].si[Vn] >= 0) {
	    bf[Vn][0] = WFDinCAMAC[cr][i].si[Vn] + (((evt[Vn] - 1) & 0xFF) << 8);
	    bf[Vn][1] = (evt[Vn] - 1) >> 8;
	    head.len = ptr[Vn] * sizeof(short) + sizeof(head);
	    head.timestamp.delim = delim[Vn];
	    polWrite(&head, (long *)bf[Vn]);
	}
	for(Vn=0; Vn<4; Vn++) {
	    evtot[Vn] += evt[Vn];
	    memReadCount[cr][i][Vn] = evtot[Vn];
	}
	if (err > 0) {
	    fprintf(LogFile,"RHICPOL-ERROR : %d Errors found while reading memory WFD %d.%d.\n", err, cr, i);
	    polData.statusS |= WARN_WFD;
	}    
	if (err >= MAXERR) fprintf(LogFile,"RHICPOL-ERROR : Too many errors found in memory WFD %d.%d, readout aborted.\n", cr, i);
	if (iDebug > 20) {
	    fprintf(LogFile, "RHICPOL-INFO : Events found in memory");
	    for (Vn=0;Vn<4;Vn++) if (WFDinCAMAC[cr][i].si[Vn] >= 0)
		fprintf(LogFile, " Ch%2.2d:%d", WFDinCAMAC[cr][i].si[Vn]+1, evtot[Vn]);
	    fprintf(LogFile, " total errs: %d\n", err);
	}
    }
    CMC_ReleaseChain(ch);
    return NULL;
}

void readMemory()
{
    pthread_t th[MAXCRATES];
    int cr, i;
    long long GrandTotal;
    struct timeval tv;
    double t,t0;
/*	Clear counters	*/
    memset(memReadCount, 0, sizeof(memReadCount));
/*	start read threads - one per crate	*/
    memset(th, 0, sizeof(th));
    if (iDebug > 1000) fprintf(LogFile, "RHICPOL-INFO : Creating memory readout threads\n");
    gettimeofday(&tv, NULL);
    t0 = tv.tv_sec + tv.tv_usec * 1.0E-6;
    for (cr=0; cr<MAXCRATES; cr++) if (CrateRequired[cr]) pthread_create(&th[cr], NULL, readThread, (void *)cr);
/*	wait them to finish			*/
    for (cr=0; cr<MAXCRATES; cr++) if (CrateRequired[cr] && th[cr] != 0) pthread_join(th[cr], NULL);
/*	some printout				*/
    gettimeofday(&tv, NULL);
    t = tv.tv_sec + tv.tv_usec * 1.0E-6;
    GrandTotal = totalSum(&memReadCount[0][0][0], sizeof(memReadCount)/sizeof(long));
    if (iCicleRun == 0 || iDebug > 5) 
	fprintf(LogFile,"RHICPOL-INFO : Total %qd events read from memory in %.2f sec (%.2f Mevt/s)\n", 
	    GrandTotal, t-t0, GrandTotal/(t-t0)*1.0e-6);
    if (iCicleRun) {
	fprintf(LogFile, "\nReadMem INFO:   %15qd events in last memory buffer. New HJet state: %s\n", 
	    GrandTotal, getJetStatusString());
	fprintf(LogFile, "Events in the last memory buffer:\n");
	for(i=0; i<Conf.NumChannels; i++) {
	    fprintf(LogFile, "Si%2d:%7d ", i+1, (SiConf[i].CrateN < 0) ? 0 :
		memReadCount[SiConf[i].CrateN][SiConf[i].CamacN][SiConf[i].VirtexN-1]);
	    if ((i%6) == 5) fprintf(LogFile, "\n");
	}
	if (i%6) fprintf(LogFile, "\n");
//	fflush(LogFile);
    }
}

void readScalers()
{
    recordScalersStruct rec;
    int i;
    CMC_chain *ch;
    
    if (Conf.NSc < 1 || Conf.CrSc < 0) return;
    ch = CMC_AllocateChain(0, 12);
    for (i=0; i<12; i++) CMC_Add2Chain(ch, CMC_STDNFA(Conf.NSc, 0, i));
    RP_CommitChain(ch, Conf.CrSc);
    for (i=0; i<6; i++) rec.data[i] = ((long long) (ch->rdata[2*i] & CMC_DMASK)) +
	(((long long) (ch->rdata[2*i+1] & CMC_DMASK)) << 24);
    rec.header.type = REC_SCALERS | recRing;
    rec.header.len = sizeof(rec);
    rec.header.timestamp.time = time(NULL);
    polWrite(&rec.header, (long *)&rec.data[0]);
}

void closeDataFile(char * comment)
{
    int len;
    long *data;
    recordEndStruct rec;
    recordHeaderStruct header;
    if (OutFile == NULL) return;
    if ((recRing & REC_JET) == 0) {
//	Target movement record for profiles
//<<<<<<< rpolutil.c
	if (NoADO == 0) {
	    header.type = REC_TAGMOVEADO | recRing;
	    len = getTagetMovementInfo(&data);
	    header.len = sizeof(recordHeaderStruct) + len*sizeof(long);
	    header.timestamp.time = time(NULL);
	    polWrite(&header, data);
	    free(data);
	}
//======= Dima: This is very bad, because causes segmentation fault in case
// NoADO==1 -- data == NULL and len undefined
	header.type = REC_TAGMOVEADO | recRing;
	if (NoADO == 0) len = getTagetMovementInfo(&data);
	header.len = sizeof(recordHeaderStruct) + len*sizeof(long);
	header.timestamp.time = time(NULL);
	polWrite(&header, data);
	free(data);
//>>>>>>> 1.3
//	Polarimeter specific data to be passed to data2hbook
	header.type = REC_POLADO | recRing;
	header.len = sizeof(recordPolAdoStruct);
	header.timestamp.time = time(NULL);
	polWrite(&header, (long *)&polData);
    }
//
    strncpy(rec.comment, comment, sizeof(rec.comment));
    rec.header.type = REC_END | recRing;
    rec.header.len = sizeof(rec);
    rec.header.timestamp.time = time(NULL);
    polWrite(&rec.header, (long *)&rec.comment[0]);
    fclose(OutFile);
    OutFile = NULL;
}

int openDataFile(char *fname, char *comment, int noAdo)
{
    recordBeginStruct rec;
    recordHeaderStruct header;
    void * buf;
    
	fprintf(LogFile, "RHICPOL-COMMENT : %s\n", comment);

    if (OutFile != NULL) closeDataFile("New file open.");
    OutFile = fopen(fname, "wb");
    if (OutFile == NULL) {
	fprintf(LogFile, "Unable to open file %s for data output: %s.\n",
	    fname, strerror(errno));
	polData.statusS |= (STATUS_ERROR | ERR_INT);
	return errno;
    }
    dataNum = 0;
//	begin record
    rec.version = VERSION;
    strncpy(rec.comment, comment, sizeof(rec.comment));
    rec.header.type = REC_BEGIN | recRing;
    rec.header.len = sizeof(rec);
    rec.header.timestamp.time = time(NULL);
    polWrite(&rec.header, (long *)&rec.version);
//	config record
    buf = malloc(sizeof(configRhicDataStruct) + sizeof(SiChanStruct)*(Conf.NumChannels-1));
    header.type = REC_RHIC_CONF | recRing;
    header.len = sizeof(recordHeaderStruct) + sizeof(configRhicDataStruct) + 
	sizeof(SiChanStruct)*(Conf.NumChannels-1);
    header.timestamp.time = time(NULL);
    // Conf without SiChan
    memcpy(buf, &Conf, sizeof(configRhicDataStruct)-sizeof(SiChanStruct));
    // Real SiConf 
    memcpy(&(((char*)buf)[sizeof(configRhicDataStruct)-sizeof(SiChanStruct)]), 
	SiConf, sizeof(SiChanStruct) * Conf.NumChannels);
    polWrite(&header, (long *)buf);
    free(buf);
//	beam data (have to write it anyway because rhic2hbook uses polpat from here)
    header.type = REC_BEAMADO | recRing;
    header.len = sizeof(recordBeamAdoStruct);
    header.timestamp.time = time(NULL);
    polWrite(&header, (long *)&beamData);
//	Store the other beam parameters
    if (recRing & REC_JET) {
	header.type = (REC_BEAMADO | recRing) & (~(REC_BLUE | REC_YELLOW));
	if (recRing & REC_BLUE) header.type |= REC_YELLOW;
	if (recRing & REC_YELLOW) header.type |= REC_BLUE;
	header.len = sizeof(recordBeamAdoStruct);
	header.timestamp.time = time(NULL);
	polWrite(&header, (long *)&beamOtherData);
    }
//
    if (noAdo) return 0;
//	target data
    header.type = REC_TAGADO | recRing;
    header.len = sizeof(recordTagAdoStruct);
    header.timestamp.time = time(NULL);
    polWrite(&header, (long *)&targetData[0]);

    //   Wcm data
    header.type = REC_WCMADO | recRing;
    header.len = sizeof(recordWcmAdoStruct);
    header.timestamp.time = time(NULL);
    polWrite(&header, (long *)&wcmData);

//	Store the other beam parameters
    if (recRing & REC_JET) {
	header.type = (REC_WCMADO | recRing) & (~(REC_BLUE | REC_YELLOW));
	if (recRing & REC_BLUE) header.type |= REC_YELLOW;
	if (recRing & REC_YELLOW) header.type |= REC_BLUE;
	header.len = sizeof(recordWcmAdoStruct);
	header.timestamp.time = time(NULL);
	polWrite(&header, (long *)&wcmOtherData);
    }
    
    return 0;
}

