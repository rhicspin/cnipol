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

#include <iostream>
#include <getopt.h>
#include <sys/stat.h>
#include <sstream>
#include <ctime>

#include "TStopwatch.h"
#include "TTimeStamp.h"

#include "EventConfig.h"

#include "AsymRoot.h"
#include "AsymRead.h"
//#include "AsymDbFile.h"
#include "AsymDbSql.h"
#include "AsymHbook.h"
#include "MseRunInfo.h"
#include "MseRunPeriod.h"

using namespace std;


/** Main program */
int main(int argc, char *argv[])
{ //{{{
   // Create a stopwatch and start it
   TStopwatch stopwatch;
   TTimeStamp timestamp;

   time_t  gtime = time(0);
   tm     *ltime = localtime(&gtime);

   // Create all main (global) objects
   gAsymRoot = new AsymRoot();

   gAsymRoot->GetRunConfigs(gRunInfo, gAnaInfo, gAnaResult);

   //gAsymDb  = new AsymDbFile();
   gAsymDb2 = new AsymDbSql();

   // for get option
   extern char *optarg;
   //extern int optind;

   // config directories
   confdir = getenv("CONFDIR");

   if ( confdir == NULL ){
      cerr << "environment CONFDIR is not defined" << endl;
      cerr << "e.g. export CONFDIR=/usr/local/cnipol/config" << endl;
      return 0;
   }

   char   cfile[32];
   //int  ramp_read = 0;  // ramp timing file read from argument:1 default:0
   char   enerange[20], cwidth[20], *ptr;
   stringstream sstr;
   int    option_index = 0;

   static struct option long_options[] = {
      {"run-name",            required_argument,   0,   'r'},
      {"feedback",            0,                   0,   'b'},
      {"no-error-detector",   0,                   0,   'a'},
      {"copy",                no_argument,         0,   AnaInfo::FLAG_COPY},
      {"copy-results",        no_argument,         0,   AnaInfo::FLAG_COPY},
      {"update-db",           no_argument,         0,   AnaInfo::FLAG_UPDATE_DB},
      {"no-update-db",        no_argument,         0,   AnaInfo::FLAG_NO_UPDATE_DB},
      {"use-db",              no_argument,         0,   AnaInfo::FLAG_USE_DB},
      {"pol-id",              required_argument,   0,   AnaInfo::OPTION_POL_ID},
      {"log",                 optional_argument,   0,   'l'},
      {"alpha",               no_argument,         0,   AnaInfo::MODE_ALPHA},
      {"calib",               no_argument,         0,   AnaInfo::MODE_CALIB},
      {"scaler",              no_argument,         0,   AnaInfo::MODE_SCALER},
      {"raw",                 no_argument,         0,   AnaInfo::MODE_RAW},
      {"target",              no_argument,         0,   AnaInfo::MODE_TARGET},
      {"profile",             no_argument,         0,   AnaInfo::MODE_PROFILE},
      {"asym",                no_argument,         0,   AnaInfo::MODE_ASYM},
      {"kinemat",             no_argument,         0,   AnaInfo::MODE_KINEMAT},
      {"pmt",                 no_argument,         0,   AnaInfo::MODE_PMT},
      {"quick",               no_argument,         0,   'q'},
      {"graph",               no_argument,         0,   AnaInfo::MODE_GRAPH},
      {"no-graph",            no_argument,         0,   AnaInfo::MODE_NO_GRAPH},
      {"mode-alpha",          no_argument,         0,   AnaInfo::MODE_ALPHA},
      {"mode-calib",          no_argument,         0,   AnaInfo::MODE_CALIB},
      {"mode-graph",          no_argument,         0,   AnaInfo::MODE_GRAPH},
      {"mode-no-graph",       no_argument,         0,   AnaInfo::MODE_NO_GRAPH},
      {"mode-normal",         no_argument,         0,   AnaInfo::MODE_NORMAL},
      {"mode-no-normal",      no_argument,         0,   AnaInfo::MODE_NO_NORMAL},
      {"mode-scaler",         no_argument,         0,   AnaInfo::MODE_SCALER},
      {"mode-raw",            no_argument,         0,   AnaInfo::MODE_RAW},
      {"mode-run",            no_argument,         0,   AnaInfo::MODE_RUN},
      {"mode-target",         no_argument,         0,   AnaInfo::MODE_TARGET},
      {"mode-profile",        no_argument,         0,   AnaInfo::MODE_PROFILE},
      {"mode-asym",           no_argument,         0,   AnaInfo::MODE_ASYM},
      {"mode-kinemat",        no_argument,         0,   AnaInfo::MODE_KINEMAT},
      {"mode-pmt",            no_argument,         0,   AnaInfo::MODE_PMT},
      {"mode-full",           no_argument,         0,   AnaInfo::MODE_FULL},
      {"mode-online",         no_argument,         0,   AnaInfo::MODE_ONLINE},
      {"set-calib",           required_argument,   0,   AnaInfo::OPTION_SET_CALIB},
      {"set-calib-alpha",     required_argument,   0,   AnaInfo::OPTION_SET_CALIB_ALPHA},
      {"set-calib-dl",        required_argument,   0,   AnaInfo::OPTION_SET_CALIB_DL},
      {"disable-det",         required_argument,   0,   AnaInfo::OPTION_DET_DISABLE},
      {"sfx",                 required_argument,   0,   AnaInfo::OPTION_SUFFIX},
      {0, 0, 0, 0}
   };

   int c;

   while ((c = getopt_long(argc, argv, "?hr:f:n:s:o:l::t:e:m:d:baCDTABZF:MNW:UGR:Sqg",
                           long_options, &option_index)) != -1)
   {
      switch (c) {

      case '?':
      case 'h':
         gAnaInfo->PrintUsage();
         exit(0);

      case 'r':
      case 'f':
         gAnaInfo->fRunName = optarg;
         gRunInfo->fRunName = optarg;
         gRunDb.fRunName   = optarg;
         break;

      case 'n':
         gMaxEventsUser = atol(optarg);
         break;

      case 's':
         gAnaInfo->thinout = atol(optarg);
         break;

      case 'l':
         gAnaInfo->fFileStdLogName = (optarg != 0 ? optarg : "");
         break;

      case AnaInfo::FLAG_COPY:
         gAnaInfo->fFlagCopyResults = kTRUE;
         break;

      case AnaInfo::FLAG_UPDATE_DB:
         gAnaInfo->fFlagUpdateDb = kTRUE;
         break;

      case AnaInfo::FLAG_NO_UPDATE_DB:
         gAnaInfo->fFlagUpdateDb = kFALSE;
         break;

      case AnaInfo::FLAG_USE_DB:
         gAnaInfo->fFlagUseDb    = kTRUE;
         gAnaInfo->fFlagUpdateDb = kTRUE;
         break;

      case 't': // set timing shift in banana cut
         gAnaInfo->tshift = atoi(optarg);
         extinput.TSHIFT = 1;
         break;

      case 'd': // set timing shift in banana cut
         gAnaInfo->dx_offset = atoi(optarg);
         break;

      case 'e': // set energy range
         strcpy(enerange, optarg);

         if ((ptr = strrchr(enerange, ':'))) {
            ptr++;
            gAnaInfo->eneu = atoi(ptr);
            strtok(enerange, ":");
            gAnaInfo->enel = atoi(enerange);
            if (gAnaInfo->enel == 0 || gAnaInfo->enel < 0)     { gAnaInfo->enel = 0;}
            if (gAnaInfo->eneu == 0 || gAnaInfo->eneu > 12000) { gAnaInfo->eneu = 2000;}
            fprintf(stdout, "ENERGY RANGE LOWER:UPPER = %d:%d\n", gAnaInfo->enel, gAnaInfo->eneu);
         } else {
            cout << "Wrong specification for energy threshold" << endl;
            return 0;
         }
         break;

      case 'a':
         Flag.EXE_ANOMALY_CHECK=0;
         break;

      case 'F':
         sprintf(cfile, optarg);
         if (!strstr(cfile, "/")) {
             strcat(reConfFile, confdir);
             strcat(reConfFile, "/");
         }
         strcat(reConfFile, cfile);
         extinput.CONFIG = 1;
         break;

      case 'b':
         gAnaInfo->FEEDBACKMODE = Flag.feedback = 1; break;
      case 'D':
         gAnaInfo->DMODE = 1; break;
      case 'T':
         gAnaInfo->TMODE = 1; break;
      case 'A':
         gAnaInfo->AMODE = 1; break;
      case 'B':
         gAnaInfo->BMODE = 1; break;
      case 'Z':
         gAnaInfo->ZMODE = 1; break;
      case 'U':
         gAnaInfo->UPDATE = 1; break;
      case 'G':
         gAnaInfo->MMODE = 1; break;
      case 'N':
         gAnaInfo->NTMODE = 1; break;
      case 'W': // constant width banana cut
         gAnaInfo->CBANANA = 1;
         strcpy(cwidth, optarg);
         if ((ptr = strrchr(cwidth,':'))) {
            ptr++;
            gAnaInfo->widthu = atoi(ptr);
            strtok(cwidth,":");
            gAnaInfo->widthl = atoi(cwidth);
            fprintf(stdout,"CONSTANT BANANA CUT LOWER:UPPER = %d:%d\n",
                    gAnaInfo->widthl,gAnaInfo->widthu);
            if (gAnaInfo->widthu == gAnaInfo->widthl)
               fprintf(stdout, "WARNING: Banana Lower = Upper Cut\a\n");
         } else {
            fprintf(stdout, "Wrong specification constant banana cut\n");
            return 0;
         }
         fprintf(stdout,"BANANA Cut : %d <==> %d \n",
                 gAnaInfo->widthl,gAnaInfo->widthu);
         break;

      case 'm':
         gAnaInfo->CBANANA = 2;
         gAnaInfo->MassSigma = atof(optarg);
         extinput.MASSCUT = 1;
         break;

      case 'R':
         sstr.str("");
         sstr << optarg;
         sstr >> gAnaInfo->fSaveTrees;
         break;

      case AnaInfo::OPTION_POL_ID:
         gRunInfo->fPolId = atoi(optarg); break;

      case AnaInfo::OPTION_SET_CALIB_ALPHA:
         gAnaInfo->fAlphaCalibRun = optarg;
         break;

      case AnaInfo::OPTION_SET_CALIB_DL:
         gAnaInfo->fDlCalibRun = optarg;
         break;

      case AnaInfo::OPTION_SET_CALIB:
         gAnaInfo->fAlphaCalibRun = optarg;
         gAnaInfo->fDlCalibRun    = optarg;
         break;

      case AnaInfo::OPTION_DET_DISABLE:
         sstr.str("");
         sstr << optarg;
         sstr >> gAnaInfo->fDisabledDetectors;
         break;

      case AnaInfo::OPTION_SUFFIX:
         gAnaInfo->fSuffix = optarg;
         break;

      case 'q':
         gAnaInfo->QUICK_MODE = 1; break;

      case 'g':
      case AnaInfo::MODE_GRAPH:
         gAnaInfo->fModes |= AnaInfo::MODE_GRAPH;
         break;

      case AnaInfo::MODE_NO_GRAPH:
         gAnaInfo->fModes &= ~AnaInfo::MODE_GRAPH;
         break;

      case 'C':
      case AnaInfo::MODE_ALPHA:
         gAnaInfo->fModes |= AnaInfo::MODE_ALPHA;
         gAnaInfo->RECONFMODE = 0;
         gAnaInfo->fModes &= ~AnaInfo::MODE_NORMAL; // turn off normal mode
         break;

      case AnaInfo::MODE_CALIB:
         gAnaInfo->fModes |= AnaInfo::MODE_CALIB;
         break;

      case AnaInfo::MODE_NO_NORMAL:
         gAnaInfo->fModes &= ~AnaInfo::MODE_NORMAL;
         break;

      case AnaInfo::MODE_SCALER:
         gAnaInfo->fModes |= AnaInfo::MODE_SCALER;
         break;

      case AnaInfo::MODE_RAW:
         gAnaInfo->fModes |= AnaInfo::MODE_RAW;
         gAnaInfo->RAWHISTOGRAM = 1;
         break;

      case AnaInfo::MODE_RUN:
         gAnaInfo->fModes |= AnaInfo::MODE_RUN; break;

      case AnaInfo::MODE_TARGET:
         gAnaInfo->fModes |= AnaInfo::MODE_TARGET; break;

      case AnaInfo::MODE_PROFILE:
         gAnaInfo->fModes |= AnaInfo::MODE_PROFILE;
         gAnaInfo->fModes |= AnaInfo::MODE_TARGET; // profile hists depend on target ones
         break;

      case AnaInfo::MODE_ASYM:
         gAnaInfo->fModes |= AnaInfo::MODE_ASYM;
         break;

      case AnaInfo::MODE_KINEMAT:
         gAnaInfo->fModes |= AnaInfo::MODE_KINEMAT;
         break;

      case AnaInfo::MODE_PMT:
         gAnaInfo->fModes |= AnaInfo::MODE_PMT;
         break;

      case AnaInfo::MODE_FULL:
         gAnaInfo->fModes |= AnaInfo::MODE_FULL; break;

      default:
         gSystem->Error("   int main()", "Invalid Option");
         gAnaInfo->PrintUsage();
         exit(0);
      }
   }

   gAnaInfo->ProcessOptions();

   // Extract RunID from input filename
   //int chrlen = strlen(ifile)-strlen(suffix) ; // f.e. 10100.101.data - .data = 10100.001
   //char RunID[chrlen];
   //strncpy(RunID, ifile, chrlen);
   //RunID[chrlen] = '\0'; // Without RunID[chrlen]='\0', RunID screwed up.

   // Set to 0 when "RunID" contains alphabetical chars
   gRunInfo->RUNID = strtod(gRunInfo->fRunName.c_str(), NULL);

   // For normal runs, RUNID != 0. Then read run conditions from run.db.
   // Otherwise, data filename with characters skip readdb and reconfig routines
   // assuming these are energy calibration or test runs.
   //if (gRunInfo->RUNID)
   //   readdb(gRunInfo->RUNID);
   //else
   //   gAnaInfo->RECONFMODE = 0;

   MseRunInfoX   *mseRunInfoX     = 0;
   MseRunInfoX   *mseRunInfoXOrig = 0;

   // Check whether the run is in database
   if (gAnaInfo->fFlagUseDb) {
      mseRunInfoX = gAsymDb2->SelectRun(gRunInfo->fRunName);
   }

   if (mseRunInfoX) { // if run found in database save its copy
      mseRunInfoXOrig  = new MseRunInfoX(gRunInfo->fRunName);
      *mseRunInfoXOrig = *mseRunInfoX;

   } else { // if run not found in database create it
      mseRunInfoX = new MseRunInfoX(gRunInfo->fRunName);
   }

   //cout << endl << "mseRunInfoX 1: " << endl;
   //mseRunInfoX->Print();

   // Read data file into memory
   RawDataProcessor *rawData = new RawDataProcessor(gAnaInfo->GetRawDataFileName());

   // Get basic information about the measurement from the data file
   // and overwrite the data base run info (mseRunInfoX) if needed
   rawData->ReadRecBegin(mseRunInfoX);

   //cout << endl << "mseRunInfoX 2: " << endl;
   //mseRunInfoX->Print();

   // We can do this for any run including alphas alpha
   MseRunPeriodX *mseRunPeriodX = gAsymDb2->CompleteRunInfoByRunPeriod(*mseRunInfoX);
   //gAsymDb2->CompleteRunInfo(*mseRunInfoX); // deprecated

   if (!mseRunPeriodX)
      gSystem->Fatal("   int main()", "Run period not specified");

   //cout << endl << "mseRunInfoX 3: " << endl;
   //mseRunInfoX->Print();
   //cout << *mseRunInfoX << endl;

   // Overwrite the offline version (if set previously)
   //gRunDb.SetAsymVersion(gRunInfo->fAsymVersion);
   mseRunInfoX->asym_version = gRunInfo->fAsymVersion;

   // We should be done reading all common/default parameters from DB by now
   //gRunDb.Print();

   //gAnaInfo->Update(gRunDb);
   //gRunInfo->Update(gRunDb);

   gAnaInfo->Update(*mseRunInfoX);
   gRunInfo->Update(*mseRunInfoX);
   gRunInfo->Update(*mseRunPeriodX);

   gAnaInfo->Print();
   gRunInfo->Print();
   mseRunInfoX->Print();

   // Book HBOOK file (deprecated)
   char hbk_outfile[256] = "out.hbook";
   printf("Booking HBOOK file %s\n", hbk_outfile);
   hist_book(hbk_outfile);

   // Book root file
   gAsymRoot->CreateRootFile(gAnaInfo->GetRootFileName());

   //gAsymDb->PrintCommon();
   //gAsymDb->Print();

   //gAsymRoot->fEventConfig->fCalibrator->Print();

   // Find RunConfig object in the calibration files and update
   //gAsymRoot->UpdateRunConfig();
   gAsymRoot->UpdateCalibrator();

   //printf("calib= %d\n", gAnaInfo->HasCalibBit());
   //gAsymRoot->fEventConfig->fCalibrator->Print();
   //return 0;

   // Create tree if requested
   if (gAnaInfo->fSaveTrees.any()) { gAsymRoot->CreateTrees(); }

   // If requested update for data (not alpha) calibration constants we need to
   // quickly do some pre-processing to extract parameters from the data
   // itself. For example, rough estimates of the dead layer and t0 are needed
   // to set preliminary cuts.

   if ( gAnaInfo->HasCalibBit() && !gAnaInfo->HasAlphaBit() ) {
      rawData->ReadDataFast();

      // XXX : remove
      RunConst::PrintAll();

      // (Roughly) Process all channel banana
      gAsymRoot->CalibrateFast();
   }

   if (!gAnaInfo->QUICK_MODE) {

      //ds: XXX
      //gAsymRoot->PreProcess();

      // Main Event Loop
      readloop(*mseRunInfoX);

      gAsymRoot->PostProcess();
   }

   //gAsymRoot->fEventConfig->fCalibrator->PrintAsPhp();

   // Delete Unnecessary ROOT Histograms
   gAsymRoot->DeleteHistogram();

   // Close histogram file
   hist_close(hbk_outfile);

   // Update calibration constants if requested
   if (gAnaInfo->HasCalibBit()) {

      // XXX : remove
      RunConst::PrintAll();

      gAsymRoot->Calibrate();
   }

   // Update calibration constants if requested
   //gRunDb.Print();

   //gAsymRoot->fEventConfig->PrintAsPhp();
   //gAsymRoot->fEventConfig->fCalibrator->PrintAsConfig();

   // Stop stopwatch and save results
   stopwatch.Stop();
   gAnaInfo->fAnaDateTime = timestamp.GetSec();
   gAnaInfo->fAnaTimeReal = stopwatch.RealTime();
   gAnaInfo->fAnaTimeCpu  = stopwatch.CpuTime();

   string tmpSqlDateTime(19, ' ');
   strftime(&tmpSqlDateTime[0], 19, "%Y-%m-%d %H:%M:%S", ltime);

   mseRunInfoX->ana_start_time   = mysqlpp::DateTime(tmpSqlDateTime);
   mseRunInfoX->ana_duration     = UInt_t(gAnaInfo->fAnaTimeReal);
   mseRunInfoX->measurement_type = UInt_t(gRunInfo->fMeasType);

   if (gAnaInfo->fFlagUpdateDb)
      gAsymDb2->UpdateInsert(mseRunInfoXOrig, mseRunInfoX);

   gAsymRoot->fEventConfig->fMseRunInfoX = mseRunInfoX;

   gAsymRoot->fEventConfig->PrintAsPhp(gAnaInfo->GetRunInfoFile());
   gAsymRoot->fEventConfig->PrintAsConfig(gAnaInfo->GetRunConfFile());
   fclose(gAnaInfo->GetRunInfoFile()); gAnaInfo->fFileRunInfo = 0;
   fclose(gAnaInfo->GetRunConfFile()); gAnaInfo->fFileRunConf = 0;

   if (gAnaInfo->HasGraphBit())
      gAsymRoot->SaveAs("^.*$", gAnaInfo->GetImageDir());
      //gAsymRoot->SaveAs("preproc", gAnaInfo->GetImageDir());
      //gAsymRoot->SaveAs("profile", gAnaInfo->GetImageDir());

   // Close ROOT File
   gAsymRoot->Finalize();

   delete gAsymRoot;

   gAnaInfo->CopyResults();

   printf("Analysis finished at: %s\n",   timestamp.AsString("l"));
   printf("Processing time: %f seconds\n", gAnaInfo->fAnaTimeReal);

   return 1;
} //}}}


// for Bunch by Bunch base analysis
int BunchSelect(int bid)
{ //{{{
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
} //}}}


// Calibration parameter
void reConfig()
{ //{{{
    ifstream configFile;
    configFile.open(reConfFile);

    if (!configFile) {
       cout << "Failed to open Config File : " << reConfFile << endl;
       cout << "Proceed with original configuration from raw data file" << endl;
       return;
    }

    printf("**********************************\n");
    printf("** Configuration is overwritten **\n");
    printf("**********************************\n");

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
          dwidthn  = atof(strtok(NULL, " ")) + gAnaInfo->dx_offset; // extra thickness
          peden    = atof(strtok(NULL, " "));
          c0n      = atof(strtok(NULL, " "));
          c1n      = atof(strtok(NULL, " "));
          c2n      = atof(strtok(NULL, " "));
          c3n      = atof(strtok(NULL, " "));
          c4n      = atof(strtok(NULL, " "));

          gConfigInfo->data.chan[stripn-1].edead  = edeadn;
          gConfigInfo->data.chan[stripn-1].ecoef  = ecn;
          gConfigInfo->data.chan[stripn-1].t0     = t0n;
          gConfigInfo->data.chan[stripn-1].A0     = a0n;
          gConfigInfo->data.chan[stripn-1].A1     = a1n;
          gConfigInfo->data.chan[stripn-1].acoef  = ealphn;
          gConfigInfo->data.chan[stripn-1].dwidth = dwidthn;
          gConfigInfo->data.chan[stripn-1].pede   = peden;
          gConfigInfo->data.chan[stripn-1].C[0]   = c0n;
          gConfigInfo->data.chan[stripn-1].C[1]   = c1n;
          gConfigInfo->data.chan[stripn-1].C[2]   = c2n;
          gConfigInfo->data.chan[stripn-1].C[3]   = c3n;
          gConfigInfo->data.chan[stripn-1].C[4]   = c4n;

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
} //}}}


// Description : Check Disabled detector
// Input       : int strip nuumber
// Return      : 1 if disabled. otherwise 0
//
/*
int DisabledDet(int det)
{ //{{{
  // det(0,1,2,3,4,5} => {0, 1, 0, 0, 1, 0} => 18
  int DeadDet = tgt.VHtarget ? 18 : 0 ;
  //                            ^   ^
  //                       H-target V-target

  return DeadDet>>det & 1 ;

} //}}}
*/
