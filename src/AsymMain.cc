//  Asymmetry Analysis of RHIC pC Polarimeter
//  file name :   AsymMain.cc
//
//  Authors   :   Itaru Nakagawa
//                Dmitri Smirnov
//
//  Creation  :   11/17/2005
//

/**
 *
 * 18 Oct, 2010 - Dmitri Smirnov
 *    - Added -R flag, moved header files, cleaned up the code
 *
 */

#include "AsymMain.h"

using namespace std;

AsymRoot gAsymRoot;

// =================
// beginning of main
// =================
int main(int argc, char *argv[])
{
   // Create stopwatch and start it
   TStopwatch stopwatch;
   TTimeStamp timestamp;

   // for get option
   extern char *optarg;
   //extern int optind;

   // Initialize Variables
   Initialization();

   // prefix directories
   char * datadir = getenv("DATADIR");
   if ( datadir == NULL ){
      cerr << "environment DATADIR is not defined" << endl;
      cerr << "e.g. export DATADIR=$HOME/2005/data" << endl;
      exit(-1);
   }

   // config directories
   confdir = getenv("CONFDIR");

   if ( confdir == NULL ){
      cerr << "environment CONFDIR is not defined" << endl;
      cerr << "e.g. export CONFDIR=/usr/local/cnipol/config" << endl;
      exit(-1);
   }

   // files
   char   ifile[32], cfile[32], hbk_outfile[256];
   int    hbk_read = 0;  // hbk file read from argument:1 default:0
   //int  ramp_read = 0;  // ramp timing file read from argument:1 default:0

   // misc
   char enerange[20], cwidth[20], *ptr;
   char suffix[] = ".data"; // suffix of data file
   stringstream sstr;

   int option_index = 0;
   static struct option long_options[] = {
      {"run-name", required_argument, 0, 'f'},
      {"raw", 0, 0, 'r'},
      {"feedback", 0, 0, 'b'},
      {"no-error-detector", 0, 0, 'a'},
      {0, 0, 0, 0}
   };

   int c;

   while ((c = getopt_long(argc, argv, "?f:n:s:c:ho:rt:m:e:d:baCDTABZF:MNW:UGR:S",
                           long_options, &option_index)) != -1)
   {
      switch (c) {
      case 'h':
      case '?':
         cout << "Usage of " << argv[0] << endl;
         cout << " -h, -?                   : print this help" <<endl;
         cout << " -f, --run-name <run_name>: name of run with raw data in $DATADIR directory" <<endl;
         cout << " -n <number>              : maximum number of events to process"
              << " (default \"-n 0\" all events)" <<endl;
         cout << " -s <number>              : only every <number> event will be"
              << " processed (default \"-s 1\" no skip)" <<endl;
         cout << " -c <calib_file_name>     : root file with calibration info" << endl;
         cout << " -o <filename>            : Output hbk file" <<endl;
         //            cout << " -r <filename>        : ramp timing file" <<endl;
         cout << " -t <time shift>          : TOF timing shift in [ns], addition to TSHIFT defined in run.db " << endl;
         cout << " -e <lower:upper>         : kinetic energy range (default [400:900] keV)" <<endl;
         //cout << " <MODE> ---------------(default on)---------" <<endl;
         //            cout << " -B                   : create banana curve on" <<endl;
         //            cout << " -G                   : mass mode on " <<endl;
         cout << " -a, --no-error-detector  : anomaly check off " << endl;
         //cout << " <MODE> ---------------(default off)--------" <<endl;
         cout << " -r, --raw                : raw histograms on " << endl;
         cout << " -b                       : feedback mode on " << endl;
         cout << " -C                       : Calibration mode on " <<endl;
         cout << " -D                       : Dead layer  mode on " <<endl;
         cout << " -d <dlayer>              : Additional deadlayer thickness [ug/cm2]" << endl;
         //            cout << " -T                   : T0 study    mode on " <<endl;
         //            cout << " -A                   : A0,A1 study mode on " <<endl;
         //            cout << " -Z                   : without T0 subtraction" <<endl;
         cout << " -F <file>                : overwrite conf file defined in run.db" <<endl;
         cout << " -W <lower:upper>         : const width banana cut" <<endl;
         cout << " -m <sigma>               : banana cut by <sigma> from 12C mass [def]:3 sigma" << endl;
         cout << " -U                       : update histogram" <<endl;
         cout << " -N                       : store Ntuple events" <<endl;
         cout << " -R <bitmask>             : save events in Root trees, " <<
                 "e.g. \"-R 101\"" <<endl;
         exit(0);
      case 'f':
         sprintf(ifile, optarg);
         // if ifile lack of suffix ".data", attach ".data"
         if (strstr(ifile,suffix)==NULL) strcat(ifile,suffix);
         strcat(datafile, datadir);
         strcat(datafile,     "/");
         strcat(datafile,   ifile);
         // Add checks for runName suffix
         //sprintf(&runinfo.runName[0], optarg);
         runinfo.runName = optarg;
         fprintf(stdout, "runName:         %s\n", runinfo.runName.c_str());
         fprintf(stdout, "Input data file: %s\n", datafile);
         break;
      case 'n':
         gMaxEventsUser = atol(optarg);
         fprintf(stdout, "Max events to process: %d\n", gMaxEventsUser);
         break;
      case 's':
         dproc.thinout = atol(optarg);
         fprintf(stdout, "Events to skip: %d\n", dproc.thinout);
         break;
      case 'c':
         dproc.userCalibFile = optarg;
         fprintf(stdout, "User defined calibration file: %s\n", dproc.userCalibFile.c_str());
         break;
      case 'o': // determine output hbk file
         sprintf(hbk_outfile, optarg);
         fprintf(stdout,"Output hbk file: %s \n",hbk_outfile);
         hbk_read = 1;
         break;
      case 't': // set timing shift in banana cut
         dproc.tshift = atoi(optarg);
         extinput.TSHIFT = 1;
         break;
      case 'd': // set timing shift in banana cut
         dproc.dx_offset = atoi(optarg);
         break;
      case 'e': // set energy range
         strcpy(enerange, optarg);
         if ((ptr = strrchr(enerange, ':'))) {
            ptr++;
            dproc.eneu = atoi(ptr);
            strtok(enerange,":");
            dproc.enel = atoi(enerange);
            if (dproc.enel == NULL || dproc.enel<0)     { dproc.enel=0;}
            if (dproc.eneu == NULL || dproc.eneu>12000) { dproc.eneu=2000;}
            fprintf(stdout,"ENERGY RANGE LOWER:UPPER = %d:%d\n",
                    dproc.enel,dproc.eneu);
         } else {
             cout << "Wrong specification for energy threshold" <<endl;
             exit(0);
         }
         break;
      case 'a':
         Flag.EXE_ANOMALY_CHECK=0;
         break;
      case 'F':
         sprintf(cfile, optarg);
         if (!strstr(cfile,"/")) {
             strcat(reConfFile, confdir);
             strcat(reConfFile, "/");
         }
         strcat(reConfFile,  cfile);
         fprintf(stdout,"overwrite conf file : %s \n", reConfFile);
         extinput.CONFIG = 1;
         break;
      case 'b':
         dproc.FEEDBACKMODE = Flag.feedback = 1;
         break;
      case 'r':
         dproc.RAWHISTOGRAM = 1;
         break;
      case 'C':
         dproc.CMODE = 1;
         dproc.RECONFMODE = 0;
         gAsymRoot.fEventConfig = new EventConfig();
         break;
      case 'D':
         dproc.DMODE = 1;
         break;
      case 'T':
         dproc.TMODE = 1;
         fprintf(stdout,"*****TMODE*****\n");
         break;
      case 'A':
         dproc.AMODE = 1;
         break;
      case 'B':
         dproc.BMODE = 1;
         break;
      case 'Z':
         dproc.ZMODE = 1;
         break;
      case 'U':
         dproc.UPDATE = 1;
         break;
      case 'G':
         dproc.MMODE = 1;
         break;
      case 'N':
         dproc.NTMODE = 1;
         break;
      case 'W': // constant width banana cut
         dproc.CBANANA = 1;
         strcpy(cwidth, optarg);
         if ((ptr = strrchr(cwidth,':'))) {
             ptr++;
             dproc.widthu = atoi(ptr);
             strtok(cwidth,":");
             dproc.widthl = atoi(cwidth);
             fprintf(stdout,"CONSTANT BANANA CUT LOWER:UPPER = %d:%d\n",
                     dproc.widthl,dproc.widthu);
             if (dproc.widthu == dproc.widthl)
               fprintf(stdout,"WARNING: Banana Lower = Upper Cut\a\n");
         } else {
             fprintf(stdout,"Wrong specification constant banana cut\n");
             exit(0);
         }
         fprintf(stdout,"BANANA Cut : %d <==> %d \n",
                 dproc.widthl,dproc.widthu);
         break;
      case 'm':
         dproc.CBANANA = 2;
         dproc.MassSigma = atof(optarg);
         extinput.MASSCUT = 1;
         break;
      case 'R':
         sstr << optarg;
         sstr >> dproc.SAVETREES;
         cout << "SAVETREES: " << dproc.SAVETREES << endl;
         break;
      default:
         fprintf(stdout,"Invalid Option \n");
         exit(0);
      }
   }

   // Extract RunID from input filename
   int chrlen = strlen(ifile)-strlen(suffix) ; // f.e. 10100.101.data - .data = 10100.001
   char RunID[chrlen];
   strncpy(RunID, ifile, chrlen);
   RunID[chrlen] = '\0'; // Without RunID[chrlen]='\0', RunID screwed up.
   runinfo.RUNID = strtod(RunID, NULL); // return 0 when "RunID" contains alphabetical char.

   // Get PolarimetryID and RHIC Beam (Yellow or Blue) from RunID
   if (!dproc.CMODE) GetPolarimetryID_and_RHICBeam(RunID);

   // For normal runs, RUNID != 0. Then read run conditions from run.db.
   // Otherwise, data filename with characters skip readdb and reconfig routines
   // assuming these are energy calibration or test runs.
   if (runinfo.RUNID) {
       readdb(runinfo.RUNID);
   } else {
       dproc.RECONFMODE = 0;
   }

   //PrintDB();
   //exit(-1);

   // Overwrite some parameters with command line options
   if (!gAsymRoot.UseCalibFile(dproc.userCalibFile)) {
      perror("Error: Supplied calibration file is not valid\n");
      exit(-1);
   }

   // if output hbk file is not specified
   if (hbk_read == 0 ) {
       sprintf(hbk_outfile, "outsampleex.hbook");
       fprintf(stdout,"Hbook DEFAULT file: %s \n",hbk_outfile);
   }

   // ---------------------------------------------------- //
   //            Hbook Histogram Booking                   //
   // ---------------------------------------------------- //
   fprintf(stdout,"Booking ... histgram file\n");
   if (hist_book(hbk_outfile) != 0) {
       perror("Error: hist_book");
       exit(-1);
   }

   // ---------------------------------------------------- //
   //                 Root Histogram Booking               //
   // ---------------------------------------------------- //
   char filename[256];
   const char* tmpEnv = getenv("CNI_RESULTS_DIR");

   if (tmpEnv) gAsymEnv["CNI_RESULTS_DIR"] = tmpEnv;
   else        gAsymEnv["CNI_RESULTS_DIR"] = ".";

   gAsymEnv["CNI_RESULTS_DIR"].append("/");
   gAsymEnv["CNI_RESULTS_DIR"].append(RunID);

   if (gAsymEnv["CNI_RESULTS_DIR"].size() > 200) {
      printf("ERROR: Results dir name too long\n"); exit(-1);
   }

   //printf("s: %s, %s, %s\n", ifile, RunID, gAsymEnv["CNI_RESULTS_DIR"].c_str());

   umask(0);
   if (mkdir(gAsymEnv["CNI_RESULTS_DIR"].c_str(), 0777) < 0)
      printf("WARNING: Perhaps dir already exists: %s\n", gAsymEnv["CNI_RESULTS_DIR"].c_str());

   sprintf(filename, "%s/%s.root", gAsymEnv["CNI_RESULTS_DIR"].data(), runinfo.runName.c_str());

   fprintf(stdout, "Booking ROOT histograms: %s\n", filename);

   if (gAsymRoot.RootFile(filename) != 0) {
      perror("Error: RootFile()");
      exit(-1);
   }

   gAsymRoot.BookHists(runinfo);

   // Create tree if requested
   if (dproc.SAVETREES.any()) { gAsymRoot.CreateTrees(); }

   // ---------------------------------------------------- //
   // Quick Scan and Fit for tshift and mass sigma fit     //
   // ---------------------------------------------------- //
   if (dproc.FEEDBACKMODE){

     printf("Feedback Sparcification Factor = 1/%d \n", dproc.thinout);

     if (readloop() != 0) {
       perror("Error: readloop");
       exit(-1);
     }

     Flag.feedback=0;

   } else {
      // ---------------------------------------------------- //
      //                  Main Event Loop                     //
      // ---------------------------------------------------- //
      if (readloop() != 0) {
         perror("Error: readloop");
         exit(-1);
      }
   }

   // ---------------------------------------------------- //
   //        Delete Unnecessary ROOT Histograms            //
   // ---------------------------------------------------- //
   if (gAsymRoot.DeleteHistogram() !=0) {
       perror("Error: DeleteHistogram()");
       exit(-1);
   }

   // ---------------------------------------------------- //
   //                  Closing Histogram File              //
   // ---------------------------------------------------- //
   if (hist_close(hbk_outfile) !=0) {
       perror("Error: hist_close");
       exit(-1);
   }

   // Update calibration constants if requested
   if (dproc.UPDATE)
      gAsymRoot.UpdateRunConfig();

   // Stop stopwatch and save results
   stopwatch.Stop();
   dproc.procDateTime =  timestamp.GetSec();
   dproc.procTimeReal =  stopwatch.RealTime();
   dproc.procTimeCpu  =  stopwatch.CpuTime();

   printf("Processing started: %s\n",   timestamp.AsString("l"));
   printf("Process time: %f seconds\n", dproc.procTimeReal);

   // Set pointers to global structures for later saving in ROOT file
   if (!gAsymRoot.UseCalibFile() || dproc.CMODE) {

      gAsymRoot.fEventConfig->fConfigInfo = cfginfo;
      //printf("fConfigInfo: %#x\n", gAsymRoot.fEventConfig->fConfigInfo);
      //gAsymRoot.fEventConfig->PrintAsPhp(stdout);
   }

   // if previously allocated delete object
   delete gAsymRoot.fEventConfig->fRunInfo;
   gAsymRoot.fEventConfig->fRunInfo    = &runinfo;

   //delete gAsymRoot.fEventConfig->fDatproc;
   gAsymRoot.fEventConfig->fDatproc    = &dproc;

   delete gAsymRoot.fEventConfig->fRunDB;
   gAsymRoot.fEventConfig->fRunDB      = &rundb;

   //gAsymRoot.fEventConfig->PrintAsPhp(stdout);

   // ---------------------------------------------------- //
   //                     Closing ROOT File                //
   // ---------------------------------------------------- //
   if (gAsymRoot.CloseROOTFile() !=0) {
       perror("Error: CloseROOTFile()");
       exit(-1);
   }

   exit(1);
}


// ===================================
// for Bunch by Bunch base analysis
// ===================================
int BunchSelect(int bid)
{
  int go = 0;
  //  int BunchList[11]={4,13,24,33,44,53,64,73,84,93,104};
  int BunchList[26]={3,6,13,16,23,26,33,36,43,46,53,56,63,66,
                     73,76,83,86,93,96,103,106};

  for (int i=0; i<14; i++) {
    //    BunchList[i]++;
    if (bid == BunchList[i]) {
      go=1;
      break;
    }
  }

  return go;
}


//
// Class name  :
// Method name : GetPolarimetryID_and_RHICBeam(char RunID[])
//
// Description : Identify Polarimety ID and RHIC Beam (blue or yellow)
// Input       : char RunID[]
// Return      :
//
int GetPolarimetryID_and_RHICBeam(char RunID[])
{
  char ID = *(strrchr(RunID,'.')+1);

  switch (ID) {
  case '0':
    runinfo.RHICBeam = 0;
    runinfo.PolarimetryID = 1; //   blue polarimeter-1
    break;
  case '1':
    runinfo.RHICBeam = 1;
    runinfo.PolarimetryID = 1; // yellow polarimeter-1
    break;
  case '2':
    runinfo.RHICBeam = 0;
    runinfo.PolarimetryID = 2; //   blue polarimeter-2
    break;
  case '3':
    runinfo.RHICBeam = 1;
    runinfo.PolarimetryID = 2; // yellow polarimeter-2
    break;
  default:
    fprintf(stdout, "Unrecognized RHIC beam and Polarimeter-ID. Perhaps calibration data..?");
    break;
  }

  /*
  fprintf(stdout,"RUNINFO: RunID=%.3f RHICBeam=%d PolarimetryID=%d\n",
          runinfo.RUNID, runinfo.RHICBeam, runinfo.PolarimetryID);
  */

  return 0;
}


// =========================
// Read the parameter file
// =========================

// Ramp timing file
int read_ramptiming(char *filename)
{
    fprintf(stdout,"\nReading ... cut parameter file : %s \n", filename);

    ifstream rtiming;
    rtiming.open(filename);

    if (!rtiming) {
        cerr << "failed to open ramp timing file" <<endl;
        exit(1);
    }
    memset(ramptshift, 0, sizeof(ramptshift));

    float runt;

    int index = 0;
    while (!rtiming.eof()) {
        rtiming >> runt >> ramptshift[index] ;
        index ++;
    }

    rtiming.close();
    return(0);
}


// Calibration parameter
void reConfig(recordConfigRhicStruct *cfginfo)
{
    ifstream configFile;
    configFile.open(reConfFile);

    if (!configFile) {
       cerr << "Failed to open Config File : " << reConfFile << endl;
       cerr << "Proceed with original configuration from raw data file" << endl;
       return;
    }

    fprintf(stdout,"**********************************\n");
    fprintf(stdout,"** Configuration is overwritten **\n");
    fprintf(stdout,"**********************************\n");

    cout << "Reading configuration info from : " << reConfFile <<endl;

    char  *tempchar;
    char   buffer[300];
    float  t0n, ecn, edeadn, a0n, a1n, ealphn, dwidthn, peden;
    float  c0n, c1n, c2n, c3n, c4n;
    int    stripn;
    int    linen = 0;

    while (!configFile.eof()) {

       configFile.getline(buffer, sizeof(buffer), '\n');

       if (strstr(buffer,"Channel")!=0) {

          tempchar = strtok(buffer,"l");
          stripn   = atoi(strtok(NULL, "="));
          t0n      = atof(strtok(NULL, " "));
          ecn      = atof(strtok(NULL, " "));
          edeadn   = atof(strtok(NULL, " "));
          a0n      = atof(strtok(NULL, " "));
          a1n      = atof(strtok(NULL, " "));
          ealphn   = atof(strtok(NULL, " "));
          dwidthn  = atof(strtok(NULL, " ")) + dproc.dx_offset; // extra thickness
          peden    = atof(strtok(NULL, " "));
          c0n      = atof(strtok(NULL, " "));
          c1n      = atof(strtok(NULL, " "));
          c2n      = atof(strtok(NULL, " "));
          c3n      = atof(strtok(NULL, " "));
          c4n      = atof(strtok(NULL, " "));

          cfginfo->data.chan[stripn-1].edead  = edeadn;
          cfginfo->data.chan[stripn-1].ecoef  = ecn;
          cfginfo->data.chan[stripn-1].t0     = t0n;
          cfginfo->data.chan[stripn-1].A0     = a0n;
          cfginfo->data.chan[stripn-1].A1     = a1n;
          cfginfo->data.chan[stripn-1].acoef  = ealphn;
          cfginfo->data.chan[stripn-1].dwidth = dwidthn;
          cfginfo->data.chan[stripn-1].pede   = peden;
          cfginfo->data.chan[stripn-1].C[0]   = c0n;
          cfginfo->data.chan[stripn-1].C[1]   = c1n;
          cfginfo->data.chan[stripn-1].C[2]   = c2n;
          cfginfo->data.chan[stripn-1].C[3]   = c3n;
          cfginfo->data.chan[stripn-1].C[4]   = c4n;

          cout << " Strip "    << stripn;
          cout << " Ecoef "    << ecn;
          cout << " T0 "       << t0n;
          cout << " A0 "       << a0n;
          cout << " A1 "       << a1n;
          cout << " Acoef "    << ealphn;
          cout << " Dwidth "   << dwidthn;
          cout << " Pedestal " << peden    << endl;
       }

       linen ++;
    }

    configFile.close();
}


//
// Class name  :
// Method name : ConfigureActiveStrip(int mask.detector)
//
// Description : Disable detector and configure active strips
//
// Input       : int mask.detector
// Return      : runinfo.ActiveDetector[i] remains masked strip configulation
//
int ConfigureActiveStrip(int mask)
{
   // Disable Detector First
   for (int i=0; i<NDETECTOR; i++) {
     if ((~mask>>i)&1) {
       runinfo.ActiveDetector[i] = 0x000;
       for (int j=0;j<NSTRIP_PER_DETECTOR; j++) {
         runinfo.NActiveStrip--;
         runinfo.ActiveStrip[i*NSTRIP_PER_DETECTOR+j] = 0;
       }
     }
   }

   // Configure Active Strips
   int det, strip=0;
   for (int i=0; i<runinfo.NDisableStrip; i++) {
     det   = runinfo.DisableStrip[i]/NSTRIP_PER_DETECTOR;

     // skip if the detector is already disabled
     if ((mask>>det)&1) {
       strip = runinfo.DisableStrip[i] - det*NSTRIP_PER_DETECTOR;
       runinfo.ActiveDetector[det] ^= int(pow(2,double(strip))); // mask strips of detector=det
       runinfo.ActiveStrip[strip+det*NSTRIP_PER_DETECTOR] = 0;
       runinfo.NActiveStrip--;
     }

   } // end-of-for(runinof.NDisableStrip) loop

   // Active Detector and Strip Configulation
   printf("ReConfigured Active Detector =");
   for (int i=0; i<NDETECTOR; i++)  printf(" %1d", runinfo.ActiveDetector[i] ? 1 : 0 );
   printf("\n");
   //    printf("Active Strip Config =");
   //    for (int i=NDETECTOR-1; i>=0; i--) printf(" %x", runinfo.ActiveDetector[i]);
   //    printf("\n");
   printf("Reconfigured Active Strip Config =");
   for (int i=0; i<NSTRIP; i++) {
     if (i%NSTRIP_PER_DETECTOR==0) printf(" ");
     printf("%d", runinfo.ActiveStrip[i]);
   }
   printf("\n");

   return 0;
}


//
// Class name  :
// Method name : DisabledDet
//
// Description : Check Disabled detector
// Input       : int strip nuumber
// Return      : 1 if disabled. otherwise 0
//
/*
int
DisabledDet(int det){

  // det(0,1,2,3,4,5} => {0, 1, 0, 0, 1, 0} => 18
  int DeadDet = tgt.VHtarget ? 18 : 0 ;
  //                            ^   ^
  //                       H-target V-target

  return DeadDet>>det & 1 ;

}
*/


// ===================
// square root formula
// ===================
// A-RightUp  B-LeftDown  C-RightDown  D-LeftUp
// elastic Carbons are scattered off more in Right for Up
int sqass(float A, float B, float C, float D, float *asym, float *easym)
{
   float den;
   den = sqrt(A*B) + sqrt(C*D);
   if ( (A*B == 0.) && (C*D == 0.) ) {
      *asym  = 0.;
      *easym = 0.;
   } else {
      *asym  = (sqrt(A*B) - sqrt(C*D))/den;
      *easym = sqrt(A*B*(C+D) + C*D*(A+B))/den/den;
   }

   return(0);
}


//
// Class name  :
// Method name : Initialization
//
// Description : Initialize variables
//             :
// Input       :
// Return      :
//
int Initialization()
{
  for (int i=0; i<NSTRIP; i++) {
    feedback.mdev[i] = 0.;
    feedback.RMS[i] = dproc.OneSigma ;
  }

  //runinfo.TgtOperation = "fixed";
  strcpy(runinfo.TgtOperation, "fixed");

  // Initiarize Strip counters
  for (int i=0; i<NSTRIP; i++) {

    for (int j=0; j<3; j++)
       cntr.reg.NStrip[j][i] = cntr.alt.NStrip[j][i] = cntr.phx.NStrip[j][i] = cntr.str.NStrip[j][i] = 0;

    for (int j=0; j<3; j++) {
       for(int kk=0;kk<MAXDELIM;kk++)
          cntr_tgt.reg.NStrip[kk][j][i] = 0;
    }
  }

  return 1;
}
