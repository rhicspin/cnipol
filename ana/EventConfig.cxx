/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/

#include "EventConfig.h"

ClassImp(EventConfig)

using namespace std;

/**
 * Default constructor.
 * We have to allocate memory for fRunInfo, fDatproc, and fRunDB as they can be
 * read from a file with a streamer. The streamers do not allocate memory by
 * themselves.
 */
EventConfig::EventConfig() : TObject(), fRandom(new TRandom()), fConfigInfo(0),
   fRunInfo(new TStructRunInfo()), fDatproc(new TDatprocStruct()), fRunDB(new TStructRunDB()),
   fCalibrator(new Calibrator())
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

   fprintf(f, "\n// TStructRunInfo data\n");
   if (!fRunInfo) {
      Error("PrintAsPhp", "fRunInfo not defined");
   } else {
      fRunInfo->PrintAsPhp(f);
   }

   fprintf(f, "\n// TDatprocStruct data\n");
   if (!fDatproc) {
      Error("PrintAsPhp", "fDatproc not defined");
   } else {
      fDatproc->PrintAsPhp(f);
   }

   fprintf(f, "\n// TStructRunDB data\n");
   if (!fRunDB) {
      Error("PrintAsPhp", "fRunDB not defined");
   } else {
      fRunDB->PrintAsPhp(f);
   }

   fprintf(f, "\n// Calibrator data\n");
   if (!fCalibrator) {
      Error("PrintAsPhp", "fCalibrator not defined");
   } else {
      fCalibrator->PrintAsPhp(f);
   }
   
   fprintf(f, "?>\n");
} //}}}


/** */
void EventConfig::PrintAsConfig(FILE *f) const
{ //{{{
	fprintf(f, "* Strip t0 ec edead A0 A1 ealph dwidth pede C0 C1 C2 C3 C4\n");
	fprintf(f, "* for the dead layer and T0  : %s\n", fRunInfo->runName.c_str());
	fprintf(f, "* for the Am calibration     : %s\n", fRunDB->alpha_calib_run_name.c_str());
	fprintf(f, "* for the Integral/Amplitude : default\n");
	fprintf(f, "* \n");

   fCalibrator->PrintAsConfig(f);
} //}}}


/** */
float EventConfig::ConvertToEnergy(UShort_t adc, UShort_t chId)
{ 
   return fConfigInfo->data.chan[chId].acoef * adc;
}
