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


/**
 *
 */
void EventConfig::PrintAsPhp(FILE *f) const
{
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

   fprintf(f, "$rc['Run']                          = %d;\n",     fRunInfo->Run          );
   fprintf(f, "$rc['RUNID']                        = %.3f;\n",   fRunInfo->RUNID        );
   fprintf(f, "$rc['StartTime']                    = %d;\n",     fRunInfo->StartTime    );
   fprintf(f, "$rc['StopTime']                     = %d;\n",     fRunInfo->StopTime     );
   fprintf(f, "$rc['RunTime']                      = %f;\n",     fRunInfo->RunTime      );
   fprintf(f, "$rc['GoodEventRate']                = %f;\n",     fRunInfo->GoodEventRate);
   fprintf(f, "$rc['EvntRate']                     = %f;\n",     fRunInfo->EvntRate     );
   fprintf(f, "$rc['ReadRate']                     = %f;\n",     fRunInfo->ReadRate     );
   fprintf(f, "$rc['WcmAve']                       = %f;\n",     fRunInfo->WcmAve       );
   fprintf(f, "$rc['WcmSum']                       = %f;\n",     fRunInfo->WcmSum       );
   fprintf(f, "$rc['BeamEnergy']                   = %f;\n",     fRunInfo->BeamEnergy   );
   fprintf(f, "$rc['RHICBeam']                     = %d;\n",     fRunInfo->RHICBeam     );
   fprintf(f, "$rc['PolarimetryID']                = %d;\n",     fRunInfo->PolarimetryID);
   fprintf(f, "$rc['MaxRevolution']                = %d;\n",     fRunInfo->MaxRevolution);
   fprintf(f, "$rc['target']                       = %d;\n",     fRunInfo->target       );
   fprintf(f, "$rc['targetID']                     = \"%c\";\n",     fRunInfo->targetID     );
   fprintf(f, "$rc['TgtOperation']                 = \"%s\";\n", fRunInfo->TgtOperation );

   for (int i=0; i!=NDETECTOR; i++)
   fprintf(f, "$rc['ActiveDetector'][%d]           = %#X;\n", i, fRunInfo->ActiveDetector[i]);

   for (int i=0; i!=NSTRIP; i++) //buf.WriteFastArray(ActiveStrip, NSTRIP);
   fprintf(f, "$rc['ActiveStrip'][%d]              = %d;\n", i, fRunInfo->ActiveStrip[i]);

   fprintf(f, "$rc['NActiveStrip']                 = %d;\n", fRunInfo->NActiveStrip );
   fprintf(f, "$rc['NDisableStrip']                = %d;\n", fRunInfo->NDisableStrip);

   for (int i=0; i!=NSTRIP; i++) //buf.WriteFastArray(DisableStrip, NSTRIP);
   fprintf(f, "$rc['DisableStrip'][%d]             = %d;\n", i, fRunInfo->DisableStrip[i]);
      
   fprintf(f, "$rc['NFilledBunch']                 = %d;\n", fRunInfo->NFilledBunch );
   fprintf(f, "$rc['NActiveBunch']                 = %d;\n", fRunInfo->NActiveBunch );
   fprintf(f, "$rc['NDisableBunch']                = %d;\n", fRunInfo->NDisableBunch);

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

   fprintf(f, "$rc['calib_file_s']                 = \"%s\";\n", fRunDB->calib_file_s.c_str());
   fprintf(f, "$rc['alpha_calib_run_name']         = \"%s\";\n", fRunDB->alpha_calib_run_name.c_str());
   fprintf(f, "$rc['config_file_s']                = \"%s\";\n", fRunDB->config_file_s.c_str());

   if (!fCalibrator) {
      fprintf(f, "ERROR: EventConfig::PrintAsPhp(): fCalibrator not defined\n");
      exit(-1);
   }

   fCalibrator->PrintAsPhp(f);
   
   fprintf(f, "?>\n");
}



float EventConfig::ConvertToEnergy(UShort_t adc, UShort_t chId)
{ 
   return fConfigInfo->data.chan[chId].acoef * adc;
}
