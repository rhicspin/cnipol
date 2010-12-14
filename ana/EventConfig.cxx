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
   fRunInfo(new TStructRunInfo()), fDatproc(0), fRunDB(new TStructRunDB()),
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
   opt = "";
   printf("EventConfig:\n");
}


/** */
void EventConfig::PrintAsPhp(FILE *f) const
{ //{{{
   //printf("EventConfig:\n");

   fprintf(f, "<?php\n");

   fprintf(f, "$rc['data']['NumChannels'] = %d;\n", fConfigInfo->data.NumChannels);

   /*
   for (int i=0; i!=fConfigInfo->data.NumChannels; i++) {
      fprintf(f, "$rc['data']['chan'][%d]['t0']    = %f;\n", i, fConfigInfo->data.chan[i].t0);
      fprintf(f, "$rc['data']['chan'][%d]['ecoef'] = %f;\n", i, fConfigInfo->data.chan[i].ecoef);
      fprintf(f, "$rc['data']['chan'][%d]['acoef'] = %f;\n", i, fConfigInfo->data.chan[i].acoef);
      //fprintf(f, "$rc['data']['chan'][%d]['acoefE'] = %f;\n", i, fConfigInfo->data.chan[i].acoefE);
      //fprintf(f, "$rc['data']['chan'][%d]['icoef'] = %f;\n", i, fConfigInfo->data.chan[i].icoef);
      //fprintf(f, "$rc['data']['chan'][%d]['icoefE'] = %f;\n", i, fConfigInfo->data.chan[i].icoefE);
   }
   */

   if (!fRunInfo) {
      fprintf(f, "ERROR: EventConfig::PrintAsPhp(): fRunInfo not defined\n");
      exit(-1);
   }

   fRunInfo->PrintAsPhp(f);

   //buf.WriteFastArray(DisableBunch, NBUNCH); // not implemented

   if (!fDatproc) {
      fprintf(f, "ERROR: EventConfig::PrintAsPhp(): fDatproc not defined\n");
      exit(-1);
   }

   fprintf(f, "$rc['enel']                         = %d;\n", fDatproc->enel);
   fprintf(f, "$rc['eneu']                         = %d;\n", fDatproc->eneu);
   fprintf(f, "$rc['widthl']                       = %d;\n", fDatproc->widthl);
   fprintf(f, "$rc['widthu']                       = %d;\n", fDatproc->widthu);
   fprintf(f, "$rc['CMODE']                        = %d;\n", fDatproc->CMODE);
   fprintf(f, "$rc['nEventsProcessed']             = %u;\n", fDatproc->nEventsProcessed);
   fprintf(f, "$rc['nEventsTotal']                 = %u;\n", fDatproc->nEventsTotal);
   fprintf(f, "$rc['thinout']                      = %u;\n", fDatproc->thinout);
   fprintf(f, "$rc['procDateTime']                 = %u;\n", (UInt_t) fDatproc->procDateTime);
   fprintf(f, "$rc['procTimeReal']                 = %f;\n", fDatproc->procTimeReal);
   fprintf(f, "$rc['procTimeCpu']                  = %f;\n", fDatproc->procTimeCpu);

   if (!fRunDB) {
      fprintf(f, "ERROR: EventConfig::PrintAsPhp(): fRunDB not defined\n");
      exit(-1);
   }

   fRunDB->PrintAsPhp(f);

   if (!fCalibrator) {
      fprintf(f, "ERROR: EventConfig::PrintAsPhp(): fCalibrator not defined\n");
      exit(-1);
   }

   fCalibrator->PrintAsPhp(f);
   
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
