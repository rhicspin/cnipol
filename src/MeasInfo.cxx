#include "MeasInfo.h"

#include <climits>
#include <sstream>
#include <algorithm>

#include "AsymGlobals.h"
#include "AsymCommon.h"

#include "AsymAnaInfo.h"
#include "MseMeasInfo.h"
#include "MseRunPeriod.h"

#include "revision-export.h"

ClassImp(MeasInfo)

using namespace std;


/** */
MeasInfo::MeasInfo() : TObject(),
   fBeamEnergy(0),
   fMachineParams(),
   fHasMachineParamsInRawData(false),
   fTargetParams(),
   fExpectedGlobalTdcOffset(0),
   fExpectedGlobalTimeOffset(0),
   fRunName(),
   fAlphaSourceCount(0),
   fProtoCutSlope(0), fProtoCutOffset(0), fProtoCutWidth(20),
   fProtoCutAdcMin(0), fProtoCutAdcMax(255), fProtoCutTdcMin(0), fProtoCutTdcMax(255),
   fPulserCutAdcMin(255), fPulserCutAdcMax(0), fPulserCutTdcMin(255), fPulserCutTdcMax(0),
   fSpinFlipperMarkers(), fFirstRevolution(UINT_MAX), fLastRevolution(0), fAcDipolePeriod(16441672),
   fRunId(-1),
   RUNID(0.0),
   fStartTime(0),
   fStopTime(0),
   fRunTime(0),
   fDataFormatVersion(0),
   fAsymVersion(ASYM_VERSION),
   fMeasType(kMEASTYPE_UNKNOWN),
   fNEventsProcessed (0),
   fNEventsTotal     (0),
   GoodEventRate     (0),
   EvntRate          (0),
   ReadRate          (0),
   fWallCurMon(), fWallCurMonAve(0), fWallCurMonSum(0),
   fPolId        (-1),       // valid values 0 - 3
   fPolBeam      (0),        // blue = 2 or yellow = 1
   fPolStream    (0),        // up =1 or down =2 stream
   PolarimetryID (1),        // PolarimetryID; Polarimetry-1 or Polarimetry-2
   MaxRevolution (0),        // MaxRevolution;
   fTargetOrient ('-'),
   fTargetId     ('-'),
   fSiliconChannels(),
   fDisabledChannels(),
   fBeamBunches()
{
   //for (int i=0; i<N_DETECTORS; i++) ActiveDetector[i] = 0xFFF;
   //ActiveDetector        = { 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF, 0xFFF };// ActiveDetector[N_DETECTORS]

   for (int i=1; i<=N_SILICON_CHANNELS; i++) {
      fSiliconChannels.insert(i);
   }

   //NActiveStrip  = N_SILICON_CHANNELS; // NAactiveStrip;
   //NDisableStrip = 0;      // NDisableStrip
   //NDisableBunch = 0;      // NDisableBunch,

   for (int i=0; i<N_BUNCHES; i++) {
      //DisableBunch[i] = 0;

      BeamBunch bbunch;
      fBeamBunches[i+1] = bbunch;
   }
}


/** */
MeasInfo::~MeasInfo() { }

Float_t     MeasInfo::GetBeamEnergyReal()           const { return fBeamEnergy; }
EBeamEnergy MeasInfo::GetBeamEnergy()               const { return (EBeamEnergy) Int_t(fBeamEnergy + 0.5); }
Float_t     MeasInfo::GetExpectedGlobalTimeOffset() const { return fExpectedGlobalTimeOffset; }
Short_t     MeasInfo::GetExpectedGlobalTdcOffset()  const { return fExpectedGlobalTdcOffset; }
Bool_t      MeasInfo::IsStarRotatorOn()             const { cout << "fStarRotatorCurrents: " << fMachineParams.fStarRotatorCurrents[1] << endl; return fMachineParams.fStarRotatorCurrents[GetRingId()-1] > 10 ? kTRUE : kFALSE; }
EMeasType   MeasInfo::GetMeasType()                 const { return fMeasType; } 
string      MeasInfo::GetAlphaCalibFileName()       const { return ""; }


/** */
void MeasInfo::SetMachineParams(const RecordMachineParams &rec)
{
   fMachineParams        = rec;
   fHasMachineParamsInRawData = true;
}


/** */
void MeasInfo::SetTargetParams(const RecordTargetParams &rec)
{
   fTargetParams = rec;
}


/** */
void MeasInfo::SetBeamEnergy(Float_t beamEnergy)
{
   fBeamEnergy = beamEnergy;

   UInt_t approxBeamEnergy = (UInt_t) (fBeamEnergy + 0.5);

   if (approxBeamEnergy >= 2*kINJECTION)
      // this number comes from the online config files. May need to add it to the run_info DB table in the future
      fExpectedGlobalTimeOffset = -8;
   else
      fExpectedGlobalTimeOffset = 0;

   fExpectedGlobalTdcOffset = (Short_t) (fExpectedGlobalTimeOffset / WFD_TIME_UNIT_HALF + 0.5);

   Info("SetBeamEnergy", "Expected time offset: %f %d\n", fExpectedGlobalTimeOffset, fExpectedGlobalTdcOffset);
}


/** */
void MeasInfo::SetMeasType(EMeasType measType) { fMeasType = measType; } 


/** */
void MeasInfo::Print(const Option_t* opt) const
{
   Info("Print", "Print members:");
   PrintAsPhp();
}


/** */
void MeasInfo::PrintAsPhp(FILE *f) const
{
   fprintf(f, "$rc['fRunId']                          = %d;\n",     fRunId          );
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
   fprintf(f, "$rc['fMachineParams']['fCavity197MHzVoltage'][0]   = %d;\n",     fMachineParams.fCavity197MHzVoltage[0]  );
   fprintf(f, "$rc['fMachineParams']['fCavity197MHzVoltage'][1]   = %d;\n",     fMachineParams.fCavity197MHzVoltage[1]  );
   fprintf(f, "$rc['fMachineParams']['fSnakeCurrents'][0]         = %f;\n",     fMachineParams.fSnakeCurrents[0]  );
   fprintf(f, "$rc['fMachineParams']['fSnakeCurrents'][1]         = %f;\n",     fMachineParams.fSnakeCurrents[1]  );
   fprintf(f, "$rc['fMachineParams']['fStarRotatorCurrents'][0]   = %f;\n",     fMachineParams.fStarRotatorCurrents[0]  );
   fprintf(f, "$rc['fMachineParams']['fStarRotatorCurrents'][1]   = %f;\n",     fMachineParams.fStarRotatorCurrents[1]  );
   fprintf(f, "$rc['fMachineParams']['fPhenixRotatorCurrents'][0] = %f;\n",     fMachineParams.fPhenixRotatorCurrents[0]  );
   fprintf(f, "$rc['fMachineParams']['fPhenixRotatorCurrents'][1] = %f;\n",     fMachineParams.fPhenixRotatorCurrents[1]  );
   fprintf(f, "$rc['fTargetParams']['fVelocity']                  = %d;\n",     fTargetParams.fVelocity );
   fprintf(f, "$rc['fTargetParams']['fProfileStartPosition']      = %d;\n",     fTargetParams.fProfileStartPosition );
   fprintf(f, "$rc['fTargetParams']['fProfileEndPosition']        = %d;\n",     fTargetParams.fProfileEndPosition );
   fprintf(f, "$rc['fTargetParams']['fProfilePeakPosition']       = %d;\n",     fTargetParams.fProfilePeakPosition );
   fprintf(f, "$rc['fPolId']                       = %d;\n",     fPolId       );
   fprintf(f, "$rc['fPolBeam']                     = %d;\n",     fPolBeam     );
   fprintf(f, "$rc['fPolStream']                   = %d;\n",     fPolStream   );
   fprintf(f, "$rc['PolarimetryID']                = %d;\n",     PolarimetryID);
   fprintf(f, "$rc['MaxRevolution']                = %d;\n",     MaxRevolution);
   fprintf(f, "$rc['fTargetOrient']                = \"%c\";\n", fTargetOrient);
   fprintf(f, "$rc['fTargetId']                    = \"%c\";\n", fTargetId    );

   stringstream ssChs("");

   //ssChs << "array(";
   //for (int i=0; i!=N_DETECTORS; i++) {
   //   ssChs << noshowbase << dec << i+1 << " => " << showbase << hex << ActiveDetector[i];
   //   ssChs << (i<N_DETECTORS-1 ? ", " : "");
   //}
   //ssChs << ")";
   //fprintf(f, "$rc['ActiveDetector']               = %s;\n", ssChs.str().c_str());

   //ssChs << dec << noshowbase;

   //ssChs.str("");
   //ssChs << "array(";
   //for (int i=0; i!=N_SILICON_CHANNELS; i++) {
   //   ssChs << i+1 << " => " << (ActiveStrip[i] ? "1" : "0");
   //   ssChs << (i<N_SILICON_CHANNELS-1 ? ", " : "");
   //}
   //ssChs << ")";
   //fprintf(f, "$rc['ActiveStrip']                  = %s;\n", ssChs.str().c_str());
   //fprintf(f, "$rc['ActiveStrip'][%d]              = %d;\n", i, ActiveStrip[i]);

   //fprintf(f, "$rc['NActiveStrip']                 = %d;\n", NActiveStrip );
   //fprintf(f, "$rc['NDisableStrip']                = %d;\n", NDisableStrip);

   // Unpack fDisabledChannels
   //ssChs.str("");
   //ssChs << "array(";
   //for (int i=0; i!=N_SILICON_CHANNELS; i++) {
   //   ssChs << i+1 << " => " << (fDisabledChannels[i] ? "1" : "0");
   //   ssChs << (i<N_SILICON_CHANNELS-1 ? ", " : "");
   //}
   //ssChs << ")";

   fprintf(f, "$rc['fSiliconChannels']             = %s;\n", SetAsPhpArray<UShort_t>(fSiliconChannels).c_str());
   fprintf(f, "$rc['fDisabledChannels']            = %s;\n", SetAsPhpArray<UShort_t>(fDisabledChannels).c_str());
   fprintf(f, "$rc['fBeamBunches']                 = %s;\n", MapAsPhpArray<UShort_t, BeamBunch>(fBeamBunches).c_str() );
   //fprintf(f, "$rc['NDisableBunch']                = %d;\n", NDisableBunch);
   fprintf(f, "$rc['fAlphaSourceCount']            = %d;\n", fAlphaSourceCount);
   fprintf(f, "$rc['fProtoCutSlope']               = %f;\n", fProtoCutSlope);
   fprintf(f, "$rc['fProtoCutOffset']              = %f;\n", fProtoCutOffset);
   fprintf(f, "$rc['fProtoCutWidth']               = %d;\n", fProtoCutWidth);
   fprintf(f, "$rc['fProtoCutAdcMin']              = %d;\n", fProtoCutAdcMin);
   fprintf(f, "$rc['fProtoCutAdcMax']              = %d;\n", fProtoCutAdcMax);
   fprintf(f, "$rc['fProtoCutTdcMin']              = %d;\n", fProtoCutTdcMin);
   fprintf(f, "$rc['fProtoCutTdcMax']              = %d;\n", fProtoCutTdcMax);
   fprintf(f, "$rc['fPulserCutAdcMin']             = %d;\n", fPulserCutAdcMin);
   fprintf(f, "$rc['fPulserCutAdcMax']             = %d;\n", fPulserCutAdcMax);
   fprintf(f, "$rc['fPulserCutTdcMin']             = %d;\n", fPulserCutTdcMin);
   fprintf(f, "$rc['fPulserCutTdcMax']             = %d;\n", fPulserCutTdcMax);
   fprintf(f, "$rc['fSpinFlipperMarkers']          = %s;\n", VecAsPhpArray<UInt_t>(fSpinFlipperMarkers).c_str());
   fprintf(f, "$rc['fSpinFlipperPhase']            = %f;\n", fSpinFlipperPhase);
   fprintf(f, "$rc['fFirstRevolution']             = %d;\n", fFirstRevolution);
   fprintf(f, "$rc['fLastRevolution']              = %d;\n", fLastRevolution);
   fprintf(f, "$rc['fAcDipolePeriod']              = %d;\n", fAcDipolePeriod);

   fprintf(f, "\n");
}


// Description : print out spin (Mode=0), fill (Mode=1) pattern
// Input       : Mode
void MeasInfo::PrintBunchPatterns() const
{
   std::stringstream ssSpin("");
   std::stringstream ssFill("");

   BeamBunchIterConst ibb = fBeamBunches.begin();

   for (int i=0; ibb!=fBeamBunches.end(); ++ibb, i++) {

      if (i%10 == 0) { ssSpin << " "; ssFill << " "; }

      if (ibb->second.fBunchSpin > 0)      ssSpin << "+";
      else if (ibb->second.fBunchSpin < 0) ssSpin << "-";
      else                                 ssSpin << ".";
      ssFill << (UShort_t) ibb->second.fIsFilled;
   }

   // Print Spin Pattern and Recover Spin Pattern by User Defined ones
   cout << "\nSpin pattern:" << endl;
   cout << ssSpin.str() << endl;

   // Print Fill Pattern and Recover Fill Pattern by User Defined ones
   cout << "\nFill pattern:" << endl;
   cout << ssFill.str() << endl;

}


// Print Out Configuration information
void MeasInfo::PrintConfig()
{
   fprintf(stdout, "=== RHIC Polarimeter Configuration (BGN) ===\n");

   // Configulation File
   fprintf(stdout,"         RUN STATUS = %s\n", gRunDb.run_status_s.c_str());
   fprintf(stdout,"         MEAS. TYPE = %s\n", gRunDb.measurement_type_s.c_str());
   fprintf(stdout,"             CONFIG = %s\n", reConfFile);
   fprintf(stdout,"              CALIB = %s\n", CalibFile);

   // banana cut configulation
   int ccutwl = (int) gAsymAnaInfo->widthl;
   int ccutwu = (int) gAsymAnaInfo->widthu;

   fprintf(stdout, "Carbon cut width : (low) %d (up) %d nsec \n", ccutwl, ccutwu);

   // tshift in [ns]
   fprintf(stdout, "             TSHIFT = %.1f\n", gAsymAnaInfo->tshift);

   // expected reference rate
   if (fRunId==5)   fprintf(stdout,"     REFERENCE_RATE = %.4f\n", gAsymAnaInfo->reference_rate);

   // target count/mm
   fprintf(stdout,"    TARGET_COUNT_MM = %.5f\n", gAsymAnaInfo->target_count_mm);

   // Disabled bunch
   //fprintf(stdout,"      #DISABLED_BUNCHES = %d\n", NDisableBunch);
   //if (NDisableBunch){
   //   fprintf(stdout,"       DISABLED_BUNCHES = ");
   //   for (int i=0; i<NDisableBunch; i++) printf("%d ", DisableBunch[i]);
   //   printf("\n");
   //}

   // Disabled strips
   //fprintf(stdout,"      #DISABLED_CHANNELS = %d\n", NDisableStrip);
   //if (NDisableStrip){
   //  fprintf(stdout,"       DISABLED_CHANNELS = ");
   //  for (int i=0;i<NDisableStrip;i++) printf("%d ", fDisabledChannels[i]+1);
   //  printf("\n");
   //}

   // Active Detector and Strip Configulation
   //printf("    Active Detector =");
   //for (int i=0; i<N_DETECTORS; i++)  printf(" %1d", ActiveDetector[i] ? 1 : 0 );
   //printf("\n");

   //printf("Active Strip Config =");
   //for (int i=N_DETECTORS-1; i>=0; i--) printf(" %x", ActiveDetector[i]);
   //printf("\n");

   //printf("Active Strip Config =");
   //for (int i=0; i<N_SILICON_CHANNELS; i++) {
   //   if (i%NSTRIP_PER_DETECTOR == 0) printf(" ");
   //   printf("%d", ActiveStrip[i]);
   //}
   //printf("\n");

   // print comment
   if (strlen(gRunDb.comment_s.c_str())>3)
     printf("            COMMENT = %s\n",    gRunDb.comment_s.c_str());
}


/** */
string MeasInfo::GetRunName() const { return fRunName; }

void MeasInfo::SetRunName(string runName)
{
   fRunName = runName;
   // Set to 0 when "RunID" contains alphabetical chars
   RUNID = strtod(fRunName.c_str(), NULL);
}


/**
 * Calculates the phase of the AC dipole marker in the data with respect to the
 * first revolution.
 */
Double_t MeasInfo::CalcSpinFlipperPhase()
{
   UInt_t spinFlipperMarkerRevId = fSpinFlipperMarkers.size() > 0 ? fSpinFlipperMarkers[0] : 0;
   Double_t delta_phase = _TWO_PI * (fAcDipolePeriod/_RHIC_AC_DIPOLE_MAX_PERIOD) * ( (Double_t) spinFlipperMarkerRevId - fFirstRevolution);
   fSpinFlipperPhase    = fmod(delta_phase, _TWO_PI);
   return fSpinFlipperPhase;
}


/** */
Short_t MeasInfo::GetPolarimeterId()
{
   TObjArray *subStrL = TPRegexp("^\\d+\\.(\\d)\\d{2}(|\\.alpha0)$").MatchS(fRunName);

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
}


/** */
Short_t MeasInfo::GetPolarimeterId(short beamId, short streamId)
{
   if (beamId == 1 && streamId == 1) { fPolId = 3; return 3; }
   if (beamId == 1 && streamId == 2) { fPolId = 1; return 1; }
   if (beamId == 2 && streamId == 1) { fPolId = 0; return 0; }
   if (beamId == 2 && streamId == 2) { fPolId = 2; return 2; }
   if (beamId == 2 && streamId == 0) { fPolId = kHJET; return kHJET; }

   printf("WARNING: MeasInfo::GetPolarimeterId(): Invalid polarimeter ID\n");
   return -1;
}


/** */
UInt_t MeasInfo::GetFillId()
{
   TObjArray *subStrL = TPRegexp("^(\\d+)\\.\\d{3}(|\\.alpha0)$").MatchS(fRunName);

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
}


bool MeasInfo::IsRunYear(int year)
{
   time_t t = fStartTime;
   struct tm *time = gmtime(&t);
   int meas_year = 1900 + time->tm_year;
   return meas_year == year;
}


/** */
void MeasInfo::GetBeamIdStreamId(Short_t polId, UShort_t &beamId, UShort_t &streamId)
{
   if (polId == 0) { beamId = 2; streamId = 1; };
   if (polId == 1) { beamId = 1; streamId = 2; };
   if (polId == 2) { beamId = 2; streamId = 2; };
   if (polId == 3) { beamId = 1; streamId = 1; };

   beamId = 0; streamId = 0;
}


/** Deprecated. */
void MeasInfo::Update(DbEntry &rundb)
{
   stringstream sstr;
   UShort_t     chId;

   sstr << rundb.fFields["DISABLED_CHANNELS"];

   while (sstr >> chId) {
      DisableChannel(chId);
   }

   // For compatibility reasons set the Run variable
   // Taken from AsymRunDb
   if (RUNID < 6500) { // Run undefined
      fRunId = 0;

   } else if (RUNID >= 6500 && RUNID < 7400) { // Run05
      fRunId = 5;
      for (int i=0; i<N_SILICON_CHANNELS; i++) gPhi[i] = phiRun5[i];

   } else if (RUNID >= 7400 && RUNID < 10018) { // Run06
      fRunId = 6;
      for (int i=0; i<N_SILICON_CHANNELS; i++) gPhi[i] = phiRun6[i];

   } else if (RUNID >= 10018 && RUNID < 14000) { // Run09
      fRunId = 9;

   } else
      fRunId = 11;
}


/** */
void MeasInfo::Update(MseMeasInfoX& run)
{
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
      
   // by fill id
   if (RUNID < 6500) { // Run undefined
      fRunId = 0;

   } else if (RUNID >= 6500 && RUNID < 7400) { // Run 5
      fRunId = 5;
      for (int i=0; i<N_SILICON_CHANNELS; i++) gPhi[i] = phiRun5[i];

   } else if (RUNID >= 7400 && RUNID < 10018) { // Run 6
      fRunId = 6;
      for (int i=0; i<N_SILICON_CHANNELS; i++) gPhi[i] = phiRun6[i];

   } else if (RUNID >= 10018 && RUNID < 11100) { // Run 9
      fRunId = 9;
   } else if (RUNID >= 14000 && RUNID < 15500) { // Run 11
      fRunId = 11;
   } else if (RUNID >= 16000 && RUNID < 16800) { // Run 12
      fRunId = 12;
   } else if (RUNID >= 17000 ) { // Run 13
      fRunId = 13;
      gCh2WfdMap = ch2WfdMap_run13;
   } else {
      // default Run value
   }
}


/** */
void MeasInfo::Update(MseRunPeriodX& runPeriod)
{
   fAlphaSourceCount = UChar_t(runPeriod.alpha_source_count);
   fProtoCutSlope    = runPeriod.cut_proto_slope;
   fProtoCutOffset   = runPeriod.cut_proto_offset;
   fProtoCutWidth    = (unsigned char) runPeriod.cut_proto_width;
   fProtoCutAdcMin   = runPeriod.cut_proto_adc_min;
   fProtoCutAdcMax   = runPeriod.cut_proto_adc_max;
   fProtoCutTdcMin   = runPeriod.cut_proto_tdc_min;
   fProtoCutTdcMax   = runPeriod.cut_proto_tdc_max;
   fPulserCutAdcMin  = runPeriod.cut_pulser_adc_min;
   fPulserCutAdcMax  = runPeriod.cut_pulser_adc_max;
   fPulserCutTdcMin  = runPeriod.cut_pulser_tdc_min;
   fPulserCutTdcMax  = runPeriod.cut_pulser_tdc_max;
}


/**
 * Call this method to override some parameters by those provided by the user
 * in the command line.
 */
void MeasInfo::Update(AsymAnaInfo& anaInfo)
{
   if (anaInfo.fAlphaSourceCount >= 0)
      fAlphaSourceCount = anaInfo.fAlphaSourceCount;

   if (anaInfo.fAcDipolePeriod > 0)
      fAcDipolePeriod = anaInfo.fAcDipolePeriod;
}


void MeasInfo::UpdateRevolutions(UInt_t revId)
{
   if (revId < fFirstRevolution) fFirstRevolution = revId;
   if (revId > fLastRevolution)  fLastRevolution  = revId;
}


//
// Deprecated.
//
// Description : Disable detector and configure active strips
//
// Input       : int mask.detector
// Return      : ActiveDetector[i] remains masked strip configulation
void MeasInfo::ConfigureActiveStrip(int mask)
{
   // Disable Detector First
   for (int i=0; i<N_DETECTORS; i++) {

      if ( (~mask>>i) & 1) {

         //ActiveDetector[i] = 0x000;

         for (int j=0; j<NSTRIP_PER_DETECTOR; j++) {
            //NActiveStrip--;
            //ActiveStrip[i*NSTRIP_PER_DETECTOR+j] = 0;
         }
      }
   }

   // Configure Active Strips
   //int det, strip=0;
   //for (int i=0; i<NDisableStrip; i++) {
   //   det = fDisabledChannels[i]/NSTRIP_PER_DETECTOR;
   //   // skip if the detector is already disabled
   //   if ( (mask >> det) & 1) {
   //      strip = fDisabledChannels[i] - det * NSTRIP_PER_DETECTOR;
   //      ActiveDetector[det] ^= int(pow(2,double(strip))); // mask strips of detector=det
   //      //ActiveStrip[strip+det*NSTRIP_PER_DETECTOR] = 0;
   //      //NActiveStrip--;
   //   }
   //}

   // Active Detector and Strip Configulation
   printf("ReConfigured Active Detector =");

   //for (int i=0; i<N_DETECTORS; i++)  printf(" %1d", ActiveDetector[i] ? 1 : 0 );
   printf("\n");
   //    printf("Active Strip Config =");
   //    for (int i=N_DETECTORS-1; i>=0; i--) printf(" %x", ActiveDetector[i]);
   //    printf("\n");

   printf("Reconfigured Active Strip Config =");

   for (int i=0; i<N_SILICON_CHANNELS; i++) {
     if (i%NSTRIP_PER_DETECTOR == 0) printf(" ");
     //printf("%d", ActiveStrip[i]);
   }

   printf("\n");
}


// Description : Identify Polarimety ID and RHIC Beam (blue or yellow)
// Input       : char RunID[]
void MeasInfo::SetPolarimetrIdRhicBeam(const char* RunID)
{
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
}


/** */
void MeasInfo::DisableChannel(UShort_t chId)
{
   Warning("DisableChannel", "Disabled channel %d", chId);

   if ( fDisabledChannels.find(chId) == fDisabledChannels.end() )
      fDisabledChannels.insert(chId);
}


/** */
void MeasInfo::DisableChannels(std::bitset<N_DETECTORS> &disabled_det)
{
   for (UShort_t iDet=0; iDet!=N_DETECTORS; ++iDet)
   {
      if (disabled_det.test(iDet) )
      {
         for (UShort_t iCh=1; iCh<=NSTRIP_PER_DETECTOR; ++iCh)
            DisableChannel(NSTRIP_PER_DETECTOR*iDet + iCh);
      }
   }
}


/** */
void MeasInfo::EnableChannel(UShort_t chId)
{
   Warning("EnableChannel", "Not implemented. Enabled channel %d", chId);
}


/** */
Bool_t MeasInfo::IsDisabledChannel(UShort_t chId)
{
   return fDisabledChannels.find(chId) != fDisabledChannels.end() ? kTRUE : kFALSE;
}


/** */
Bool_t MeasInfo::IsSiliconChannel(UShort_t chId)
{
   if ( chId > 0 && chId <= N_SILICON_CHANNELS)
      return true;

   return false;
}


/** */
UShort_t MeasInfo::GetNumDisabledChannels() const
{
   return fDisabledChannels.size();
}


/** */
UShort_t MeasInfo::GetNumActiveSiChannels() const
{
   UShort_t nCh = 0;
   ChannelSetConstIter iCh = fSiliconChannels.begin();

   for ( ; iCh != fSiliconChannels.end(); ++iCh) {
      if ( fDisabledChannels.find(*iCh) == fDisabledChannels.end() ) nCh++;
   }

   return nCh;
}


/** */
Bool_t MeasInfo::IsHamaChannel(UShort_t chId)
{
   if ( ( (EPolarimeterId) fPolId == kB2D || (EPolarimeterId) fPolId == kY1D) &&
        ( (chId >= 13 && chId <= 24) || (chId >= 49 && chId <= 60) )
      )
      return true;

   return false;
}


/** */
Bool_t MeasInfo::IsPmtChannel(UShort_t chId) const
{
   if ( ((EPolarimeterId) fPolId == kY2U || (EPolarimeterId) fPolId == kY1D) &&
        chId > N_SILICON_CHANNELS && chId <= N_SILICON_CHANNELS+4 )
      return true;

   return false;
}


/** */
Bool_t MeasInfo::IsSpinFlipperMarkerChannel(UShort_t chId) const
{
   if ( (EPolarimeterId) fPolId == kB1U && chId == 76 ) // need to add a constrain by time
      return true;

   return false;
}


/** */
BeamBunchMap MeasInfo::GetBunches() const
{
   return fBeamBunches;
}


/** */
BeamBunchMap MeasInfo::GetFilledBunches() const
{
   BeamBunchMap bunches;

   BeamBunchIterConst ibb = fBeamBunches.begin();

   for (; ibb!=fBeamBunches.end(); ++ibb) {
      
      if (ibb->second.IsFilled())
         bunches.insert(*ibb);
   }

   return bunches;
}


/** */
BeamBunchMap MeasInfo::GetEmptyBunches() const
{
   BeamBunchMap bunches;

   BeamBunchIterConst ibb = fBeamBunches.begin();

   for (; ibb!=fBeamBunches.end(); ++ibb) {
      
      if (!ibb->second.IsFilled())
         bunches.insert(*ibb);
   }

   return bunches;
}


/** */
UShort_t MeasInfo::GetNumFilledBunches() const
{
   return GetFilledBunches().size();
}


/** */
UShort_t MeasInfo::GetNumEmptyBunches() const
{
   return GetEmptyBunches().size();
}


/** */
Bool_t MeasInfo::IsEmptyBunch(UShort_t bid) const
{
   return !fBeamBunches.find(bid)->second.IsFilled();
}


/** */
ESpinState MeasInfo::GetBunchSpin(UShort_t bid) const
{
   return fBeamBunches.find(bid)->second.GetSpin();
}


/** */
ERingId MeasInfo::GetRingId() const
{
   switch (fPolBeam) {
   case 1:
	   return kYELLOW_RING;
   case 2:
	   return kBLUE_RING;
   default:
      return kUNKNOWN_RING;
   }
}


/** */
ETargetOrient MeasInfo::GetTargetOrient() const
{
   switch (fTargetOrient) {
   case 'H':
	   return kTARGET_H;
   case 'V':
	   return kTARGET_V;
   default:
      return kUNKNOWN_ORIENT;
   }
}


/** */
UShort_t MeasInfo::GetTargetId() const
{
   //string sTgtId(fTargetId);
   //return (UShort_t) atoi(sTgtId.c_str());
   return (UShort_t) atoi(&fTargetId);
}


/** */
EPolarimeterId MeasInfo::ExtractPolarimeterId(std::string runName)
{
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
}
