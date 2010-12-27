
#include "RunInfo.h"

using namespace std;


/** */
TStructRunInfo::TStructRunInfo() : Run(0), RUNID(0.0), runName(100, ' ')
{
   //Run                   = 6;  // Run05, Run06,..
   //RUNID                 = 7279.005; // RUNID
   //runName;
   StartTime             = 0; // StartTime;
   StopTime              = 0; // StopTime;
   RunTime               = 0; // RunTime;
   GoodEventRate         = 0; // GoodEventRate;
   EvntRate              = 0; // EvntRate;
   ReadRate              = 0; // ReadRate;
   WcmAve                = 0; // WcmAve;
   WcmSum                = 0; // WcmSum;
   BeamEnergy            = 0; // BeamEnergy;
   RHICBeam              = 0; // RHICBeam;
   fPolStream            = 0; // up =1 or down =2 stream
   PolarimetryID         = 1; // PolarimetryID; Polarimetry-1 or Polarimetry-2
   MaxRevolution         = 0; // MaxRevolution;
   target                = 'V'; // target
   targetID              = '-'; // targetID
   //TgtOperation;              // TgtOperation (Initialization is done in Initialization() )
   for (int i=0; i<NDETECTOR; i++) ActiveDetector[i] = 0xFFF;
   //ActiveDetector        = { 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF };// ActiveDetector[NDETECTOR]
   for (int i=0; i<NSTRIP; i++) { ActiveStrip[i] = 1; DisableStrip[i] = 0; }
   //ActiveStrip           = { 1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,
   //                          1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1, 
   //                          1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1 }; // ActiveStrip[NSTRIP]
   NActiveStrip          = NSTRIP; // NAactiveStrip;
   NDisableStrip         = 0; // NDisableStrip
   //DisableStrip          = { 0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,
   //                          0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0, 
   //                          0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0 }; // DisableStrip[NSTRIP]
   NFilledBunch          = 0; // NFilledBunch;
   NActiveBunch          = 0; // NActiveBunch;
   NDisableBunch         = 0; // NDisableBunch,
   for (int i=0; i<NBUNCH; i++) { DisableBunch[i] = 0; }
   //DisableBunch[NBUNCH]
}


/** */
TStructRunInfo::~TStructRunInfo() { }

/*
TStructRunInfo runinfo = {
    6, // Run05, Run06,..
    7279.005, // RUNID
    //"                            ",//(string(" ", 100)),
    0, // StartTime;
    0, // StopTime;
    0, // RunTime;
    0, // GoodEventRate;
    0, // EvntRate;
    0, // ReadRate;
    0, // WcmAve;
    0, // WcmSum;
    0, // BeamEnergy;
    0, // RHICBeam;
    1, // PolarimetryID; Polarimetry-1 or Polarimetry-2
    0, // MaxRevolution;
  'V', // target
  '-',// targetID
    "", // TgtOperation (Initialization is done in Initialization() )
    {  // ActiveDetector[NDETECTOR]
      0xFFF,0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF
    },
    {  // ActiveStrip[NSTRIP]
      1,1,1,1,1,1,1,1,1,1,1,1,
      1,1,1,1,1,1,1,1,1,1,1,1,
      1,1,1,1,1,1,1,1,1,1,1,1,
      1,1,1,1,1,1,1,1,1,1,1,1,
      1,1,1,1,1,1,1,1,1,1,1,1,
      1,1,1,1,1,1,1,1,1,1,1,1
    },
    NSTRIP, // NAactiveStrip;
    0, // NDisableStrip
    {  // DisableStrip[NSTRIP]
      0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,0,0,0
    },
    0, // NFilledBunch;
    0, // NActiveBunch;
    0  // NDisableBunch,
};
*/


/** */
TBuffer & operator<<(TBuffer &buf, TStructRunInfo *&rec)
{
   if (!rec) return buf;
   //printf("operator<<(TBuffer &buf, TStructRunInfo *rec) : \n");
   rec->Streamer(buf);
   return buf;
}


/** */
TBuffer & operator>>(TBuffer &buf, TStructRunInfo *&rec)
{
   //printf("operator>>(TBuffer &buf, TStructRunInfo *rec) : \n");

   //if (!rec) {
   //   printf("ERROR in operator>>\n");
   //   exit(-1);
   //}

   // if object has been created already delete it
   //free(rec);

   //rec = (TStructRunInfo *) realloc(rec, sizeof(TStructRunInfo) + 11*sizeof(char));
   //rec = (TStructRunInfo *) realloc(rec, sizeof(TStructRunInfo));
   //rec->TgtOperation = new char[10];

   rec->Streamer(buf);
   return buf;
}


/** */
void TStructRunInfo::Streamer(TBuffer &buf)
{
   if (buf.IsReading()) {
      //printf("reading TStructRunInfo::Streamer(TBuffer &buf) \n");
      buf >> Run;
      buf >> RUNID;
      TString tstr;
      buf >> tstr; runName = tstr.Data();
      buf >> StartTime;
      buf >> StopTime;
      buf >> RunTime;
      buf >> GoodEventRate;
      buf >> EvntRate;
      buf >> ReadRate;
      buf >> WcmAve;
      buf >> WcmSum;
      buf >> BeamEnergy;
      buf >> RHICBeam;
      buf >> fPolStream;
      buf >> PolarimetryID;
      buf >> MaxRevolution;
      buf >> target;
      buf >> targetID;
      buf >> TgtOperation;
      //buf.ReadString(TgtOperation, 32);
      buf.ReadFastArray(ActiveDetector, NDETECTOR);
      buf.ReadFastArray(ActiveStrip, NSTRIP);
      buf >> NActiveStrip;
      buf >> NDisableStrip;
      buf.ReadFastArray(DisableStrip, NSTRIP);
      buf >> NFilledBunch;
      buf >> NActiveBunch;
      buf >> NDisableBunch;
      buf.ReadFastArray(DisableBunch, NBUNCH);

   } else {
      //printf("writing TStructRunInfo::Streamer(TBuffer &buf) \n");
      buf << Run;
      buf << RUNID;
      TString tstr = runName;
      buf << tstr;
      buf << StartTime;
      buf << StopTime;
      buf << RunTime;
      buf << GoodEventRate;
      buf << EvntRate;
      buf << ReadRate;
      buf << WcmAve;
      buf << WcmSum;
      buf << BeamEnergy;
      buf << RHICBeam;
      buf << fPolStream;
      buf << PolarimetryID;
      buf << MaxRevolution;
      buf << target;
      buf << targetID;
      buf << TgtOperation;
      buf.WriteFastArray(ActiveDetector, NDETECTOR);
      buf.WriteFastArray(ActiveStrip, NSTRIP);
      buf << NActiveStrip;
      buf << NDisableStrip;
      buf.WriteFastArray(DisableStrip, NSTRIP);
      buf << NFilledBunch;
      buf << NActiveBunch;
      buf << NDisableBunch;
      buf.WriteFastArray(DisableBunch, NBUNCH);
   }
}


/** */
void TStructRunInfo::PrintAsPhp(FILE *f) const
{ //{{{
   fprintf(f, "$rc['Run']                          = %d;\n",     Run          );
   fprintf(f, "$rc['RUNID']                        = %.3f;\n",   RUNID        );
   fprintf(f, "$rc['runName']                      = \"%s\";\n", runName.c_str() );
   fprintf(f, "$rc['StartTime']                    = %d;\n",     StartTime    );
   fprintf(f, "$rc['StopTime']                     = %d;\n",     StopTime     );
   fprintf(f, "$rc['RunTime']                      = %f;\n",     RunTime      );
   fprintf(f, "$rc['GoodEventRate']                = %f;\n",     GoodEventRate);
   fprintf(f, "$rc['EvntRate']                     = %f;\n",     EvntRate     );
   fprintf(f, "$rc['ReadRate']                     = %f;\n",     ReadRate     );
   fprintf(f, "$rc['WcmAve']                       = %f;\n",     WcmAve       );
   fprintf(f, "$rc['WcmSum']                       = %f;\n",     WcmSum       );
   fprintf(f, "$rc['BeamEnergy']                   = %f;\n",     BeamEnergy   );
   fprintf(f, "$rc['RHICBeam']                     = %d;\n",     RHICBeam     );
   fprintf(f, "$rc['fPolStream']                   = %d;\n",     fPolStream   );
   fprintf(f, "$rc['PolarimetryID']                = %d;\n",     PolarimetryID);
   fprintf(f, "$rc['MaxRevolution']                = %d;\n",     MaxRevolution);
   fprintf(f, "$rc['target']                       = %d;\n",     target       );
   fprintf(f, "$rc['targetID']                     = \"%c\";\n", targetID     );
   fprintf(f, "$rc['TgtOperation']                 = \"%s\";\n", TgtOperation );

   for (int i=0; i!=NDETECTOR; i++)
   fprintf(f, "$rc['ActiveDetector'][%d]           = %#X;\n", i, ActiveDetector[i]);

   for (int i=0; i!=NSTRIP; i++) //buf.WriteFastArray(ActiveStrip, NSTRIP);
   fprintf(f, "$rc['ActiveStrip'][%d]              = %d;\n", i, ActiveStrip[i]);

   fprintf(f, "$rc['NActiveStrip']                 = %d;\n", NActiveStrip );
   fprintf(f, "$rc['NDisableStrip']                = %d;\n", NDisableStrip);

   for (int i=0; i!=NSTRIP; i++) //buf.WriteFastArray(DisableStrip, NSTRIP);
   fprintf(f, "$rc['DisableStrip'][%d]             = %d;\n", i, DisableStrip[i]);
      
   fprintf(f, "$rc['NFilledBunch']                 = %d;\n", NFilledBunch );
   fprintf(f, "$rc['NActiveBunch']                 = %d;\n", NActiveBunch );
   fprintf(f, "$rc['NDisableBunch']                = %d;\n", NDisableBunch);
} //}}}
