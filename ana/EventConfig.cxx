/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "EventConfig.h"

#include "AnaInfo.h"
#include "DbEntry.h"
#include "RunInfo.h"
#include "AnaResult.h"

ClassImp(EventConfig)

using namespace std;

/**
 * Default constructor.
 * We have to allocate memory for fRunInfo, fAnaInfo, and fDbEntry as they can be
 * read from a file with a streamer. The streamers do not allocate memory by
 * themselves.
 */
EventConfig::EventConfig() : TObject(), fRandom(new TRandom()), fConfigInfo(0),
   fRunInfo(new RunInfo()), fAnaInfo(new AnaInfo()), fDbEntry(new DbEntry()),
   fCalibrator(new Calibrator()), fAnaResult(new AnaResult())
{
}


/** Default destructor. */
EventConfig::~EventConfig()
{
   // there should be more than that...
   //delete fConfigInfo;
}


void EventConfig::Print(const Option_t* opt) const
{
   printf("EventConfig:\n");
   PrintAsPhp();
}


/** */
void EventConfig::PrintAsPhp(FILE *f) const
{ //{{{
   fprintf(f, "<?php\n");

   fprintf(f, "\n// TRecordConfigRhicStruct data\n");
   if (!fConfigInfo) {
      Error("PrintAsPhp", "fConfigInfo not defined");
   } else {
      fprintf(f, "$rc['data']['NumChannels'] = %d;\n", fConfigInfo->data.NumChannels);
   }

   fprintf(f, "\n// RunInfo data\n");
   if (!fRunInfo) {
      Error("PrintAsPhp", "fRunInfo not defined");
   } else {
      fRunInfo->PrintAsPhp(f);
   }

   fprintf(f, "\n// AnaInfo data\n");
   if (!fAnaInfo) {
      Error("PrintAsPhp", "fAnaInfo not defined");
   } else {
      fAnaInfo->PrintAsPhp(f);
   }

   fprintf(f, "\n// DbEntry data\n");
   if (!fDbEntry) {
      Error("PrintAsPhp", "fDbEntry not defined");
   } else {
      fDbEntry->PrintAsPhp(f);
   }

   fprintf(f, "\n// Calibrator data\n");
   if (!fCalibrator) {
      Error("PrintAsPhp", "fCalibrator not defined");
   } else {
      fCalibrator->PrintAsPhp(f);
   }

   fprintf(f, "\n// AnaResult data\n");
   if (!fAnaResult) {
      Error("PrintAsPhp", "fAnaResult not defined");
   } else {
      fAnaResult->PrintAsPhp(f);
   }
   
   fprintf(f, "?>\n");
} //}}}


/** */
void EventConfig::PrintAsConfig(FILE *f) const
{ //{{{
	fprintf(f, "* Strip t0 ec edead A0 A1 ealph dwidth pede C0 C1 C2 C3 C4\n");
	fprintf(f, "* for the dead layer and T0  : %s\n", fRunInfo->runName.c_str());
	fprintf(f, "* for the Am calibration     : %s\n", fDbEntry->alpha_calib_run_name.c_str());
	fprintf(f, "* for the Integral/Amplitude : default\n");
	fprintf(f, "* \n");

   fCalibrator->PrintAsConfig(f);
} //}}}


/** */
float EventConfig::ConvertToEnergy(UShort_t adc, UShort_t chId)
{ 
   return fConfigInfo->data.chan[chId].acoef * adc;
}
