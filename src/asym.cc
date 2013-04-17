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
#include <cstdio>

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
 * You will need to have aclina host defined in your ssh_config, like this
 *
 * Host aclina
 * 	IdentityFile    /star/u/veprbl/.ssh/acnlina5_rsa
 * 	User            dsmirnov
 * 	HostName        localhost
 * 	Port            8022
 *
 * @returns 0 if everything was ok
 */
int read_ssh(const char *logger, const char *cells, time_t start, time_t end, map<string, double> *mean_value)
{
   char buf[1024], startStr[32], endStr[32];
   vector<string> cell_name;

   if (index(logger, '\'') || (index(cells, '\'')) || index(logger, '"') || (index(cells, '"')))
   {
      Error("read_ssh", "Wrong params.");
      return 1;
   }

   char *copy = strdup(cells);
   cell_name.push_back(strtok(copy, ","));
   const char *tok;
   while((tok = strtok(NULL, ",")))
   {
      cell_name.push_back(tok);
   }
   free(copy);

   ctime_r(&start, startStr); // this should have timezone problems
   ctime_r(&end, endStr);
   startStr[strlen(startStr) - 1] = 0; // replace newlines with end-of-string marks
   endStr[strlen(endStr) - 1] = 0;
   snprintf(buf, sizeof(buf),
            "ssh aclina \""
            "setenv PATH /usr/controls/bin; setenv LD_LIBRARY_PATH /ride/release/X86/lib;"
            "exportLoggerData"
            " -logger '%s'"
            " -cells '%s'"
            " -timeformat 'unix'"
            " -dataformat '%%10.6f'"
            " -excluderowswithholes"
            " -start '%s'"
            " -stop '%s'"
            "\"",
            logger, cells, startStr, endStr);

   Info("read_ssh", "Running %s", buf);

   FILE *fd = popen(buf, "r");
   if (!fd)
   {
      Error("read_ssh", "popen failed, errno = %i", errno);
      return 1;
   }

   int row_count = 0;
   mean_value->clear();

   while (!feof(fd)) {
      char c;
      c = ungetc(fgetc(fd), fd);
      if (c == '#') {
         if (fgets(buf, sizeof(buf), fd)) {
            buf[strlen(buf) - 1] = 0;
            Info("read_ssh", "Read comment: %s", buf);
         }
      } else {
         int len;
         double time, value;
         len = fscanf(fd, "%lf", &time);
         if (len != 1) break;

         for(vector<string>::const_iterator it = cell_name.begin();
             it != cell_name.end(); it++) {
            len = fscanf(fd, "%lf", &value);
            if (len != 1) {
               Error("read_ssh", "unexpected end of row");
               return 1;
            }
            (*mean_value)[*it] += value;
         }
         row_count++;
      }
   }

   for (map<string, double>::iterator it = mean_value->begin();
       it != mean_value->end(); it++)
   {
      double &value = it->second;
      value /= row_count;
   }

   int retcode = pclose(fd);

   if (retcode == -1)
   {
      Error("read_ssh", "pclose failed, errno = %i", errno);
      return 1;
   }
   if (retcode)
   {
       Error("read_ssh", "process returned %i", retcode);
   }

   return retcode;
}


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

   // Check whether the run is in database
   if (gAsymAnaInfo->fFlagUseDb) {
      mseMeasInfoX = gAsymDb->SelectRun(gMeasInfo->GetRunName());
   }

   if (mseMeasInfoX) { // If run found in database get a copy of it for later sql update
      mseMeasInfoXOrig  = new MseMeasInfoX(gMeasInfo->GetRunName());
      *mseMeasInfoXOrig = *mseMeasInfoX;
   } else {            // If run not found in database create a new object
      mseMeasInfoX = new MseMeasInfoX(gMeasInfo->GetRunName());
   }

   // Read data file into memory
   RawDataProcessor rawData(gAsymAnaInfo->GetRawDataFileName());

   // Get basic information about the measurement from the data file
   // and overwrite the run info from database (MseMeasInfoX) if needed
   rawData.ReadRecBegin(*mseMeasInfoX);
   rawData.ReadMeasInfo(*mseMeasInfoX);

   MseRunPeriodX *mseRunPeriodX = 0;

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

   if ((!gMeasInfo->fMachineParamsPresent) && (!gAsymAnaInfo->HasNoSshBit()))
   {
      map<string, double> mean_value;
      double startTime = gMeasInfo->fStartTime;
      double endTime = gMeasInfo->fStopTime;

      if (endTime < startTime)
      {
          endTime = startTime + 600;
      }

      int retval = read_ssh(
         "RHIC/Rf/Voltage_Monitor_StripChart,RHIC/PowerSupplies/rot-ps,RHIC/PowerSupplies/snake-ps",
         "cavTuneLoop.4a-rf-b197-1.3:probeMagInVoltsScaledM:value[0],"
         "cavTuneLoop.4a-rf-y197-1.3:probeMagInVoltsScaledM:value[0],"
         "bi5-rot3-outer,yo5-rot3-outer,bo7-rot3-outer,yi7-rot3-outer,bo3-snk7-outer,yi3-snk7-outer",
         startTime, endTime,
         &mean_value
      );

      if (retval)
      {
         Error("asym", "Some problems with read_ssh");
         return EXIT_FAILURE;
      }

      for(map<string, double>::const_iterator it = mean_value.begin();
          it != mean_value.end(); it++) {
         const string &key = it->first;
         double value = it->second;

         Info("asym", "Mean %s equals to %f", key.c_str(), value);
      }

      RecordMachineParams machineParams;
      machineParams.fCavity197MHzVoltage[0]   = mean_value["cavTuneLoop.4a-rf-b197-1.3:probeMagInVoltsScaledM:value[0]"];
      machineParams.fCavity197MHzVoltage[1]   = mean_value["cavTuneLoop.4a-rf-y197-1.3:probeMagInVoltsScaledM:value[0]"];
      machineParams.fSnakeCurrents[0]         = mean_value["bo3-snk7-outer"];
      machineParams.fSnakeCurrents[1]         = mean_value["yi3-snk7-outer"];
      machineParams.fStarRotatorCurrents[0]   = mean_value["bi5-rot3-outer"];
      machineParams.fStarRotatorCurrents[1]   = mean_value["yo5-rot3-outer"];
      machineParams.fPhenixRotatorCurrents[0] = mean_value["bo7-rot3-outer"];
      machineParams.fPhenixRotatorCurrents[1] = mean_value["yi7-rot3-outer"];

      gMeasInfo->SetMachineParams(machineParams);
   }

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

   // Create trees if requested by user
   if (gAsymAnaInfo->fSaveTrees.any()) {
      gAsymRoot->CreateTrees();
   }

   // If requested update for data (not alpha) calibration constants we need to
   // quickly do some pre-processing to extract parameters from the data.
   // For example, rough estimates of the dead layer and t0 are needed
   // to set preliminary cuts.

   if ( gAsymAnaInfo->HasCalibBit() ) {
      rawData.ReadDataPassOne(*mseMeasInfoX);  // Fill primary histograms
      //if ( gAsymAnaInfo->HasAlphaBit() )
      gAsymRoot->FillDerivedPassOne();         // Fill other histograms from the primary ones
      gAsymRoot->Calibrate();                  // Process all channel alpha peak. XXX Fix order!
      gAsymRoot->PostFillPassOne();            // Make decisions based on hist content/data
   }

   // For debugging
   //gAsymRoot->fEventConfig->fCalibrator->Print();

   // PassTwo
   if ( !gAsymAnaInfo->HasAlphaBit() ) {
      gAsymRoot->PreFill();

      // Main event Loop
      rawData.ReadDataPassTwo(*mseMeasInfoX);

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
   gAsymAnaInfo->fFileRunConf  = 0;

   if (gAsymAnaInfo->HasGraphBit())
      gAsymRoot->SaveAs("^.*$", gAsymAnaInfo->GetImageDir());

   // Close ROOT file
   gAsymRoot->Finalize();

   delete gAsymRoot;
   delete gAsymDb;

   gAsymAnaInfo->CopyResults();

   printf("Analysis finished at: %s\n",   timestamp.AsString("l"));
   printf("Processing time: %f seconds\n", gAsymAnaInfo->fAnaTimeReal);

   return EXIT_SUCCESS;
}


// for Bunch by Bunch base analysis
int BunchSelect(int bid)
{
   int go = 0;
   //int BunchList[11]={4,13,24,33,44,53,64,73,84,93,104};
   int BunchList[26]={3,6,13,16,23,26,33,36,43,46,53,56,63,66,73,76,83,86,93,96,103,106};

   for (int i=0; i<14; i++) {
     //BunchList[i]++;
     if (bid == BunchList[i]) {
       go=1;
       break;
     }
   }

   return go;
}
