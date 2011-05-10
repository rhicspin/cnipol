#include "RunInfo.h"

#include <sstream>

#include "TSystem.h"

#include "AsymGlobals.h"

#include "AnaInfo.h"
#include "MseRunInfo.h"
#include "MseRunPeriod.h"

using namespace std;


/** */
RunInfo::RunInfo() :
   fBeamEnergy  (0),
	fExpectedGlobalTdcOffset(0),
	fExpectedGlobalTimeOffset(0),
   Run(-1),
	RUNID(0.0),
	fRunName(100, ' '),
	StartTime(0),
	StopTime(0),
	RunTime(0),
	fDataFormatVersion(0),
   fAsymVersion(ASYM_VERSION),
   fMeasType(kMEASTYPE_UNKNOWN),
   GoodEventRate(0),      // GoodEventRate;
   EvntRate     (0),      // EvntRate;
   ReadRate     (0),      // ReadRate;
   WcmAve       (0),      // WcmAve;
   WcmSum       (0),      // WcmSum;
   fPolId       (-1),     // valid values 0 - 3
   fPolBeam     (0),      // blue = 2 or yellow = 1
   fPolStream   (0),      // up =1 or down =2 stream
   PolarimetryID(1),      // PolarimetryID; Polarimetry-1 or Polarimetry-2
   MaxRevolution(0),      // MaxRevolution;
   fTargetOrient('-'),
   targetID     ('-'),
	fProtoCutSlope(0), fProtoCutOffset(0)
{
   strcpy(TgtOperation, "fixed");
 
   for (int i=0; i<N_DETECTORS; i++) ActiveDetector[i] = 0xFFF;
   //ActiveDetector        = { 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF };// ActiveDetector[N_DETECTORS]
   for (int i=0; i<NSTRIP; i++) { ActiveStrip[i] = 1; fDisabledChannels[i] = 0; }
   NActiveStrip          = NSTRIP; // NAactiveStrip;
   NDisableStrip         = 0;      // NDisableStrip
   NFilledBunch          = 0;      // NFilledBunch;
   NActiveBunch          = 0;      // NActiveBunch;
   NDisableBunch         = 0;      // NDisableBunch,
   for (int i=0; i<N_BUNCHES; i++) { DisableBunch[i] = 0; }
}


/** */
RunInfo::~RunInfo() { }


/** */
void RunInfo::SetBeamEnergy(Float_t beamEnergy)
{
   fBeamEnergy = beamEnergy;

   UInt_t approxBeamEnergy = (UInt_t) (fBeamEnergy + 0.5);

   if (approxBeamEnergy == kFLATTOP)
	   fExpectedGlobalTimeOffset = -8;
   else
	   fExpectedGlobalTimeOffset = 0;

   fExpectedGlobalTdcOffset = (Short_t) (fExpectedGlobalTimeOffset / WFD_TIME_UNIT_HALF + 0.5);

   printf("expected offset: %f %d\n", fExpectedGlobalTimeOffset, fExpectedGlobalTdcOffset);
}

Float_t RunInfo::GetBeamEnergy() { return fBeamEnergy; }

Float_t RunInfo::GetExpectedGlobalTimeOffset() { return fExpectedGlobalTimeOffset; }

Short_t RunInfo::GetExpectedGlobalTdcOffset() { return fExpectedGlobalTdcOffset; }


string RunInfo::GetAlphaCalibFileName() const
{ return ""; }


string RunInfo::GetDlCalibFileName() const
{ return ""; }


/** */
TBuffer & operator<<(TBuffer &buf, RunInfo *&rec)
{
   if (!rec) return buf;
   //printf("operator<<(TBuffer &buf, RunInfo *rec) : \n");
   rec->Streamer(buf);
   return buf;
}


/** */
TBuffer & operator>>(TBuffer &buf, RunInfo *&rec)
{
   //printf("operator>>(TBuffer &buf, RunInfo *rec) : \n");

   //if (!rec) {
   //   printf("ERROR in operator>>\n");
   //   exit(-1);
   //}

   // if object has been created already delete it
   //free(rec);

   //rec = (RunInfo *) realloc(rec, sizeof(RunInfo) + 11*sizeof(char));
   //rec = (RunInfo *) realloc(rec, sizeof(RunInfo));
   //rec->TgtOperation = new char[10];

   rec->Streamer(buf);
   return buf;
}


/** */
void RunInfo::Streamer(TBuffer &buf)
{
   TString tstr;
   Int_t   tint;

   if (buf.IsReading()) {
      //printf("reading RunInfo::Streamer(TBuffer &buf) \n");
      buf >> Run;
      buf >> RUNID;
      buf >> tstr; fRunName = tstr.Data();
      buf >> StartTime;
      buf >> StopTime;
      buf >> RunTime;
      buf >> fDataFormatVersion;
      buf >> tstr; fAsymVersion = tstr.Data();
      buf >> tint; fMeasType = (EMeasType) tint;
      buf >> GoodEventRate;
      buf >> EvntRate;
      buf >> ReadRate;
      buf >> WcmAve;
      buf >> WcmSum;
      buf >> fBeamEnergy;
      buf >> fPolId;
      buf >> fPolBeam;
      buf >> fPolStream;
      buf >> PolarimetryID;
      buf >> MaxRevolution;
      buf >> fTargetOrient;
      buf >> targetID;
      buf >> TgtOperation;
      //buf.ReadString(TgtOperation, 32);
      buf.ReadFastArray(ActiveDetector, N_DETECTORS);
      buf.ReadFastArray(ActiveStrip, NSTRIP);
      buf >> NActiveStrip;
      buf >> NDisableStrip;
      buf.ReadFastArray(fDisabledChannels, NSTRIP);
      buf >> NFilledBunch;
      buf >> NActiveBunch;
      buf >> NDisableBunch;
      buf.ReadFastArray(DisableBunch, N_BUNCHES);
      buf >> fProtoCutSlope;
      buf >> fProtoCutOffset;

   } else {
      //printf("writing RunInfo::Streamer(TBuffer &buf) \n");
      buf << Run;
      buf << RUNID;
      tstr = fRunName; buf << tstr;
      buf << StartTime;
      buf << StopTime;
      buf << RunTime;
      buf << fDataFormatVersion;
      tstr = fAsymVersion; buf << tstr;
      buf << (Int_t) fMeasType;
      buf << GoodEventRate;
      buf << EvntRate;
      buf << ReadRate;
      buf << WcmAve;
      buf << WcmSum;
      buf << fBeamEnergy;
      buf << fPolId;
      buf << fPolBeam;
      buf << fPolStream;
      buf << PolarimetryID;
      buf << MaxRevolution;
      buf << fTargetOrient;
      buf << targetID;
      buf << TgtOperation;
      buf.WriteFastArray(ActiveDetector, N_DETECTORS);
      buf.WriteFastArray(ActiveStrip, NSTRIP);
      buf << NActiveStrip;
      buf << NDisableStrip;
      buf.WriteFastArray(fDisabledChannels, NSTRIP);
      buf << NFilledBunch;
      buf << NActiveBunch;
      buf << NDisableBunch;
      buf.WriteFastArray(DisableBunch, N_BUNCHES);
      buf << fProtoCutSlope;
      buf << fProtoCutOffset;
   }
}


/** */
void RunInfo::Print(const Option_t* opt) const
{
   PrintAsPhp();
}


/** */
void RunInfo::PrintAsPhp(FILE *f) const
{ //{{{
   fprintf(f, "$rc['Run']                          = %d;\n",     Run          );
   fprintf(f, "$rc['RUNID']                        = %.3f;\n",   RUNID        );
   fprintf(f, "$rc['fRunName']                     = \"%s\";\n", fRunName.c_str() );
   fprintf(f, "$rc['StartTime']                    = %ld;\n",    StartTime    );
   fprintf(f, "$rc['StopTime']                     = %ld;\n",    StopTime     );
   fprintf(f, "$rc['RunTime']                      = %f;\n",     RunTime      );
   fprintf(f, "$rc['fDataFormatVersion']           = %d;\n",     fDataFormatVersion);
   fprintf(f, "$rc['fAsymVersion']                 = \"%s\";\n", fAsymVersion.c_str());
   fprintf(f, "$rc['fMeasType']                    = %#04X;\n",  fMeasType);
   fprintf(f, "$rc['GoodEventRate']                = %f;\n",     GoodEventRate);
   fprintf(f, "$rc['EvntRate']                     = %f;\n",     EvntRate     );
   fprintf(f, "$rc['ReadRate']                     = %f;\n",     ReadRate     );
   fprintf(f, "$rc['WcmAve']                       = %f;\n",     WcmAve       );
   fprintf(f, "$rc['WcmSum']                       = %f;\n",     WcmSum       );
   fprintf(f, "$rc['fBeamEnergy']                  = %f;\n",     fBeamEnergy  );
   fprintf(f, "$rc['fPolId']                       = %d;\n",     fPolId       );
   fprintf(f, "$rc['fPolBeam']                     = %d;\n",     fPolBeam     );
   fprintf(f, "$rc['fPolStream']                   = %d;\n",     fPolStream   );
   fprintf(f, "$rc['PolarimetryID']                = %d;\n",     PolarimetryID);
   fprintf(f, "$rc['MaxRevolution']                = %d;\n",     MaxRevolution);
   fprintf(f, "$rc['fTargetOrient']                = \"%c\";\n", fTargetOrient);
   fprintf(f, "$rc['targetID']                     = \"%c\";\n", targetID     );
   fprintf(f, "$rc['TgtOperation']                 = \"%s\";\n", TgtOperation );

   stringstream ssChs("");

   ssChs << "array(";

   for (int i=0; i!=N_DETECTORS; i++) {
      ssChs << noshowbase << dec << i+1 << " => " << showbase << hex << ActiveDetector[i];
      ssChs << (i<N_DETECTORS-1 ? ", " : "");
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
   fprintf(f, "$rc['fProtoCutSlope']               = %f;\n", fProtoCutSlope);
   fprintf(f, "$rc['fProtoCutOffset']              = %f;\n", fProtoCutOffset);
} //}}}


/** */
short RunInfo::GetPolarimeterId()
{
   TObjArray *subStrL = TPRegexp("^\\d+\\.(\\d)\\d{2}$").MatchS(fRunName);

   if (subStrL->GetEntriesFast() < 1) {
      printf("WARNING: RunInfo::GetPolarimeterId(): Invalid polarimeter ID\n");
      return -1;
   }

   TString spolid = ((TObjString *) subStrL->At(1))->GetString();
   delete subStrL;

   fPolId = spolid.Atoi();
 
   if (fPolId >=0 && fPolId <=3)
      GetBeamIdStreamId(fPolId, fPolBeam, fPolStream);
   else {
      printf("WARNING: RunInfo::GetPolarimeterId(): Invalid polarimeter ID\n");
      return -1;
   }

   return fPolId;
}


/** */
short RunInfo::GetPolarimeterId(short beamId, short streamId)
{
   if (beamId == 1 && streamId == 1) { fPolId = 3; return 3; }
   if (beamId == 1 && streamId == 2) { fPolId = 1; return 1; }
   if (beamId == 2 && streamId == 1) { fPolId = 0; return 0; }
   if (beamId == 2 && streamId == 2) { fPolId = 2; return 2; }
   
   printf("WARNING: RunInfo::GetPolarimeterId(): Invalid polarimeter ID\n");
   return -1;
}


/** */
void RunInfo::GetBeamIdStreamId(Short_t polId, UShort_t &beamId, UShort_t &streamId)
{
   if (polId == 3) { beamId = 1; streamId = 1; };
   if (polId == 1) { beamId = 1; streamId = 2; };
   if (polId == 0) { beamId = 2; streamId = 1; };
   if (polId == 2) { beamId = 2; streamId = 2; };

   beamId = 0; streamId = 0;
}


/** */
void RunInfo::Update(DbEntry &rundb)
{
   stringstream sstr;
   UShort_t     chId;

   sstr << rundb.fFields["DISABLED_CHANNELS"];

   while (sstr >> chId) fDisabledChannels[chId-1] = 1;

   // For compatibility reasons set the Run variable
   // Taken from AsymRunDb
   if (RUNID < 6500) { // Run undefined
      Run = 0;

   } else if (RUNID >= 6500 && RUNID < 7400) { // Run05
      Run = 5;
      for (int i=0; i<NSTRIP; i++) gPhi[i] = phiRun5[i];

   } else if (RUNID >= 7400 && RUNID < 10018) { // Run06
      Run = 6;
      for (int i=0; i<NSTRIP; i++) gPhi[i] = phiRun6[i];

   } else if (RUNID >= 10018 && RUNID < 14000) { // Run09
      Run = 9;

   } else
      Run = 11;
}


/** */
void RunInfo::Update(MseRunInfoX& run)
{
   stringstream sstr;
   UShort_t     chId;

   sstr << run.disabled_channels;

   while (sstr >> chId) fDisabledChannels[chId-1] = 1;

   // For compatibility reasons set the Run variable
   // Taken from AsymRunDb
   if (RUNID < 6500) { // Run undefined
      Run = 0;

   } else if (RUNID >= 6500 && RUNID < 7400) { // Run05
      Run = 5;
      for (int i=0; i<NSTRIP; i++) gPhi[i] = phiRun5[i];

   } else if (RUNID >= 7400 && RUNID < 10018) { // Run06
      Run = 6;
      for (int i=0; i<NSTRIP; i++) gPhi[i] = phiRun6[i];

   } else if (RUNID >= 10018 && RUNID < 14000) { // Run09
      Run = 9;

   } else
      Run = 11;
}


/** */
void RunInfo::Update(MseRunPeriodX& runPeriod)
{
   fProtoCutSlope  = runPeriod.cut_proto_slope;
   fProtoCutOffset = runPeriod.cut_proto_offset;
}


// Description : Disable detector and configure active strips
//
// Input       : int mask.detector
// Return      : ActiveDetector[i] remains masked strip configulation
void RunInfo::ConfigureActiveStrip(int mask)
{ //{{{
   // Disable Detector First
   for (int i=0; i<N_DETECTORS; i++) {

      if ( (~mask>>i) & 1) {

         ActiveDetector[i] = 0x000;

         for (int j=0; j<NSTRIP_PER_DETECTOR; j++) {
            NActiveStrip--;
            ActiveStrip[i*NSTRIP_PER_DETECTOR+j] = 0;
         }
      }
   }

   // Configure Active Strips
   int det, strip=0;

   for (int i=0; i<NDisableStrip; i++) {

      det = fDisabledChannels[i]/NSTRIP_PER_DETECTOR;

      // skip if the detector is already disabled
      if ( (mask >> det) & 1) {
         strip = fDisabledChannels[i] - det * NSTRIP_PER_DETECTOR;
         ActiveDetector[det] ^= int(pow(2,double(strip))); // mask strips of detector=det
         ActiveStrip[strip+det*NSTRIP_PER_DETECTOR] = 0;
         NActiveStrip--;
      }
   }

   // Active Detector and Strip Configulation
   printf("ReConfigured Active Detector =");

   for (int i=0; i<N_DETECTORS; i++)  printf(" %1d", ActiveDetector[i] ? 1 : 0 );
   printf("\n");
   //    printf("Active Strip Config =");
   //    for (int i=N_DETECTORS-1; i>=0; i--) printf(" %x", ActiveDetector[i]);
   //    printf("\n");

   printf("Reconfigured Active Strip Config =");

   for (int i=0; i<NSTRIP; i++) {
     if (i%NSTRIP_PER_DETECTOR == 0) printf(" ");
     printf("%d", ActiveStrip[i]);
   }

   printf("\n");
} //}}}


// Print Out Configuration information
void RunInfo::PrintConfig()
{ //{{{
   fprintf(stdout, "=== RHIC Polarimeter Configuration (BGN) ===\n");

   // Configulation File
   fprintf(stdout,"         RUN STATUS = %s\n", gRunDb.run_status_s.c_str());
   fprintf(stdout,"         MEAS. TYPE = %s\n", gRunDb.measurement_type_s.c_str());
   fprintf(stdout,"             CONFIG = %s\n", reConfFile);
   fprintf(stdout,"              CALIB = %s\n", CalibFile);

   // banana cut configulation

   int ccutwu;
   int ccutwl;

   if (gAnaInfo.CBANANA == 0) {
      ccutwl = (int) gConfigInfo->data.chan[3].ETCutW;
      ccutwu = (int) gConfigInfo->data.chan[3].ETCutW;
   } else if (gAnaInfo.CBANANA == 2) {
      fprintf(stdout,"            MASSCUT = %.1f\n", gAnaInfo.MassSigma);
   } else {
      ccutwl = (int) gAnaInfo.widthl;
      ccutwu = (int) gAnaInfo.widthu;
   }

   if (gAnaInfo.CBANANA!=2)
     fprintf (stdout,"Carbon cut width : (low) %d (up) %d nsec \n", ccutwl, ccutwu);

   // tshift in [ns]
   fprintf(stdout,"             TSHIFT = %.1f\n",gAnaInfo.tshift);

   // expected reference rate
   if (Run==5)   fprintf(stdout,"     REFERENCE_RATE = %.4f\n",gAnaInfo.reference_rate);

   // target count/mm
   fprintf(stdout,"    TARGET_COUNT_MM = %.5f\n", gAnaInfo.target_count_mm);

   // Disabled bunch
   fprintf(stdout,"      #DISABLED_BUNCHES = %d\n", NDisableBunch);
   if (NDisableBunch){
     fprintf(stdout,"       DISABLED_BUNCHES = ");
     for (int i=0; i<NDisableBunch; i++) printf("%d ", DisableBunch[i]);
     printf("\n");
   }

   // Disabled strips
   fprintf(stdout,"      #DISABLED_CHANNELS = %d\n", NDisableStrip);
   if (NDisableStrip){
     fprintf(stdout,"       DISABLED_CHANNELS = ");
     for (int i=0;i<NDisableStrip;i++) printf("%d ", fDisabledChannels[i]+1);
     printf("\n");
   }

   // Active Detector and Strip Configulation
   printf("    Active Detector =");
   for (int i=0; i<N_DETECTORS; i++)  printf(" %1d", ActiveDetector[i] ? 1 : 0 );
   printf("\n");
   //    printf("Active Strip Config =");
   //    for (int i=N_DETECTORS-1; i>=0; i--) printf(" %x", ActiveDetector[i]);
   //    printf("\n");

   printf("Active Strip Config =");

   for (int i=0; i<NSTRIP; i++) {
      if (i%NSTRIP_PER_DETECTOR == 0) printf(" ");
      printf("%d", ActiveStrip[i]);
   }
   printf("\n");

   // print comment
   if (strlen(gRunDb.comment_s.c_str())>3)
     printf("            COMMENT = %s\n",    gRunDb.comment_s.c_str());
} //}}}


// Description : Identify Polarimety ID and RHIC Beam (blue or yellow)
// Input       : char RunID[]
void RunInfo::SetPolarimetrIdRhicBeam(const char* RunID)
{ //{{{
  char ID = *(strrchr(RunID,'.')+1);

  switch (ID) {
  case '0':
     fPolBeam      = 2;
     PolarimetryID = 1; // blue polarimeter-1
     fPolStream    = 1;
     break;
  case '1':
     fPolBeam      = 1;
     PolarimetryID = 1; // yellow polarimeter-1
     fPolStream    = 2;
     break;
  case '2':
     fPolBeam      = 2;
     PolarimetryID = 2; // blue polarimeter-2
     fPolStream    = 2;
     break;
  case '3':
     fPolBeam      = 1;
     PolarimetryID = 2; // yellow polarimeter-2
     fPolStream    = 1;
     break;
  default:
     gSystem->Error("SetPolarimetrIdRhicBeam", "Unrecognized RHIC beam and Polarimeter-ID. Perhaps calibration data..?");
     break;
  }

  /*
  fprintf(stdout,"RUNINFO: RunID=%.3f fPolBeam=%d PolarimetryID=%d\n",
          gRunInfo.RUNID, gRunInfo.fPolBeam, gRunInfo.PolarimetryID);
  */
} //}}}
