#include <iostream>
#include <getopt.h>
#include <sys/stat.h>
#include <sstream>

#include "TROOT.h"
#include "TTimeStamp.h"

#include <opencdev.h>

#include "EventConfig.h"

#include "AsymGlobals.h"
#include "AsymRoot.h"
#include "AsymAnaInfo.h"
#include "AsymDbSql.h"
#include "MseMeasInfo.h"
#include "RawDataReader.h"
#include "RunPeriod.h"
//#include "SpinTuneMotor.h"

using namespace std;


/**
 * The main function of `asym`
 */
int main(int argc, char *argv[])
{
   TTimeStamp timestamp;

   time_t gtime = time(0);
   tm     start_time = *localtime(&gtime);

   // Create all main (global) objects
   gAsymRoot = new AsymRoot();

   gAsymRoot->GetMeasConfigs(gMeasInfo, gAsymAnaInfo, gAnaMeasResult);

   AsymDbSql *gAsymDb = new AsymDbSql();

   gAsymAnaInfo->ProcessOptions(argc, argv);
   gAsymAnaInfo->VerifyOptions();

   // Book root file
   gAsymRoot->CreateRootFile(gAsymAnaInfo->GetRootFileName());
   MseMeasInfoX *mseMeasInfoX     = 0;
   MseMeasInfoX *mseMeasInfoXOrig = 0;
   //spin tune motor zchang
   //if(gAsymAnaInfo->HasSTMBit()){
	//gAsymRoot->fSpinTuneMotor = new SpinTuneMotor;
   //}
   //
   // Check whether the measurement is already in database
   if (gAsymAnaInfo->fFlagUseDb) {
      mseMeasInfoX = gAsymDb->SelectRun(gMeasInfo->GetRunName());
   }

   if (mseMeasInfoX) { // If measurement found in database get its copy for later update with SQL
      mseMeasInfoXOrig  = new MseMeasInfoX(gMeasInfo->GetRunName());
      *mseMeasInfoXOrig = *mseMeasInfoX;
      Info("MeasInfo", "found in DB");
   } else {            // If measurement not found in database create a new object
      Info("MeasInfo", "not found in DB");
      mseMeasInfoX = new MseMeasInfoX(gMeasInfo->GetRunName());
   }

   // Read data file into memory
   RawDataReader rawDataReader(gAsymAnaInfo->GetRawDataFileName());

   // Get basic information about the measurement from the data file
   // and overwrite the measurement info from database (MseMeasInfoX) if needed
   rawDataReader.ReadRecBegin(*mseMeasInfoX);
   rawDataReader.ReadMeasInfo(*mseMeasInfoX);

   const RunPeriod *runPeriod = find_run_period(
         (time_t)mseMeasInfoX->start_time, (EPolarimeterId)mseMeasInfoX->polarimeter_id);
   if (!runPeriod) {
      Error("asym", "Run period not found!");
      exit(EXIT_FAILURE);
   }

   mseMeasInfoX->alpha_calib_run_name = runPeriod->alpha_calib_run_name;
   mseMeasInfoX->disabled_channels    = runPeriod->disabled_channels;
   mseMeasInfoX->disabled_bunches     = runPeriod->disabled_bunches;

   // Overwrite the offline version (if set previously)
   mseMeasInfoX->asym_version = gAsymAnaInfo->fAsymVersion;

   // Update the final running parameters based on what was read from DB and
   // what was requested by the user
   gAsymAnaInfo->Update(*mseMeasInfoX);
   gMeasInfo->Update(*mseMeasInfoX);
   gMeasInfo->Update(*runPeriod);
   gMeasInfo->Update(*gAsymAnaInfo);  // Can override some parameters by the user ones

   if (!gMeasInfo->HasMachineParamsInRawData())
   {
      opencdev::mean_result_t mean_value;
      double startTime = gMeasInfo->fStartTime;
      double endTime = gMeasInfo->fStopTime;

      if (endTime < startTime)
      {
          endTime = startTime + 600;
      }

      opencdev::LocalLogReader log_reader(gAsymAnaInfo->GetSlowControlLogDir());
      log_reader.query_timerange_mean("RHIC/Rf/Voltage_Monitor_StripChart", startTime, endTime, &mean_value);
      log_reader.query_timerange_mean("RHIC/PowerSupplies/rot-ps", startTime, endTime, &mean_value);
      log_reader.query_timerange_mean("RHIC/PowerSupplies/snake-ps", startTime, endTime, &mean_value);

      for(opencdev::mean_result_t::const_iterator it = mean_value.begin(); it != mean_value.end(); it++)
      {
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

   // Print some information about the measurement for debugging
   gAsymAnaInfo->Print();
   gMeasInfo->Print();
   mseMeasInfoX->Print();

   // A Calibrator object holds calibration constants for individual channels.
   // It knows how to updated/calibrate them if requested by the user
   gAsymRoot->UpdateCalibrator();

   // Create trees if requested by user
   if (gAsymAnaInfo->fSaveTrees.any()) {
      gAsymRoot->CreateTrees();
   }

   // We need to do some pre-processing first to extract parameters from the
   // data. For example, rough estimates of the dead layer and t0 are needed to
   // set preliminary cuts.

   rawDataReader.ReadDataPassOne(*mseMeasInfoX);  // Fill primary histograms
   //zchang check noise cut
   //if(gAsymRoot->fChannelEvent->PassCutNoiseLower()) gAsymRoot->FillDerivedPassOne();
   gAsymRoot->FillDerivedPassOne();
   // Fill other histograms from the primary ones
   if (gAsymAnaInfo->HasNormalBit())
   {
      Info("calibrate",  "normal bit");
      gAsymRoot->Calibrate();
   }
   gAsymRoot->PostFillPassOne();            // Make decisions based on hist content/data

   // PassTwo
   gAsymRoot->PreFill();
   rawDataReader.ReadDataPassTwo(*mseMeasInfoX);
   if (gAsymAnaInfo->HasAlphaBit()) {
      gAsymRoot->Calibrate();
   }
   gAsymRoot->FillDerived();
   gAsymRoot->PostFill();

   if (gAsymAnaInfo->HasNormalBit())
   {
      // Add info to database entry
      mseMeasInfoX->profile_ratio       = gAnaMeasResult->fProfilePolarR.first;
      mseMeasInfoX->profile_ratio_error = gAnaMeasResult->fProfilePolarR.second;

      mseMeasInfoX->polarization        = gAnaMeasResult->GetPCPolar().first;
      mseMeasInfoX->polarization_error  = gAnaMeasResult->GetPCPolar().second;
      mseMeasInfoX->phase               = gAnaMeasResult->GetPCPolarPhase().first;
      mseMeasInfoX->phase_error         = gAnaMeasResult->GetPCPolarPhase().second;
   }

   // No data analysis is done beyond this point only bookkeeping

   gAsymAnaInfo->fAnaDateTime = timestamp.GetSec();

   string tmpSqlDateTime(19, ' ');
   strftime(&tmpSqlDateTime[0], 20, "%Y-%m-%d %H:%M:%S", &start_time);

   mseMeasInfoX->ana_start_time   = mysqlpp::DateTime(tmpSqlDateTime);
   mseMeasInfoX->ana_duration     = UInt_t(gAsymAnaInfo->fAnaTimeReal);
   mseMeasInfoX->measurement_type = UInt_t(gMeasInfo->GetMeasType());

   if (gAsymAnaInfo->fFlagUpdateDb && gAsymAnaInfo->fSuffix.empty())
      gAsymDb->UpdateInsert(mseMeasInfoXOrig, mseMeasInfoX);

   gAsymRoot->fEventConfig->fMseMeasInfoX = mseMeasInfoX;

   if (gAsymAnaInfo->GetAnaInfoFile())
   {
      gAsymRoot->fEventConfig->PrintAsPhp(gAsymAnaInfo->GetAnaInfoFile());
      fclose(gAsymAnaInfo->GetAnaInfoFile());
   }
   if (gAsymAnaInfo->GetRunConfFile())
   {
      gAsymRoot->fEventConfig->PrintAsConfig(gAsymAnaInfo->GetRunConfFile());
      fclose(gAsymAnaInfo->GetRunConfFile());
   }
   gAsymAnaInfo->fFileMeasInfo = 0;
   gAsymAnaInfo->fFileRunConf  = 0;

   if (gAsymAnaInfo->HasGraphBit())
      gAsymRoot->SaveAs("^.*$", gAsymAnaInfo->GetImageDir());

   // Close ROOT file
   gAsymRoot->Finalize();

   delete gAsymRoot;
   delete gAsymDb;

   gAsymAnaInfo->CopyResults();

   return EXIT_SUCCESS;
}
