
#include <sstream>

#include "RunInfo.h"

using namespace std;


/** */
TStructRunInfo::TStructRunInfo() : Run(-1), RUNID(0.0), runName(100, ' '),
   StartTime(0), StopTime(0), RunTime(0), fDataFormatVersion(0), fAsymVersion(ASYM_VERSION)
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
   for (int i=0; i<NSTRIP; i++) { ActiveStrip[i] = 1; fDisabledChannels[i] = 0; }
   NActiveStrip          = NSTRIP; // NAactiveStrip;
   NDisableStrip         = 0; // NDisableStrip
   NFilledBunch          = 0; // NFilledBunch;
   NActiveBunch          = 0; // NActiveBunch;
   NDisableBunch         = 0; // NDisableBunch,
   for (int i=0; i<NBUNCH; i++) { DisableBunch[i] = 0; }
   //DisableBunch[NBUNCH]
}


/** */
TStructRunInfo::~TStructRunInfo() { }


string TStructRunInfo::GetAlphaCalibFileName() const
{ return ""; }


string TStructRunInfo::GetDlCalibFileName() const
{ return ""; }


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
   TString tstr;

   if (buf.IsReading()) {
      //printf("reading TStructRunInfo::Streamer(TBuffer &buf) \n");
      buf >> Run;
      buf >> RUNID;
      buf >> tstr; runName = tstr.Data();
      buf >> StartTime;
      buf >> StopTime;
      buf >> RunTime;
      buf >> fDataFormatVersion;
      buf >> tstr; fAsymVersion = tstr.Data();
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
      buf.ReadFastArray(fDisabledChannels, NSTRIP);
      buf >> NFilledBunch;
      buf >> NActiveBunch;
      buf >> NDisableBunch;
      buf.ReadFastArray(DisableBunch, NBUNCH);

   } else {
      //printf("writing TStructRunInfo::Streamer(TBuffer &buf) \n");
      buf << Run;
      buf << RUNID;
      tstr = runName; buf << tstr;
      buf << StartTime;
      buf << StopTime;
      buf << RunTime;
      buf << fDataFormatVersion;
      tstr = fAsymVersion; buf << tstr;
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
      buf.WriteFastArray(fDisabledChannels, NSTRIP);
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
   fprintf(f, "$rc['fDataFormatVersion']           = %d;\n",     fDataFormatVersion);
   fprintf(f, "$rc['fAsymVersion']                 = \"%s\";\n", fAsymVersion.c_str());
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

   stringstream ssChs("");

   ssChs << "array(";

   for (int i=0; i!=NDETECTOR; i++) {
      ssChs << noshowbase << dec << i+1 << " => " << showbase << hex << ActiveDetector[i];
      ssChs << (i<NDETECTOR-1 ? ", " : "");
   }

   ssChs << ")";

   fprintf(f, "$rc['ActiveDetector']               = %s;\n", ssChs.str().c_str());

   ssChs << dec << noshowbase;

   ssChs.str("");
   ssChs << "array(";

   for (int i=0; i!=NSTRIP; i++) {
      ssChs << i+1 << " => " << (ActiveStrip[i] ? "1" : "0");
      ssChs << (i<NSTRIP-1 ? ", " : "");
   }

   ssChs << ")";

   fprintf(f, "$rc['ActiveStrip']                  = %s;\n", ssChs.str().c_str());
   //fprintf(f, "$rc['ActiveStrip'][%d]              = %d;\n", i, ActiveStrip[i]);

   fprintf(f, "$rc['NActiveStrip']                 = %d;\n", NActiveStrip );
   fprintf(f, "$rc['NDisableStrip']                = %d;\n", NDisableStrip);

   ssChs.str("");
   ssChs << "array(";

   for (int i=0; i!=NSTRIP; i++) {
      ssChs << i+1 << " => " << (fDisabledChannels[i] ? "1" : "0");
      ssChs << (i<NSTRIP-1 ? ", " : "");
   }

   ssChs << ")";

   fprintf(f, "$rc['fDisabledChannels']            = %s;\n", ssChs.str().c_str());
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


/** */
void TStructRunInfo::Update(TStructRunDB &rundb)
{
   stringstream sstr;

   UShort_t chId;

   sstr.str(""); sstr << rundb.fFields["DISABLED_CHANNELS"];

   while (sstr >> chId) fDisabledChannels[chId-1] = 1;

   // For compatibility reasons set the Run variable
   // Taken from AsymRunDb
   if (RUNID < 6500) { // Run undefined
      gRunInfo.Run = 0;

   } else if (RUNID >= 6500 && RUNID < 7400) { // Run05
      gRunInfo.Run = 5;
      for (int i=0; i<NSTRIP; i++) gPhi[i] = phiRun5[i];

   } else if (RUNID >= 7400 && RUNID < 10018) { // Run06
      gRunInfo.Run = 6;
      for (int i=0; i<NSTRIP; i++) gPhi[i] = phiRun6[i];

   } else if (RUNID >= 10018 && RUNID < 14000) { // Run09
      gRunInfo.Run = 9;

   } else
      gRunInfo.Run = 11;
}
