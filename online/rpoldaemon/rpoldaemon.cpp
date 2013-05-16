/********************************************************
 *      RHIC Polarimeter Daemon Program                 *
 *      I. Alekseev and D. Svirida                      *
 *              2001-2011                               *
 * The code runs all the time waiting for the command   *
 * to start measurement. Takes care of two polarimeters *
 * of the same DAQ system. Uses script to run actual    *
 * measurement. The script must take the following      *
 * arguments: polarimeter_name command runId            *
 ********************************************************
 * Changes for run 12 - "blue" and "yellow" again       *
 ********************************************************/

/**
 * Jan 30, 2012 - Dmitri Smirnov
 *    -
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

#include "globals.h"
#include "rhicpol.h"
#include "rcdev.h"

#define EXIT_BADSCRIPT  127

char myName[2][20] = {"Blue", "Yellow"};
int  myDev[2][2]   = {{0, 2}, {1, 3}};                            // polCDEVName for Blue/Yellow

FILE *LogFile;
char LogFileName[256] = "/dev/stdout";
char ScriptName[256]  = "rpolMeasure.sh";
int iVerbose = 0;

pid_t ChildPid         =  0;
int MyPolarimeter      = -1;   // Blue/Yellow
int CurrentPolarimeter = -1;   // we can run one of two our polarimeters only
int gStatus            =  0;   // Our error status
int iStop              =  0;   // we should Stop
int iChild             =  0;   // Child exitted

struct RQST {
   int polarim;                // polarimeter
   char cmd[256];              // command
} Request = { -1, ""};         // Request to Run/Stop etc.

//      this function removes \n from the standard ctime function output
char * cctime(time_t *itime)
{
   char *str;
   str = ctime(itime);
   str[strlen(str) - 1] = '\0';
   return str;
}

// Set CDEV:statusStringS
int UpdateMessage(int polarim, char * fmt, ...)
{
   cdevData data;
   char str[80];
   int irc = 0;
   va_list ap;

   va_start(ap, fmt);
   vsprintf(str, fmt, ap);

   cdevDevice & pol = cdevDevice::attachRef(polCDEVName[polarim]);
   data.insert("value", str);
   DEVSEND(pol, "set statusStringS", &data, NULL, LogFile, irc);
   return irc;
}

// Set CDEV:DataAcquisitionS
int SetState(int polarim, char *state)
{
   cdevData data;
   int irc = 0;
   cdevDevice & pol = cdevDevice::attachRef(polCDEVName[polarim]);
   data.insert("value", state);
   DEVSEND(pol, "set dataAcquisitionS", &data, NULL, LogFile, irc);
   return irc;
}


// Translate status bits to string message
char* Status2String(int status)
{
   static char ststr[80];

   char longnames[32][50] = {
      // first 16 bits
      "W-Internal warning, see logfile",
      "W-Cancelled",
      "W-No fill number availiable",
      "W-WFD unreliable",
      "W-CNTS small internal counts inconsistency",
      "", "", "", "", "", "", "", "", "", "", "",
      // next 16 bits
      "E-This is not a real measurement, test only",
      "E-Severe internal error, see logfile",
      "E-Serious WFD error",
      "E-Camac access error",
      "E-Configuration file error",
      "E-Can't switch shaper LV PS",
      "E-Can't access RHIC ADO",
      "",
      "E-Can't start measurement script",
      "E-Target control error",
      "E-Device is busy",
      "", "", "", "", ""
   };

   char shortnames[32][10] = {
      "WInt", "WCan", "WNoFill#", "WWFD",
      "WCNTS", "", "", "", "", "", "", "", "", "", "", "",
      "ETest", "EIntr", "EWFD", "ECAMAC", "EConf", "ELVPS",
      "EADO", "", "EStart", "ETarg",
      "EBUSY", "", "", "", "", ""
   };

   int i, j, k;

   // Count active status bits
   for (i = 0, j = 0, k = -1; i < 31; i++) {
      if (((status >> i) & 1) == 1) {
         j++;
         k = i;
      }
   }

   if (j == 0) {               // no errors or warnings
      strcpy(ststr, "OK");
   }
   else if (j == 1) {          // if only one message write it out fully
      strcpy(ststr, &longnames[k][0]);
   }
   else {                      // if many messages write them out using short names
      ststr[0] = '\0';
      for (i = 0; i < 31; i++) {
         if (((status >> i) & 1) == 1) {
            strncpy(&ststr[strlen(ststr)], &shortnames[i][0], sizeof(ststr) - strlen(ststr) - 1);
            strncpy(&ststr[strlen(ststr)], " ", sizeof(ststr) - strlen(ststr) - 1);
         }
      }
   }

   return ststr;
}


// Send our status to CDEV:statusS
int SetStatus(int polarim)
{
   cdevData data;
   int irc = 0;
   cdevDevice & pol = cdevDevice::attachRef(polCDEVName[polarim]);
   data.insert("value", gStatus);
   DEVSEND(pol, "set statusS", &data, NULL, LogFile, irc);
   return irc;
}


// Get status from CDEV:statusS
int GetStatus(int polarim)
{
   cdevData data;
   int irc = 0;
   cdevDevice & pol = cdevDevice::attachRef(polCDEVName[polarim]);
   DEVSEND(pol, "get statusS", NULL, &data, LogFile, irc);
   data.get("value", &irc);
   return irc;
}


// Start measurement
void StartMeasurement(int polarim, char *cmd)
{
   int      irc = 0;
   cdevData data;
   pid_t    pid;
   time_t   tm;
   double   runId;
   int      fillNumber;
   char     str[20];

   struct {
      int fill;
      int polarim;
      int run;
   } ID;

   // Already started - ignore start if we are already running
   if (CurrentPolarimeter >= 0) return;

   CurrentPolarimeter = polarim;       // this polarimeter will be running
   gStatus = 0;
   SetStatus(polarim);                 // clear old errors

   // first get necessary parameters - we need to create run number
   cdevDevice &pol  = cdevDevice::attachRef(polCDEVName[polarim]);        // the source of the old run number
   cdevDevice &spec = cdevDevice::attachRef(specCDEVName[MyPolarimeter]); // the source of the fill number

   DEVSEND(pol, "get runIdS", NULL, &data, LogFile, irc);
   data.get("value", &runId);

   DEVSEND(spec, "get fillNumberM", NULL, &data, LogFile, irc);
   data.get("value", &fillNumber);

   if (irc != 0 || fillNumber <= 0) {
      gStatus |= (STATUS_ERROR | WARN_NOBEAM | ERR_NOADO);
      UpdateMessage(polarim, "Run parameters unaccessible");
      SetStatus(polarim);
      SetState(polarim, "Stop");
      CurrentPolarimeter = -1;
      return;
   }

   // decode runId to ID
   ID.fill    = (int) (runId + 0.0001);
   ID.polarim = (int) (10.0 * (runId + 0.0001 - ID.fill));
   ID.run     = (int) (1000.0 * (runId + 0.0001 - ID.fill - 0.1 * ID.polarim));

   if (ID.fill == fillNumber) { // same fill number
      ID.run++;
   }
   else {   // new fill number
      ID.fill = fillNumber;
      ID.polarim = polarim;
      ID.run = 1;
   }

   if (toupper(cmd[0]) == 'T') {
      runId = 99999.999;
   }
   else {
      runId = ID.fill + 0.1 * ID.polarim + 0.001 * ID.run;
      data.insert("value", runId);
      DEVSEND(pol, "set runIdS", &data, NULL, LogFile, irc);
      if (irc) {
         fprintf(LogFile, "RHICDAEMON-ERROR: %s set runIdS(%8.3f) irc= %d\n", polCDEVName[polarim], runId, irc);
         fflush(LogFile);
      }
   }

   tm = time(NULL);
   fprintf(LogFile, "RHICDAEMON-INFO : %s: Starting %s for %s RunID=%8.3f\n", cctime(&tm), cmd, polCDEVName[polarim], runId);
   fflush(LogFile);
   UpdateMessage(polarim, "Starting...");

   // Start the script to measure polarization
   pid = fork();

   if (pid == 0) {
      // This is the child process. Execute the shell command
      sprintf(str, "%8.3f", runId);
      if (iVerbose) {
         fprintf(LogFile, "RHICDAEMON-DEBUG : %s %s %s\n", polCDEVName[polarim], cmd, str);
         fflush(LogFile);
      }

      execl(SHELL, SHELL, ScriptName, polCDEVName[polarim], cmd, str, NULL);

      // We shall never be here if script can be executed
      fprintf(LogFile, "RHICDAEMON-ERROR : Unable to run the measurement script %s\n", ScriptName);
      fflush(LogFile);
      _exit(EXIT_BADSCRIPT);
   }
   else if (pid < 0) {
      // The fork failed.  Report failure
      gStatus |= STATUS_ERROR | ERR_FAILSTART;
      fprintf(LogFile, "RHICDAEMON-ERROR : Unable to run the measurement script - fork failed.\n");
      fflush(LogFile);
      UpdateMessage(polarim, "Start FAILED");
      SetState(polarim, "Stop");
      SetStatus(polarim);
      CurrentPolarimeter = -1;
      ChildPid = 0;
   }
   else {
      // Everything is OK so far, this is the parent process - catch pid
      ChildPid = pid;
   }
}


// Stop measurement (with data readout and analysis etc.)
void StopMeasurement(int polarim)
{
   time_t tm;

   if (CurrentPolarimeter != polarim) return;  // Nothing to do
   if (ChildPid) kill(ChildPid, SIGINT);       // Inform the script that it is stopping

   UpdateMessage(CurrentPolarimeter, "Stopping...");
   SetStatus(CurrentPolarimeter);

   tm = time(NULL);
   fprintf(LogFile, "RHICDAEMON-INFO : %s: Stop requested for %s, status: %8.8X\n", cctime(&tm), polCDEVName[CurrentPolarimeter], gStatus);
   fflush(LogFile);
}


// Cancel measurement - stop immediately
void CancelMeasurement(int polarim)
{
   time_t tm;

   if (CurrentPolarimeter != polarim) {
      SetState(polarim, "Stop");              // return the button to Stop
      return;
   }

   if (ChildPid) kill(ChildPid, SIGTERM);
   ChildPid = 0;

   tm = time(NULL);
   fprintf(LogFile, "RHICDAEMON-INFO : %s: Cancel for %s,\n", cctime(&tm), polCDEVName[CurrentPolarimeter]);
   fflush(LogFile);

   gStatus |= WARN_CANCELLED;
   SetStatus(CurrentPolarimeter);
   UpdateMessage(CurrentPolarimeter, "Cancelled");
   SetState(CurrentPolarimeter, "Stop");

   CurrentPolarimeter = -1;
}


/** Asynchronous get handler */
void handleGetAsync(int status, void* arg, cdevRequestObject& req, cdevData& data)
{
   int i;

   cdevDevice& device = req.device();

   //  Find from which polarimeter. We will look for all possible, ignoring
   //  here error of getting from other DAQ.
   //  This should never happen...
   for (i = 0; i < 4; i++) if (strcmp(polCDEVName[i], device.name()) == 0) break;

   if (i == 4) {
      fprintf(LogFile, "RHICDAEMON-WARN : Unexpected source of event %s\n", device.name());
      fflush(LogFile);
      return;
   }

   data.get("value", Request.cmd, sizeof(Request.cmd));        // get the command
   Request.polarim = i;
}


// Exit on signals grecefully
void exit_handle(int sig)
{
   iStop = sig;
}


/** The child ended - the measuremnt is done */
void child_handle(int sig)
{
   int pid, status;
   while (1) {
      pid = waitpid(WAIT_ANY, &status, WNOHANG);
      if (pid <= 0) break;
      iChild = 0x100000 + status;
   }
}


/** */
int main(int argc, char** argv)
{
   int i;
   time_t itime, tm;

   // process comman line options
   while ((i = getopt(argc, argv, "UDBYhvl:r:")) != -1) switch (i) {
      case 'U' :  // leaving U just in case...
      case 'B' :
         MyPolarimeter = 0;
         break;
      case 'D' :  // leaving D just in case...
      case 'Y' :
         MyPolarimeter = 1;
         break;
      case 'v' :
         iVerbose++;
         break;
      case 'h' :
      case '?' :
         printf( "\n\n\t\tPolarimeter daemon\n"
                 "Usage: rpoldaemon -B|Y [-options]. Possible options are:\n"
                 "B/Y          - Blue/Yellow DAQ;\n"
                 "h,?          - print this message and exit;\n"
                 "r [filename] - script to run for polarization measurement;\n"
                 "l [filename] - log file name;\n"
                 "v            - more printout (you may use more 'v' to get more).\n");
         return 0;
      case 'l' :
         strncpy(LogFileName, optarg, sizeof(LogFileName));
         break;
      case 'r' :
         strncpy(ScriptName, optarg, sizeof(ScriptName));
         break;
      }

   if (MyPolarimeter < 0) {
      printf("You must select Upstream or Downstream! Exitting ...\n");
      return 0;
   }

   // Create logfile
   LogFile = fopen(LogFileName, "at");

   if (LogFile == NULL) {
      LogFile = stdout;
      fprintf(LogFile, "RHICDAEMON-ERROR : can not open log file %s. Sending messages to stdout\n", LogFileName);
      fflush(LogFile);
   }

   // Catch these signals to exit grecefully
   signal(SIGINT,  exit_handle);       // normal exit
   signal(SIGHUP,  exit_handle);
   signal(SIGTERM, exit_handle);
   signal(SIGQUIT, exit_handle);
   signal(SIGCHLD, child_handle);      // end of measurement

   // Print something
   itime = time(NULL);
   fprintf(LogFile, "RHICDAEMON-INFO : %s: %s Polarimeter daemon started.\n", cctime(&itime), myName[MyPolarimeter]);
   fflush(LogFile);

   // Initialize state
   if (SetState(myDev[MyPolarimeter][0], "Stop") || SetState(myDev[MyPolarimeter][1], "Stop")) {
      fprintf(LogFile, "RHICDAEMON-FATAL : CDEV not accessible, exiting after 60 s pause...\n");
      fflush(LogFile);
      sleep(60);
      return -2;
   }

   UpdateMessage(myDev[MyPolarimeter][0], "");
   UpdateMessage(myDev[MyPolarimeter][1], "");

   // Hook Action button
   cdevSystem &defSystem = cdevSystem::defaultSystem();
   cdevCallback cb(handleGetAsync, NULL);

   cdevDevice &dev0 = cdevDevice::attachRef(polCDEVName[myDev[MyPolarimeter][0]]);
   dev0.sendCallback("monitorOn dataAcquisitionS", NULL, cb);

   cdevDevice &dev1 = cdevDevice::attachRef(polCDEVName[myDev[MyPolarimeter][1]]);
   dev1.sendCallback("monitorOn dataAcquisitionS", NULL, cb);

   // Initialize flags
   CurrentPolarimeter = -1;
   iStop              = 0;
   iChild             = 0;
   Request.polarim    = -1;

   // Infinite loop
   for (;;) {
      if (iStop) break;

      // iChild != 0 when the child exits
      if (iChild) {
         if (iVerbose) {
            fprintf(LogFile, "RHICDAEMON-DEBUG : Child %d returned to %d with %X.\n", ChildPid, CurrentPolarimeter, iChild);
            fflush(LogFile);
         }

         ChildPid = 0; // the measurement is over

         if (CurrentPolarimeter >= 0) {
            // Get current status from CDEV
            gStatus = GetStatus(CurrentPolarimeter);
            SetState(CurrentPolarimeter, "Stop");

            if (iChild == (0x100000 + (EXIT_BADSCRIPT << 8))) {
               gStatus |= STATUS_ERROR | ERR_FAILSTART;
            }

            SetStatus(CurrentPolarimeter);
            UpdateMessage(CurrentPolarimeter, Status2String(gStatus));
            tm = time(NULL);

            fprintf(LogFile, "RHICDAEMON-INFO : %s: Finishing %s with %s\n", cctime(&tm),
                    polCDEVName[CurrentPolarimeter], Status2String(gStatus));
            fflush(LogFile);
            CurrentPolarimeter = -1;
         }

         iChild = 0;
      }

      if (Request.polarim >= 0) {     // Process commands
         if (CurrentPolarimeter >= 0 && CurrentPolarimeter != Request.polarim) {
            SetState(Request.polarim, "Stop");              // return the button to Stop
            UpdateMessage(Request.polarim, "BUSY...");
         }
         else {
            // Only the first letter of the command matters...
            switch (toupper(Request.cmd[0])) {
            case 'S':       // Stop
               StopMeasurement(Request.polarim);
               break;
            case 'C':       // Cancel
               CancelMeasurement(Request.polarim);
               break;
            default :
               StartMeasurement(Request.polarim, Request.cmd);
               break;
            }
         }

         Request.polarim = -1;
      }

      defSystem.pend((double) 0.2);
   }

   if (iStop != 0 && CurrentPolarimeter >= 0) CancelMeasurement(CurrentPolarimeter);

   itime = time(NULL);
   fprintf(LogFile, "RHICDAEMON-INFO : %s: Exiting %s polarimeter daemon.\n\n", cctime(&itime), myName[MyPolarimeter]);
   fflush(LogFile);
   fclose(LogFile);

   signal(SIGINT,  SIG_DFL);
   signal(SIGHUP,  SIG_DFL);
   signal(SIGTERM, SIG_DFL);
   signal(SIGQUIT, SIG_DFL);
   signal(SIGCHLD, SIG_DFL);

   return 0;
}
