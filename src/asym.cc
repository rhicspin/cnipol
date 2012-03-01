//  Asymmetry Analysis of RHIC pC Polarimeter
//  file name :   former AsymMain.cc
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

#include "asym.h"

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
#include "MseMeasInfo.h"
#include "MseRunPeriod.h"

using namespace std;


/**
 * Main program
 */
int main(int argc, char *argv[])
{ //{{{
   // Create a stopwatch and start it
   TStopwatch stopwatch;
   TTimeStamp timestamp;

   time_t  gtime = time(0);
   tm     *ltime = localtime(&gtime);

   // Create all main (global) objects
   gAsymRoot = new AsymRoot();

   gAsymRoot->GetRunConfigs(gMeasInfo, gAnaInfo, gAnaMeasResult);

   //gAsymDb  = new AsymDbFile();
   gAsymDb = new AsymDbSql();

   // for get option
   extern char *optarg;

   char         cfile[32];
   char         enerange[20], cwidth[20], *ptr;
   stringstream sstr;
   int          option_index = 0;

   static struct option long_options[] = {
      {"run-name",            required_argument,   0,   'r'},
      {"feedback",            0,                   0,   'b'},
      {"no-error-detector",   0,                   0,   'a'},
      {"copy",                no_argument,         0,   AnaInfo::FLAG_COPY},
      {"copy-results",        no_argument,         0,   AnaInfo::FLAG_COPY},
      {"update-db",           no_argument,         0,   AnaInfo::FLAG_UPDATE_DB},
      {"no-update-db",        no_argument,         0,   AnaInfo::FLAG_NO_UPDATE_DB},
      {"use-db",              no_argument,         0,   AnaInfo::FLAG_USE_DB},
      {"thumb",               no_argument,         0,   AnaInfo::FLAG_CREATE_THUMBS},
      {"pol-id",              required_argument,   0,   AnaInfo::OPTION_POL_ID},
      {"log",                 optional_argument,   0,   'l'},
      {"alpha",               no_argument,         0,   AnaInfo::MODE_ALPHA},
      {"calib",               no_argument,         0,   AnaInfo::MODE_CALIB},
      {"scaler",              no_argument,         0,   AnaInfo::MODE_SCALER},
      {"raw",                 no_argument,         0,   AnaInfo::MODE_RAW},
      {"raw-ext",             no_argument,         0,   AnaInfo::MODE_RAW_EXTENDED},
      {"target",              no_argument,         0,   AnaInfo::MODE_TARGET},
      {"profile",             no_argument,         0,   AnaInfo::MODE_PROFILE},
      {"asym",                no_argument,         0,   AnaInfo::MODE_ASYM},
      {"kinema",              no_argument,         0,   AnaInfo::MODE_KINEMA},
      {"pmt",                 no_argument,         0,   AnaInfo::MODE_PMT},
      {"pulser",              no_argument,         0,   AnaInfo::MODE_PULSER},
      {"online",              no_argument,         0,   AnaInfo::MODE_ONLINE},
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
      {"mode-raw-extended",   no_argument,         0,   AnaInfo::MODE_RAW_EXTENDED},
      {"mode-run",            no_argument,         0,   AnaInfo::MODE_RUN},
      {"mode-target",         no_argument,         0,   AnaInfo::MODE_TARGET},
      {"mode-profile",        no_argument,         0,   AnaInfo::MODE_PROFILE},
      {"mode-asym",           no_argument,         0,   AnaInfo::MODE_ASYM},
      {"mode-kinema",         no_argument,         0,   AnaInfo::MODE_KINEMA},
      {"mode-pmt",            no_argument,         0,   AnaInfo::MODE_PMT},
      {"mode-online",         no_argument,         0,   AnaInfo::MODE_ONLINE},
      {"mode-full",           no_argument,         0,   AnaInfo::MODE_FULL},
      {"set-calib",           required_argument,   0,   AnaInfo::OPTION_SET_CALIB},
      {"set-calib-alpha",     required_argument,   0,   AnaInfo::OPTION_SET_CALIB_ALPHA},
      {"set-calib-dl",        required_argument,   0,   AnaInfo::OPTION_SET_CALIB_DL},
      {"disable-det",         required_argument,   0,   AnaInfo::OPTION_DET_DISABLE},
      {"sfx",                 required_argument,   0,   AnaInfo::OPTION_SUFFIX},
      {0, 0, 0, 0}
   };

   int c;

   while ((c = getopt_long(argc, argv, "?hr:f:n:s:o:l::t:e:m:d:baCDTBZF:MNW:UGR:Sg",
                           long_options, &option_index)) != -1)
   {
      switch (c) {

      case '?':
      case 'h':
         gAnaInfo->PrintUsage();
         exit(0);

      case 'r':
      case 'f':
         gAnaInfo->SetRunName(optarg);
         gRunDb.fRunName = optarg; // deprecated, should be removed later
         break;

      case 'n':
         gMaxEventsUser = atol(optarg);
         break;

      case 's':
         sstr.str("");
         sstr << optarg;
         sstr >> gAnaInfo->fThinout;
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

      case AnaInfo::FLAG_CREATE_THUMBS:
         gAnaInfo->fFlagCreateThumbs = kTRUE;
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
         strcat(reConfFile, cfile);
         extinput.CONFIG = 1;
         break;

      case 'b':
         gAnaInfo->FEEDBACKMODE = Flag.feedback = 1; break;
      case 'D':
         gAnaInfo->DMODE = 1; break;
      case 'T':
         gAnaInfo->TMODE = 1; break;
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
         gMeasInfo->fPolId = atoi(optarg); break;

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
         break;

      case AnaInfo::MODE_RAW_EXTENDED:
         gAnaInfo->fModes |= AnaInfo::MODE_RAW_EXTENDED;
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

      case AnaInfo::MODE_KINEMA:
         gAnaInfo->fModes |= AnaInfo::MODE_KINEMA;
         break;

      case AnaInfo::MODE_PMT:
         gAnaInfo->fModes |= AnaInfo::MODE_PMT;
         break;

      case AnaInfo::MODE_PULSER:
         gAnaInfo->fModes |= AnaInfo::MODE_PULSER;
         break;

      case AnaInfo::MODE_ONLINE:
         gAnaInfo->fModes |= AnaInfo::MODE_ONLINE;
         break;

      case AnaInfo::MODE_FULL:
         gAnaInfo->fModes |= AnaInfo::MODE_FULL; break;

      default:
         gSystem->Error("   int main()", "Invalid Option");
         gAnaInfo->PrintUsage();
         exit(0);
      }
   }

   // Include all interesting modes for now
   //if (!gAnaInfo->HasAlphaBit()) {
   //   gAnaInfo->fModes |= AnaInfo::MODE_NORMAL;
   //   gAnaInfo->fModes |= AnaInfo::MODE_CALIB;
   //   gAnaInfo->fModes |= AnaInfo::MODE_PROFILE;
   //   gAnaInfo->fModes |= AnaInfo::MODE_TARGET;
   //   gAnaInfo->fModes |= AnaInfo::MODE_PULSER;
   //   gAnaInfo->fModes |= AnaInfo::MODE_ASYM;
   //   gAnaInfo->fModes |= AnaInfo::MODE_RAW;
   //}

   gAnaInfo->ProcessOptions();

   // Book HBOOK file (deprecated) should be removed from future releases
   char hbk_outfile[256] = "out.hbook";
   printf("Booking HBOOK file %s\n", hbk_outfile);
   hist_book(hbk_outfile);

   // Book root file
   gAsymRoot->CreateRootFile(gAnaInfo->GetRootFileName());

   MseMeasInfoX  *mseMeasInfoX     = 0;
   MseMeasInfoX  *mseMeasInfoXOrig = 0;
   MseRunPeriodX *mseRunPeriodX    = 0;

   // Check whether the run is in database
   if (gAnaInfo->fFlagUseDb) {
      mseMeasInfoX = gAsymDb->SelectRun(gMeasInfo->GetRunName());
   }

   if (mseMeasInfoX) { // if run found in database save its copy
      mseMeasInfoXOrig  = new MseMeasInfoX(gMeasInfo->GetRunName());
      *mseMeasInfoXOrig = *mseMeasInfoX;
   } else { // if run not found in database create it
      mseMeasInfoX = new MseMeasInfoX(gMeasInfo->GetRunName());
   }

   // Read data file into memory
   RawDataProcessor *rawData = new RawDataProcessor(gAnaInfo->GetRawDataFileName());

   // Get basic information about the measurement from the data file
   // and overwrite the run info from database (MseMeasInfoX) if needed
   rawData->ReadRecBegin(mseMeasInfoX);
   rawData->ReadMeasInfo(*mseMeasInfoX);

   // We can do this for any run type including alpha runs
   if (gAnaInfo->fFlagUseDb) {
      mseRunPeriodX = gAsymDb->CompleteMeasInfoByRunPeriod(*mseMeasInfoX);
   }

   if (!mseRunPeriodX) {
      mseRunPeriodX = new MseRunPeriodX();
   }

   //cout << endl << "mseMeasInfoX 3: " << endl;
   //mseMeasInfoX->Print();
   //cout << *mseMeasInfoX << endl;

   // Overwrite the offline version (if set previously)
   //gRunDb.SetAsymVersion(gMeasInfo->fAsymVersion);
   mseMeasInfoX->asym_version = gMeasInfo->fAsymVersion;

   // We should be done reading all common/default parameters from DB by now
   //gRunDb.Print();

   //gAnaInfo->Update(gRunDb);
   //gMeasInfo->Update(gRunDb);

   gAnaInfo->Update(*mseMeasInfoX);
   gMeasInfo->Update(*mseMeasInfoX);
   gMeasInfo->Update(*mseRunPeriodX);

   gAnaInfo->Print();
   gMeasInfo->Print();
   mseMeasInfoX->Print();

   //gAsymDb->PrintCommon();
   //gAsymDb->Print();

   //gAsymRoot->fEventConfig->fCalibrator->Print();

   // A Calibrator object holds calibration constants for individual channels
   // and knows how to updated/calibrate them if requested by the user
   gAsymRoot->UpdateCalibrator();

   // Create trees if requested
   if (gAnaInfo->fSaveTrees.any()) { gAsymRoot->CreateTrees(); }

   // If requested update for data (not alpha) calibration constants we need to
   // quickly do some pre-processing to extract parameters from the data
   // itself. For example, rough estimates of the dead layer and t0 are needed
   // to set preliminary cuts.

   if ( gAnaInfo->HasCalibBit() && !gAnaInfo->HasAlphaBit() )
   {
      rawData->ReadDataFast();          // fill histograms
      gAsymRoot->FillDerivedPassOne();
      gAsymRoot->PostFillPassOne();     // make decisions based on hist content/data
      gAsymRoot->CalibrateFast();       // Process all channel banana
   }

   // XXX : debug
   //gAsymRoot->fEventConfig->fCalibrator->Print();

   //
   gAsymRoot->PreFill();

   // Main Event Loop
   readloop(*mseMeasInfoX);

   gAsymRoot->FillDerived();

   gAsymRoot->PostFill(*mseMeasInfoX);

   // Delete Unnecessary ROOT Histograms
   gAsymRoot->DeleteHistogram();

   // Close histogram file
   hist_close(hbk_outfile);

   // XXX : debug
   //gAsymRoot->fEventConfig->fCalibrator->Print();

   // Update calibration constants if requested
   // XXX logic??? For normal data runs calibration should be done only once
   // during the first pass
   if ( gAnaInfo->HasCalibBit() && gAnaInfo->HasAlphaBit() ) {

      gAsymRoot->Calibrate();
   }

   //gRunDb.Print();
   //gAsymRoot->fEventConfig->Print();
   //gAsymRoot->fEventConfig->fCalibrator->Print();

   // No analysis is done beyond this point only bookkeeping

   // Stop stopwatch and save results
   stopwatch.Stop();
   gAnaInfo->fAnaDateTime = timestamp.GetSec();
   gAnaInfo->fAnaTimeReal = stopwatch.RealTime();
   gAnaInfo->fAnaTimeCpu  = stopwatch.CpuTime();

   string tmpSqlDateTime(19, ' ');
   strftime(&tmpSqlDateTime[0], 19, "%Y-%m-%d %H:%M:%S", ltime);

   mseMeasInfoX->ana_start_time   = mysqlpp::DateTime(tmpSqlDateTime);
   mseMeasInfoX->ana_duration     = UInt_t(gAnaInfo->fAnaTimeReal);
   mseMeasInfoX->measurement_type = UInt_t(gMeasInfo->fMeasType);

   if (gAnaInfo->fFlagUpdateDb && gAnaInfo->fSuffix.empty())
      gAsymDb->UpdateInsert(mseMeasInfoXOrig, mseMeasInfoX);

   gAsymRoot->fEventConfig->fMseMeasInfoX = mseMeasInfoX;

   gAsymRoot->fEventConfig->PrintAsPhp(gAnaInfo->GetMeasInfoFile());
   gAsymRoot->fEventConfig->PrintAsConfig(gAnaInfo->GetRunConfFile());
   fclose(gAnaInfo->GetMeasInfoFile()); gAnaInfo->fFileMeasInfo = 0;
   fclose(gAnaInfo->GetRunConfFile()); gAnaInfo->fFileRunConf = 0;

   if (gAnaInfo->HasGraphBit())
      gAsymRoot->SaveAs("^.*$", gAnaInfo->GetImageDir());

   // Close ROOT File
   gAsymRoot->Finalize();

   delete gAsymRoot;
   delete gAsymDb;

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
