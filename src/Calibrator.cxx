#include "Calibrator.h"

#include "TMath.h"

#include "AsymCommon.h"
#include "AsymGlobals.h"
#include "MeasInfo.h"


ClassImp(Calibrator)

using namespace std;


/** Default constructor. */
Calibrator::Calibrator() : TObject(), fChannelCalibs(),
   fMeanChannel(), fMeanOfLogsChannel(), fRMSBananaChi2Ndf(), fRMSOfLogsBananaChi2Ndf()
{
}


/** Default destructor. */
Calibrator::~Calibrator()
{
}


/**
 * Calculates some statistical quantities (mean, chi^2/Ndf) for all valid
 * channels in fChannelCalibs and saves them in fMeanChannel, a special
 * dummy "channel" container.
 */
void Calibrator::UpdateMeanChannel()
{
   fMeanChannel.ResetToZero();
   fMeanOfLogsChannel.ResetToZero();

	vector<Float_t> vT0Coef;
   vector<Float_t> vDLWidth;
   vector<Float_t> vBananaChi2Ndf;
   vector<Float_t> vBananaChi2NdfLog;

   ChannelCalibMap::const_iterator iCh;
   ChannelCalibMap::const_iterator mb = fChannelCalibs.begin();
   ChannelCalibMap::const_iterator me = fChannelCalibs.end();

   UInt_t nChannels = 0;

   for (iCh=mb; iCh!=me; ++iCh)
   {
      if (gMeasInfo->IsSiliconChannel(iCh->first) && !std::isnan(iCh->second.fT0Coef) && !isinf(iCh->second.fT0Coef) &&
          !gMeasInfo->IsDisabledChannel(iCh->first) && iCh->second.GetFitStatus() == kDLFIT_OK )
      {
         vT0Coef.push_back(iCh->second.fT0Coef);
         vDLWidth.push_back(iCh->second.fDLWidth);
         vBananaChi2Ndf.push_back(iCh->second.fBananaChi2Ndf);
         vBananaChi2NdfLog.push_back( TMath::Log(iCh->second.fBananaChi2Ndf) );

         nChannels++;
      }
   }

   if (nChannels) {
      fMeanChannel.fT0Coef        = TMath::Mean(nChannels, &vT0Coef[0]);
      fMeanChannel.fT0CoefErr     = TMath::RMS (nChannels, &vT0Coef[0]);
      fMeanChannel.fDLWidth       = TMath::Mean(nChannels, &vDLWidth[0]);
      fMeanChannel.fDLWidthErr    = TMath::RMS (nChannels, &vDLWidth[0]);
      fMeanChannel.fBananaChi2Ndf = TMath::Mean(nChannels, &vBananaChi2Ndf[0]);
      fRMSBananaChi2Ndf           = TMath::RMS (nChannels, &vBananaChi2Ndf[0]);

      fMeanOfLogsChannel.fBananaChi2Ndf = TMath::Mean(nChannels, &vBananaChi2NdfLog[0]);
      fRMSOfLogsBananaChi2Ndf           = TMath::RMS (nChannels, &vBananaChi2NdfLog[0]);
   }
}


/** */
void Calibrator::CopyAlphaCoefs(Calibrator &other)
{
   ChannelCalibMap::const_iterator iCh;
   ChannelCalibMap::const_iterator mb = other.fChannelCalibs.begin();
   ChannelCalibMap::const_iterator me = other.fChannelCalibs.end();

   for (iCh=mb; iCh!=me; ++iCh) {
      UShort_t chId = iCh->first;
      const ChannelCalib &other_calib = iCh->second;

      ChannelCalib &calib = fChannelCalibs[chId];
      calib.CopyAlphaCoefs(other_calib);
   }
}


void Calibrator::UsePlainAlphaGain()
{
   if (!gAsymAnaInfo->HasAlphaBit()) {
      for(ChannelCalibMap::iterator iCh = fChannelCalibs.begin();
            iCh != fChannelCalibs.end(); iCh++) {
         ChannelCalib &calib = iCh->second;
         calib.fEffectiveGain = 1/calib.fAmAmp.fCoef;
      }
   }
}


void Calibrator::ApplyBiasCurrentCorrection(MeasInfo *measInfo, bool direct)
{
  int no_slope = 0;
  for(int i = 0; i < N_DETECTORS; i++)
    if (measInfo->fGainSlope[i] == 0)
      no_slope++;

   if (no_slope == N_DETECTORS) {
      Warning("ApplyBiasCurrentCorrection",
              "No gain vs bias current slopes in run configuration"
              " - Falling back to plain alpha gain");
      UsePlainAlphaGain();
      return;
   }

   vector<double> bc = measInfo->GetBiasCurrents();

   int not_avail = 0;
   for(int i = 0; i < N_DETECTORS; i++)
   {
      if (std::isnan(bc[i]))
      {
         not_avail++;
      }
   }

   if (not_avail > 0) {
      Error("ApplyBiasCurrentCorrection",
            "Bias current measurements are not avaliable for %i out of %i detectors"
            " - Falling back to plain alpha gain", not_avail, N_DETECTORS);
      UsePlainAlphaGain();
      return;
   }

   for(ChannelCalibMap::iterator iCh = fChannelCalibs.begin();
         iCh != fChannelCalibs.end(); iCh++) {
      int chId = iCh->first;
      if (chId == 0)
      {
         continue; // skip channel containing mean data
      }

      int det = ((chId - 1) / NSTRIP_PER_DETECTOR);
      ChannelCalib &calib = iCh->second;
      assert(det < N_DETECTORS);
      double correction = measInfo->fGainSlope[det] * bc.at(det);

      if (direct) {
         calib.fEffectiveGain = calib.fZeroBiasGain + correction;
      } else {
         calib.fZeroBiasGain = (1/calib.fAmAmp.fCoef) - correction;
      }
      if ((chId % NSTRIP_PER_DETECTOR) == 0)
      {
         Info("ApplyBiasCurrentCorrection", "strip %i -> (1/calib.fAmAmp.fCoef) = %f", chId, 1/calib.fAmAmp.fCoef);
         Info("ApplyBiasCurrentCorrection", "strip %i -> calib.fZeroBiasGain = %f", chId, calib.fZeroBiasGain);
         Info("ApplyBiasCurrentCorrection", "strip %i -> calib.fEffectiveGain = %f", chId, calib.fEffectiveGain);
         Info("ApplyBiasCurrentCorrection", "strip %i -> bc.at(chId) = %f", chId, bc.at(det));
      }
   }
}


/** */
void Calibrator::Calibrate(DrawObjContainer *c)
{
	Info("Calibrate(DrawObjContainer *c)", "Called...");
}


/** */
Float_t Calibrator::GetDLWidth(UShort_t chId) const
{
   return fChannelCalibs.find(chId)->second.fDLWidth;
}


/** */
Float_t Calibrator::GetDLWidthErr(UShort_t chId) const
{
   return fChannelCalibs.find(chId)->second.fDLWidthErr;
}


/** */
Float_t Calibrator::GetT0Coef(UShort_t chId) const
{
   return fChannelCalibs.find(chId)->second.fT0Coef;
}


/** */
Float_t Calibrator::GetBananaChi2Ndf(UShort_t chId) const
{
   return fChannelCalibs.find(chId)->second.fBananaChi2Ndf;
}


/** */
UInt_t Calibrator::GetFitStatus(UShort_t chId) const
{
   ChannelCalibMapConstIter iChCalib = fChannelCalibs.find(chId);

   if (iChCalib != fChannelCalibs.end()) {
      return iChCalib->second.GetFitStatus();
   }

   return kUNKNOWN;
}


/** */
Float_t Calibrator::GetT0CoefErr(UShort_t chId) const
{
   return fChannelCalibs.find(chId)->second.fT0CoefErr;
}


/** Returns energy registered by channel chId */
Float_t Calibrator::GetEnergyA(UShort_t adc, UShort_t chId) const
{
   if (std::isnan(fChannelCalibs.find(chId)->second.fEffectiveGain)) {
      Fatal("GetEnergyA", "NAN in effective gain");
   }

   return adc / fChannelCalibs.find(chId)->second.fEffectiveGain;
}


/** */
Float_t Calibrator::GetKinEnergyA(UShort_t adc, UShort_t chId) const
{
   Float_t emeas = GetEnergyA(adc, chId);
   Float_t eloss = fChannelCalibs.find(chId)->second.fAvrgEMiss;
   return  emeas + eloss;
}


/** */
Float_t Calibrator::GetTime(UShort_t tdc) const
{
   return WFD_TIME_UNIT_HALF * tdc;
}


/** */
Float_t Calibrator::GetTimeOfFlight(UShort_t tdc, UShort_t chId) const
{
   Float_t t0coef = fChannelCalibs.find(chId)->second.fT0Coef;
   return GetTime(tdc) + t0coef;
}


/** */
TFitResultPtr Calibrator::CalibrateOld(TH1 *h, TH1D *hMeanTime, UShort_t chId, Bool_t wideLimits)
{
   return 0;
}


/** */
void Calibrator::Calibrate(TH1 *h, TH1D *hMeanTime, UShort_t chId, Bool_t wideLimits)
{
}


/** */
void Calibrator::Print(const Option_t* opt) const
{
   Info("Print", "Calibrator members:");

   printf("              fAmAmp.fCoef fAmAmp.fCoefErr fAmAmp.fChi2Ndf    fAmInt.fCoef fAmInt.fCoefErr fAmInt.fChi2Ndf fDLWidth " \
          "fDLWidthErr fT0Coef fT0CoefErr fAvrgEMiss fAvrgEMissErr " \
          "fBananaChi2Ndf fFitStatus\n");

   ChannelCalibMap::const_iterator iCh;
   ChannelCalibMap::const_iterator mb = fChannelCalibs.begin();
   ChannelCalibMap::const_iterator me = fChannelCalibs.end();

   for (iCh=mb; iCh!=me; iCh++) {
	   printf("Channel %2d: ", iCh->first);
		iCh->second.Print();
   }
}


/** */
void Calibrator::PrintAsPhp(FILE *f) const
{
   ChannelCalibMap::const_iterator iCh;
   ChannelCalibMap::const_iterator mb = fChannelCalibs.begin();
   ChannelCalibMap::const_iterator me = fChannelCalibs.end();

   for (iCh=mb; iCh!=me; iCh++) {
  
      UShort_t chId = iCh->first;

      fprintf(f, "$rc['calib'][%d] = ", chId);
      iCh->second.PrintAsPhp(f);
      fprintf(f, ";\n");
   }
}


/** */
void Calibrator::PrintAsConfig(FILE *f) const
{
   ChannelCalibMap::const_iterator iCh;
   ChannelCalibMap::const_iterator mb = fChannelCalibs.begin();
   ChannelCalibMap::const_iterator me = fChannelCalibs.end();

   UShort_t chId;
   const ChannelCalib *ch;

   for (iCh=mb; iCh!=me; iCh++) {
  
      chId =  iCh->first;
      ch   = &iCh->second;

      Float_t onlineT0 = gMeasInfo->GetBeamEnergy() > kINJECTION ? -1*(ch->fT0Coef + gMeasInfo->GetExpectedGlobalTimeOffset()) : -1*ch->fT0Coef;

      fprintf(f, "Channel%02d=%5.3f %5.3f %7.1f %4.1f %5.2f %5.3f %4.1f %4.1f %4.3G %4.3G %4.3G %4.3G %4.3G\n",
         chId, onlineT0, ch->fAmAmp.fCoef*ch->fEMeasDLCorr, ch->fAvrgEMiss,
         10., 100., ch->fAmAmp.fCoef, 0., 0., 0., 0., 0., 0., 0.);
   }

	// XXX need to add 8 more default channels here
}
