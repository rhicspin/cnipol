#include "MeasInfo.h"

#include <sstream>
#include <algorithm>

#include "AsymGlobals.h"
#include "Asym.h"

#include "AnaInfo.h"
#include "MseMeasInfo.h"
#include "MseRunPeriod.h"

ClassImp(MeasInfo)

using namespace std;


/** */
MeasInfo::MeasInfo() : TObject(),
   fStartVoltage(0),
   fEndVoltage(0),
   fBeamEnergy(0),
   fExpectedGlobalTdcOffset(0),
   fExpectedGlobalTimeOffset(0),
   fRunName(100, ' '),
   fProtoCutSlope(0), fProtoCutOffset(0),
   fProtoCutAdcMin(0), fProtoCutAdcMax(255), fProtoCutTdcMin(0), fProtoCutTdcMax(255),
   fPulserCutAdcMin(255), fPulserCutAdcMax(0), fPulserCutTdcMin(255), fPulserCutTdcMax(0),

   Run(-1),
   RUNID(0.0),
   fStartTime(0),
   fStopTime(0),
   fRunTime(0),
   fDataFormatVersion(0),
   fAsymVersion(ASYM_VERSION),
   fMeasType(kMEASTYPE_UNKNOWN),
   fNEventsProcessed (0),
   fNEventsTotal     (0),
   GoodEventRate     (0),        // GoodEventRate;
   EvntRate          (0),        // EvntRate;
   ReadRate          (0),        // ReadRate;
   fWallCurMon(), fWallCurMonAve(0), fWallCurMonSum(0),
   fPolId        (-1),       // valid values 0 - 3
   fPolBeam      (0),        // blue = 2 or yellow = 1
   fPolStream    (0),        // up =1 or down =2 stream
   PolarimetryID (1),        // PolarimetryID; Polarimetry-1 or Polarimetry-2
   MaxRevolution (0),        // MaxRevolution;
   fTargetOrient ('-'),
   fTargetId     ('-'),
   fDisabledChannelsVec(),   // should not be used. will deprecate
   fSiliconChannels(),
   fActiveSiliconChannels(), // Only good channels used in the analysis
   fBeamBunches()
{
   for (int i=0; i<N_DETECTORS; i++) ActiveDetector[i] = 0xFFF;
   //ActiveDetector        = { 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF };// ActiveDetector[N_DETECTORS]

   for (int i=1; i<=N_SILICON_CHANNELS; i++) {
      ActiveStrip[i-1]       = 1;
      fDisabledChannels[i-1] = 0;
      fSiliconChannels.insert(i);
      fActiveSiliconChannels.insert(i);
   }

   NActiveStrip          = N_SILICON_CHANNELS; // NAactiveStrip;
   NDisableStrip         = 0;      // NDisableStrip
   NDisableBunch         = 0;      // NDisableBunch,

   for (int i=0; i<N_BUNCHES; i++) {
      DisableBunch[i] = 0;

      BeamBunch bbunch;
      fBeamBunches[i+1] = bbunch;
   }
}


/** */
MeasInfo::~MeasInfo() { }

Float_t   MeasInfo::GetEndVoltage()               const { return fEndVoltage;}
Float_t   MeasInfo::GetStartVoltage()             const { return fStartVoltage;}
Float_t   MeasInfo::GetBeamEnergy()               const { return fBeamEnergy; }
Float_t   MeasInfo::GetExpectedGlobalTimeOffset() const { return fExpectedGlobalTimeOffset; }
Short_t   MeasInfo::GetExpectedGlobalTdcOffset()  const { return fExpectedGlobalTdcOffset; }
EMeasType MeasInfo::GetMeasType()                 const { return fMeasType; } 
string    MeasInfo::GetAlphaCalibFileName()       const { return ""; }
string    MeasInfo::GetDlCalibFileName()          const { return ""; }


void MeasInfo::SetVoltages(int begin, int end)
{
 fStartVoltage=(float)begin;
 fEndVoltage=(float)end;
}




/** */
void MeasInfo::SetBeamEnergy(Float_t beamEnergy)
{ //{{{
   fBeamEnergy = beamEnergy;

   UInt_t approxBeamEnergy = (UInt_t) (fBeamEnergy + 0.5);

   if (approxBeamEnergy == kBEAM_ENERGY_100 || approxBeamEnergy == kBEAM_ENERGY_250)
      // this number comes from the online config files. May need to add it to the run_info DB table in the future
      fExpectedGlobalTimeOffset = -8;
   else
      fExpectedGlobalTimeOffset = 0;

   fExpectedGlobalTdcOffset = (Short_t) (fExpectedGlobalTimeOffset / WFD_TIME_UNIT_HALF + 0.5);

   printf("expected offset: %f %d\n", fExpectedGlobalTimeOffset, fExpectedGlobalTdcOffset);
} //}}}


/** */
void MeasInfo::SetMeasType(EMeasType measType) { fMeasType = measType; } 


/** */
void MeasInfo::Print(const Option_t* opt) const
{ //{{{
   Info("Print", "Print members:");
   PrintAsPhp();
} //}}}


/** */
void MeasInfo::PrintAsPhp(FILE *f) const
{ //{{{
   fprintf(f, "$rc['Run']                          = %d;\n",     Run          );
   fprintf(f, "$rc['RUNID']                        = %.3f;\n",   RUNID        );
   fprintf(f, "$rc['fRunName']                     = \"%s\";\n", fRunName.c_str() );
   fprintf(f, "$rc['fStartTime']                   = %ld;\n",    fStartTime   );
   fprintf(f, "$rc['fStopTime']                    = %ld;\n",    fStopTime    );
   fprintf(f, "$rc['fRunTime']                     = %f;\n",     fRunTime      );
   fprintf(f, "$rc['fDataFormatVersion']           = %d;\n",     fDataFormatVersion);
   fprintf(f, "$rc['fAsymVersion']                 = \"%s\";\n", fAsymVersion.c_str());
   fprintf(f, "$rc['fMeasType']                    = %#010X;\n", fMeasType);
   fprintf(f, "$rc['fNEventsProcessed']            = %u;\n",     fNEventsProcessed);
   fprintf(f, "$rc['fNEventsTotal']                = %u;\n",     fNEventsTotal);
   fprintf(f, "$rc['GoodEventRate']                = %f;\n",     GoodEventRate);
   fprintf(f, "$rc['EvntRate']                     = %f;\n",     EvntRate     );
   fprintf(f, "$rc['ReadRate']                     = %f;\n",     ReadRate     );
   fprintf(f, "$rc['fWallCurMon']                  = %s;\n",     MapAsPhpArray<UShort_t, Float_t>(fWallCurMon).c_str() );
   fprintf(f, "$rc['fWallCurMonAve']               = %f;\n",     fWallCurMonAve );
   fprintf(f, "$rc['fWallCurMonSum']               = %f;\n",     fWallCurMonSum );
   fprintf(f, "$rc['fBeamEnergy']                  = %f;\n",     fBeamEnergy  );
   fprintf(f, "$rc['fStartVoltage']		            = %f;\n",     fStartVoltage);
   fprintf(f, "$rc['fEndVoltage']                  = %f;\n",     fEndVoltage);
   fprintf(f, "$rc['fPolId']                       = %d;\n",     fPolId       );
   fprintf(f, "$rc['fPolBeam']                     = %d;\n",     fPolBeam     );
   fprintf(f, "$rc['fPolStream']                   = %d;\n",     fPolStream   );
   fprintf(f, "$rc['PolarimetryID']                = %d;\n",     PolarimetryID);
   fprintf(f, "$rc['MaxRevolution']                = %d;\n",     MaxRevolution);
   fprintf(f, "$rc['fTargetOrient']                = \"%c\";\n", fTargetOrient);
   fprintf(f, "$rc['fTargetId']                    = \"%c\";\n", fTargetId    );

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

   for (int i=0; i!=N_SILICON_CHANNELS; i++) {
      ssChs << i+1 << " => " << (ActiveStrip[i] ? "1" : "0");
      ssChs << (i<N_SILICON_CHANNELS-1 ? ", " : "");
   }

   ssChs << ")";

   fprintf(f, "$rc['ActiveStrip']                  = %s;\n", ssChs.str().c_str());
   //fprintf(f, "$rc['ActiveStrip'][%d]              = %d;\n", i, ActiveStrip[i]);

   fprintf(f, "$rc['NActiveStrip']                 = %d;\n", NActiveStrip );
   fprintf(f, "$rc['NDisableStrip']                = %d;\n", NDisableStrip);

   // Unpack fDisabledChannels
   ssChs.str("");
   ssChs << "array(";

   for (int i=0; i!=N_SILICON_CHANNELS; i++) {
      ssChs << i+1 << " => " << (fDisabledChannels[i] ? "1" : "0");
      ssChs << (i<N_SILICON_CHANNELS-1 ? ", " : "");
   }

   ssChs << ")";

   fprintf(f, "$rc['fDisabledChannels']            = %s;\n", ssChs.str().c_str());
   fprintf(f, "$rc['fDisabledChannelsVec']         = %s;\n", VecAsPhpArray<UShort_t>(fDisabledChannelsVec).c_str());
   fprintf(f, "$rc['fSiliconChannels']             = %s;\n", SetAsPhpArray<UShort_t>(fSiliconChannels).c_str());
   fprintf(f, "$rc['fActiveSiliconChannels']       = %s;\n", SetAsPhpArray<UShort_t>(fActiveSiliconChannels).c_str());
   fprintf(f, "$rc['fBeamBunches']                 = %s;\n", MapAsPhpArray<UShort_t, BeamBunch>(fBeamBunches).c_str() );
   fprintf(f, "$rc['NDisableBunch']                = %d;\n", NDisableBunch);
   fprintf(f, "$rc['fProtoCutSlope']               = %f;\n", fProtoCutSlope);
   fprintf(f, "$rc['fProtoCutOffset']              = %f;\n", fProtoCutOffset);
   fprintf(f, "$rc['fProtoCutAdcMin']              = %d;\n", fProtoCutAdcMin);
   fprintf(f, "$rc['fProtoCutAdcMax']              = %d;\n", fProtoCutAdcMax);
   fprintf(f, "$rc['fProtoCutTdcMin']              = %d;\n", fProtoCutTdcMin);
   fprintf(f, "$rc['fProtoCutTdcMax']              = %d;\n", fProtoCutTdcMax);
   fprintf(f, "$rc['fPulserCutAdcMin']             = %d;\n", fPulserCutAdcMin);
   fprintf(f, "$rc['fPulserCutAdcMax']             = %d;\n", fPulserCutAdcMax);
   fprintf(f, "$rc['fPulserCutTdcMin']             = %d;\n", fPulserCutTdcMin);
   fprintf(f, "$rc['fPulserCutTdcMax']             = %d;\n", fPulserCutTdcMax);

   fprintf(f, "\n");
} //}}}


// Description : print out spin (Mode=0), fill (Mode=1) pattern
// Input       : Mode
void MeasInfo::PrintBunchPatterns() const
{ //{{{
   std::stringstream ssSpin("");
   std::stringstream ssFill("");

   BeamBunchIterConst ibb = fBeamBunches.begin();

   for (int i=0; ibb!=fBeamBunches.end(); ++ibb, i++) {

      if (i%10 == 0) { ssSpin << " "; ssFill << " "; }

      ssSpin << ibb->second.fBunchSpin;
      ssFill << (UShort_t) ibb->second.fIsFilled;
   }

   // Print Spin Pattern and Recover Spin Pattern by User Defined ones
   cout << "\nSpin pattern used:" << endl;
   cout << ssSpin.str() << endl;

   // Print Fill Pattern and Recover Fill Pattern by User Defined ones
   cout << "\nFill Pattern Used:" << endl;
   cout << ssFill.str() << endl;

} //}}}


// Print Out Configuration information
void MeasInfo::PrintConfig()
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

   if (gAnaInfo->CBANANA == 0) {
      ccutwl = (int) gConfigInfo->data.chan[3].ETCutW;
      ccutwu = (int) gConfigInfo->data.chan[3].ETCutW;
   } else if (gAnaInfo->CBANANA == 2) {
      fprintf(stdout,"            MASSCUT = %.1f\n", gAnaInfo->MassSigma);
   } else {
      ccutwl = (int) gAnaInfo->widthl;
      ccutwu = (int) gAnaInfo->widthu;
   }

   if (gAnaInfo->CBANANA!=2)
     fprintf (stdout,"Carbon cut width : (low) %d (up) %d nsec \n", ccutwl, ccutwu);

   // tshift in [ns]
   fprintf(stdout,"             TSHIFT = %.1f\n", gAnaInfo->tshift);

   // expected reference rate
   if (Run==5)   fprintf(stdout,"     REFERENCE_RATE = %.4f\n", gAnaInfo->reference_rate);

   // target count/mm
   fprintf(stdout,"    TARGET_COUNT_MM = %.5f\n", gAnaInfo->target_count_mm);

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

   for (int i=0; i<N_SILICON_CHANNELS; i++) {
      if (i%NSTRIP_PER_DETECTOR == 0) printf(" ");
      printf("%d", ActiveStrip[i]);
   }
   printf("\n");

   // print comment
   if (strlen(gRunDb.comment_s.c_str())>3)
     printf("            COMMENT = %s\n",    gRunDb.comment_s.c_str());
} //}}}


/** */
string MeasInfo::GetRunName() const { return fRunName; }
void   MeasInfo::SetRunName(std::string runName) {
   fRunName = runName;
   // Set to 0 when "RunID" contains alphabetical chars
   RUNID = strtod(fRunName.c_str(), NULL);
}


/** */
Short_t MeasInfo::GetPolarimeterId()
{ //{{{
   TObjArray *subStrL = TPRegexp("^\\d+\\.(\\d)\\d{2}$").MatchS(fRunName);

   if (subStrL->GetEntriesFast() < 1) {
      Error("GetPolarimeterId", "Cannot extract polarimeter id from run name");
      return -1;
   }

   TString spolid = ((TObjString *) subStrL->At(1))->GetString();
   delete subStrL;

   fPolId = spolid.Atoi();

   if (fPolId >=0 && fPolId <=3)
      GetBeamIdStreamId(fPolId, fPolBeam, fPolStream);
   else {
		Error("GetPolarimeterId", "Invalid polarimeter ID");
      return -1;
   }

   return fPolId;
} //}}}


/** */
Short_t MeasInfo::GetPolarimeterId(short beamId, short streamId)
{ //{{{
   if (beamId == 1 && streamId == 1) { fPolId = 3; return 3; }
   if (beamId == 1 && streamId == 2) { fPolId = 1; return 1; }
   if (beamId == 2 && streamId == 1) { fPolId = 0; return 0; }
   if (beamId == 2 && streamId == 2) { fPolId = 2; return 2; }

   printf("WARNING: MeasInfo::GetPolarimeterId(): Invalid polarimeter ID\n");
   return -1;
} //}}}


/** */
UInt_t MeasInfo::GetFillId()
{ //{{{
   TObjArray *subStrL = TPRegexp("^(\\d+)\\.\\d{3}$").MatchS(fRunName);

   if (subStrL->GetEntriesFast() < 1) {
      Error("GetFillId", "Cannot extract fill Id from run name");
      return 0;
   }

   TString sfillid = ((TObjString *) subStrL->At(1))->GetString();
   delete subStrL;

   UInt_t fFillId = sfillid.Atoi();

   if (fFillId <=0 ) {
      Error("GetFillId", "Invalid fill ID");
      return 0;
   }

   return fFillId;
} //}}}


/** */
void MeasInfo::GetBeamIdStreamId(Short_t polId, UShort_t &beamId, UShort_t &streamId)
{ //{{{
   if (polId == 0) { beamId = 2; streamId = 1; };
   if (polId == 1) { beamId = 1; streamId = 2; };
   if (polId == 2) { beamId = 2; streamId = 2; };
   if (polId == 3) { beamId = 1; streamId = 1; };

   beamId = 0; streamId = 0;
} //}}}


/** Deprecated. */
void MeasInfo::Update(DbEntry &rundb)
{ //{{{
   stringstream sstr;
   UShort_t     chId;

   sstr << rundb.fFields["DISABLED_CHANNELS"];

   while (sstr >> chId) {
      DisableChannel(chId);
   }

   // For compatibility reasons set the Run variable
   // Taken from AsymRunDb
   if (RUNID < 6500) { // Run undefined
      Run = 0;

   } else if (RUNID >= 6500 && RUNID < 7400) { // Run05
      Run = 5;
      for (int i=0; i<N_SILICON_CHANNELS; i++) gPhi[i] = phiRun5[i];

   } else if (RUNID >= 7400 && RUNID < 10018) { // Run06
      Run = 6;
      for (int i=0; i<N_SILICON_CHANNELS; i++) gPhi[i] = phiRun6[i];

   } else if (RUNID >= 10018 && RUNID < 14000) { // Run09
      Run = 9;

   } else
      Run = 11;
} //}}}


/** */
void MeasInfo::Update(MseMeasInfoX& run)
{ //{{{
   stringstream sstr;
   UShort_t     chId;

   sstr << run.disabled_channels;

   while (sstr >> chId) {
      DisableChannel(chId);
   }

   // Check for horizontal targets and disable 90 degree detectors
   if (fTargetOrient == 'H') {
      bitset<N_DETECTORS> disabled_horiz_det(string("010010"));
      DisableChannels(disabled_horiz_det);
   }
      

   // For compatibility reasons set the Run variable
   // Taken from AsymRunDb
   if (RUNID < 6500) { // Run undefined
      Run = 0;

   } else if (RUNID >= 6500 && RUNID < 7400) { // Run 5
      Run = 5;
      for (int i=0; i<N_SILICON_CHANNELS; i++) gPhi[i] = phiRun5[i];

   } else if (RUNID >= 7400 && RUNID < 10018) { // Run 6
      Run = 6;
      for (int i=0; i<N_SILICON_CHANNELS; i++) gPhi[i] = phiRun6[i];

   } else if (RUNID >= 10018 && RUNID < 14000) { // Run 9
      Run = 9;
   } else if (RUNID >= 14700 && RUNID < 15480) { // Run 11
      Run = 11;
   } else {
      Run = 12;
   }
} //}}}


/** */
void MeasInfo::Update(MseRunPeriodX& runPeriod)
{ //{{{
   fProtoCutSlope   = runPeriod.cut_proto_slope;
   fProtoCutOffset  = runPeriod.cut_proto_offset;
   fProtoCutAdcMin  = runPeriod.cut_proto_adc_min;
   fProtoCutAdcMax  = runPeriod.cut_proto_adc_max;
   fProtoCutTdcMin  = runPeriod.cut_proto_tdc_min;
   fProtoCutTdcMax  = runPeriod.cut_proto_tdc_max;
   fPulserCutAdcMin = runPeriod.cut_pulser_adc_min;
   fPulserCutAdcMax = runPeriod.cut_pulser_adc_max;
   fPulserCutTdcMin = runPeriod.cut_pulser_tdc_min;
   fPulserCutTdcMax = runPeriod.cut_pulser_tdc_max;
} //}}}


// Description : Disable detector and configure active strips
//
// Input       : int mask.detector
// Return      : ActiveDetector[i] remains masked strip configulation
void MeasInfo::ConfigureActiveStrip(int mask)
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

   for (int i=0; i<N_SILICON_CHANNELS; i++) {
     if (i%NSTRIP_PER_DETECTOR == 0) printf(" ");
     printf("%d", ActiveStrip[i]);
   }

   printf("\n");
} //}}}


// Description : Identify Polarimety ID and RHIC Beam (blue or yellow)
// Input       : char RunID[]
void MeasInfo::SetPolarimetrIdRhicBeam(const char* RunID)
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
     Error("SetPolarimetrIdRhicBeam", "Unrecognized RHIC beam and Polarimeter-ID. Perhaps calibration data..?");
     break;
  }

  /*
  fprintf(stdout,"MeasInfo: RunID=%.3f fPolBeam=%d PolarimetryID=%d\n",
          gMeasInfo->RUNID, gMeasInfo->fPolBeam, gMeasInfo->PolarimetryID);
  */
} //}}}


/** */
void MeasInfo::DisableChannel(UShort_t chId)
{ //{{{
   Warning("DisableChannel", "Disabled channel %d", chId);

   fDisabledChannels[chId-1] = 1;

   if (find(fDisabledChannelsVec.begin(), fDisabledChannelsVec.end(), chId) == fDisabledChannelsVec.end() )
      fDisabledChannelsVec.push_back(chId);

   fActiveSiliconChannels.erase(chId);
} //}}}


/** */
void MeasInfo::DisableChannels(std::bitset<N_DETECTORS> &disabled_det)
{ //{{{
   for (UShort_t iDet=0; iDet!=N_DETECTORS; ++iDet)
   {
      if (disabled_det.test(iDet) )
      {
         for (UShort_t iCh=1; iCh<=NSTRIP_PER_DETECTOR; ++iCh)
            DisableChannel(NSTRIP_PER_DETECTOR*iDet + iCh);
      }
   }
} //}}}


/** */
void MeasInfo::EnableChannel(UShort_t chId)
{ //{{{
   Warning("EnableChannel", "Enabled channel %d", chId);

   //fDisabledChannels[chId-1] = 1;

   //if (find(fDisabledChannelsVec.begin(), fDisabledChannelsVec.end(), chId) == fDisabledChannelsVec.end() )
   //   fDisabledChannelsVec.push_back(chId);

   //fActiveSiliconChannels.erase(chId);
} //}}}


/** */
void MeasInfo::EnableChannels(std::bitset<N_DETECTORS> &disabled_det)
{ //{{{
   //for (UShort_t iDet=0; iDet!=N_DETECTORS; ++iDet)
   //{
   //   if (disabled_det.test(iDet) )
   //   {
   //      for (UShort_t iCh=1; iCh<=NSTRIP_PER_DETECTOR; ++iCh)
   //         DisableChannel(NSTRIP_PER_DETECTOR*iDet + iCh);
   //   }
   //}
} //}}}


/** */
Bool_t MeasInfo::IsDisabledChannel(UShort_t chId)
{ //{{{
   return find(fDisabledChannelsVec.begin(), fDisabledChannelsVec.end(), chId) != fDisabledChannelsVec.end() ? kTRUE : kFALSE;
} //}}}


/** */
Bool_t MeasInfo::IsSiliconChannel(UShort_t chId)
{ //{{{
   if ( chId > 0 && chId <= N_SILICON_CHANNELS)
      return true;

   return false;
} //}}}


/** */
Bool_t MeasInfo::IsHamaChannel(UShort_t chId)
{ //{{{
   if ( ( (EPolarimeterId) fPolId == kB2D || (EPolarimeterId) fPolId == kY1D) &&
        ( (chId >= 13 && chId <= 24) || (chId >= 49 && chId <= 60) )
      )
      return true;

   return false;
} //}}}


/** */
Bool_t MeasInfo::IsPmtChannel(UShort_t chId)
{ //{{{
   if ((EPolarimeterId) fPolId == kY2U && chId > N_SILICON_CHANNELS && chId <= N_SILICON_CHANNELS+4)
      return true;

   return false;
} //}}}


/** */
BeamBunchMap MeasInfo::GetBunches() const
{ //{{{
   return fBeamBunches;
} //}}}


/** */
BeamBunchMap MeasInfo::GetFilledBunches() const
{ //{{{
   BeamBunchMap bunches;

   BeamBunchIterConst ibb = fBeamBunches.begin();

   for (; ibb!=fBeamBunches.end(); ++ibb) {
      
      if (ibb->second.IsFilled())
         bunches.insert(*ibb);
   }

   return bunches;
} //}}}


/** */
BeamBunchMap MeasInfo::GetEmptyBunches() const
{ //{{{
   BeamBunchMap bunches;

   BeamBunchIterConst ibb = fBeamBunches.begin();

   for (; ibb!=fBeamBunches.end(); ++ibb) {
      
      if (!ibb->second.IsFilled())
         bunches.insert(*ibb);
   }

   return bunches;
} //}}}


/** */
UShort_t MeasInfo::GetNumFilledBunches() const
{ //{{{
   return GetFilledBunches().size();
} //}}}


/** */
UShort_t MeasInfo::GetNumEmptyBunches() const
{ //{{{
   return GetEmptyBunches().size();
} //}}}


/** */
Bool_t MeasInfo::IsEmptyBunch(UShort_t bid) const
{ //{{{
   return !fBeamBunches.find(bid)->second.IsFilled();
} //}}}


/** */
ESpinState MeasInfo::GetBunchSpin(UShort_t bid) const
{ //{{{
   return fBeamBunches.find(bid)->second.GetSpin();
} //}}}


/** */
ERingId MeasInfo::GetRingId() const
{ //{{{
   switch (fPolBeam) {
   case 1:
	   return kYELLOW_RING;
   case 2:
	   return kBLUE_RING;
   default:
      return kUNKNOWN_RING;
   }
} //}}}


/** */
ETargetOrient MeasInfo::GetTargetOrient() const
{ //{{{
   switch (fTargetOrient) {
   case 'H':
	   return kTARGET_H;
   case 'V':
	   return kTARGET_V;
   default:
      return kUNKNOWN_ORIENT;
   }
} //}}}


/** */
UShort_t MeasInfo::GetTargetId() const
{ //{{{
   //string sTgtId(fTargetId);
   //return (UShort_t) atoi(sTgtId.c_str());
   return (UShort_t) atoi(&fTargetId);
} //}}}


/** */
EPolarimeterId MeasInfo::ExtractPolarimeterId(std::string runName)
{ //{{{
   TObjArray *subStrL = TPRegexp("^\\d+\\.(\\d)\\d{2}$").MatchS(runName);

   if (subStrL->GetEntriesFast() < 1) {
      gSystem->Error("ExtractPolarimeterId", "Cannot extract polarimeter id from run name");
      return kUNKNOWN_POLID;
   }

   TString spolid = ((TObjString *) subStrL->At(1))->GetString();
   delete subStrL;

   EPolarimeterId polId = (EPolarimeterId) spolid.Atoi();

   if ( gRunConfig.fPolarimeters.find(polId) == gRunConfig.fPolarimeters.end() || polId == kUNKNOWN_POLID ) {
		gSystem->Error("ExtractPolarimeterId", "Invalid polarimeter ID");
      return kUNKNOWN_POLID;
	}

   return polId;
} //}}}
