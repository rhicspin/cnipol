
#include "RunInfo.h"

using namespace std;


/** */
TStructRunInfo::TStructRunInfo() : Run(0), RUNID(0.0), runName(100, ' '),
   StartTime(0), StopTime(0), RunTime(0), fDataFormatVersion(0)
{
   GoodEventRate         = 0; // GoodEventRate;
   EvntRate              = 0; // EvntRate;
   ReadRate              = 0; // ReadRate;
   WcmAve                = 0; // WcmAve;
   WcmSum                = 0; // WcmSum;
   BeamEnergy            = 0; // BeamEnergy;
   fPolId                = -1; // valid values 0 - 3
   fPolBeam              = 0; // blue = 2 or yellow = 1
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
    0, // fPolBeam;
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
      buf >> fPolId;
      buf >> fPolBeam;
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
      buf << fPolId;
      buf << fPolBeam;
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
void TStructRunInfo::Print(const Option_t* opt) const
{
   PrintAsPhp();
}


/** */
void TStructRunInfo::PrintAsPhp(FILE *f) const
{ //{{{
   fprintf(f, "$rc['Run']                          = %d;\n",     Run          );
   fprintf(f, "$rc['RUNID']                        = %.3f;\n",   RUNID        );
   fprintf(f, "$rc['runName']                      = \"%s\";\n", runName.c_str() );
   fprintf(f, "$rc['StartTime']                    = %ld;\n",    StartTime    );
   fprintf(f, "$rc['StopTime']                     = %ld;\n",    StopTime     );
   fprintf(f, "$rc['RunTime']                      = %f;\n",     RunTime      );
   fprintf(f, "$rc['GoodEventRate']                = %f;\n",     GoodEventRate);
   fprintf(f, "$rc['EvntRate']                     = %f;\n",     EvntRate     );
   fprintf(f, "$rc['ReadRate']                     = %f;\n",     ReadRate     );
   fprintf(f, "$rc['WcmAve']                       = %f;\n",     WcmAve       );
   fprintf(f, "$rc['WcmSum']                       = %f;\n",     WcmSum       );
   fprintf(f, "$rc['BeamEnergy']                   = %f;\n",     BeamEnergy   );
   fprintf(f, "$rc['fPolId']                       = %d;\n",     fPolId       );
   fprintf(f, "$rc['fPolBeam']                     = %d;\n",     fPolBeam     );
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


/** */
short TStructRunInfo::GetPolarimeterId()
{
   TObjArray *subStrL = TPRegexp("^\\d+\\.(\\d)\\d{2}$").MatchS(runName);

   if (subStrL->GetEntriesFast() < 1) {
      printf("WARNING: TStructRunInfo::GetPolarimeterId(): Invalid polarimeter ID\n");
      return -1;
   }

   TString spolid = ((TObjString *) subStrL->At(1))->GetString();
   delete subStrL;

   fPolId = spolid.Atoi();
 
   if (fPolId >=0 && fPolId <=3)
      GetBeamIdStreamId(fPolId, fPolBeam, fPolStream);
   else {
      printf("WARNING: TStructRunInfo::GetPolarimeterId(): Invalid polarimeter ID\n");
      return -1;
   }

   return fPolId;
}


/** */
short TStructRunInfo::GetPolarimeterId(short beamId, short streamId)
{
   if (beamId == 1 && streamId == 1) { fPolId = 3; return 3; }
   if (beamId == 1 && streamId == 2) { fPolId = 1; return 1; }
   if (beamId == 2 && streamId == 1) { fPolId = 0; return 0; }
   if (beamId == 2 && streamId == 2) { fPolId = 2; return 2; }
   
   printf("WARNING: TStructRunInfo::GetPolarimeterId(): Invalid polarimeter ID\n");
   return -1;
}


/** */
void TStructRunInfo::GetBeamIdStreamId(Short_t polId, UShort_t &beamId, UShort_t &streamId)
{
   if (polId == 3) { beamId = 1; streamId = 1; };
   if (polId == 1) { beamId = 1; streamId = 2; };
   if (polId == 0) { beamId = 2; streamId = 1; };
   if (polId == 2) { beamId = 2; streamId = 2; };

   beamId = 0; streamId = 0;
}
