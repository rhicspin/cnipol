#include "Calibrator.h"

#include "TMath.h"

#include "AsymCommon.h"
#include "AsymGlobals.h"
#include "MeasInfo.h"


ClassImp(Calibrator)

using namespace std;


/** Default constructor. */
Calibrator::Calibrator() : TObject(), fRandom(new TRandom()), fChannelCalibs(),
   fMeanChannel(), fMeanOfLogsChannel(), fRMSBananaChi2Ndf(), fRMSOfLogsBananaChi2Ndf()
{
}


/** */
Calibrator::Calibrator(TRandom *random) : TObject(), fRandom(random),
   fChannelCalibs(), fMeanChannel(), fMeanOfLogsChannel(), fRMSBananaChi2Ndf(), fRMSOfLogsBananaChi2Ndf()
{
}


/** Default destructor. */
Calibrator::~Calibrator()
{
}


/** */
void Calibrator::UpdateMeanChannel()
{
	//Info("UpdateMeanChannel", "Called");

   fMeanChannel.ResetToZero();
   fMeanOfLogsChannel.ResetToZero();

	vector<Float_t> vT0Coef;
	//vector<Float_t> vT0CoefErr;
   vector<Float_t> vDLWidth;
   //vector<Float_t> vDLWidthErr;
   vector<Float_t> vBananaChi2Ndf;
   vector<Float_t> vBananaChi2NdfLog;

   ChannelCalibMap::const_iterator iCh;
   ChannelCalibMap::const_iterator mb = fChannelCalibs.begin();
   ChannelCalibMap::const_iterator me = fChannelCalibs.end();

   UInt_t nChannels = 0;

   for (iCh=mb; iCh!=me; ++iCh)
   {
      if (gMeasInfo->IsSiliconChannel(iCh->first) && !isnan(iCh->second.fT0Coef) && !isinf(iCh->second.fT0Coef) &&
          !gMeasInfo->IsDisabledChannel(iCh->first) && iCh->second.GetFitStatus() == kDLFIT_OK )
      {
         vT0Coef.push_back(iCh->second.fT0Coef);
         //vT0CoefErr.push_back(iCh->second.fT0CoefErr);
         vDLWidth.push_back(iCh->second.fDLWidth);
         //vDLWidthErr.push_back(iCh->second.fDLWidthErr);
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
void Calibrator::CopyAlphaCoefs(Calibrator &calibrator)
{
   ChannelCalibMap::const_iterator iCh;
   ChannelCalibMap::const_iterator mb = calibrator.fChannelCalibs.begin();
   ChannelCalibMap::const_iterator me = calibrator.fChannelCalibs.end();

   for (iCh=mb; iCh!=me; ++iCh) {

      ChannelCalibMap::iterator iChCalib = fChannelCalibs.find(iCh->first);

      if (iChCalib != fChannelCalibs.end()) {
         iChCalib->second.CopyAlphaCoefs(iCh->second);
      } else {
         ChannelCalib newChCalib;
         newChCalib.CopyAlphaCoefs(iCh->second);
         fChannelCalibs[iCh->first] = newChCalib;
      }
   }
}


/** */
void Calibrator::Calibrate(DrawObjContainer *c)
{
	Info("Calibrate(DrawObjContainer *c)", "Called...");
}


/** */
//void Calibrator::CalibrateFast(DrawObjContainer *c)
//{
//	Info("CalibrateFast", "Executing CalibrateFast()");
//}


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


/** */
Float_t Calibrator::GetEnergyA(UShort_t adc, UShort_t chId) const
{
   return fChannelCalibs.find(chId)->second.fAmAmp.fCoef * adc;
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
   //return WFD_TIME_UNIT_HALF * (tdc + fRandom->Rndm() - 0.5);
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
      //printf("\n");
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
