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

#include "TROOT.h"
#include "TStopwatch.h"
#include "TTimeStamp.h"

#include "EventConfig.h"

#include "AsymGlobals.h"
#include "AsymRoot.h"
#include "AsymRead.h"
#include "AsymAnaInfo.h"
#include "AsymDbSql.h"
//#include "AsymHbook.h"
#include "MseMeasInfo.h"
#include "MseRunPeriod.h"

using namespace std;


/**
 * Main program
 */
int main(int argc, char *argv[])
{
   // Create a stopwatch and start it
   TStopwatch stopwatch;
   TTimeStamp timestamp;

   time_t  gtime = time(0);
   //tm     *ltime = localtime(&gtime);
	tm      start_time = *localtime(&gtime);

   // Create all main (global) objects
   gAsymRoot = new AsymRoot();

   gAsymRoot->GetMeasConfigs(gMeasInfo, gAsymAnaInfo, gAnaMeasResult);

   AsymDbSql *gAsymDb = new AsymDbSql();

   gAsymAnaInfo->ProcessOptions(argc, argv);
   gAsymAnaInfo->VerifyOptions();

   // Book HBOOK file (deprecated) should be removed from future releases
   //char hbk_outfile[256] = "out.hbook";
   //printf("Booking HBOOK file %s\n", hbk_outfile);
   //hist_book(hbk_outfile);

   // Book root file
   gAsymRoot->CreateRootFile(gAsymAnaInfo->GetRootFileName());

   MseMeasInfoX  *mseMeasInfoX     = 0;
   MseMeasInfoX  *mseMeasInfoXOrig = 0;
   MseRunPeriodX *mseRunPeriodX    = 0;

   // Check whether the run is in database
   if (gAsymAnaInfo->fFlagUseDb) {
      mseMeasInfoX = gAsymDb->SelectRun(gMeasInfo->GetRunName());
   }

   if (mseMeasInfoX) { // if run found in database save its copy
      mseMeasInfoXOrig  = new MseMeasInfoX(gMeasInfo->GetRunName());
      *mseMeasInfoXOrig = *mseMeasInfoX;
   } else { // if run not found in database create it
      mseMeasInfoX = new MseMeasInfoX(gMeasInfo->GetRunName());
   }

   // Read data file into memory
   RawDataProcessor *rawData = new RawDataProcessor(gAsymAnaInfo->GetRawDataFileName());

   // Get basic information about the measurement from the data file
   // and overwrite the run info from database (MseMeasInfoX) if needed
   rawData->ReadRecBegin(mseMeasInfoX);
   rawData->ReadMeasInfo(*mseMeasInfoX);

   // We can do this for any run type including alpha runs
   if (gAsymAnaInfo->fFlagUseDb) {
      mseRunPeriodX = gAsymDb->CompleteMeasInfoByRunPeriod(*mseMeasInfoX);
   }

   if (!mseRunPeriodX) {
      mseRunPeriodX = new MseRunPeriodX();
   }

   //cout << endl << "mseMeasInfoX 3: " << endl;
   //mseMeasInfoX->Print();
   //cout << *mseMeasInfoX << endl;

   // Overwrite the offline version (if set previously)
   mseMeasInfoX->asym_version = gAsymAnaInfo->fAsymVersion;

   // Update the final running parameters based on what was read from DB and
   // what was requested by the user
   gAsymAnaInfo->Update(*mseMeasInfoX);
   gMeasInfo->Update(*mseMeasInfoX);
   gMeasInfo->Update(*mseRunPeriodX);

   // For debugging
   gAsymAnaInfo->Print();
   gMeasInfo->Print();
   mseMeasInfoX->Print();
   //gAsymDb->PrintCommon();
   //gAsymDb->Print();
   //gAsymRoot->fEventConfig->fCalibrator->Print();

   // A Calibrator object holds calibration constants for individual channels.
   // It knows how to updated/calibrate them if requested by the user
   gAsymRoot->UpdateCalibrator();

   // Create trees if requested
   if (gAsymAnaInfo->fSaveTrees.any()) { gAsymRoot->CreateTrees(); }

   // If requested update for data (not alpha) calibration constants we need to
   // quickly do some pre-processing to extract parameters from the data.
   // For example, rough estimates of the dead layer and t0 are needed
   // to set preliminary cuts.

   //if ( gAsymAnaInfo->HasCalibBit() && !gAsymAnaInfo->HasAlphaBit() )
   if ( gAsymAnaInfo->HasCalibBit() )
   {
      rawData->ReadDataFast();          // fill histograms
      gAsymRoot->FillDerivedPassOne();
      gAsymRoot->PostFillPassOne();     // make decisions based on hist content/data
      //gAsymRoot->CalibrateFast();       // Process all channel banana
      gAsymRoot->Calibrate();       // Process all channel banana
   }

   // For debugging
   //gAsymRoot->fEventConfig->fCalibrator->Print();

   // PassTwo
   if ( !gAsymAnaInfo->HasAlphaBit() )
   {
      gAsymRoot->PreFill();

      // Main event Loop
      readloop(*mseMeasInfoX);

      gAsymRoot->FillDerived();
      gAsymRoot->PostFill(*mseMeasInfoX);
	}

   // Close histogram file
   //hist_close(hbk_outfile);

   // For debugging only
   //gAsymRoot->fEventConfig->fCalibrator->Print();

   // Update calibration constants if requested
   // XXX logic??? For normal data runs calibration should be done only once
   // during the first pass
   // Currently used for alpha runs only
   //if ( gAsymAnaInfo->HasCalibBit() && gAsymAnaInfo->HasAlphaBit() )
   //{
   //   gAsymRoot->Calibrate();
   //}

   //gAsymRoot->fEventConfig->Print();
   //gAsymRoot->fEventConfig->fCalibrator->Print();

   // No analysis is done beyond this point only bookkeeping

   // Stop stopwatch and save results
   stopwatch.Stop();
   gAsymAnaInfo->fAnaDateTime = timestamp.GetSec();
   gAsymAnaInfo->fAnaTimeReal = stopwatch.RealTime();
   gAsymAnaInfo->fAnaTimeCpu  = stopwatch.CpuTime();

   string tmpSqlDateTime(19, ' ');
   strftime(&tmpSqlDateTime[0], 20, "%Y-%m-%d %H:%M:%S", &start_time);

   mseMeasInfoX->ana_start_time   = mysqlpp::DateTime(tmpSqlDateTime);
   mseMeasInfoX->ana_duration     = UInt_t(gAsymAnaInfo->fAnaTimeReal);
   mseMeasInfoX->measurement_type = UInt_t(gMeasInfo->GetMeasType());

   if (gAsymAnaInfo->fFlagUpdateDb && gAsymAnaInfo->fSuffix.empty())
      gAsymDb->UpdateInsert(mseMeasInfoXOrig, mseMeasInfoX);

   gAsymRoot->fEventConfig->fMseMeasInfoX = mseMeasInfoX;

   gAsymRoot->fEventConfig->PrintAsPhp(gAsymAnaInfo->GetAnaInfoFile());
   gAsymRoot->fEventConfig->PrintAsConfig(gAsymAnaInfo->GetRunConfFile());
   fclose(gAsymAnaInfo->GetAnaInfoFile());
   fclose(gAsymAnaInfo->GetRunConfFile());
   gAsymAnaInfo->fFileMeasInfo = 0;
   gAsymAnaInfo->fFileRunConf = 0;

   if (gAsymAnaInfo->HasGraphBit())
      //gAsymRoot->SaveAs("^.*AsymVs.*$", gAsymAnaInfo->GetImageDir());
      gAsymRoot->SaveAs("^.*$", gAsymAnaInfo->GetImageDir());

   // Close ROOT File
   gAsymRoot->Finalize();

   delete gAsymRoot;
   delete gAsymDb;

   gAsymAnaInfo->CopyResults();

   printf("Analysis finished at: %s\n",   timestamp.AsString("l"));
   printf("Processing time: %f seconds\n", gAsymAnaInfo->fAnaTimeReal);

   return 1;
}


// for Bunch by Bunch base analysis
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


// Description : Check Disabled detector
// Input       : int strip nuumber
// Return      : 1 if disabled. otherwise 0
//
/*
int DisabledDet(int det)
{
  // det(0,1,2,3,4,5} => {0, 1, 0, 0, 1, 0} => 18
  int DeadDet = tgt.VHtarget ? 18 : 0 ;
  //                            ^   ^
  //                       H-target V-target

  return DeadDet>>det & 1 ;

}
*/
