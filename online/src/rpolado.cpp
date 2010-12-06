/****************************************/
/*      RHIC Polarimeter Main Program   */
/*      I. Alekseev and D. Svirida      */
/*              2001-2010               */
/****************************************/

/** 
 *
 * Contributors: Dmitri Smirnov
 *
 */

#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <stdarg.h>
#include <cdevRequestObject.h>
#include <cdevCallback.h>
#include <cdevSystem.h>

#include "rcdev.h"
#include "rhicpol.h"
#include "rpolado_mux.h"

char myName[2][20] = {"Upstream", "Downstream"};
char specCDEVName[2][20] = {"ringSpec.blue", "ringSpec.yellow"};
int  myDev[2][2] = {{0, 3}, {1, 2}};                            // polCDEVName for Upstream/Downstream

FILE *LogFile;
char LogFileName[256] = "polarimeter.log";
char ScriptName[256] = "rpolMeasure.sh";
int iVerbose = 0;

pid_t ChildPid = 0;
int MyPolarimeter = -1;         // Upstream/downstream
int CurrentPolarimeter = -1;    // we can run one of two our polarimeters only
int Status = 0;                 // Our error status
int iCmd   = 0;                 // the last command
int iStop  = 0;                 // we should Stop
int iChild = 0;                 // child exitted
int iAlarm = 0;                 //

//      this function removes \n from the standard ctime function output
char * cctime(time_t *itime)
{
    char *str;
    str = ctime(itime);
    str[strlen(str)-1] = '\0';
    return str;
}

int UpdateMessage(int polarim, char * fmt, ...)
{
    cdevData data;
    char str[80];
    int irc;
    va_list ap;

    va_start(ap, fmt);
    vsprintf(str, fmt, ap);

    irc = 0;
    cdevDevice & pol = cdevDevice::attachRef(polCDEVName[polarim]);
    data.insert("value", str);
    DEVSEND(pol, "set statusStringS", &data, NULL, LogFile, irc);
    return irc;
}

int SetState(int ring, char * state)
{
    cdevData data;
    int irc;

    irc = 0;
    cdevDevice & pol = cdevDevice::attachRef(polCDEVName[ring]);
    data.insert("value", state);
    DEVSEND(pol, "set dataAcquisitionS", &data, NULL, LogFile, irc);
    return irc;
}

int SetStatus(int ring)
{
    cdevData data;
    int irc;

    irc = 0;
    cdevDevice & pol = cdevDevice::attachRef(polCDEVName[ring]);
    data.insert("value", Status);
    DEVSEND(pol, "set statusS", &data, NULL, LogFile, irc);
    return irc;
}

int GetStatus(int ring)
{
    cdevData data;
    int irc;

    irc = 0;
    cdevDevice & pol = cdevDevice::attachRef(polCDEVName[ring]);
    DEVSEND(pol, "get statusS", NULL, &data, LogFile, irc);
    data.get("value", &irc);
    return irc;
}

void StartMeasurement(int mode)
{
    int irc;
    cdevData data;
    int i, j;
    pid_t pid;
    time_t tm;
    long maxTime, numberEvents, fillNumber;
    double beamEnergy, runId;
        char str1[20];

    struct {
        int fill;
        int ring;
        int run;
    } ID;

    irc = 0;
    // Already started
    if (CurrentPolarimeter >= 0) return;

    Status = 0;
    CurrentPolarimeter = MyPolarimeter;
    SetStatus(MyPolarimeter);

    // first get necessary parameters
    cdevDevice & pol = cdevDevice::attachRef(polCDEVName[MyPolarimeter]);
    cdevDevice & spec = cdevDevice::attachRef(specCDEVName[MyPolarimeter]);
    cdevDevice & mux = cdevDevice::attachRef(muxCDEVName[MyPolarimeter]);

    DEVSEND(pol, "get maxTimeToRunS", NULL, &data, LogFile, irc);
    data.get("value", &maxTime);
    DEVSEND(pol, "get numberEventsToDoS", NULL, &data, LogFile, irc);
    data.get("value", &numberEvents);
    if (mode==2) {  // read run number for emit. scans
      DEVSEND(pol, "get emitRunIdS", NULL, &data, LogFile, irc);
      data.get("value", &runId);
    } else {        // or get run number for polarization measurements
      DEVSEND(pol, "get runIdS", NULL, &data, LogFile, irc);
      data.get("value", &runId);
    }
    DEVSEND(spec, "get fillNumberM", NULL, &data, LogFile, irc);
    data.get("value", &fillNumber);
    DEVSEND(spec, "get beamEnergyM", NULL, &data, LogFile, irc);
    data.get("value", &beamEnergy);
    if (irc != 0) {
        Status |= (STATUS_ERROR | WARN_NOBEAM | ERR_NOADO);
        UpdateMessage(MyPolarimeter, "Run parameters unaccessible");
        SetStatus(MyPolarimeter);
        SetState(MyPolarimeter, "Stop");
        CurrentPolarimeter = -1;
        return;
    }
    DEVSEND(mux, "get polNumM", NULL, &data, LogFile, irc);
    data.get("value", &iTarget);
    if (mode == 4) {
        DEVSEND(pol, "get rampMeasTimeS", NULL, &data, LogFile, irc);
        data.get("value", &maxTime);    // never directly used by .sh
        numberEvents = maxTime / 2;     // the meaning is the number of 2 sec subruns
        beamEnergy = 100.;              // we always synchronize like @ 100 GeV
    }

//      Run number
    if (fillNumber <= 0) {
        char sss[80];
        Status |= (WARN_NOBEAM | ERR_NOADO);
        // if we can't get real fill number, let's assume it ddmmyyhh
        tm = time(NULL);
        strftime(sss, sizeof(sss), "%y%m%d%H", localtime(&tm));
        fprintf(LogFile,"RHICADO-WARN : Can't get fill number, setting %s\n",sss);
        fflush(LogFile);
        SetStatus(MyPolarimeter);
        fillNumber = strtol(sss, NULL, 0);
    runId = (float)fillNumber + 0.001;
    }

    double frun, fring, intpart;

    frun = modf(runId, &intpart);
    ID.fill = (int)floor(intpart+0.5);
    fring = modf(10.*frun, &intpart);
    ID.ring = (int)floor(fring+0.5);
    ID.run = (int)floor(100*fring+0.5);

    if (ID.fill == fillNumber) {
         ID.run++;
    }
    else {
         ID.fill = fillNumber;
         ID.run = 1;
    }
        if (mode==2) {
         ID.ring = (MyPolarimeter == 0) ? 5 : 4;  // for emit. scans
        }
    else {
         ID.ring = (MyPolarimeter == 0) ? 1 : 0;
        }
    if (iTarget == 2) ID.ring += 2;

// we can only have 1 ramp meas per fill, let it have always runid=0
    if (mode != 4) runId = ID.fill + 0.1*ID.ring + 0.001*ID.run;
    else runId = ID.fill + 0.1*ID.ring;
/*
// Now write the new run number to CDEV...
    data.insert("value", runId);
    if (mode==2) {  // run number for emittance scans
      DEVSEND(pol, "set emitRunIdS", &data, NULL, LogFile, irc);
    }
    else {        // run number for polarization measurements
      DEVSEND(pol, "set runIdS", &data, NULL, LogFile, irc);
    }
*/
    tm=time(NULL);
    fprintf(LogFile,"RHICADO-INFO : %s: Starting for %s ring @ %5.2f GeV RunID =%8.3f\n",
        cctime(&tm), myColor[MyPolarimeter], beamEnergy, runId);
    fflush(LogFile);
    UpdateMessage(MyPolarimeter,"Starting...");
//      Start the script to measure polarization
    pid = fork ();
    if (pid == 0) {
        /* This is the child process.  Execute the shell command. */
        char s1[20], s2[20], s3[20], s4[20], s6[20];
        sprintf(s1, "%7.3f", runId);
        sprintf(s2, "%d", maxTime);
        sprintf(s3, "%d", numberEvents);
        sprintf(s4, "%3.0f", beamEnergy);
        if (mode==0) {
            sprintf(s6, "s");    // scaler mode
        } else if (mode==1) {
            sprintf(s6, "d");    // data mode
        } else if (mode==2) {
            sprintf(s6, "e");    // emittance scan
        } else if (mode==3) {
            sprintf(s6, "t");     // target scan
        } else if (mode==4) {
            sprintf(s6, "r");     // ramp scaler measurement
        }
        fprintf(LogFile,"RHICADO-INFO : %s %s %s %s %s %s %s \n",s1,s2,s3,s4,(MyPolarimeter == 0)?"y":"b",(iTarget == 2)?"2":"1",s6);
        fflush(LogFile);
        execl (SHELL, SHELL, ScriptName, s1, s2, s3, s4,
                 (MyPolarimeter == 0)?"y":"b", (iTarget == 2)?"2":"1", s6, NULL);
       _exit (EXIT_FAILURE);
    } else if (pid < 0) {
        /* The fork failed.  Report failure.  */
        Status |= STATUS_ERROR | ERR_FAILSTART;
        fprintf(LogFile,"RHICADO-ERR : Unable to run the measurement script\n");
        fflush(LogFile);
        UpdateMessage(MyPolarimeter,"Start FAILED");
        SetState(MyPolarimeter, "Stop");
        SetStatus(MyPolarimeter);
        CurrentPolarimeter = -1;
        ChildPid = 0;
        return;
    } else {
         /* This is the parent process. Only set alarm timer */
        alarm(maxTime + MAXANALYSISTIME + (int)(numberEvents*RTIMEPEREVENT));
        ChildPid = pid;
    }
}

void StopMeasurement()
{
    int i, j, irc;
    char sss[80];
    time_t tm;

    if (CurrentPolarimeter < 0) {
        if ((iCmd & 0xFFF) == CMD_CAN) SetState(MyPolarimeter, "Stop");
        return;
    }

    alarm(0);

    // Stop child script if necessary
    if (!(iCmd & CMD_DONE) && (ChildPid > 0)) {
        if ((iCmd & 0xFFF) == CMD_CAN)
            kill(ChildPid, SIGTERM);
        else
            kill(ChildPid, SIGINT);
        ChildPid = 0;
    }

    UpdateMessage(MyPolarimeter,"Finishing...");

    tm=time(NULL);
    fprintf(LogFile,"RHICADO-INFO : %s: Finished for %s ring, status: %8.8X\n",
                cctime(&tm), myColor[MyPolarimeter], Status);
    fflush(LogFile);

    if ((iCmd & 0xFFF) == CMD_CAN) {
        SetState(MyPolarimeter, "Cancel");
        Status |= WARN_CANCELLED;
    } else {
        SetState(MyPolarimeter, "Stop");
    }
    Status |= GetStatus(MyPolarimeter);
    SetStatus(MyPolarimeter);
    UpdateMessage(MyPolarimeter,"%s", stat2str(Status));

    CurrentPolarimeter = -1;
}

/* Asynchronous get handler */
void handleGetAsync(int status, void* arg, cdevRequestObject& req, cdevData& data)
{
    char cmd[20];

    cdevDevice& device = req.device();
    // the event is from polarimeter ADO
    if (strcmp(polCDEVName[MyPolarimeter], device.name()) != 0) {
        fprintf(LogFile,"RHICADO-WARN : Unexpected sourse of event %s\n",
                device.name());
        fflush(LogFile);
        return;
    }

    data.get("value", cmd, sizeof(cmd));
    switch (toupper(cmd[0])) {
    case 'S':
        iCmd = CMD_STOP | (CMD_YELLOW << MyPolarimeter);
        break;
    case 'C':
        iCmd = CMD_CAN | (CMD_YELLOW << MyPolarimeter);
        break;
    case 'R':
        iCmd = CMD_START | (CMD_YELLOW << MyPolarimeter);
        break;
    case 'D':
        iCmd = CMD_DATA | (CMD_YELLOW << MyPolarimeter);
        break;
    case 'E':
        iCmd = CMD_EMIT | (CMD_YELLOW << MyPolarimeter);
        break;
    case 'T':
        iCmd = CMD_TEST | (CMD_YELLOW << MyPolarimeter);
        break;
    case 'P':
        iCmd = CMD_RAMP | (CMD_YELLOW << MyPolarimeter);
        break;
    default :
        fprintf(LogFile,"RHICADO-WARN : Unknown command %s from dataAcquisitionS: %s\n",
                        cmd, device.name());
        fflush(LogFile);
        break;
    }
}


//      exit on signals grecefully
void exit_handle(int sig)
{
    iStop = sig;
}

void child_handle(int sig)
{
    int pid, status, serrno;
    serrno = errno;
    while (1) {
        pid = waitpid(WAIT_ANY, &status, WNOHANG);
        if (pid <= 0) break;
        iChild = 1;
    }
}

void alarm_handle(int sig)
{
    iAlarm = 1;
}

int main(int argc, char** argv)
{
    int i, irc;
    int iDaemonFlag = 1;
    int keepStd = 0;
    time_t itime;

//      process comman line options
    while ((i = getopt(argc, argv, "bydhvl:r:")) != -1) switch(i) {
    case 'y' :
        MyPolarimeter = 0;
        break;
    case 'b' :
        MyPolarimeter = 1;
        break;
    case 'v' :
        iVerbose++;
        break;
    case 'd' :
        iDaemonFlag = 0; // don't become a daemon
        keepStd = 1;
        break;
    case 'h' :
    case '?' :
        printf("\n\n\t\tPolarimeter ADO daemon\n"
"Usage: polado -y|b [-options]. Possible options are:\n"
"b: select BLUE ring;\n"
"d: run in terminal mode (not as daemon);\n"
"h: print this message and exit;\n"
"r [filename]: script to run for polarization measurement;\n"
"l [filename]: log file name;\n"
"v: more printout (you may use more 'v' to get more);\n"
"y: select YELLOW ring\n");
        return 0;
    case 'l' :
        strncpy(LogFileName, optarg, sizeof(LogFileName));
        break;
    case 'r' :
        strncpy(ScriptName, optarg, sizeof(ScriptName));
        break;
    }
    if (MyPolarimeter < 0) {
        printf("You must select the ring ! Exitting ...\n");
        return 0;
    }
// Create logfile.
    LogFile = fopen(LogFileName, "at");
    if (LogFile == NULL) {
        if (iDaemonFlag != 0) {
            fprintf(stderr, "RHICADO-ERR : Error creating logfile %s %s\n",
                LogFileName, strerror(errno));
            return -1;  // we can't be too silent
        } else {
            LogFile = stdout;
            keepStd = 1;
        }
    }
    if (keepStd == 0) {
//      Close standard io channels
        for(i=0; i<3; i++) close(i);
        for (i=0; i<3; i++) {
            irc = open("/dev/null", O_RDWR, 0);
            if (irc < 0)
                fprintf(LogFile, "/dev/null open error %s.\n", strerror(errno));
        }
    }

    if (iDaemonFlag != 0) {
//      This is the trick to detach the terminal
        if (fork())  return 0;
        setsid();
    }
//      catch these signals to exit grecefully
    signal(SIGINT, exit_handle);        // normal exit
    signal(SIGHUP, exit_handle);
    signal(SIGTERM, exit_handle);
    signal(SIGQUIT, exit_handle);
    signal(SIGALRM, alarm_handle);
    signal(SIGCHLD, child_handle);
//      print something
    itime = time(NULL);
    fprintf(LogFile, "RHICADO-INFO : %s: %s Polarimeter daemon started.\n", cctime(&itime), myColor[MyPolarimeter]);
    fflush(LogFile);

    /* Initialize state */
    if (SetState(MyPolarimeter, "Stop")) {
        fprintf(LogFile,"RHICADO-ERR : CDEV not accessible, exiting after 60 s pause...\n");
        fflush(LogFile);
        sleep(60);
        return -2;
    }
    CurrentPolarimeter = -1;

    cdevSystem   & defSystem      = cdevSystem::defaultSystem();
    cdevCallback cb(handleGetAsync, NULL);

    cdevDevice & myring = cdevDevice::attachRef(polCDEVName[MyPolarimeter]);
    myring.sendCallback("monitorOn dataAcquisitionS", NULL, cb);

    for (iCmd=0; iStop==0; ) {
        defSystem.pend((double)0.5);
        if (iChild != 0) {
            iCmd = CMD_STOP | CMD_DONE;
            iChild = 0;
        }
        if (iAlarm != 0) {
            iCmd = CMD_CAN;
            iAlarm = 0;
        }
        switch (iCmd & 0xFFF) {
            case CMD_START :
                StartMeasurement(0);
                break;
            case CMD_DATA :
                StartMeasurement(1);
                break;
            case CMD_EMIT :
                StartMeasurement(2);
                break;
            case CMD_TEST :
                StartMeasurement(3);
                break;
            case CMD_RAMP :
                StartMeasurement(4);
                break;
            case CMD_STOP :
            case CMD_CAN :
                StopMeasurement();
                break;
            default:
                break;
        }
        iCmd = 0;
    }

    itime = time(NULL);
    fprintf(LogFile, "RHICADO-INFO : %s: Exiting %s polarimeter daemon.\n\n", cctime(&itime), myColor[MyPolarimeter]);
    fflush(LogFile);
    fclose(LogFile);
    signal(SIGINT, SIG_DFL);
    signal(SIGHUP, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    signal(SIGALRM, SIG_DFL);
    signal(SIGCHLD, SIG_DFL);
    return 0;
}
